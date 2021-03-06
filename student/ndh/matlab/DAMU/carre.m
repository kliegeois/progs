% ********************************************************************
% *                  Travail El�ments aux Fronti�res                 *
% *                                                                  *
% *                       Le carr� de cot� 2a                        *
% ********************************************************************

% ********************************************************************
%      Programme optimis� tenant compte de toutes les sym�tries
% ********************************************************************

clear all; close all;

a=1.2;     % Demi c�t� du carr�
B=80.;     % BETA (en W/m^3)
K=400.;    % Coeff. de conductivit� thermique (en W/m.K)

N=input('Nb de sous-domaines par cot� ?');
W=input('Nb d''intervalles pour l''int�gration num�rique ?');
P=input('Nb de points pour le calcul de T dans le domaine ? ');
% ------------------------------------
% Calcul de grandeurs caract�ristiques
% ------------------------------------

tt=cputime;

L=2*a/N;

% Calcul de la condition limite

for i=1:round(N/2)
    Phi(i,1)=-B/(2*K)*(a^2+(L/2-a+(i-1)*L)^2);
end

if N/2==round(N/2)
    for i=N/2+1:N
        Phi(i,1)=Phi(N-i+1);
    end
else
    for i=round(N/2)+1:N
        Phi(i,1)=Phi(N-i+1,1);
    end
end

for k=1:3
    Phi(1+k*N:N+k*N,1)=Phi(1:N,1);
end

% -------------------
% Calcul de H et de G
% -------------------

% Calcul des round(N/2) premi�res lignes de H et de G

for i=1:round(N/2)
    
    H1(i,i)=0.5;
    
    if i==1
        G1(1,1)=L/(2*pi)*(log(2/L)+1);
        for j=2:N
            for n=0:W
                f2(n+1)=L/(2*pi)*log(1/(L/2+(n/W+j-2)*L));
            end
            G1(1,j)=0.;
            for n=1:W
                G1(1,j)=G1(1,j)+(f2(n)+f2(n+1))/(2*W);
            end
        end
        
        for j=2*N+1:3*N
            
            x=-2*a;
            for n=0:W
                y=2*a-L/2-(n/W+j-2*N-1)*L;
                r=sqrt(x^2+y^2);
                f1(n+1)=L/(2*pi*r^2)*x;
                f2(n+1)=L/(2*pi)*log(1/r);
            end
            H1(1,j)=0.;
            G1(1,j)=0.;
            for n=1:W
                H1(1,j)=H1(1,j)+(f1(n)+f1(n+1))/(2*W);
                G1(1,j)=G1(1,j)+(f2(n)+f2(n+1))/(2*W);
            end
        end
        
    elseif i>1
        
        for k=1:i
            G1(i,k)=G1(1,i-k+1);
        end
        for k=i+1:N
            G1(i,k)=G1(1,k-i+1);
        end
        
        for j=2*N+1:3*N-1
            H1(i,j)=H1(i-1,j+1);
            G1(i,j)=G1(i-1,j+1);
        end
        
        H1(i,3*N)=H1(1,3*N-(i-1));
        G1(i,3*N)=G1(1,3*N-(i-1));
        
    end
    
    y=L/2+(N-i)*L;
    for j=N+1:2*N
        for n=0:W
            x=-((n/W)+j-(N+1))*L;
            r=sqrt(x^2+y^2);
            f1(n+1)=-L/(2*pi*r^2)*y;
            f2(n+1)=L/(2*pi)*log(1/r);
        end
        H1(i,j)=0.;
        G1(i,j)=0.;
        for n=1:W
            H1(i,j)=H1(i,j)+(f1(n)+f1(n+1))/(2*W);
            G1(i,j)=G1(i,j)+(f2(n)+f2(n+1))/(2*W);
        end
    end
    
    y=-L/2-(i-1)*L;
    for j=3*N+1:4*N
        for n=0:W
            x=-2*a+(n/W+j-(3*N+1))*L;
            r=sqrt(x^2+y^2);
            f1(n+1)=L/(2*pi*r^2)*y;
            f2(n+1)=L/(2*pi)*log(1/r);
        end,
        H1(i,j)=0.;
        G1(i,j)=0.;
        for n=1:W
            H1(i,j)=H1(i,j)+(f1(n)+f1(n+1))/(2*W);
            G1(i,j)=G1(i,j)+(f2(n)+f2(n+1))/(2*W);
        end
    end
end

% -------------------------------------
% Calcul des deux membres de l'�quation
% -------------------------------------

% Construction des matrices r�duites MG et MD
% � partir des round(N/2) premi�res
% lignes de H et G

MG=H1(1:round(N/2),:)*Phi;

for s1=0:round(N/2)-1
    for s2=0:round(N/2)-1
        MD(s1+1,s2+1)=0.;
        for k=0:3
            MD(s1+1,s2+1)=MD(s1+1,s2+1)+G1(s1+1,1+s2+k*N)+G1(s1+1,N-s2+k*N);
        end
        if N/2<round(N/2)
            if s2==round(N/2)-1
                MD(s1+1,s2+1)=0.5*MD(s1+1,s2+1);
            end
        end
    end
