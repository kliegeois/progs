// pour compiler : cc -g -lm -o cyl.x  cylindre.c
// ca cree un executable test.x
// on tape test.x et ca cree out.dat lisible dans BACON
// pour modifier la sphere, aller voir dans param�tres ! (un peu plus bas)

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{

    int cyl_creux, nbe, nbc, nbz, noe_ini, maille_ini, mat1rig, loi1rig, mat1def, loi1def, type1,
        mat2rig, loi2rig, mat2def, loi2def, type2, nocyl, cyl_ouvert, i, j, k,
        taille1, taille2, taille3, ***tab, ***cube, taille, *liste, nz, couche, ne, no, nint,
        n[4], n0[4], don[10][2], level1, level2, noe, noe1, noe2, noe3, noe4, noe5, noe6, noe7, noe8,
        out, n1, n2, nbnoe, l, c, maille, ne2, cote, nbe2;

    double rint, rext, centre[3], ext[3], norm[3], ray, theta0, **coord, vec1[3], vabs, longext, theta,
        vrot[3];

    FILE *fp_out, *fp_out2;

    fp_out = fopen("cyl_noe.dat", "w");
    fp_out2 = fopen("cyl_mco.dat", "w");

    fprintf(fp_out, ".DEL.*\n");

    // PARAMETRES

    cyl_creux = 1;      // 1 si creuse, 0 si pleine
    rint = 97.;         // rayon interne si creuse, demi-diagonale du cube central si pleine
    rext = 100.;        // rayon externe
    longext = 460.;     // longueur du cylindre
    cyl_ouvert = 0;     // 1 si ouvert, 0 si ferm�
    theta0 = 360.;      // ouverture en degr� si ouvert
    centre[0] = -100.1; // coor x du centre
    centre[1] = 0.0;    // coor y du centre
    centre[2] = -230.0; // coor z du centre
    vec1[0] = 1.0;      // position du premier noeud
    vec1[1] = 0.0;      //
    vec1[2] = 0.0;      //
    norm[0] = 0.0;      // coor x de la normale au plan de l arc
    norm[1] = 0.0;      // coor y de la normale au plan du l arc
    norm[2] = 1.0;      // coor z de la normale au plan du c arc
    ext[0] = 0.0;       // coor x de la direction d extrusion
    ext[1] = 0.0;       // coor y de la direction d extrusion
    ext[2] = 1.0;       // coor z de la direction d extrusion
    nbe = 24;           // nombre d elements sur l arc d un m�ridien
    nbc = 1;            // nombre d elements sur l epaisseur
    nbz = 20;           // nombre d elements sur la hauteur
    noe_ini = 0;
    maille_ini = 0;
    nocyl = 1;

    // face externe

    mat1rig = 1;
    mat1def = 1;
    loi1rig = 1;
    loi1def = 1;
    type1 = 1; // -1 pas contact, 0 si rigide, 1 si defo-defo, 2 si les deux

    // face interne

    if (cyl_creux == 1)
    {
        mat2rig = 3;
        mat2def = 4;
        loi2rig = 3;
        loi2def = 4;
        type2 = -1; // -1 pas contact, 0 si rigide, 1 si defo-defo, 2 si les deux
    }
    else
    {
        type2 = -1;
    }

    // FIN DES PARAMETRES

    // inversion des rayons pour rext > rint

    if (theta0 != 360.)
    {
        cyl_ouvert = 1;
    }
    theta0 = theta0 * atan(1.) / 45.;
    if (rext < rint)
    {
        ray = rext;
        rext = rint;
        rint = ray;
        ray = 0;
    }

    if (cyl_creux == 0)
    {
        if (cyl_ouvert == 1)
        {
            return;
        }
        if (nbe % 4 != 0)
        {
            nbe = (nbe / 4 + 1) * 4;
        }
    }

    if (cyl_ouvert == 1)
    {
        nbe2 = (nbe + 1);
    }
    else if (cyl_ouvert == 0)
    {
        nbe2 = nbe;
    }

    mat1def = -mat1def;
    mat2def = -mat2def;

    // norme les vecteur de definition

    vrot[0] = vec1[1] * norm[2] - vec1[2] * norm[1];
    vrot[1] = vec1[2] * norm[0] - vec1[0] * norm[2];
    vrot[2] = vec1[0] * norm[1] - vec1[1] * norm[0];

    vec1[0] = norm[1] * vrot[2] - norm[2] * vrot[1];
    vec1[1] = norm[2] * vrot[0] - norm[0] * vrot[2];
    vec1[2] = norm[0] * vrot[1] - norm[1] * vrot[0];

    vabs = sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1] + vec1[2] * vec1[2]);
    if (vabs < 1.E-8)
    {
        return;
    }
    for (i = 0; i < 3; i++)
    {
        vec1[i] = vec1[i] / vabs;
    }
    vabs = sqrt(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);
    for (i = 0; i < 3; i++)
    {
        norm[i] = norm[i] / vabs;
    }
    vabs = sqrt(ext[0] * ext[0] + ext[1] * ext[1] + ext[2] * ext[2]);
    for (i = 0; i < 3; i++)
    {
        ext[i] = ext[i] / vabs;
    }

    // allocation du tableau tab(nz,couche,ne)

    taille1 = (nbz + 1);
    tab = (int ***)calloc(taille1, sizeof(int **));
    for (i = 0; i < taille1; i++)
    {
        taille2 = (nbc + 1);
        tab[i] = (int **)calloc(taille2, sizeof(int *));
        for (j = 0; j < taille2; j++)
        {
            taille3 = nbe2;
            tab[i][j] = (int *)calloc(taille3, sizeof(int));
        }
    }

    // allocation du tableau cube(nz,lig,col)

    if (cyl_creux == 0)
    {
        taille1 = (nbz + 1);
        cube = (int ***)calloc(taille1, sizeof(int **));
        for (i = 0; i < taille1; i++)
        {
            taille2 = (nbe / 4 + 1);
            cube[i] = (int **)calloc(taille2, sizeof(int *));
            for (j = 0; j < taille2; j++)
            {
                taille3 = (nbe / 4 + 1);
                cube[i][j] = (int *)calloc(taille3, sizeof(int));
            }
        }
    }

    // allocation du tableau coord(numint, xyz)

    taille = (nbe2) * (nbc + 1) * (nbz + 1);
    if (cyl_creux == 0)
    {
        taille = taille + (nbe / 4 - 1) * (nbe / 4 - 1) * (nbz + 1);
    }

    coord = (double **)calloc(taille, sizeof(double *));
    for (i = 0; i < taille; i++)
    {
        coord[i] = (double *)calloc(3, sizeof(double));
    }

    //  allocation du vecteur liste(numint)=numdao

    taille = (nbe2) * (nbc + 1) * (nbz + 1);
    if (cyl_creux == 0)
    {
        taille = taille + (nbe / 4 - 1) * (nbe / 4 - 1) * (nbz + 1);
    }
    liste = (int *)calloc(taille, sizeof(int));

    // fin des allocations

    // remplissage de tab

    no = -1;

    for (nz = 0; nz < nbz + 1; nz++)
    {
        for (couche = 0; couche < nbc + 1; couche++)
        {
            for (ne = 0; ne < nbe2; ne++)
            {
                no = no + 1;
                tab[nz][couche][ne] = no;
            }
        }
    }

    // remplissage de cube

    if (cyl_creux == 0)
    {
        for (nz = 0; nz < nbz + 1; nz++)
        {
            for (c = 0; c < nbe / 4 + 1; c++)
            {
                for (l = 0; l < nbe / 4 + 1; l++)
                {
                    if (l == 0)
                    {
                        cube[nz][l][c] = tab[nz][nbc][c];
                    }
                    else if (l == (nbe / 4))
                    {
                        cube[nz][l][c] = tab[nz][nbc][(3 * (nbe / 4)) - c];
                    }
                    else if (c == 0)
                    {
                        cube[nz][l][c] = tab[nz][nbc][(4 * (nbe / 4)) - l];
                    }
                    else if (c == (nbe / 4))
                    {
                        cube[nz][l][c] = tab[nz][nbc][((nbe / 4)) + l];
                    }
                    else
                    {
                        no = no + 1;
                        cube[nz][l][c] = no;
                    }
                }
            }
        }
    }

    // Calcul des coordonn�es

    // le point 1

    noe1 = tab[0][0][0];
    ray = rext;
    for (i = 0; i < 3; i++)
    {
        coord[noe1][i] = vec1[i] * ray;
    }

    // la couche exterieure du level 0

    for (ne = 1; ne < nbe2; ne++)
    {
        nint = tab[0][0][ne];
        theta = theta0 * (ne * 1.) / (nbe * 1.);
        coord[nint][0] = coord[noe1][0] * cos(theta) + (norm[1] * coord[noe1][2] - norm[2] * coord[noe1][1]) * sin(theta);
        coord[nint][1] = coord[noe1][1] * cos(theta) + (norm[2] * coord[noe1][0] - norm[0] * coord[noe1][2]) * sin(theta);
        coord[nint][2] = coord[noe1][2] * cos(theta) + (norm[0] * coord[noe1][1] - norm[1] * coord[noe1][0]) * sin(theta);
    }

    // couche interieure du level 0

    if (cyl_creux == 1)
    {
        for (ne = 0; ne < nbe2; ne++)
        {
            noe2 = tab[0][nbc][ne];
            noe1 = tab[0][0][ne];
            ray = rint / rext;
            for (i = 0; i < 3; i++)
            {
                coord[noe2][i] = coord[noe1][i] * ray;
            }
        }
    }
    else if (cyl_creux == 0)
    {
        for (cote = 0; cote < 4; cote++)
        {
            n0[cote] = tab[0][0][cote * nbe / 4];
            n[cote] = tab[0][nbc][cote * nbe / 4];
            ray = rint / rext;
            for (i = 0; i < 3; i++)
            {
                coord[n[cote]][i] = coord[n0[cote]][i] * ray;
            }
        }
        for (cote = 0; cote < 4; cote++)
        {
            noe1 = n[cote];
            if (cote == 3)
            {
                noe2 = n[0];
            }
            else
            {
                noe2 = n[cote + 1];
            }
            for (c = 1; c < nbe / 4; c++)
            {
                nint = tab[0][nbc][cote * nbe / 4 + c];
                for (i = 0; i < 3; i++)
                {
                    coord[nint][i] = coord[noe1][i] + (c * 1.) / ((nbe / 4) * 1.) * (coord[noe2][i] - coord[noe1][i]);
                }
            }
        }
    }

    // Toutes les couches de la face 0

    if (nbc > 1)
    {
        for (couche = 1; couche < nbc; couche++)
        {
            for (ne = 0; ne < nbe2; ne++)
            {
                noe = tab[0][couche][ne];
                noe1 = tab[0][0][ne];
                noe2 = tab[0][nbc][ne];
                for (i = 0; i < 3; i++)
                {
                    coord[noe][i] = coord[noe1][i] + (couche * 1.) / (nbc * 1.) * (coord[noe2][i] - coord[noe1][i]);
                }
            }
        }
    }

    // generation du cube central

    if (cyl_creux == 0)
    {
        for (l = 1; l < (nbe / 4); l++)
        {
            for (c = 1; c < (nbe / 4); c++)
            {
                noe = cube[0][l][c];
                noe1 = cube[0][0][c];
                noe2 = cube[0][(nbe / 4)][c];
                for (i = 0; i < 3; i++)
                {
                    coord[noe][i] = coord[noe1][i] + (l * 1.) / ((nbe / 4) * 1.) * (coord[noe2][i] - coord[noe1][i]);
                }
            }
        }
    }

    // mise � jour des positions avec le centre centre[i]

    for (couche = 0; couche < nbc + 1; couche++)
    {
        for (ne = 0; ne < nbe2; ne++)
        {
            nint = tab[0][couche][ne];
            for (i = 0; i < 3; i++)
            {
                coord[nint][i] = coord[nint][i] + centre[i];
            }
        }
    }

    if (cyl_creux == 0)
    {
        for (l = 1; l < (nbe / 4); l++)
        {
            for (c = 1; c < (nbe / 4); c++)
            {
                nint = cube[0][l][c];
                for (i = 0; i < 3; i++)
                {
                    coord[nint][i] = coord[nint][i] + centre[i];
                }
            }
        }
    }

    // extrusion de la face de base

    for (nz = 1; nz < nbz + 1; nz++)
    {
        for (couche = 0; couche < nbc + 1; couche++)
        {
            for (ne = 0; ne < nbe2; ne++)
            {
                noe1 = tab[0][couche][ne];
                noe2 = tab[nz][couche][ne];
                for (i = 0; i < 3; i++)
                {
                    coord[noe2][i] = coord[noe1][i] + (nz * 1.) / (nbz * 1.) * longext * ext[i];
                }
            }
        }
        if (cyl_creux == 0)
        {
            for (l = 1; l < (nbe / 4); l++)
            {
                for (c = 1; c < (nbe / 4); c++)
                {
                    noe1 = cube[0][l][c];
                    noe2 = cube[nz][l][c];
                    for (i = 0; i < 3; i++)
                    {
                        coord[noe2][i] = coord[noe1][i] + (nz * 1.) / (nbz * 1.) * longext * ext[i];
                    }
                }
            }
        }
    }

    // fin du remplissage du tableau des coord.

    //  impression des donn�es du probl�me

    if (cyl_creux == 1)
    {
        fprintf(fp_out, "! Cylindre %2d Creux \n\n", nocyl);
    }
    if (cyl_creux == 0)
    {
        fprintf(fp_out, "! Cylindre %2d Plein \n\n", nocyl);
    }
    fprintf(fp_out, "abrev '/rext' '%15.8E' ! Rayon Exterieur \n", rext);
    if (cyl_creux == 1)
    {
        fprintf(fp_out, "abrev '/rint' '%15.8E' ! Rayon Interieur \n", rint);
    }
    if (cyl_creux == 0)
    {
        fprintf(fp_out, "abrev '/rap' '%15.8E' ! Rapport de la diagonale du carre central au rayon exterieur \n", (rint / rext));
    }

    fprintf(fp_out, "abrev '/xcentre' '%15.8E' ! Coordonnee X du centre \n", centre[0]);
    fprintf(fp_out, "abrev '/ycentre' '%15.8E' ! Coordonnee Y du centre \n", centre[1]);
    fprintf(fp_out, "abrev '/zcentre' '%15.8E' ! Coordonnee Z du centre \n", centre[2]);

    fprintf(fp_out, "abrev '/xnormplan' '%15.8E' ! Coordonnee X de la normale au plan \n", norm[0]);
    fprintf(fp_out, "abrev '/ynormplan' '%15.8E' ! Coordonnee Y de la normale au plan \n", norm[1]);
    fprintf(fp_out, "abrev '/znormplan' '%15.8E' ! Coordonnee Z de la normale au plan \n", norm[2]);

    fprintf(fp_out, "abrev '/xextru' '%15.8E' ! Coordonnee X du vecteur d extrusion \n", ext[0]);
    fprintf(fp_out, "abrev '/yextru' '%15.8E' ! Coordonnee Y du vecteur d extrusion \n", ext[1]);
    fprintf(fp_out, "abrev '/zextru' '%15.8E' ! Coordonnee Z du vecteur d extrusion \n", ext[2]);

    fprintf(fp_out, "abrev '/nbe' '%3d' ! Nombre d'elements sur l arc de cercle \n", nbe);
    fprintf(fp_out, "abrev '/nbc' '%3d' ! Nombre de couches d'�l�ments (hors carre central si plein) \n", nbc);
    fprintf(fp_out, "abrev '/noeini' '%3d' ! Numero du noeud initial - 1\n", noe_ini);
    fprintf(fp_out, "abrev '/maiini' '%3d' ! Numero de la maille initiale - 1\n", maille_ini);
    if (type1 == 0 || type1 == 2)
    {
        fprintf(fp_out, "! Sa surface ext�rieure est la matrice de contact rigide numero %2d \n", mat1rig);
    }
    if (type1 > 0)
    {
        fprintf(fp_out, "! Sa surface ext�rieure est la matrice de contact deformable numero %2d \n", mat1def);
    }
    if (type2 == 0 || type2 == 2)
    {
        fprintf(fp_out, "! Sa surface int�rieure est la matrice de contact rigide numero %2d \n", mat2rig);
    }
    if (type2 > 0)
    {
        fprintf(fp_out, "! Sa surface int�rieure est la matrice de contact deformable numero %2d \n", mat2def);
    }
    fprintf(fp_out, "! Tout ceci pour le cylindre numero %2d  \n", nocyl);
    fprintf(fp_out, " \n");
    fprintf(fp_out, " \n");

    //  impression du .NOE

    fprintf(fp_out, "\n.NOEUD\n");
    noe = noe_ini;

    for (ne = 0; ne < nbe2; ne++)
    {
        for (nz = 0; nz < nbz + 1; nz++)
        {
            nint = tab[nz][0][ne];
            if (liste[nint] == 0)
            {
                noe = noe + 1;
                if (noe % 11111 == 0 || noe == 9999)
                {
                    noe = noe + 1;
                }
                if (noe > 99998)
                {
                    fprintf(fp_out, "Erreur, numero de noeuds trop grand pour BACON\n");
                    return;
                }
                fprintf(fp_out, "I %5d  X %11.4E Y %11.4E Z %11.4E\n",
                        noe, coord[nint][0], coord[nint][1], coord[nint][2]);
                liste[nint] = noe;
            }
        }
    }
    for (ne = 0; ne < nbe2; ne++)
    {
        for (nz = 0; nz < nbz + 1; nz++)
        {
            nint = tab[nz][nbc][ne];
            if (liste[nint] == 0)
            {
                noe = noe + 1;
                if (noe % 11111 == 0 || noe == 9999)
                {
                    noe = noe + 1;
                }
                if (noe > 99998)
                {
                    fprintf(fp_out, "Erreur, numero de noeuds trop grand pour BACON\n");
                    return;
                }
                fprintf(fp_out, "I %5d X %11.4E Y %11.4E Z %11.4E\n",
                        noe, coord[nint][0], coord[nint][1], coord[nint][2]);
                liste[nint] = noe;
            }
        }
    }
    if (nbc > 1)
    {
        for (couche = 1; couche < nbc; couche++)
        {
            for (ne = 0; ne < nbe2; ne++)
            {
                for (nz = 0; nz < nbz + 1; nz++)
                {
                    nint = tab[nz][couche][ne];
                    if (liste[nint] == 0)
                    {
                        noe = noe + 1;
                        if (noe % 11111 == 0 || noe == 9999)
                        {
                            noe = noe + 1;
                        }
                        if (noe > 99998)
                        {
                            fprintf(fp_out, "Erreur, numero de noeuds trop grand pour BACON\n");
                            return;
                        }
                        fprintf(fp_out, "I %5d X %11.4E Y %11.4E Z %11.4E\n",
                                noe, coord[nint][0], coord[nint][1], coord[nint][2]);
                        liste[nint] = noe;
                    }
                }
            }
        }
    }

    if (cyl_creux == 0)
    {
        for (l = 1; l < (nbe / 4); l++)
        {
            for (c = 1; c < (nbe / 4); c++)
            {
                for (nz = 0; nz < nbz + 1; nz++)
                {
                    nint = cube[nz][l][c];
                    if (liste[nint] == 0)
                    {
                        noe = noe + 1;
                        if (noe % 11111 == 0 || noe == 9999)
                        {
                            noe = noe + 1;
                        }
                        if (noe > 99998)
                        {
                            fprintf(fp_out, "Erreur, numero de noeuds trop grand pour BACON\n");
                            return;
                        }
                        fprintf(fp_out, "I %5d X %11.4E Y %11.4E Z %11.4E\n",
                                noe, coord[nint][0], coord[nint][1], coord[nint][2]);
                        liste[nint] = noe;
                    }
                }
            }
        }
    }

    // impression du .MAI

    fprintf(fp_out, "\n.MAI\n");
    maille = maille_ini;

    for (nz = 0; nz < nbz; nz++)
    {
        for (couche = 0; couche < nbc; couche++)
        {
            for (ne = 0; ne < nbe; ne++)
            {
                maille = maille + 1;
                ne2 = ne + 1;
                if (cyl_ouvert == 0 && ne == nbe - 1)
                {
                    ne2 = 0;
                }
                noe1 = tab[nz][couche][ne];
                noe2 = tab[nz][couche][ne2];
                noe3 = tab[nz][couche + 1][ne2];
                noe4 = tab[nz][couche + 1][ne];
                noe5 = tab[nz + 1][couche][ne];
                noe6 = tab[nz + 1][couche][ne2];
                noe7 = tab[nz + 1][couche + 1][ne2];
                noe8 = tab[nz + 1][couche + 1][ne];
                fprintf(fp_out, "I %5d N %4d %4d %4d %4d 0 %4d %4d %4d %4d AT %1d\n", maille,
                        liste[noe1], liste[noe2], liste[noe3], liste[noe4],
                        liste[noe5], liste[noe6], liste[noe7], liste[noe8], nocyl);
            }
        }
    }
    if (cyl_creux == 0)
    {
        for (nz = 0; nz < nbz; nz++)
        {
            for (l = 0; l < (nbe / 4); l++)
            {
                for (c = 0; c < (nbe / 4); c++)
                {
                    maille = maille + 1;
                    noe1 = cube[nz][l][c];
                    noe2 = cube[nz][l][c + 1];
                    noe3 = cube[nz][l + 1][c + 1];
                    noe4 = cube[nz][l + 1][c];
                    noe5 = cube[nz + 1][l][c];
                    noe6 = cube[nz + 1][l][c + 1];
                    noe7 = cube[nz + 1][l + 1][c + 1];
                    noe8 = cube[nz + 1][l + 1][c];
                    fprintf(fp_out, "I %5d N %4d %4d %4d %4d 0 %4d %4d %4d %4d AT %1d\n", maille,
                            liste[noe1], liste[noe2], liste[noe3], liste[noe4],
                            liste[noe5], liste[noe6], liste[noe7], liste[noe8], nocyl);
                }
            }
        }
    }

    //  impression du .MCO

    fprintf(fp_out2, "\n.MCO\n\n");

    //   couche exterieure

    //   matrice rigide

    if (type1 == 0 || type1 == 2)
    {

        for (i = 0; i < 10; i++)
        {
            for (j = 0; j < 2; j++)
            {
                don[i][j] = 0;
            }
        }
        nbnoe = nbe2 * (nbz + 1);
        n1 = noe_ini + 1;
        n2 = n1 + nbnoe - 1;
        i = -1;
        out = 0;
        do
        {
            i = i + 1;
            if ((i == 0 && n1 < 9999) || (i > 0 && n1 < (i * 11111)))
            {
                don[i][0] = n1;
                level1 = i;
                out = 1;
            }
        } while (out == 0 && i < 10);
        out = 0;
        i = i - 1;
        do
        {
            i = i + 1;
            if ((i == 0 && n2 < 9999) || (i > 0 && n2 < (i * 11111)))
            {
                don[i][1] = n2;
                level2 = i;
                out = 1;
            }
            else
            {
                if (i == 0)
                {
                    don[i][1] = 9998;
                    don[i + 1][0] = 10000;
                    n2 = n2 + 1;
                }
                else
                {
                    don[i][1] = (i * 11111) - 1;
                    don[i + 1][0] = (i * 11111) + 1;
                    n2 = n2 + 1;
                }
            }
        } while (out == 0 && i < 10);
        for (i = level1; i <= level2; i++)
        {
            if (don[i][0] != 0)
            {
                fprintf(fp_out2, "I %6d      J %6d    MAT %2d   LOI %2d \n",
                        don[i][0], don[i][1], mat1rig, loi1rig);
            }
        }
        fprintf(fp_out2, "\n");
    }

    //   matrice souple

    if (type1 > 0)
    {
        couche = 0;

        for (ne = 0; ne < nbe2; ne++)
        {
            noe1 = liste[tab[0][0][ne]];
            noe2 = liste[tab[nbz][0][ne]];
            fprintf(fp_out2, "I %6d  J %6d  K 1  MAT %3d  LOI %2d \n", noe1, noe2, mat1def, loi1def);
            if (ne != nbe2 - 1)
            {
                fprintf(fp_out2, "I %8d \n", -3);
            }
            if (cyl_ouvert == 0 && ne == nbe2 - 1)
            {
                fprintf(fp_out2, "I -2 \n");
            }
        }
    }
    fprintf(fp_out2, "\n");

    //   couche interieure

    //   matrice rigide

    if (type2 == 0 || type2 == 2)
    {

        for (i = 0; i < 10; i++)
        {
            for (j = 0; j < 2; j++)
            {
                don[i][j] = 0;
            }
        }
        nbnoe = nbe2 * (nbz + 1);
        n1 = liste[tab[0][nbc][0]];
        n2 = n1 + nbnoe - 1;
        i = -1;
        out = 0;
        do
        {
            i = i + 1;
            if ((i == 0 && n1 < 9999) || (i > 0 && n1 < (i * 11111)))
            {
                don[i][0] = n1;
                level1 = i;
                out = 1;
            }
        } while (out == 0 && i < 10);
        out = 0;
        i = i - 1;
        do
        {
            i = i + 1;
            if ((i == 0 && n2 < 9999) || (i > 0 && n2 < (i * 11111)))
            {
                don[i][1] = n2;
                level2 = i;
                out = 1;
            }
            else
            {
                if (i == 0)
                {
                    don[i][1] = 9998;
                    don[i + 1][0] = 10000;
                    n2 = n2 + 1;
                }
                else
                {
                    don[i][1] = (i * 11111) - 1;
                    don[i + 1][0] = (i * 11111) + 1;
                    n2 = n2 + 1;
                }
            }
        } while (out == 0 && i < 10);
        for (i = level1; i <= level2; i++)
        {
            if (don[i][0] != 0)
            {
                fprintf(fp_out2, "I %6d      J %6d    MAT %2d   LOI %2d \n",
                        don[i][0], don[i][1], mat2rig, loi2rig);
            }
        }
        fprintf(fp_out2, "\n");
    }

    //   matrice souple

    if (type2 > 0)
    {
        for (ne = 0; ne < nbe2; ne++)
        {
            noe1 = liste[tab[0][nbc][ne]];
            noe2 = liste[tab[nbz][nbc][ne]];
            fprintf(fp_out2, "I %6d  J %6d  K -1  MAT %3d  LOI %2d \n", noe2, noe1, mat2def, loi2def);
            if (ne != nbe2 - 1)
            {
                fprintf(fp_out2, "I %8d \n", -3);
            }
            if (cyl_ouvert == 0 && ne == nbe2 - 1)
            {
                fprintf(fp_out2, "I -2 \n");
            }
        }
    }
    //     fprintf(fp_out2,"I -4 \n") ;
    fprintf(fp_out2, "return \n");
    fprintf(fp_out, "return \n");
    //   fprintf(fp_out,".DES\n") ;
    //   fprintf(fp_out,"grap effa \n") ;
    //   fprintf(fp_out,"grap remp 0 \n") ;
    //   fprintf(fp_out,"grap divise cloture 4 \n") ;
    //   fprintf(fp_out,"grap sel clo 1 \n") ;
    //   fprintf(fp_out,"/g vise 1 0 0 \n") ;
    //   fprintf(fp_out,"vi \n") ;
    //   fprintf(fp_out,"grap effa 0 \n") ;
    //   fprintf(fp_out,"/g sel clo 2 \n") ;
    //   fprintf(fp_out,"/g vise 0 1 0 \n") ;
    //   fprintf(fp_out,"vi \n") ;
    //   fprintf(fp_out,"/g sel clo 3 \n") ;
    //   fprintf(fp_out,"/g vise 0 0 1 \n") ;
    //   fprintf(fp_out,"VI\n") ;
    //   fprintf(fp_out,"/g sel clo 4 \n") ;
    //   fprintf(fp_out,"grap vc visee 2 3 1\n") ;
    //   fprintf(fp_out,"grap remp 0 visee 2 3 1\n") ;
    //   fprintf(fp_out,"VI\n") ;
}
