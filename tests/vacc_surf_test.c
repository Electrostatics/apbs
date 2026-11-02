/**
 * @file    vacc_surf_test.c
 * @brief   Unit and regression tests for Vacc_atomSurf neighbor caching
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
#include "maloc/maloc.h"

typedef struct sAtomSpec {
    double x;
    double y;
    double z;
    double radius;
} AtomSpec;

static Valist* build_alist(const AtomSpec *specs, int natoms) {
    Valist *alist;
    Vatom *atom;
    int i, d;
    double c;

    alist = Valist_ctor();
    VASSERT(alist != VNULL);
    alist->number = natoms;
    alist->atoms = (Vatom *)Vmem_malloc(alist->vmem, (size_t)natoms,
                                        sizeof(Vatom));
    VASSERT(alist->atoms != VNULL);

    for (d = 0; d < 3; d++) {
        alist->mincrd[d] = VLARGE;
        alist->maxcrd[d] = -VLARGE;
        alist->center[d] = 0.0;
    }
    alist->maxrad = 0.0;
    alist->charge = 0.0;

    for (i = 0; i < natoms; i++) {
        atom = &(alist->atoms[i]);
        atom->position[0] = specs[i].x;
        atom->position[1] = specs[i].y;
        atom->position[2] = specs[i].z;
        Vatom_setRadius(atom, specs[i].radius);
        Vatom_setCharge(atom, 0.0);
        Vatom_setPartID(atom, -1);
        Vatom_setEpsilon(atom, 0.0);
        Vatom_setAtomID(atom, i);
        atom->resName[0] = '\0';
        atom->atomName[0] = '\0';

        for (d = 0; d < 3; d++) {
            c = atom->position[d];
            if (c < alist->mincrd[d]) alist->mincrd[d] = c;
            if (c > alist->maxcrd[d]) alist->maxcrd[d] = c;
        }
        if (specs[i].radius > alist->maxrad) {
            alist->maxrad = specs[i].radius;
        }
    }

    for (d = 0; d < 3; d++) {
        alist->center[d] = 0.5 * (alist->mincrd[d] + alist->maxcrd[d]);
    }
    return alist;
}

static VaccSurf* make_custom_ref(Vmem *mem, const double dirs[][3], int npts) {
    VaccSurf *ref;
    int i;
    double nrm;

    ref = VaccSurf_ctor(mem, 0.0, npts);
    VASSERT(ref != VNULL);
    for (i = 0; i < npts; i++) {
        nrm = sqrt(dirs[i][0] * dirs[i][0]
                   + dirs[i][1] * dirs[i][1]
                   + dirs[i][2] * dirs[i][2]);
        VASSERT(nrm > 0.0);
        ref->xpts[i] = dirs[i][0] / nrm;
        ref->ypts[i] = dirs[i][1] / nrm;
        ref->zpts[i] = dirs[i][2] / nrm;
        ref->bpts[i] = 0;
    }
    return ref;
}

static int oracle_point_accessible(Vclist *clist, double pos[3],
                                   double prad, int exclude_id) {
    VclistCell *cell;
    Vatom *atom;
    double *apos;
    double dist2;
    int iatom;

    cell = Vclist_getCell(clist, pos);
    if (cell == VNULL) {
        return 1;
    }
    for (iatom = 0; iatom < cell->natoms; iatom++) {
        atom = cell->atoms[iatom];
        if (atom->id == exclude_id) continue;
        apos = atom->position;
        dist2 = VSQR(pos[0] - apos[0])
              + VSQR(pos[1] - apos[1])
              + VSQR(pos[2] - apos[2]);
        if (dist2 < VSQR(atom->radius + prad)) {
            return 0;
        }
    }
    return 1;
}

static int verify_surf_against_oracle(const char *label, Vacc *acc,
                                      Vclist *clist, Vatom *atom,
                                      VaccSurf *ref, double prad,
                                      int *out_npts,
                                      int *expected_flags_opt) {
    VaccSurf *surf;
    char *orig_bpts;
    double *apos;
    double rad, pos[3], expected_area;
    int i, exp_npts, got_flag, idx;

    orig_bpts = VNULL;
    if (ref->npts > 0) {
        orig_bpts = (char *)malloc((size_t)ref->npts);
        VASSERT(orig_bpts != VNULL);
        for (i = 0; i < ref->npts; i++) {
            orig_bpts[i] = ref->bpts[i];
        }
    }

    surf = Vacc_atomSurf(acc, atom, ref, prad);
    if (surf == VNULL) {
        free(orig_bpts);
        fprintf(stderr, "FAIL [%s]: Vacc_atomSurf returned NULL\n", label);
        return 0;
    }

    for (i = 0; i < ref->npts; i++) {
        if (ref->bpts[i] != orig_bpts[i]) {
            fprintf(stderr,
                    "FAIL [%s]: ref->bpts[%d] = %d (expected %d)\n",
                    label, i, (int)ref->bpts[i], (int)orig_bpts[i]);
            free(orig_bpts);
            VaccSurf_dtor(&surf);
            return 0;
        }
    }
    free(orig_bpts);

    apos = Vatom_getPosition(atom);
    rad = Vatom_getRadius(atom) + prad;
    exp_npts = 0;
    idx = 0;

    for (i = 0; i < ref->npts; i++) {
        pos[0] = rad * ref->xpts[i] + apos[0];
        pos[1] = rad * ref->ypts[i] + apos[1];
        pos[2] = rad * ref->zpts[i] + apos[2];
        got_flag = oracle_point_accessible(clist, pos, prad, atom->id);
        if (expected_flags_opt != VNULL && got_flag != expected_flags_opt[i]) {
            fprintf(stderr,
                    "FAIL [%s]: test geometry sample %d oracle=%d != expected=%d\n",
                    label, i, got_flag, expected_flags_opt[i]);
            VaccSurf_dtor(&surf);
            return 0;
        }
        if (got_flag) {
            if (idx >= surf->npts) {
                fprintf(stderr,
                        "FAIL [%s]: surf->npts=%d smaller than oracle (>%d) at sample %d\n",
                        label, surf->npts, idx, i);
                VaccSurf_dtor(&surf);
                return 0;
            }
            if (surf->bpts[idx] != 1
                || fabs(surf->xpts[idx] - pos[0]) > 1e-12
                || fabs(surf->ypts[idx] - pos[1]) > 1e-12
                || fabs(surf->zpts[idx] - pos[2]) > 1e-12) {
                fprintf(stderr,
                        "FAIL [%s]: point mismatch at idx=%d (sample %d)\n",
                        label, idx, i);
                VaccSurf_dtor(&surf);
                return 0;
            }
            idx++;
            exp_npts++;
        }
    }

    if (surf->npts != exp_npts) {
        fprintf(stderr, "FAIL [%s]: surf->npts=%d != oracle=%d\n",
                label, surf->npts, exp_npts);
        VaccSurf_dtor(&surf);
        return 0;
    }

    expected_area = 4.0 * VPI * rad * rad
                  * ((double)exp_npts) / ((double)ref->npts);
    if (fabs(surf->area - expected_area) > 1e-12) {
        fprintf(stderr, "FAIL [%s]: surf->area=%.15g != expected=%.15g\n",
                label, surf->area, expected_area);
        VaccSurf_dtor(&surf);
        return 0;
    }

    if (out_npts != VNULL) {
        *out_npts = surf->npts;
    }
    VaccSurf_dtor(&surf);
    return 1;
}

/* Case 1 & 2: consecutive cached hits and fallback after a cache miss */
static int test_cached_hits_and_miss_fallback(void) {
    static const AtomSpec specs[3] = {
        {0.0, 0.0, 0.0, 2.0},
        {3.0, 0.0, 0.0, 2.0},
        {0.0, 3.0, 0.0, 2.0}
    };
    /*
     * Probe radius 1.4 => Atom 0 sample radius 3.4, neighbor cutoff^2 = 11.56:
     *  0: (+1, 0, 0)       -> buried by Atom 1 (populates last_hit = Atom 1)
     *  1: (0.96, 0.28, 0)  -> consecutive cached hit on Atom 1
     *  2: (0.80, 0.60, 0)  -> second consecutive cached hit on Atom 1
     *  3: (-1, 0, 0)       -> cache miss on Atom 1, cell scan -> accessible (1)
     *  4: (0, +1, 0)       -> cache miss on Atom 1, cell scan -> buried by Atom 2 (0)
     *  5: (0, 0, +1)       -> cache miss on Atom 2, cell scan -> accessible (1)
     */
    static const double dirs[6][3] = {
        { 1.0,  0.0,  0.0},
        { 0.96, 0.28, 0.0},
        { 0.80, 0.60, 0.0},
        {-1.0,  0.0,  0.0},
        { 0.0,  1.0,  0.0},
        { 0.0,  0.0,  1.0}
    };
    int expected_flags[6] = {0, 0, 0, 1, 0, 1};
    int npts_grid[3] = {7, 7, 7};
    double dummy[3] = {0.0, 0.0, 0.0};
    Valist *alist;
    Vclist *clist;
    Vacc *acc;
    VaccSurf *ref;
    int got_npts, ok;

    alist = build_alist(specs, 3);
    clist = Vclist_ctor(alist, 1.4, npts_grid, CLIST_AUTO_DOMAIN, dummy, dummy);
    acc = Vacc_ctor(alist, clist, 5.0);
    ref = make_custom_ref(acc->mem, dirs, 6);

    got_npts = -1;
    ok = verify_surf_against_oracle("auto_cached_hits_and_miss_fallback",
                                    acc, clist, Valist_getAtom(alist, 0),
                                    ref, 1.4, &got_npts, expected_flags);
    if (ok && got_npts != 2) {
        fprintf(stderr, "FAIL [auto_cached_hits_and_miss_fallback]: npts=%d != 2\n",
                got_npts);
        ok = 0;
    }

    VaccSurf_dtor(&ref);
    Vacc_dtor(&acc);
    Vclist_dtor(&clist);
    Valist_dtor(&alist);
    return ok;
}

