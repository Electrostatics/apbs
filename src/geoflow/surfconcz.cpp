///  @file    surfconcz.cpp
///  @author  Andrew Stevens, Kyle Monson, Zhan Chen, Guowei Wei
///  @brief solves non-polar solvation using geometric flow explicit Eulerian formulation
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
#include <vector>
#include <cmath>

#include <iostream>
#include <fstream>

using namespace std;


double left(const valarray<double>& pr, double h, double ev){
    return floor( (pr - ev).min()/h )*h - ev;
}

double right(const valarray<double>& pr, double h, double ev){
    return ceil( (pr + ev).max()/h )*h + ev;
}

void domainini(double xyzr[MAXATOMS][XYZRWIDTH], const size_t natm, const double extvalue){
    double dx = comdata.deltax, dy = comdata.deltay, dz = comdata.deltaz;
    valarray<double> atom_x(natm), atom_y(natm), atom_z(natm), atom_r(natm);
    for(size_t i=0; i<natm; ++i){
        atom_x[i] = xyzr[i][0];
        atom_y[i] = xyzr[i][1];
        atom_z[i] = xyzr[i][2];
        atom_r[i] = xyzr[i][3];
    }
    
    double xleft = left(atom_x - atom_r, dx, extvalue);
    double yleft = left(atom_y - atom_r, dy, extvalue);
    double zleft = left(atom_z - atom_r, dz, extvalue);

    double xright = right(atom_x + atom_r, dx, extvalue);
    double yright = right(atom_y + atom_r, dy, extvalue);
    double zright = right(atom_z + atom_r, dz, extvalue);
    
    int nx = (xright - xleft)/dx + 1;
    int ny = (yright - yleft)/dy + 1;
    int nz = (zright - zleft)/dz + 1;

    xright = xleft + dx*(nx - 1);
    yright = yleft + dy*(ny - 1);
    zright = zleft + dz*(nz - 1);

    //set the stupid globals...
    comdata.xleft = xleft; comdata.xright = xright;
    comdata.yleft = yleft; comdata.yright = yright;
    comdata.zleft = zleft; comdata.zright = zright;
    comdata.nx = nx; comdata.ny = ny; comdata.nz = nz;
}

void volumintegration(const double* f, const int& nx, const int& ny, const int& nz, const double& dcel, double& volume){ //FIXME return volume
    valarray<double> ff(f, nx*ny*nz);//maybe divide before sum to avoid floating pt stuff
    ff /= 1000.0;
    ff *= comdata.deltax*comdata.deltay*comdata.deltaz;
    volume = ff.sum();
}



double volumeIntegration(Mat<> f, double dcel){
    valarray<double> ff(f.data(), f.size());
    ff /= 1000.0;
    ff *= comdata.deltax*comdata.deltay*comdata.deltaz;
    return ff.sum();
}

void upwinding(double dx, double dt, int nt, Mat<>& g, Mat<>& surfu, Mat<>& phitotx){
    Mat<> surfnew(surfu);
    for(int t=0; t<nt; ++t){ 
        for(Stencil<double> phi = surfu.stencilBegin(comdata.deltax); phi != surfu.stencilEnd(dx); ++phi){
            if(g[phi.i] > 2e-2){
                surfnew[phi.i] = min(1000.0, max(0.0,
                  *(phi.c) + dt*phi.deriv(phitotx[phi.i])
                ));
            }
        }
        surfu = surfnew;
    }
}

