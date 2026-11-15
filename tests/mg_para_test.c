/**
 * @file    mg_para_test.c
 * @brief   Unit tests for non-MPI mg-para partition expansion and foldParaPartitions
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apbscfg.h"
#include "generic/mgparm.h"
#include "generic/nosh.h"
#include "generic/pbeparm.h"
#include "generic/valist.h"
#include "generic/vatom.h"
#include "generic/vhal.h"
#include "maloc/maloc.h"
#include "routines.h"

static int test_fold_para_partitions(void) {
    NOsh *nosh;
    double totEnergy[NOSH_MAXCALC];
    double qfEnergy[NOSH_MAXCALC];
    double qmEnergy[NOSH_MAXCALC];
    double dielEnergy[NOSH_MAXCALC];
    int nenergy[NOSH_MAXCALC];
    double *atomEnergy[NOSH_MAXCALC];
    int nforce[NOSH_MAXCALC];
    AtomForce *atomForce[NOSH_MAXCALC];
    double ae_buf[9][2];
    AtomForce af_buf[9][2];
    int p, lev, idx, k;

    memset(totEnergy, 0, sizeof(totEnergy));
    memset(qfEnergy, 0, sizeof(qfEnergy));
    memset(qmEnergy, 0, sizeof(qmEnergy));
    memset(dielEnergy, 0, sizeof(dielEnergy));
    memset(nenergy, 0, sizeof(nenergy));
    memset(atomEnergy, 0, sizeof(atomEnergy));
    memset(nforce, 0, sizeof(nforce));
    memset(atomForce, 0, sizeof(atomForce));
    memset(ae_buf, 0, sizeof(ae_buf));
    memset(af_buf, 0, sizeof(af_buf));

    nosh = NOsh_ctor(0, 1);
    nosh->nelec = 2;
    nosh->ncalc = 9;
    nosh->elec2calc[0] = 7; /* 4 partitions x 2 levels = calcs 0..7 */
    nosh->elec2calc[1] = 8; /* trailing mg-auto = calc 8 */

    for (p = 0; p < 4; p++) {
        for (lev = 0; lev < 2; lev++) {
            idx = 2 * p + lev;
            nosh->calc[idx] = NOsh_calc_ctor(NCT_MG);
            nosh->calc[idx]->mgparm->partRank = p;
            nosh->calc[idx]->pbeparm->calcenergy = PCE_COMPS;
            nosh->calc[idx]->pbeparm->calcforce = PCF_COMPS;
            /* Coarse levels hold bogus large numbers that MUST NOT be summed */
            totEnergy[idx] = (lev == 0) ? 999.0 : (10.0 * (double)(p + 1));
            qfEnergy[idx] = (lev == 0) ? 888.0 : (1.0 * (double)(p + 1));
            qmEnergy[idx] = (lev == 0) ? 777.0 : (2.0 * (double)(p + 1));
            dielEnergy[idx] = (lev == 0) ? 666.0 : (3.0 * (double)(p + 1));
            nenergy[idx] = 2;
            atomEnergy[idx] = ae_buf[idx];
            ae_buf[idx][0] = (lev == 0) ? 555.0 : (0.5 * (double)(p + 1));
            ae_buf[idx][1] = (lev == 0) ? 444.0 : (1.5 * (double)(p + 1));
            nforce[idx] = 2;
            atomForce[idx] = af_buf[idx];
            for (k = 0; k < 2; k++) {
                af_buf[idx][k].qfForce[0] = (lev == 0) ? 111.0 : (double)(p + 1 + k);
                af_buf[idx][k].ibForce[1] = (lev == 0) ? 222.0 : (2.0 * (double)(p + 1));
                af_buf[idx][k].dbForce[2] = (lev == 0) ? 333.0 : (-0.25 * (double)(p + 1));
            }
        }
    }

    nosh->calc[8] = NOsh_calc_ctor(NCT_MG);
    nosh->calc[8]->mgparm->partRank = -1;
    nosh->calc[8]->pbeparm->calcenergy = PCE_TOTAL;
    totEnergy[8] = -42.5;

    foldParaPartitions(nosh, totEnergy, qfEnergy, qmEnergy, dielEnergy,
                       nenergy, atomEnergy, nforce, atomForce);

    /* Sum of (p+1) for p=0..3 is 10 */
    if (fabs(totEnergy[7] - 100.0) > 1e-15 ||
        fabs(qfEnergy[7] - 10.0) > 1e-15 ||
        fabs(qmEnergy[7] - 20.0) > 1e-15 ||
        fabs(dielEnergy[7] - 30.0) > 1e-15 ||
        fabs(atomEnergy[7][0] - 5.0) > 1e-15 ||
        fabs(atomEnergy[7][1] - 15.0) > 1e-15 ||
        fabs(atomForce[7][0].qfForce[0] - 10.0) > 1e-15 ||
        fabs(atomForce[7][1].qfForce[0] - 14.0) > 1e-15 ||
        fabs(atomForce[7][0].ibForce[1] - 20.0) > 1e-15 ||
        fabs(atomForce[7][0].dbForce[2] - (-2.5)) > 1e-15 ||
        fabs(totEnergy[8] - (-42.5)) > 1e-15) {
        fprintf(stderr, "FAIL: foldParaPartitions observable mismatch\n");
        NOsh_dtor(&nosh);
        return 0;
    }

    NOsh_dtor(&nosh);
    return 1;
}