/* Case 3: switching across multiple occluding neighbors + full refSphere */
static int test_occluder_switching_and_ref_sphere(void) {
    static const AtomSpec specs[5] = {
        { 0.0,  0.0, 0.0, 2.0},
        { 3.0,  0.0, 0.0, 1.8},
        {-3.0,  0.0, 0.0, 1.8},
        { 0.0,  3.0, 0.0, 1.8},
        { 0.0, -3.0, 0.0, 1.8}
    };
    static const double dirs[10][3] = {
        { 1.0,  0.0,  0.0},
        { 0.0,  1.0,  0.0},
        {-1.0,  0.0,  0.0},
        { 0.0, -1.0,  0.0},
        { 0.0,  0.0,  1.0},
        {-1.0,  0.0,  0.0},
        { 1.0,  0.0,  0.0},
        { 0.0,  0.0, -1.0},
        { 0.0, -1.0,  0.0},
        { 0.0,  1.0,  0.0}
    };
    int expected_flags[10] = {0, 0, 0, 0, 1, 0, 0, 1, 0, 0};
    int npts_grid[3] = {9, 9, 9};
    double dummy[3] = {0.0, 0.0, 0.0};
    Valist *alist;
    Vclist *clist;
    Vacc *acc;
    VaccSurf *custom_ref, *sphere_ref;
    int got_npts, i, ok;
    char label[64];

    alist = build_alist(specs, 5);
    clist = Vclist_ctor(alist, 1.4, npts_grid, CLIST_AUTO_DOMAIN, dummy, dummy);
    acc = Vacc_ctor(alist, clist, 10.0);
    custom_ref = make_custom_ref(acc->mem, dirs, 10);

    got_npts = -1;
    ok = verify_surf_against_oracle("auto_occluder_switching",
                                    acc, clist, Valist_getAtom(alist, 0),
                                    custom_ref, 1.4, &got_npts, expected_flags);
    if (ok && got_npts != 2) {
        fprintf(stderr, "FAIL [auto_occluder_switching]: npts=%d != 2\n",
                got_npts);
        ok = 0;
    }

    sphere_ref = VaccSurf_refSphere(acc->mem, 256);
    for (i = 0; i < 5 && ok; i++) {
        sprintf(label, "auto_ref_sphere_atom_%d", i);
        ok = verify_surf_against_oracle(label, acc, clist,
                                        Valist_getAtom(alist, i),
                                        sphere_ref, 1.4, VNULL, VNULL);
    }

    VaccSurf_dtor(&sphere_ref);
    VaccSurf_dtor(&custom_ref);
    Vacc_dtor(&acc);
    Vclist_dtor(&clist);
    Valist_dtor(&alist);
    return ok;
}

