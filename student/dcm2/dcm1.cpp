//   Copyright 2017 Romain Boman
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                             DCM: Travail n�2
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                                                                        R.B.
// Vendredi 10.02.95

#include "dcm1.h"
#include "jacobi.h"

Dcm::Dcm()
{
    // data
    densite = 2700.0; // Aluminium
    enverg = 22.0;
    Mmoteurs = 14000.0;
    Mfuselage = 50000.0;
    MYoung = 65e9;
    ep = 0.005;
    c0 = 1.20;
    c1 = 0.30;
    T = 1.0;
    F0 = 150000.0;
    np = 80;     // pr�cision du trac� en x ds MATLAB    [TODO] changer en "int" ??
    np2 = 40;    //                    en t              [TODO] changer en "int" ??
    Nperiod = 2; // nbre de p�riodes en t (pour MATLAB)
    Nmodes = 6;    // nbre de modes � calculer avec prec.
    PREC = 1E-4;
    PREC2 = 1E-2;
}

void Dcm::calcule()
{
    //-------------------------------------------------------------------------
    //            Calcul de la masse et de l'inertie de l'aile
    //-------------------------------------------------------------------------
    Polynome Unite(0);
    Unite[0] = 1;

    Polynome h(1);
    h[0] = c0;
    h[1] = (c1 - c0) / enverg;

    Polynome m(1);
    m = densite * (2.0 * ep * (11.0 * h - 2.0 * ep * Unite));

    Polynome I(3);
    I = h * h * h * (31.0 / 6.0) * ep - 11.0 * h * h * pow(ep, 2) + (26.0 / 3.0) * h * pow(ep, 3);
    I[0] = I[0] - 4.0 / 3.0 * pow(ep, 4);

    Masses MSX[4];
    MSX[0].masse = Mfuselage;
    MSX[0].x = 0.0;
    MSX[1].masse = Mmoteurs;
    MSX[1].x = enverg / 2.0;
    MSX[2].masse = Mmoteurs;
    MSX[2].x = -enverg / 2.0;
    MSX[3].masse = NULL;
    MSX[3].x = 0.0;

    std::cout << "--------\n";
    std::cout << "Masse   : " << m << '\n';
    std::cout << "Inertie : " << I << '\n';
    std::cout << "--------\n";

    //-------------------------------------------------------------------------
    //                   Cr�ation d'une base de polynome
    //-------------------------------------------------------------------------

    Polynome p(0);
    p[0] = 1.0 / sqrt(m.integrale(0.0, enverg) // 1er poly = cte.
                      + (!m).integrale(-enverg, 0.0) + Mfuselage + 2 * Mmoteurs);
    std::cout << "Polynome #0:" << p << '\n';

    BasePoly Base(MSX, I, m, MYoung, enverg, p);

    for (int i = 2; i < Nmodes + 1; i++) // Ajoute 5 poly.
        double **KM = Base.ajoute_suivant();
    int nopoly = Nmodes;

    double *ValPro2 = new double[Nmodes];
    ValPro2--; // Tableaux auxil. contenant

    double **ModPro2 = new double *[Nmodes];
    ModPro2--;                       // les val. & vect. pr.

    for (int i = 1; i < Nmodes + 1; i++) // pour (n-1) poly...
    {
        ValPro2[i] = 0.0; // ...et initialisation � 0.
        ModPro2[i] = new double[Nmodes];
        ModPro2[i]--;
        for (int j = 1; j <= nopoly; j++)
            ModPro2[i][j] = 0.0;
    }

    //-------------------------------------------------------------------------
    //    Ajoute un polynome et compare les val. et vect. propres avec
    //             ceux et celles de l'it�ration pr�c�dente.
    //-------------------------------------------------------------------------

    double *ValPro;
    double **ModPro;

    int rate = 1;
    while (rate == 1)
    {
        double **KM = Base.ajoute_suivant();
        nopoly++;

        double **COPY_K = new double *[nopoly]; // Copie la matrice K dans COPY_K
        for (int i = 0; i < nopoly; i++)
        {
            COPY_K[i] = new double[nopoly];
            for (int j = 0; j < nopoly; j++)
                COPY_K[i][j] = KM[i][j];
        }
        COPY_K--;                         // indice minimal = 1 (pour Jacobi())
        for (int j = 1; j <= nopoly; j++)
            COPY_K[j]--;

        ValPro = new double[nopoly];
        ValPro--;

        ModPro = new double *[nopoly];
        ModPro--;
        for (int j = 1; j <= nopoly; j++)
        {
            ModPro[j] = new double[nopoly];
            ModPro[j]--;
        }
        int nrot;
        jacobi(COPY_K, nopoly, ValPro, ModPro, nrot);

        // Trie les VP (Bubble sort)
        int j = nopoly;
        while (j != 1)
        {
            int i = 0;
            int k = 0;
            while (i != j - 1)
            {
                if (ValPro[i + 1] > ValPro[i + 2])
                {
                    dswap(&ValPro[i + 1], &ValPro[i + 2]);
                    Polynome swap(0);
                    swap = Base[i];
                    Base[i] = Base[i + 1];
                    Base[i + 1] = swap;
                    for (int compt = 0; compt < nopoly; compt++)
                        dswap(&ModPro[i + 1][compt], &ModPro[i + 2][compt]);
                    k = i;
                }
                i++;
            }
            j = k + 1;
        }

        // Teste les valeurs obtenues
        rate = 0;
        for (int i = 1; i < Nmodes + 1; i++)
        {
            if (fabs(ValPro[i]) > 1E-10)
                if (fabs(1.0 - sqrt(ValPro2[i]) / sqrt(ValPro[i])) > PREC)
                    rate = 1;
            ValPro2[i] = ValPro[i];
        }
        for (int i = 1; i < Nmodes + 1; i++)
            for (int j = 1; j < Nmodes + 1; j++)
            {
                if (fabs(ModPro[i][j]) > 1E-10)
                    if (fabs(1.0 - ModPro2[i][j] / ModPro[i][j]) > PREC2)
                        rate = 1;
                ModPro2[i][j] = ModPro[i][j];
            }

        //---Destruction des tabl. auxil.--------
        if (rate == 1) // Vire les VP & VP dans le
        {              // cas o� la pr�cision n'est pas atteinte
            ValPro++;
            delete ValPro;
            for (int j = 1; j <= nopoly; j++)
            {
                ModPro[j]++;
                delete ModPro[j];
            }
            ModPro++;
            delete ModPro;
        }

        for (int j = 1; j <= nopoly; j++) // Vire COPY_K
            COPY_K[j]++;
        COPY_K++;
        for (int i = 0; i < nopoly; i++)
            delete COPY_K[i];
        delete COPY_K;
    }
    std::cout << '\n'
              << nopoly << " poly. n�cessaires.\n";
    //getch();

    //---"Nettoie" les modes propres------------
    for (int j = 1; j <= nopoly; j++)
        for (int k = 1; k <= nopoly; k++)
            if (fabs(ModPro[j][k]) < 1E-10)
                ModPro[j][k] = 0.0;                

    //-------------------------------------------------------------------------
    //                       affichage des r�sultats
    //-------------------------------------------------------------------------
    std::cout << "\nValeurs Propres:\n";
    for (int j = 1; j <= nopoly; j++)
        std::cout << j << ": " << sqrt(ValPro[j]) << '\n';
    //getch();
    std::cout << "\nVecteurs Propres:\n";
    for (int j = 1; j <= nopoly; j++)
    {
        std::cout << j << ": (";
        for (int k = 1; k <= nopoly; k++)
            std::cout << ModPro[j][k] << ",";
        std::cout << ")\n";
    }
    // Base.affiche_K(nopoly); getch();

    //-------------------------------------------------------------------------
    //           Transfert des val. et vect. propres vers Matlab
    //-------------------------------------------------------------------------
    toMatlab1(ValPro, ModPro, Nmodes);

    //-------------------------------------------------------------------------
    //               Calcul des modes propres Yi(x) -> MP[i]
    //          Cr�ation d'un prog *.m pour afficher les r�sultats
    //-------------------------------------------------------------------------
    Polynome *MP = new Polynome[nopoly]; // Cr�e un tableau de poly.
    for (int i = 0; i < nopoly; i++)
    {
        MP[i][0] = 0.0;
        for (int j = 0; j < nopoly; j++)
            MP[i] = MP[i] + ModPro[i + 1][j + 1] * Base[j];
    }

    //---Calcul de la matrice pour MATLAB--------
    XX = new double[np + 1];      // Matrice abcisse.
    MODES = new double *[nopoly]; // Matrice des Yi(x)
    for (int i = 0; i < nopoly; i++)
    {
        MODES[i] = new double[np + 1];
        for (int j = 0; j < np + 1; j++) // Initialisation � 0.
        {
            XX[j] = 0.0;
            MODES[i][j] = 0.0;
        }
    }
    int compt; 
    double t;
    for (compt = 0, t = -enverg; compt <= np; t += 2 * enverg / np, compt++)
    {
        XX[compt] = t;
        for (int i = 0; i < nopoly; i++)
            MODES[i][compt] += MP[i](t);
    }

    //-------------------------------------------------------------------------
    //                      Calcul des masses r�duites
    //              pour une �ventuelle normalisation des modes
    //-------------------------------------------------------------------------
    double *mu = new double[nopoly];
    for (int i = 0; i < nopoly; i++)
    {
        mu[i] = (m * MP[i] * MP[i]).integrale(0.0, enverg) + ((!m) * MP[i] * MP[i]).integrale(-enverg, 0.0) + MP[i](0) * MP[i](0) * Mfuselage + MP[i](-enverg / 2) * MP[i](-enverg / 2) * Mmoteurs + MP[i](enverg / 2) * MP[i](enverg / 2) * Mmoteurs;
    }

    // Ecriture dans un fichier *.m
    toMatlab2(MP);

    //-------------------------------------------------------------------------
    //                    Calcul du moment fl�chissant et
    //             de l'effort tranchant � l'emplanture de l'aile
    //-------------------------------------------------------------------------

    double *Moment = new double[Nperiod * np2 + 1];
    double *Tranchant = new double[Nperiod * np2 + 1];
    double alp0 = -2 * F0 * T / pi * MP[0](0.0);

    // M et T pdt l'appl. de la force
    //double t;
    Polynome M;
    for (t = 0, compt = 0; compt < np2 + 1; t += T / np2, compt++)
    {
        M = M - M;
        M[0] = 0.0;
        M = MP[0] * (MP[0](0.0) * F0 * T * T / (pi * pi) * sin(pi * t / T) + (alp0 + MP[0](0.0) * F0 * T / pi) * t);
        for (int i = 3; i < Nmodes; i += 2)
        {
            double om = sqrt(ValPro[i]);
            M = M + MP[i - 1] * MP[i - 1](0.0) * (F0 / om * (T / pi * sin(om * t) - om * T * T / (pi * pi) * sin(pi * t / T)) / (1 - om * om * T * T / (pi * pi)));
        }
        M = M.derive().derive();
        M = M * MYoung * I;
        Moment[compt] = M(0.0);

        Polynome DM = M.derive();
        Tranchant[compt] = DM(0.0);
    }

    // Calcul de M & T apr�s l'appl. de F
    double *alpha = new double[6];
    double *alphap = new double[6];

    alpha[0] = (alp0 + F0 * MP[0](0.0) * T / pi) * T;
    alphap[0] = 0.0;
    for (int i = 3; i < Nmodes; i += 2)
    {
        double om = sqrt(ValPro[i]);
        alpha[i] = F0 * MP[i - 1](0.0) / om * (T / (pi)*sin(om * T)) / (1 - ((om * om * T * T) / (pi * pi)));
        alphap[i] = F0 * MP[i - 1](0.0) / om * (T / (pi)*om * cos(om * T) + om * T / (pi)) / (1 - ((om * om * T * T) / (pi * pi)));
    }

    for (t = T + T / np2; compt < np2 * Nperiod + 1; t += T / np2, compt++)
    {
        M = M - M;
        M[0] = 0.0;
        M = MP[0] * (alphap[0] * (t - T) + alpha[0]);
        for (int i = 3; i < Nmodes; i += 2)
        {
            double om = sqrt(ValPro[i]);
            M = M + MP[i - 1] * (alpha[i] * cos(om * (t - T)) + alphap[i] / om * sin(om * (t - T)));
        }
        M = M.derive().derive();
        M = M * MYoung * I;
        Moment[compt] = M(0.0);

        Polynome DM = M.derive();
        Tranchant[compt] = DM(0.0);
    }

    // Prog. MATLAB -> Graphe M et T
    toMatlab3(Moment, Tranchant);
}

