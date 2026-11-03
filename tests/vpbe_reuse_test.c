/**
 * @file    vpbe_reuse_test.c
 * @brief   Unit tests for Vpbe_ctorReuse Vclist/Vacc ownership transfer
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "apbscfg.h"
#include "generic/vacc.h"
#include "generic/valist.h"
#include "generic/vatom.h"
#include "generic/vclist.h"
#include "generic/vhal.h"
#include "generic/vpbe.h"
#include "maloc/maloc.h"

static Valist* make_test_alist(int natoms, double offset) {
    Valist *alist;
    Vatom *a;
    int i, d;

    alist = Valist_ctor();
    VASSERT(alist != VNULL);
    alist->number = natoms;
    alist->atoms = (Vatom *)Vmem_malloc(alist->vmem, (size_t)natoms,
                                        sizeof(Vatom));
    VASSERT(alist->atoms != VNULL);

    for (d = 0; d < 3; d++) {
        alist->mincrd[d] = VLARGE;
        alist->maxcrd[d] = -VLARGE;
    }
    alist->maxrad = 0.0;
    alist->charge = 0.0;

    for (i = 0; i < natoms; i++) {
        a = &(alist->atoms[i]);
        a->position[0] = offset + 1.7 * (double)i;
        a->position[1] = -0.9 * (double)i;
        a->position[2] = 0.5 * (double)i;
        Vatom_setRadius(a, 1.5 + 0.1 * (double)(i % 3));
        Vatom_setCharge(a, (i % 2 == 0) ? 0.5 : -0.5);
        Vatom_setPartID(a, -1);
        Vatom_setEpsilon(a, 0.0);
        Vatom_setAtomID(a, i);
        a->resName[0] = '\0';
        a->atomName[0] = '\0';

        for (d = 0; d < 3; d++) {
            if (a->position[d] < alist->mincrd[d]) {
                alist->mincrd[d] = a->position[d];
            }
            if (a->position[d] > alist->maxcrd[d]) {
                alist->maxcrd[d] = a->position[d];
            }
        }
        if (a->radius > alist->maxrad) {
            alist->maxrad = a->radius;
        }
    }

    for (d = 0; d < 3; d++) {
        alist->center[d] = 0.5 * (alist->mincrd[d] + alist->maxcrd[d]);
    }
    return alist;
}

static int test_vpbe_ctor_reuse(void) {
    Valist *a1;
    Valist *a2;
    double ionc[2] = {0.15, 0.15};
    double ionr[2] = {2.0, 2.0};
    double ionr_alt[2] = {2.5, 2.5};
    double ionq[2] = {1.0, -1.0};
    Vpbe *p1;
    Vpbe *p2;
    Vpbe *p3;
    Vclist *orig_clist;
    Vacc *orig_acc;
    VaccSurf **orig_surf_ptr;
    int npts[3] = {5, 5, 5};
    double lo[3] = {-5.0, -5.0, -5.0};
    double hi[3] = {5.0, 5.0, 5.0};
    Vclist *manual_clist;
    double sasa1;
    double sasa2;

    a1 = make_test_alist(6, 0.0);
    a2 = make_test_alist(6, 2.0);

    /* 1. Vpbe_ctor leaves existing behavior unchanged even for focusFlag=2 */
    p1 = Vpbe_ctor(a1, 2, ionc, ionr, ionq, 298.15, 2.0, 78.54,
                   1.4, 2, 10.0, 0.0, 0.0, 2.0, 0.0);
    if (p1 == VNULL || p1->clist == VNULL || p1->acc == VNULL) {
        fprintf(stderr, "FAIL: Vpbe_ctor returned NULL clist/acc\n");
        return 0;
    }

    sasa1 = Vacc_SASA(p1->acc, 1.4);
    orig_clist = p1->clist;
    orig_acc = p1->acc;
    orig_surf_ptr = p1->acc->surf;
    if (orig_surf_ptr == VNULL || sasa1 <= 0.0) {
        fprintf(stderr, "FAIL: Vacc_SASA did not populate p1->acc->surf\n");
        return 0;
    }

    /* 2. Compatible reuse transfers clist & acc and clears source pointers */
    p2 = Vpbe_ctorReuse(a1, 2, ionc, ionr, ionq, 298.15, 2.0, 2.0,
                        1.4, 1, 10.0, 0.0, 0.0, 2.0, 0.0, p1);
    if (p2->clist != orig_clist || p2->acc != orig_acc
        || p2->acc->surf != orig_surf_ptr) {
        fprintf(stderr, "FAIL: Vpbe_ctorReuse did not transfer clist/acc/surf\n");
        return 0;
    }
    if (p1->clist != VNULL || p1->acc != VNULL) {
        fprintf(stderr, "FAIL: Vpbe_ctorReuse did not clear reuseFrom pointers\n");
        return 0;
    }
    Vpbe_dtor(&p1);
    sasa2 = Vacc_SASA(p2->acc, 1.4);
    if (fabs(sasa2 - sasa1) > 1e-12) {
        fprintf(stderr, "FAIL: transferred Vacc_SASA changed after Vpbe_dtor\n");
        return 0;
    }

    /* 3. Different Valist builds fresh clist/acc and leaves p2 intact */
    p3 = Vpbe_ctorReuse(a2, 2, ionc, ionr, ionq, 298.15, 2.0, 78.54,
                        1.4, 1, 10.0, 0.0, 0.0, 2.0, 0.0, p2);
    if (p3->clist == orig_clist || p3->acc == orig_acc
        || p2->clist != orig_clist || p2->acc != orig_acc) {
        fprintf(stderr, "FAIL: mismatched Valist should not reuse\n");
        return 0;
    }
    Vpbe_dtor(&p3);

    /* 4. Different solvent probe radius (srad) builds fresh clist/acc */
    p3 = Vpbe_ctorReuse(a1, 2, ionc, ionr, ionq, 298.15, 2.0, 78.54,
                        1.8, 1, 10.0, 0.0, 0.0, 2.0, 0.0, p2);
    if (p3->clist == orig_clist || p3->acc == orig_acc
        || p2->clist != orig_clist) {
        fprintf(stderr, "FAIL: mismatched srad should not reuse\n");
        return 0;
    }
    Vpbe_dtor(&p3);

    /* 5. Different maxIonRadius builds fresh clist/acc */
    p3 = Vpbe_ctorReuse(a1, 2, ionc, ionr_alt, ionq, 298.15, 2.0, 78.54,
                        1.4, 1, 10.0, 0.0, 0.0, 2.0, 0.0, p2);
    if (p3->clist == orig_clist || p3->acc == orig_acc
        || p2->clist != orig_clist) {
        fprintf(stderr, "FAIL: mismatched maxIonRadius should not reuse\n");
        return 0;
    }
    Vpbe_dtor(&p3);

    /* 6. Different surface density (sdens) builds fresh clist/acc */
    p3 = Vpbe_ctorReuse(a1, 2, ionc, ionr, ionq, 298.15, 2.0, 78.54,
                        1.4, 1, 20.0, 0.0, 0.0, 2.0, 0.0, p2);
    if (p3->clist == orig_clist || p3->acc == orig_acc
        || p2->clist != orig_clist) {
        fprintf(stderr, "FAIL: mismatched sdens should not reuse\n");
        return 0;
    }
    Vpbe_dtor(&p3);

    /* 7. CLIST_MANUAL_DOMAIN is never reused */
    manual_clist = Vclist_ctor(a1, 2.5, npts, CLIST_MANUAL_DOMAIN, lo, hi);
    Vclist_dtor(&(p2->clist));
    p2->clist = manual_clist;
    p2->acc->clist = manual_clist;
    p3 = Vpbe_ctorReuse(a1, 2, ionc, ionr, ionq, 298.15, 2.0, 78.54,
                        1.4, 1, 10.0, 0.0, 0.0, 2.0, 0.0, p2);
    if (p3->clist == manual_clist || p2->clist != manual_clist) {
        fprintf(stderr, "FAIL: CLIST_MANUAL_DOMAIN should not reuse\n");
        return 0;
    }
    Vpbe_dtor(&p3);
    Vpbe_dtor(&p2);
    Valist_dtor(&a1);
    Valist_dtor(&a2);
    return 1;
}

int main(void) {
    if (!test_vpbe_ctor_reuse()) {
        return 1;
    }
    printf("vpbe_reuse_test: all tests passed.\n");
    return 0;
}