#ifndef HAVE_MPI_H
static int test_nosh_setup_mgpara_shared(void) {
    const char *deck =
        "read\n"
        "    mol pqr test_ion.pqr\n"
        "end\n"
        "elec name para_blk\n"
        "    mg-para\n"
        "    dime 33 33 33\n"
        "    pdime 2 2 1\n"
        "    ofrac 0.1\n"
        "    cglen 20.0 20.0 20.0\n"
        "    fglen 10.0 10.0 10.0\n"
        "    cgcent mol 1\n"
        "    fgcent mol 1\n"
        "    mol 1\n"
        "    lpbe\n"
        "    bcfl sdh\n"
        "    pdie 2.0\n"
        "    sdie 78.54\n"
        "    srfm mol\n"
        "    chgm spl2\n"
        "    sdens 10.0\n"
        "    srad 1.4\n"
        "    temp 298.15\n"
        "    calcenergy total\n"
        "    calcforce no\n"
        "end\n"
        "elec name auto_blk\n"
        "    mg-auto\n"
        "    dime 33 33 33\n"
        "    cglen 20.0 20.0 20.0\n"
        "    fglen 10.0 10.0 10.0\n"
        "    cgcent mol 1\n"
        "    fgcent mol 1\n"
        "    mol 1\n"
        "    lpbe\n"
        "    bcfl sdh\n"
        "    pdie 2.0\n"
        "    sdie 78.54\n"
        "    srfm mol\n"
        "    chgm spl2\n"
        "    sdens 10.0\n"
        "    srad 1.4\n"
        "    temp 298.15\n"
        "    calcenergy total\n"
        "    calcforce no\n"
        "end\n"
        "quit\n";
    char in_file[] = "mg_para_test_deck.in";
    FILE *fp;
    NOsh *nosh;
    Valist *alist[NOSH_MAXMOL];
    Vatom *atom;
    int p;

    memset(alist, 0, sizeof(alist));
    fp = fopen(in_file, "wb");
    VASSERT(fp != NULL);
    fwrite(deck, 1, strlen(deck), fp);
    fclose(fp);

    nosh = NOsh_ctor(0, 1);
    VASSERT(NOsh_parseInputFile(nosh, in_file) == 1);
    remove(in_file);

    alist[0] = Valist_ctor();
    alist[0]->number = 1;
    alist[0]->atoms = (Vatom *)Vmem_malloc(alist[0]->vmem, 1, sizeof(Vatom));
    atom = &(alist[0]->atoms[0]);
    atom->position[0] = 0.0;
    atom->position[1] = 0.0;
    atom->position[2] = 0.0;
    Vatom_setRadius(atom, 2.0);
    Vatom_setCharge(atom, 1.0);
    Vatom_setAtomID(atom, 0);
    alist[0]->maxrad = 2.0;
    alist[0]->charge = 1.0;

    if (NOsh_setupElecCalc(nosh, alist) != 1) {
        fprintf(stderr, "FAIL: NOsh_setupElecCalc failed on non-MPI mg-para\n");
        Valist_dtor(&(alist[0]));
        NOsh_dtor(&nosh);
        return 0;
    }

    /* 4 partitions x 2 levels = 8 calcs, plus 2 levels for auto_blk = 10 calcs */
    if (nosh->ncalc != 10 || nosh->ispara != 0 || nosh->proc_rank != 0) {
        fprintf(stderr, "FAIL: unexpected ncalc=%d ispara=%d proc_rank=%d\n",
                nosh->ncalc, nosh->ispara, nosh->proc_rank);
        Valist_dtor(&(alist[0]));
        NOsh_dtor(&nosh);
        return 0;
    }
    for (p = 0; p < 4; p++) {
        if (nosh->calc[2 * p]->mgparm->partRank != p ||
            nosh->calc[2 * p + 1]->mgparm->partRank != p) {
            fprintf(stderr, "FAIL: partRank mismatch at partition %d\n", p);
            Valist_dtor(&(alist[0]));
            NOsh_dtor(&nosh);
            return 0;
        }
    }
    if (nosh->calc[8]->mgparm->partRank != -1 ||
        nosh->calc[9]->mgparm->partRank != -1) {
        fprintf(stderr, "FAIL: partRank leaked into subsequent mg-auto block\n");
        Valist_dtor(&(alist[0]));
        NOsh_dtor(&nosh);
        return 0;
    }

    Valist_dtor(&(alist[0]));
    NOsh_dtor(&nosh);
    return 1;
}
#endif

int main(void) {
    Vio_start();
    Vnm_redirect(1);
    if (!test_fold_para_partitions()) { Vio_stop(); return 1; }
#ifndef HAVE_MPI_H
    if (!test_nosh_setup_mgpara_shared()) { Vio_stop(); return 1; }
#endif
    Vio_stop();
    remove("io.mc");
    printf("mg_para_test: ALL PASSED\n");
    return 0;
}
