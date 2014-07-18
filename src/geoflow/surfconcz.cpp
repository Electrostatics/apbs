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
/// Copyright (c) 2010-2014 Battelle Memorial Institute. Developed at the
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

#include <cmath>
#include <iostream>
#include <valarray>

#include "modules.h"
#include "Mat.h"

double left(const std::valarray<double>& pr, double h, double ev)
{
	return floor( (pr - ev).min()/h ) * h - ev;
}

double right(const std::valarray<double>& pr, double h, double ev)
{
	return ceil( (pr + ev).max()/h ) * h + ev;
}

void domainini(double xyzr[MAXATOMS][XYZRWIDTH], const size_t natm,
		const double extvalue)
{
	double dx = comdata.deltax, dy = comdata.deltay, dz = comdata.deltaz;
	std::valarray<double> atom_x(natm), atom_y(natm), atom_z(natm), atom_r(natm);
	for(size_t i = 0; i < natm; ++i) {
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


double volumeIntegration(Mat<> f)
{
	double sumf = f.baseInterface().sum();
	return sumf/1000*comdata.deltax*comdata.deltay*comdata.deltaz;
}

void upwinding(double dt, int nt, Mat<>& g, Mat<>& su,
		Mat<>& phitotx)
{
	Mat<> surfnew(su);
	for (int t = 0; t < nt; ++t) {
		for (Stencil<double> phi = su.stencilBegin();
				phi != su.stencilEnd(); ++phi) {
			if (g[phi.i] > 2e-2) {
				surfnew[phi.i] = fmin(1000.0,
						fmax(0.0, *(phi.c) + dt * phi.deriv(phitotx[phi.i])));
			}
		}
		su = surfnew;
	}
}

void initial(double xl, double yl, double zl, int n_atom,
		const std::valarray<double>& atom_x, const std::valarray<double>& atom_y,
		const std::valarray<double>& atom_z, const std::valarray<double>& atom_r,
		Mat<>& g, Mat<>& phi)
{
	double dx = comdata.deltax, dy = comdata.deltay, dz = comdata.deltaz;
	g = 1.0;
	phi = 0.0;

	double alpha = 1e3;

	for(int a = 0; a < n_atom; ++a) {
		double r = atom_r[a];
		double r2 = r*r;
		double zmin = ((atom_z[a] - zl - r)/dz + 1.0);
		double zmax = ((atom_z[a] - zl + r)/dz + 1.0);

		for(int z = ceil(zmin); z <= floor(zmax); ++z) {
			double distxy = (zl + (z-1)*dz - atom_z[a]);
			double distxy2 = distxy*distxy;
			double rxy2 = fabs(r2 - distxy2);
			double rxy = sqrt(rxy2);
			double ymin = ((atom_y[a] - yl - rxy)/dy + 1.0);
			double ymax = ((atom_y[a] - yl + rxy)/dy + 1.0);

			for(int y = ceil(ymin); y <= floor(ymax); ++y) {
				double distx = (yl + (y-1)*dy - atom_y[a]);
				double distx2 = distx*distx;
				double rx = sqrt(fabs(rxy2 - distx2));
				double xmin = ((atom_x[a] - xl - rx)/dx + 1.0);
				double xmax = ((atom_x[a] - xl + rx)/dx + 1.0);

				for(int x = ceil(xmin); x <= floor(xmax); ++x) {
					g(x,y,z) = 0;
					phi(x,y,z) = alpha;
				}
			}
		}
	}
}

void potIntegral(double rcfactor, size_t natm,
		std::valarray<double>& atom_x, std::valarray<double>& atom_y,
		std::valarray<double>& atom_z, std::valarray<double>& seta12,
		std::valarray<double>& seta6, std::valarray<double>& epsilon,
		std::valarray<double>& sigma, Mat<>& g, Mat<>& potr, Mat<>& pota)
{
	double dx = comdata.deltax, dy = comdata.deltay, dz = comdata.deltaz;
	for (size_t x = 2; x < potr.nx(); ++x) {
		for (size_t y = 2; y < potr.ny(); ++y) {
			for (size_t z = 2; z < potr.nz(); ++z) {

				if (g(x,y,z) == 0) { continue; }

				double pr=0, pa=0;
				for (size_t a = 0; a < natm; ++a) {
					const double xi = comdata.xleft + (x-1)*dx;
					const double yi = comdata.yleft + (y-1)*dy;
					const double zi = comdata.zleft + (z-1)*dz;
					const double dist = sqrt( dot(xi-atom_x[a], yi-atom_y[a],
							zi-atom_z[a]) ) + lj.prob;
					const double ratio = (dist==0.0) ? 1.0 : sigma[a]/dist;
					const double ratio6 = ratio*ratio*ratio*ratio*ratio*ratio;
					const double ratio12 = ratio6*ratio6;

					if (lj.iwca == 1) {
						if (ratio*rcfactor > 1) {
							pr += seta12[a]*ratio12 - seta6[a]*ratio6 + epsilon[a];
							pa -= epsilon[a];
						} else {
							pa = pa - seta6[a]*ratio6 + seta12[a]*ratio12;
						}
					} else {
						pr += ratio12*seta12[a];
						pa -= ratio6*seta6[a];
					}
				}
				potr(x,y,z) = pr;
				pota(x,y,z) = pa;
			}
		}
	}
}

//void adicor(double* su, double* g, double& ddx, double& dt, int& nz,int& ny,
//		int& nx, double* phitotx);

void yhsurface(double xyzr[MAXATOMS][XYZRWIDTH], double* ljepsilon, size_t natm,
		double tott, double dt, Mat<>& phitotx, Mat<>& surfu, int iloop,
		double& area, double& volume, double& attint, double alpha, int iadi,
		int igfin)
{
	size_t nx = comdata.nx, ny = comdata.ny, nz = comdata.nz;
	double xl = comdata.xleft, yl = comdata.yleft, zl = comdata.zleft;
	std::valarray<double> atom_x(natm), atom_y(natm), atom_z(natm), atom_r(natm);
	for (size_t i = 0; i < natm; ++i) {
		atom_x[i] = xyzr[i][0];
		atom_y[i] = xyzr[i][1];
		atom_z[i] = xyzr[i][2];
		atom_r[i] = xyzr[i][3];
	}

	Mat<> su(surfu), g(surfu);
	initial(xl, yl, zl, natm, atom_x,atom_y, atom_z, atom_r, g, su);
	if (iloop > 1 && igfin == 1)
		su = surfu;

	double rcfactor = (lj.ffmodel == 1) ? 1.0 : pow(2.0, 1.0/6.0);
	std::valarray<double> sigma(atom_r);
	std::valarray<double> seta12(natm), seta6(natm), epsilon(natm);

	if (lj.ffmodel == 1) {
		for (size_t i = 0; i < natm; ++i) {
			sigma[i] = atom_r[i] + lj.sigmas;
			if (lj.vdwdispersion != 0) {
				double se = sigma[i]/(atom_r[i] + lj.prob);
				epsilon[i] = pow( pow(se, 12.0) - 2.0*pow(se, 6.0) , -1.0);
			}
			seta12[i] = lj.iosetar*lj.vdwdispersion*epsilon[i];
			seta6[i] = 2.0*lj.iosetaa*lj.vdwdispersion*epsilon[i];
		}
	} else {
		for (size_t i = 0; i < natm; ++i) {
			sigma[i] = sqrt(4.0*atom_r[i]*lj.sigmas);
			if (lj.vdwdispersion != 0) {
				epsilon[i] = sqrt(ljepsilon[i]*lj.epsilonw);
				seta12[i] = 4.0*epsilon[i];
				seta6[i] = 4.0*epsilon[i];
			}
		}
	}

	Mat<> potr(nx,ny,nz), pota(nx,ny,nz);
	potIntegral(rcfactor, natm, atom_x, atom_y, atom_z, seta12, seta6,
			epsilon, sigma, g, potr, pota);

	if (lj.iwca == 1)
		potr = 0;

	for (size_t i = 0; i < phitotx.size(); ++i) {
		phitotx[i] = -lj.conms - phitotx[i] + lj.roro*(potr[i] + pota[i]);
	}

	if (iadi == 0 || iloop > 1) {
		int nt = ceil(tott/dt) + 1;
		upwinding(dt, nt, g, su, phitotx);
	} else {
		std::cerr << "ADI not implemented..." << std::endl;
		exit(1);
	}

	if (iloop > 1) {
		for (size_t i = 0; i < surfu.size(); ++i) {
		   surfu[i] = surfu[i]*alpha + su[i]*(1.0 - alpha);
		}
		su = surfu;
	} else {
		surfu = su;
	}

	volume = volumeIntegration(su);
	std::cout << "volume = " << volume << std::endl;

	Mat<> fintegr(nx,ny,nz);
	double dx = comdata.deltax, dy = comdata.deltay, dz = comdata.deltaz;
	for (size_t x = 2; x < nx; ++x) {
		for (size_t y = 2; y < ny; ++y) {
			for (size_t z = 2; z < nz; ++z) {
				double sux = su(x+1,y,z) - su(x-1,y,z);
				double suy = su(x,y+1,z) - su(x,y-1,z);
				double suz = su(x,y,z+1) - su(x,y,z-1);
				fintegr(x,y,z) = sqrt(dot(sux/(2.0*dx), suy/(2.0*dy),
						suz/(2.0*dz)));
			}
		}
	}

	area = volumeIntegration(fintegr);
	std::cout << "area = " << area << std::endl;

	potIntegral(rcfactor, natm, atom_x, atom_y, atom_z, seta12, seta6,
			epsilon, sigma, g, potr, pota);

	if (lj.iwca == 1) {
		for (size_t i = 0; i < fintegr.size(); ++i) {
			fintegr[i] = pota[i]*(1e3 - su[i]);
		}
	} else {
		for (size_t i = 0; i < fintegr.size(); ++i) {
			fintegr[i] = (pota[i] + potr[i])*(1e3 - su[i]);
		}
	}

	attint = volumeIntegration(fintegr);
	std::cout << "attint = " << attint << std::endl;
}