void Dcm::dswap(double *a, double *b)
{
    double tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void main()
{
    Polynome::demo();
    std::cout << "\n\n---\n\n";
    Dcm dcm;
    dcm.calcule();
}


void Dcm::toMatlab1(double *ValP, double **VectP, int n)
{
    std::ofstream fich("vpvp.m", std::ios::out);
    fich << "vap=[";
    for (int i = 1; i <= n - 1; i++)
        fich << sqrt(ValP[i]) << ",...\n ";
    fich << sqrt(ValP[n]) << "];\n\nvep=[";
    for (int i = 1; i <= n; i++)
    {
        fich << "[";
        for (int j = 1; j <= n - 1; j++)
            fich << VectP[i][j] << ",...\n ";
        fich << VectP[i][n] << "]\n";
    }
    fich << "];";
    fich.close();
    std::cout << "vpvp.m cree.\n";
}

void Dcm::toMatlab2(Polynome *MP)
{
    std::ofstream fich("graphe.m", std::ios::out);
    fich << "x=["; // Vecteur abcisse  : x
    for (int i = 0; i < np; i++)
        fich << XX[i] << ",...\n ";
    fich << XX[(int)np] << "];\n\ngrafic=[";
    for (int i = 0; i < Nmodes; i++) // Matrice Yi(x)    : grafic
    {
        fich << "[";
        for (int j = 0; j < np; j++)
            fich << MODES[i][j] << ",...\n ";
        fich << MODES[i][(int)np] << "];\n";
    }
    fich << "];\n\nyo=[";
    for (int j = 0; j < Nmodes; j++) // Valeurs de Yi(0);
        fich << MP[j](0.0) << ",...\n ";
    fich << MP[(int)Nmodes](0.0) << "];\n";
    fich << "\nnp=" << np << ";\n";
    fich << "np2=" << np2 << ";\n";
    fich << "NPERIOD=" << Nperiod << ";\n";
    fich << "T=" << T << ";\n";
    fich << "F0=" << F0 << ";\n";
    fich << "NMOD=" << Nmodes - 1 << ";\n";
    fich << "\ngrafic=grafic'; x=x';\n"; // Commandes de trac�.
    fich << "figure;\n";
    fich << "v=[-" << enverg << "," << enverg << ",-0.04,0.04]; axis(v);\n";
    fich << "plot(x,grafic,'k');\n";
    fich << "title('Modes propres (normes)');\n";
    fich << "xlabel('x');\nylabel('Yi(x)');\n";
    fich.close();
    std::cout << "graphe.m cree.\n";
}

void Dcm::toMatlab3(double *Moment, double *Tranchant)
{
    std::ofstream fich2("mt.m", std::ios::out);
    fich2 << "M=[";
    for (int i = 0; i < compt - 1; i++)
        fich2 << Moment[i] << ",...\n";
    fich2 << Moment[compt - 1] << "];\n\nET=[";
    for (int i = 0; i < compt - 1; i++)
        fich2 << Tranchant[i] << ",...\n";
    fich2 << Tranchant[compt - 1] << "];\n";
    fich2 << "figure;\n";
    fich2 << "plot(0:T/np2:NPERIOD*T,M,'k',0:T/np2:NPERIOD*T,ET,'k');\n";
    fich2 << "%gtext('M(t)');\n";
    fich2 << "%gtext('T(t)');\n";
    fich2 << "grid;\n";
    fich2 << "title(' Moment et effort tranchant a l'' emplanture de l'' aile');\n";
    fich2 << "xlabel('temps');\nylabel('M(t) & T(t) en x=0');\n";
    fich2.close();
    std::cout << "mt.m cree.\n";
}

