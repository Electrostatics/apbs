#include "modules.h"
#include "Mat.h"

extern "C"{

void xadicor_(double* u_, double* g_, double& deltah, double& deltat, int& nx, int& ny, int& nz, double* ux_,

double* a3_, double* a2_, double* a1xyz, double*cxyz
){
    Mat u(u_, nx,ny,nz), ux(ux_, nx,ny,nz), g(g_, nx,ny,nz), a3(a3_, nx,ny,nz), a2(a2_, nx,ny,nz);
//    valarray<double> a1xyz(), cxyz();
    double a = 2.0*deltah, b = a*a;
    for(int k=2; k<nz; ++k){
    for(int i=2; i<ny; ++i){
    for(int j=2; j<nx; ++j){
        int m = (k-1)*nx*ny + nx*(i-1) + j;

        double phix = (u(k,i,j+1) - u(k,i,j-1))/a;
        double phiy = (u(k,i+1,j) - u(k,i-1,j))/a;
        double phiz = (u(k+1,i,j) - u(k-1,i,j))/a;

        double phixy = (u(k,i+1,j+1) - u(k,i+1,j-1) - u(k,i-1,j+1) + u(k,i-1,j-1))/b;
        double phixz = (u(k+1,i,j+1) - u(k+1,i,j-1) - u(k-1,i,j+1) + u(k-1,i,j-1))/b;
        double phiyz = (u(k+1,i+1,j) - u(k+1,i-1,j) - u(k-1,i+1,j) + u(k-1,i-1,j))/b;

        double de = (1 + dot(phix,phiy,phiz));
        a3(k,i,j) = g(k,i,j)*(1 + phix*phix + phiy*phiy)*deltat/de;
        a2(k,i,j) = g(k,i,j)*(1 + phix*phix + phiz*phiz)*deltat/de;
        a1xyz[m] = g(k,i,j)*(1 + phiy*phiy + phiz*phiz)*deltat/de;
        cxyz[m] = g(k,i,j)*( 2.0*(phix*phiy*phixy + phix*phiy*phixz + phiy*phiz*phiyz)/de - ux(k,i,j)*sqrt(de))*deltat;
    }}}
}

}//extern
