///  @file    solvationsub.cpp
///  @author  Andrew Stevens, Kyle Monson, Zhan Chen, Guowei Wei
///  @brief solvation stuff
///  @ingroup Geoflow
///  @version $Id$
///  @attention
///  @verbatim
///
/// APBS -- Adaptive Poisson-Boltzmann Solver
///
///  Nathan A. Baker (nathan.baker@pnnl.gov)
///  Pacific Northwest National Laboratory
///
///  Additional contributing authors listed in the code documentation.
///
/// Copyright (c) 2010-2012 Battelle Memorial Institute. Developed at the
/// Pacific Northwest National Laboratory, operated by Battelle Memorial
/// Institute, Pacific Northwest Division for the U.S. Department of Energy.
///
/// Portions Copyright (c) 2002-2010, Washington University in St. Louis.
/// Portions Copyright (c) 2002-2010, Nathan A. Baker.
/// Portions Copyright (c) 1999-2002, The Regents of the University of
/// California.
/// Portions Copyright (c) 1995, Michael Holst.
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// Redistributions of source code must retain the above copyright notice, this
/// list of conditions and the following disclaimer.
///
/// Redistributions in binary form must reproduce the above copyright notice,
/// this list of conditions and the following disclaimer in the documentation
/// and/or other materials provided with the distribution.
///
/// Neither the name of the developer nor the names of its contributors may be
/// used to endorse or promote products derived from this software without
/// specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
/// THE POSSIBILITY OF SUCH DAMAGE.
///
/// @endverbatim

#include "modules.h"
#include "Mat.h"
#include <valarray>
#include <cmath>

using namespace std;

void chargedist(double xyzr[MAXATOMS][XYZRWIDTH],
double* chratm, Mat<>& charget, Mat<>& corlocqt, Mat<size_t>& loc_qt, size_t iatm){
    double x_q = xyzr[iatm-1][0];
    double y_q = xyzr[iatm-1][1];
    double z_q = xyzr[iatm-1][2];
    double q_q = chratm[iatm-1];

    size_t i_q = inverx(x_q);
    int j_q = invery(y_q);
    int k_q = inverz(z_q);


    Mat<size_t> loc_q(8,3);
    Mat<> corlocq(8,3);
    for(size_t i=0; i<=1; ++i){
    for(size_t j=0; j<=1; ++j){
    for(size_t k=0; k<=1; ++k){
        size_t ind = 4*k + 2*j + i + 1;
    
        size_t ind_i = i_q + i;
        size_t ind_j = j_q + j;
        size_t ind_k = k_q + k;
        
        corlocq(ind,1) = xvalue(ind_i);
        corlocq(ind,2) = yvalue(ind_j);
        corlocq(ind,3) = zvalue(ind_k);
        
        loc_q(ind,1) = ind_i;
        loc_q(ind,2) = ind_j;
        loc_q(ind,3) = ind_k;

    }}}

    double x = xvalue(i_q);
    double y = yvalue(j_q);
    double z = zvalue(k_q);

    double xd1 = x_q - x;
    double yd1 = y_q - y;
    double zd1 = z_q - z;

    valarray<double> charge(0.0, 8);
    if(xd1 !=0 && yd1 !=0 && zd1 !=0){
        for(int i=0; i<=1; ++i){
        for(int j=0; j<=1; ++j){
        for(int k=0; k<=1; ++k){
            int ind = 4*k + 2*j + i;
        
            double xd = i*comdata.deltax - xd1;
            double yd = j*comdata.deltay - yd1;
            double zd = k*comdata.deltaz - zd1;

            charge[ind] = 1.0/abs(xd*yd*zd);
        }}}
    }else if( (xd1 !=0 && yd1 !=0) || (xd1 !=0 && zd1 !=0) || (yd1 !=0 && zd1 !=0) ){
        if(xd1 == 0){
            for(int j=0; j<=1; ++j){
            for(int k=0; k<=1; ++k){
                double yd = j*comdata.deltay - yd1;
                double zd = k*comdata.deltaz - zd1;
                charge[4*j + 2*k] = 1.0/abs(yd*zd);
            }}
        }else if(yd1 == 0){
            for(int i=0; i<=1; ++i){
            for(int k=0; k<=1; ++k){
                double xd = i*comdata.deltax - xd1;
                double zd = k*comdata.deltaz - zd1;
                charge[i + 4*k] = 1.0/abs(xd*zd);
            }}
        }else if(zd1 == 0){
            for(int i=0; i<=1; ++i){
            for(int j=0; j<=1; ++j){
                double xd = i*comdata.deltax - xd1;
                double yd = j*comdata.deltay - yd1;
                charge[i + 2*j] = 1.0/abs(xd*yd);
            }}
        }
    }else if(xd1 !=0  || yd1 !=0 || zd1 !=0 ){
        if(xd1!=0){
            charge[0] = 1.0/xd1;
            charge[1] = 1.0/(comdata.deltax-xd1);
        }else if(yd1!=0){
            charge[0] = 1.0/yd1;
            charge[2] = 1.0/(comdata.deltay-yd1);
        }else if(zd1!=0){
            charge[0] = 1.0/zd1;
            charge[4] = 1.0/(comdata.deltaz-zd1);
        }
    }else{
        charge[0] = 1.0;
    }
    
    charge=q_q*charge/charge.sum();

    for(size_t j=1; j<=charget.ny(); ++j){
        charget(iatm,j) = charge[j-1];
        for (size_t k=1; k<=corlocqt.nz(); ++k){
            corlocqt(iatm,j,k) = corlocq(j,k);
            loc_qt(iatm,j,k) = loc_q(j,k);

        }
    }
}
 