end

% --------------------------
% Calcul du vecteur des flux
% --------------------------

Q=inv(MD)*MG

if N/2==round(N/2)
    Q(N/2+1:N,1)=flipud(Q(1:N/2,1));
else
    Q(round(N/2)+1:N,1)=flipud(Q(1:round(N/2)-1,1));
end

for k=1:3
    Q(1+k*N:N+k*N,1)=Q(1:N,1);
end

% --------------------------------------
% Calcul de la temp�rature en P^2 points
% --------------------------------------

% Calcul de Nmax

Nmax=0;
for i=1:P
    Nmax=Nmax+i;
end

% Boucle sur les �l�ments du domaine

for ii=1:P
    
    xx(ii)=(ii-1)*a/P;
    yy(ii)=(ii-1)*a/P;
    
    for jj=ii:P
        
        xp=(jj-1)*a/P;
        yp=(ii-1)*a/P;
        
        
        % i [1,N]
        
        x=a-xp;
        for i=1:N
            for n=0:W
                y=-a-yp+(n/W+i-1)*L;
                r=sqrt(x^2+y^2);
                f1(n+1)=-L/(2*pi*r^2)*x;
                f2(n+1)=L/(2*pi)*log(1/r);
            end,
            H2(i)=0.;
            G2(i)=0.;
            for n=1:W
                H2(i)=H2(i)+(f1(n)+f1(n+1))/(2*W);
                G2(i)=G2(i)+(f2(n)+f2(n+1))/(2*W);
            end
        end
        
        % i [N+1,2N]
        
        y=a-yp;
        for i=N+1:2*N
            for n=0:W
                x=a-xp-(n/W+i-N-1)*L;
                r=sqrt(x^2+y^2);
                f1(n+1)=-L/(2*pi*r^2)*y;
                f2(n+1)=L/(2*pi)*log(1/r);
            end
            H2(i)=0.;
            G2(i)=0.;
            for n=1:W
                H2(i)=H2(i)+(f1(n)+f1(n+1))/(2*W);
                G2(i)=G2(i)+(f2(n)+f2(n+1))/(2*W);
            end
        end
        
        % i [2N+1,3N]
        
        x=-a-xp;
        for i=1+2*N:3*N
            for n=0:W
                y=a-yp-(n/W+i-2*N-1)*L;
                r=sqrt(x^2+y^2);
                f1(n+1)=L/(2*pi*r^2)*x;
                f2(n+1)=L/(2*pi)*log(1/r);
            end,
            H2(i)=0.;
            G2(i)=0.;
            for n=1:W
                H2(i)=H2(i)+(f1(n)+f1(n+1))/(2*W);
                G2(i)=G2(i)+(f2(n)+f2(n+1))/(2*W);
            end
        end
        
        % i [3N+1,4N]
        
        y=-a-yp;
        for i=3*N+1:4*N
            for n=0:W
                x=-a-xp+(n/W+i-3*N-1)*L;
                r=sqrt(x^2+y^2);
                f1(n+1)=L/(2*pi*r^2)*y;
                f2(n+1)=L/(2*pi)*log(1/r);
            end
            H2(i)=0.;
            G2(i)=0.;
            for n=1:W
                H2(i)=H2(i)+(f1(n)+f1(n+1))/(2*W);
                G2(i)=G2(i)+(f2(n)+f2(n+1))/(2*W);
            end
        end
        
        F(ii,jj)=0.;
        
        for j=1:4*N
            F(ii,jj)=F(ii,jj)+G2(j)*Q(j,1)-H2(j)*Phi(j,1);
        end
        
        T_approx(ii,jj)=F(ii,jj)+B/(2*K)*(xp^2+yp^2);
        
        T(ii,jj)=0.;
        for nn=1:2:7
            T(ii,jj)=T(ii,jj)+(-1)^((nn-1)/2)/nn^3*cos(nn*pi*xp/(2*a))*(1-(cosh(nn*pi*yp/(2*a))/(cosh(nn*pi/2))));
        end
        
        T(ii,jj)=-32*B*a^2/(K*(pi)^3)*T(ii,jj);
        
    end
end

err=0.;

for ii=1:P
    for jj=ii:P
        err=err+abs((T(ii,jj)-T_approx(ii,jj))/T(ii,jj));
    end,
end,

err=err/Nmax;

tt=cputime-tt;

TT=T+tril(T',-1);
TTA=T_approx+tril(T_approx',-1);
FF=F+tril(F',-1);

figure,
mesh(xx,yy,FF),view(-45,-30),title('Solution de Laplacien(T)=0 (le point le plus proche = le coin)'),
figure,
mesh(xx,yy,TT),title('Solution analytique (le point le plus proche = le centre)'),
figure,
mesh(xx,yy,TTA),title(['Solution El�ments Fronti�res => ',num2str(0),' flops et Temps CPU =',num2str(tt),' s']),
figure,
mesh(xx,yy,abs(TT-TTA)),title(['Diff�rence entre solution analytique et EF avec ',num2str(4*N),' �l�ments => erreur relative = ',num2str(err*100),' %']),
