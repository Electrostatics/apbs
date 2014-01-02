/**
 *  @file    coulomb.c
 *  @author  Nathan Baker
 *  @brief   Small program to calculate Coulombic energies
 *  @version $Id$
 */

#include "apbs.h"

/**
 * @author  Nathan Baker
 * @brief   Generalized Born function fGB
 * @param   rij  Distance between atom i and j (in Ang)
 * @param   Ri   Radius of atom i (in Ang)
 * @param   Rj   Radius of atom j (in Ang)
 * returns  Effective distance (in Ang)
 */
#define fGB(rij, Ri, Rj) \
  (VSQRT(VSQR(rij)+(Ri)*(Rj)*VEXP(-0.25*VSQR(rij)/(Ri)/(Rj))))

/**
 * @author  Nathan Baker
 * @brief   Derivative of Generalized Born function fGB with respect to rij
 * @param   rij  Distance between atom i and j (in Ang)
 * @param   Ri   Radius of atom i (in Ang)
 * @param   Rj   Radius of atom j (in Ang)
 * returns  Unitless derivative
 */
#define dfGB_drij(rij, Ri, Rj) \
  (((rij)-0.25*VEXP(-0.25*VSQR(rij)/(Ri)/(Rj)))/fGB(rij,Ri,Rj))

/**
 * @author  Nathan Baker
 * @brief   Derivative of Generalized Born function fGB with respect to Ri
 * @param   rij  Distance between atom i and j (in Ang)
 * @param   Ri   Radius of atom i (in Ang)
 * @param   Rj   Radius of atom j (in Ang)
 * returns  Unitless derivative
 */
#define dfGB_dRi(rij, Ri, Rj) \
 (0.5*(0.25*VSQR(rij)/(Ri)+(Rj))*VEXP(-0.25*VSQR(rij)/(Ri)/(Rj))/fGB(rij,Ri,Rj))

/**
 * @author  Nathan Baker
 * @brief   Derivative of Generalized Born function fGB with respect to Rj
 * @param   rij  Distance between atom i and j (in Ang)
 * @param   Ri   Radius of atom i (in Ang)
 * @param   Rj   Radius of atom j (in Ang)
 * returns  Unitless derivative
 */
#define dfGB_dRj(rij, Ri, Rj) (dfGB_dRi(rij, Rj, Ri))