void initial(double xl, double yl, double zl, double deleteme, int n_atom, const valarray<double>& atom_x, const valarray<double>& atom_y, const valarray<double>& atom_z, const valarray<double>& atom_r, Mat<>& g, Mat<>& phi){
    double dx = comdata.deltax, dy = comdata.deltay, dz = comdata.deltaz;
    g = 1.0;
    phi = 0.0;

    double alpha = 1e3;
    
    for(int a=0; a<n_atom; ++a){
        double r = atom_r[a];
        double r2 = r*r;
        double zmin = ((atom_z[a] - zl - r)/dz + 1.0);
        double zmax = ((atom_z[a] - zl + r)/dz + 1.0);

        for(int z = ceil(zmin); z<=floor(zmax); ++z){
            double distxy = (zl + (z-1)*dz - atom_z[a]);
            double distxy2 = distxy*distxy;
            double rxy2 = abs(r2 - distxy2);
            double rxy = sqrt(rxy2);
            double ymin = ((atom_y[a] - yl - rxy)/dy + 1.0);
            double ymax = ((atom_y[a] - yl + rxy)/dy + 1.0);

            for(int y=ceil(ymin); y<=floor(ymax); ++y){
                double distx = (yl + (y-1)*dy - atom_y[a]);
                double distx2 = distx*distx;
                double rx = sqrt(abs(rxy2 - distx2));
                double xmin = ((atom_x[a] - xl - rx)/dx + 1.0);
                double xmax = ((atom_x[a] - xl + rx)/dx + 1.0);

                for(int x=ceil(xmin); x<=floor(xmax); ++x){
                    g(x,y,z) = 0;
                    phi(x,y,z) = alpha;
                }
            }
        }
    }
}

void potIntegral(double rcfactor, double ddx, size_t natm, valarray<double>& atom_x,valarray<double>& atom_y,valarray<double>& atom_z,valarray<double>& seta12, valarray<double>& seta6, valarray<double>& epsilon, valarray<double>& sigma, Mat<>& g, Mat<>& potr, Mat<>& pota){
    double dx = comdata.deltax, dy = comdata.deltay, dz = comdata.deltaz;
    for(size_t x=2; x<potr.nx(); ++x){ 
    for(size_t y=2; y<potr.ny(); ++y){ 
    for(size_t z=2; z<potr.nz(); ++z){
        if(g(x,y,z) == 0){ continue; }
        double pr=0, pa=0;
        for(size_t a=0; a<natm; ++a){
            const double xi = comdata.xleft + (x-1)*dx;
            const double yi = comdata.yleft + (y-1)*dy;
            const double zi = comdata.zleft + (z-1)*dz;
            const double dist = sqrt( dot(xi-atom_x[a], yi-atom_y[a], zi-atom_z[a]) ) + lj.prob;
            const double ratio = (dist==0.0) ? 1.0 : sigma[a]/dist;
            const double ratio6 = ratio*ratio*ratio*ratio*ratio*ratio;
            const double ratio12 = ratio6*ratio6;
            
            if(lj.iwca == 1){
                if(ratio*rcfactor > 1){
                    pr += seta12[a]*ratio12 - seta6[a]*ratio6 + epsilon[a];
                    pa -= epsilon[a];
                }else{
                    pa = pa - seta6[a]*ratio6 + seta12[a]*ratio12;
                }
            }else{
                pr += ratio12*seta12[a];
                pa -= ratio6*seta6[a];
            }
        }
        potr(x,y,z) = pr; 
        pota(x,y,z) = pa;
    }}}
}

//void adicor(double* su, double* g, double& ddx, double& dt, int& nz,int& ny,int& nx, double* phitotx);

