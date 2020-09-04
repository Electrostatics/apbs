
#pragma once

#include <string>
#include <vector>

extern "C"
{
#include "apbscfg.h" 
#include "routines.h"
#include "generic/nosh.h"
#include "generic/valist.h"
#include "generic/vatom.h"
}

/**
 * @file tools/python-pybind/bind_nosh.hpp
 * @author Asher Mancinelli <asher.mancinelli@pnnl.gov>
 * @brief Contains bindings for nosh-related functions.
 *
 * @note keep all implementations in the impl unless templated.
 */

/**
 * @todo request help documenting
 */
int parseInputFromString(NOsh *nosh, std::string str);

/**
 * @todo request help documenting
 */
template<typename T>
std::vector<T> getPotentials(NOsh *nosh, PBEparm *pbeparm, Vpmg *pmg, Valist *alist)
{
    Vgrid *grid;
    Vatom *atom; 
    int i, rc, nx, ny, nz;
    double hx, hy, hzed, xcent, ycent, zcent, xmin, ymin, zmin;
    double value;
    double *position;
    std::vector<T> values;
    
    nx = pmg->pmgp->nx;
    ny = pmg->pmgp->ny;
    nz = pmg->pmgp->nz;
    hx = pmg->pmgp->hx;
    hy = pmg->pmgp->hy;
    hzed = pmg->pmgp->hzed;
    xcent = pmg->pmgp->xcent;
    ycent = pmg->pmgp->ycent;
    zcent = pmg->pmgp->zcent;
    xmin = xcent - 0.5*(nx-1)*hx;
    ymin = ycent - 0.5*(ny-1)*hy;
    zmin = zcent - 0.5*(nz-1)*hzed;
   
    Vpmg_fillArray(pmg, pmg->rwork, VDT_POT, 0.0, pbeparm->pbetype, pbeparm);
    grid = Vgrid_ctor(nx, ny, nz, hx, hy, hzed, xmin, ymin, zmin,
                  pmg->rwork);
    for (i=0;i<Valist_getNumberAtoms(alist);i++){
        atom = Valist_getAtom(alist, i);
        position = Vatom_getPosition(atom); 
        Vgrid_value(grid, position, &value);
        values[i] = value;
    } 
    Vgrid_dtor(&grid);    
    return values;
}