/* Case 4: CLIST_MANUAL_DOMAIN out-of-domain sample overlapping cached neighbor */
static int test_manual_domain_out_of_bounds(void) {
    static const AtomSpec specs[2] = {
        {0.0, 0.0, 0.0, 2.0},
        {3.0, 0.0, 0.0, 2.5}
    };
    /*
     * Manual domain x in [-2, 1] with npts[0] = 4 => spacs[0] = 1.0, so
     * Vclist_getCell returns VNULL whenever ic[0] = (int)(x + 2.0) >= 4 (x >= 2.0):
     *  0: (0.5, sqrt(3)/2, 0) -> pos = (1.70, 2.9445, 0), ic[0] = 3 (in-domain),
     *     dist^2 to Atom 1 = 10.36 < (2.5 + 1.4)^2 = 15.21 => buried (0).
     *  1: (0.8, 0.6, 0)       -> pos = (2.72, 2.0400, 0), ic[0] = 4 (out-of-domain,
     *     Vclist_getCell == VNULL), even though dist^2 to Atom 1 = 4.24 < 15.21 =>
     *     must be classified accessible (1) matching uncached Vclist_getCell.
     */
    static const double dirs[2][3] = {
        {0.5, 0.8660254037844386, 0.0},
        {0.8, 0.6, 0.0}
    };
    int expected_flags[2] = {0, 1};
    int npts_grid[3] = {4, 4, 4};
    double lower[3] = {-2.0, -4.0, -4.0};
    double upper[3] = { 1.0,  4.0,  4.0};
    Valist *alist;
    Vclist *clist;
    Vacc *acc;
    VaccSurf *custom_ref, *sphere_ref;
    int got_npts, i, ok;
    char label[64];

    alist = build_alist(specs, 2);
    clist = Vclist_ctor(alist, 1.4, npts_grid, CLIST_MANUAL_DOMAIN,
                        lower, upper);
    acc = Vacc_ctor(alist, clist, 10.0);
    custom_ref = make_custom_ref(acc->mem, dirs, 2);

    got_npts = -1;
    ok = verify_surf_against_oracle("manual_domain_consecutive_samples",
                                    acc, clist, Valist_getAtom(alist, 0),
                                    custom_ref, 1.4, &got_npts, expected_flags);
    if (ok && got_npts != 1) {
        fprintf(stderr,
                "FAIL [manual_domain_consecutive_samples]: npts=%d != 1\n",
                got_npts);
        ok = 0;
    }

    sphere_ref = VaccSurf_refSphere(acc->mem, 256);
    for (i = 0; i < 2 && ok; i++) {
        sprintf(label, "manual_domain_ref_sphere_atom_%d", i);
        ok = verify_surf_against_oracle(label, acc, clist,
                                        Valist_getAtom(alist, i),
                                        sphere_ref, 1.4, VNULL, VNULL);
    }

    VaccSurf_dtor(&sphere_ref);
    VaccSurf_dtor(&custom_ref);
    Vacc_dtor(&acc);
    Vclist_dtor(&clist);
    Valist_dtor(&alist);
    return ok;
}

int main(void) {
    if (!test_cached_hits_and_miss_fallback()) {
        return 1;
    }
    if (!test_occluder_switching_and_ref_sphere()) {
        return 1;
    }
    if (!test_manual_domain_out_of_bounds()) {
        return 1;
    }
    printf("vacc_surf_test: all tests passed.\n");
    return 0;
}