void yhsurface(double xyzr[MAXATOMS][XYZRWIDTH], double* ljepsilon, size_t natm, double tott, double dt, Mat<>& phitotx, Mat<>& surfu, int iloop, double& area, double& volume, double& attint, double alpha, int iadi, int igfin
){
    int nx = comdata.nx, ny = comdata.ny, nz = comdata.nz;
    double xl = comdata.xleft, yl = comdata.yleft, zl = comdata.zleft;
    double ddx = 0;
    valarray<double> atom_x(natm), atom_y(natm), atom_z(natm), atom_r(natm);
    for(size_t i=0; i<natm; ++i){
        atom_x[i] = xyzr[i][0];
        atom_y[i] = xyzr[i][1];
        atom_z[i] = xyzr[i][2];
        atom_r[i] = xyzr[i][3];
    }

    Mat<> su(nx,ny,nz), g(nx,ny,nz);

    initial(xl,yl,zl, ddx, natm, atom_x,atom_y,atom_z,atom_r, g, su);
    if(iloop > 1 && igfin == 1){ su = surfu; }

    double rcfactor = (lj.ffmodel == 1) ? 1.0 : pow(2.0, 1.0/6.0);
    valarray<double> sigma(atom_r);
    valarray<double> seta12(natm), seta6(natm), epsilon(natm);
    
    if(lj.ffmodel == 1){
        for(size_t i=0; i<natm; ++i){
            sigma[i] = atom_r[i] + lj.sigmas;
            if(lj.vdwdispersion != 0){
                double se = sigma[i]/(atom_r[i] + lj.prob);
                epsilon[i] = pow( pow(se, 12.0) - 2.0*pow(se, 6.0) , -1.0);
            }
            seta12[i] = lj.iosetar*lj.vdwdispersion*epsilon[i];
            seta6[i] = 2.0*lj.iosetaa*lj.vdwdispersion*epsilon[i];
        }
    }else{
        for(size_t i=0; i<natm; ++i){
            sigma[i] = sqrt(4.0*atom_r[i]*lj.sigmas);
            if(lj.vdwdispersion != 0){
                epsilon[i] = sqrt(ljepsilon[i]*lj.epsilonw);
                seta12[i] = 4.0*epsilon[i];
                seta6[i] = 4.0*epsilon[i];
            }
        }
    }

    Mat<> potr(nx,ny,nz), pota(nx,ny,nz);
    potIntegral(rcfactor, ddx, natm, atom_x, atom_y, atom_z, seta12, seta6, epsilon, sigma, g, potr, pota);

    if(lj.iwca == 1) potr = 0;
    for(size_t i=0; i<phitotx.size(); ++i){ 
        phitotx[i] = -lj.conms - phitotx[i] + lj.roro*(potr[i] + pota[i]);
    }

    if(iadi==0 || iloop>1){
        int nt = ceil(tott/dt);
        //dt = deltat;
        upwinding(ddx,dt, nt, g,su,phitotx);
    }else{
        cerr << "ADI not implemented..." << endl;
        exit(1);
        //for(int t=1; t<=nt; ++t){  dt is unset in parent call!!!
        //    adicor(su.data(), g.data(), ddx, dt, nz,ny,nx, phitotx.data()); //this has issues, should have &
        //}
    }

    if(iloop < 2){
        surfu = su;
    }else{
        for(size_t i=0; i<surfu.size(); ++i){ 
           surfu[i] = surfu[i]*alpha + su[i]*(1.0 - alpha);
        }
        su = surfu;
    }

    volume = volumeIntegration(su, ddx);

    Mat<> fintegr(nx,ny,nz);
    double dx = comdata.deltax, dy = comdata.deltay, dz = comdata.deltaz;
    for(size_t x=2; x<nx; ++x){
    for(size_t y=2; y<ny; ++y){
    for(size_t z=2; z<nz; ++z){
        double sux = su(x+1,y,z) - su(x-1,y,z);
        double suy = su(x,y+1,z) - su(x,y-1,z);
        double suz = su(x,y,z+1) - su(x,y,z-1);
        fintegr(x,y,z) = sqrt(dot(sux/(2*dx*dx),suy/(2*dy*dy),suz/(2*dz*dz)));
    }}}
    area = volumeIntegration(fintegr, ddx);
   
    potIntegral(rcfactor, ddx, natm, atom_x, atom_y, atom_z, seta12, seta6, epsilon, sigma, g, potr, pota);

    if(lj.iwca==1){
        for(size_t i=0; i<fintegr.size(); ++i){
            fintegr[i] = pota[i]*(1e3 - su[i]);
        }
    }else{
        for(size_t i=0; i<fintegr.size(); ++i){ 
            fintegr[i] = (pota[i] + potr[i])*(1e3 - su[i]);
        }

    }

    attint = volumeIntegration(fintegr, ddx);
}