int main(int argc, char **argv) {

    /* OBJECTS */
    Valist *alist;
    Vatom *atom1, *atom2;
    Vio *sock = VNULL;
    double charge1, charge2, dist, dist2, *pos1, *pos2, energy, myenergy;
    double zmagic, rad1, rad2, eps, disp[3], force[5], myforce[5], dG_drij;
    double dG_dRi, dG_dRj;
    int i, j;
    int verbose = 0;
    int doforce = 1;

    /* SYSTEM PARAMETERS */
    char *path;

    char *usage = "\n\n\
This program calculates electrostatic properties using a generalized\n\
Born equation.\n\
Usage: born [-v] [-f] <epsilon> <molecule.pqr>\n\n\
   where <epsilon> is the unitless solvent dielectric constant and \n\
   <molecule.pqr> is the path to the molecule structure in PQR format.\n\
   This program supports the following options:\n\
       -v      give per-atom information\n\
       -f      calculate forces in addition to energies\n\n";

    Vio_start();

    if ((argc > 5) || (argc < 3)) {
        printf("\n*** Syntax error: got %d arguments, expected 2.\n",argc);
        printf("%s", usage);
        exit(666);
    };
    if (argc > 3) {
        for (i=1; i<(argc-2); i++) {
            if (strcmp("-v", argv[i]) == 0) {
                printf("Providing per-atom information...\n");
                verbose = 1;
            } else if (strcmp("-f", argv[i]) == 0) {
                printf("Calculating forces...\n");
                doforce = 1;
            } else {
                printf("Ignoring option %s\n", argv[i]);
                verbose = 0;
            }
        }
        path = argv[argc-1];
        sscanf(argv[argc-2], "%lf", &eps);
    } else {
        verbose = 0;
        path = argv[2];
        sscanf(argv[1], "%lf", &eps);
    }

    printf("Setting up atom list from %s\n", path);
    alist = Valist_ctor();
    sock = Vio_ctor("FILE", "ASC", VNULL, path, "r");
    if (sock == VNULL) {
        Vnm_print(2, "Problem opening virtual socket %s!\n",
                  path);
        return 0;
    }
    if (Vio_accept(sock, 0) < 0) {
        Vnm_print(2, "Problem accepting virtual socket %s!\n",
                  path);
        return 0;
    }
    Valist_readPQR(alist,VNULL,sock);
    printf("Read %d atoms\n", Valist_getNumberAtoms(alist));

    /* Energy scaling factor */
    zmagic  = (1e-3)*(1e10)*Vunit_ec*Vunit_ec*Vunit_Na/(4*VPI*Vunit_eps0);

    energy = 0.0;
    force[0] = force[1] = force[2] = force[3] = force[4] = 0.0;

    printf("Using solvent diel %g, distance in Ang, and charge in e....\n",
      eps);
    printf("Calculating...\n");
    fflush(stdout);
    for (i=0; i<Valist_getNumberAtoms(alist); i++) {
        atom1 = Valist_getAtom(alist, i);
        pos1 = Vatom_getPosition(atom1);
        charge1 = Vatom_getCharge(atom1);
        rad1 = Vatom_getRadius(atom1);
        myenergy = -0.5*VSQR(charge1)/VSQR(rad1);
        myforce[0] = myforce[1] = myforce[2] = myforce[3] = myforce[4] = 0.0;
        for (j=0; j<Valist_getNumberAtoms(alist); j++) {
            if (j != i) {
                atom2 = Valist_getAtom(alist, j);
                pos2 = Vatom_getPosition(atom2);
                rad2 = Vatom_getRadius(atom2);
                charge2 = Vatom_getCharge(atom2);
                disp[0] = pos1[0] - pos2[0];
                disp[1] = pos1[1] - pos2[1];
                disp[2] = pos1[2] - pos2[2];
                dist2 = (VSQR(disp[0]) + VSQR(disp[1]) + VSQR(disp[2]));
                dist = VSQRT(dist2);
                dG_drij = 0.5*charge1*charge2*dfGB_drij(dist,rad1,rad2) /
                  VSQR(fGB(dist, rad1, rad2));
                dG_dRi = 0.5*charge1*charge2*dfGB_dRi(dist,rad1,rad2) /
                  VSQR(fGB(dist, rad1, rad2));
                dG_dRj = 0.5*charge1*charge2*dfGB_dRj(dist,rad1,rad2) /
                  VSQR(fGB(dist, rad1, rad2));
                myforce[0] += -disp[0]*dG_drij/(dist*dist2);
                myforce[1] += -disp[1]*dG_drij/(dist*dist2);
                myforce[2] += -disp[2]*dG_drij/(dist*dist2);
                myforce[3] += dG_dRi;
                myforce[4] += dG_dRj;
                myenergy -= 0.5*(charge1*charge2/fGB(dist, rad1, rad2));
            }
        }
        myenergy = myenergy*(1-1/eps);
        myforce[0] = myforce[0]*(1-1/eps);
        myforce[1] = myforce[1]*(1-1/eps);
        myforce[2] = myforce[2]*(1-1/eps);
        myforce[3] = myforce[3]*(1-1/eps);
        myforce[4] = myforce[4]*(1-1/eps);
        energy += myenergy;
        force[0] += myforce[0];
        force[1] += myforce[1];
        force[2] += myforce[2];
        force[3] += myforce[3];
        force[4] += myforce[4];
        myenergy = myenergy*zmagic;
        myforce[0] = myforce[0]*zmagic;
        myforce[1] = myforce[1]*zmagic;
        myforce[2] = myforce[2]*zmagic;
        myforce[3] = myforce[3]*zmagic;
        myforce[4] = myforce[4]*zmagic;
        if (verbose) {
            printf("\tAtom %d:  Energy  = %1.12E kJ/mol\n", i, myenergy);
            if (doforce) {
                printf("\tAtom %d:  x-force = %1.12E kJ/mol/A\n", i,
                  myforce[0]);
                printf("\tAtom %d:  y-force = %1.12E kJ/mol/A\n", i,
                  myforce[1]);
                printf("\tAtom %d:  z-force = %1.12E kJ/mol/A\n", i,
                  myforce[2]);
                printf("\tAtom %d:  Ri-force = %1.12E kJ/mol/A\n", i,
                  myforce[3]);
                printf("\tAtom %d:  Rj-force = %1.12E kJ/mol/A\n", i,
                  myforce[4]);
            }
        }
    }

    energy = energy*zmagic;

    printf("\n\n-------------------------------------------------------\n");
    printf("GB solvation energy = %1.12e kJ/mol.\n", energy);
    printf("GB solvation x-force = %1.12e kJ/mol.\n", force[0]);
    printf("GB solvation y-force = %1.12e kJ/mol.\n", force[1]);
    printf("GB solvation z-force = %1.12e kJ/mol.\n", force[2]);
    printf("GB solvation Ri-force = %1.12e kJ/mol.\n", force[3]);
    printf("GB solvation Rj-force = %1.12e kJ/mol.\n", force[4]);

    return 0;
}
