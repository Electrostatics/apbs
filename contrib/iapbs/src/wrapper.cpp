#include <iostream>
#include <stdlib.h>
#include <stdio.h>

extern "C" {
#include "apbscfg.h"
#include "apbs.h"
#include "routines.h"
#include "apbs_driver.h"
}
using namespace std;


bool apbs();
// =========================================================================
int main(int argc,char **argv)
{
/**

C++ version of wrapper.f; a simple program to use iAPBS API.
This is a working but bare minimum example how to interact with iAPBS.

Contributed by E. Prabhu Raman (prabhu@outerbanks.umaryland.edu)

How to compile:
APBS_LIB=${APBS_HOME}/lib
APBS_INCL=${APBS_HOME}/include
g++ -g wrapper.cpp -I${APBS_INCL} -I${APBS_INCL}/apbs  -I${APBS_INCL}/iapbs -I${APBS_INCL}/maloc  -L${APBS_LIB} -liapbs -lapbs_routines  -lapbs_mg -lapbs_generic -lapbs_pmgc -lm -lmaloc -o wrapper.o


*/

  cout<<"Hello World"<<endl;
  cout<<"Testing iAPBS-APBS"<<endl;
  cout<<"Test Molecule: Methanol (hardcoded geomtery and params)"<<endl;

  apbs();

  return(0);
}
// =========================================================================
bool apbs(){
  int rc, natom, i, j, loop;

  double r_param[9];
  int i_param[25];
  int ispara;
  int dime[3], pdime[3];
  double grid[3], glen[3], center[3], cglen[3], fglen[3];
  double ccenter[3], fcenter[3];
  double ofrac;

  double ionq[MAXION], ionc[MAXION], ionrr[MAXION];

  double esenergy[1],npenergy[1];
  double *x = new double[NATOMS];
  double *y = new double[NATOMS];
  double *z = new double[NATOMS];
  double *radius = new double[NATOMS];
  double *charge = new double[NATOMS];
  double *apbsdx = new double[NATOMS];
  double *apbsdy = new double[NATOMS];
  double *apbsdz = new double[NATOMS];
  double *apbsqfx = new double[NATOMS];
  double *apbsqfy = new double[NATOMS];  
  double *apbsqfz = new double[NATOMS];
  double *apbsdbx = new double[NATOMS];
  double *apbsdby = new double[NATOMS];
  double *apbsdbz = new double[NATOMS];
  double *apbsnpx = new double[NATOMS];
  double *apbsnpy = new double[NATOMS];
  double *apbsnpz = new double[NATOMS];
  double *apbsibx = new double[NATOMS];
  double *apbsiby = new double[NATOMS];
  double *apbsibz = new double[NATOMS];

  double apbsnp[3];
  double apbsgrid_meta[13] ;

  double *apbsgrid = new double[3*NATOMS];

  int apbs_debug;

  double pdie, sdie, srad, swin, temp, gamma, sdens;
  double smvolume, smsize;
  int nonlin, bcfl, nion, srfm, calcenergy, calcforce;
  int calc_type, nlev, cmeth, ccmeth, fcmeth, chgm;
  int wpot, wchg, wsmol, wkappa, wdiel, rchg, rkappa;
  int watompot, rpot, rdiel;
  int calcnpenergy, calcnpforce;

  int dummyi;
  char dummyc;
  double dummyr, maxx, minx, maxy, miny, maxz, minz;

  //initialization of input data
  for(int i=0;i<3;i++){
    grid[i] = 0.0;
    dime[i] = 0;
    glen[i] = 0.0;
    center[i] = 0.0;
    pdime[i] = 0;
    cglen[i] = 0.0;
    fglen[i] = 0.0;
    ccenter[i] = 0.0;
    fcenter[i] = 0.0;
  }
  for(int i=0;i<13;i++){
    apbsgrid_meta[i] = 0.0;
  }
  for(int i=0;i<3*NATOMS;i++){
    apbsgrid[i] = 0.0;
  }

  //defaults
  for(int i=0;i<3;i++)
    grid[i] = 0.25;

  pdie = 2.0;
  sdie = 78.0;
  srad = 0.0 ;  //1.4  radius of the solvent molecules
  swin = 0.3;   //size of the support (i.e., the rate of change) for spline-based
                //surface definitions (see srfm spl2). The value is usually set to 0.3 Å
  temp = 298.0;
  sdens = 10.0; //10.0 the number of quadrature points per Å2 to use in surface terms 
                //  (e.g.,  molecular surface, solvent accessible surface) for apolar


  gamma = 0.0085; //0.105  surface tension coefficient for apolar solvation models
  smvolume = 10.0; // controls the lattice size (in A^3) used in the SMPBE formalism.
  smsize = 1000.0; // controls the relative size of the ions (in Angstroms) such that 
                   //   !each lattice site can contain a single ion of volume radius^3 
                   //   !or size ions of volume radius^3/size.

  calc_type = 0;
  nlev = 4;
  cmeth = 1;
  ccmeth = 1 ;
  fcmeth = 1 ;
  chgm = 0 ;  // !1! the method by which the biomolecular point charges (i.e., 
             //    ! Dirac delta functions) are mapped to the grid for a multigrid PB

  nonlin = 0; //!non-linear PB
  bcfl = 2; // !1  !Specifies the type of boundary conditions used to solve PB
  srfm = 1; // ! 2  !the model used to construct the solvent-related surface and volume
  calcenergy = 1;
  calcforce = 0;
  calcnpenergy = 1;
  calcnpforce = 0;

  //Read/Write only
  wpot = 0 ; //Writes electrostatic potential data to iapbs-pot.dx in DX format
  wchg = 0 ; // Writes charge data to iapbs-charge.dx in DX format
  wsmol = 0 ; //!Writes molecular surface data to iapbs-smol.dx in DX format
  wkappa = 0 ;
  wdiel= 0 ;
  watompot = 0 ;
  rchg = 0 ; //!Reads charge data from iapbs-charge.dx in DX format
  rkappa = 0 ; //!Reads the ion-accessibility kappa map from iapbs-kappa.dx in DX format
  rdiel = 0 ; //!Reads dielectric maps from iapbs-diel[x,y,z].dx in DX format
  rpot = 0 ;

  nion = 0;
  for(int i=0;i<MAXION;i++){
    ionq[i] = 0.0;
    ionc[i] = 0.0;
    ionrr[i] = 0.0;
  }

  ofrac = 0.3;

  apbs_debug = 3;
  loop = 1;

  cout<<"Hardcoded PQR Data ..."<<endl;
  x[0]=0.0190; y[0]=0.0000; z[0]=0.6556; charge[0]=0.2700; radius[0]=1.8875;
  x[1]=-0.1197; y[1]=0.0000; z[1]=-0.7372; charge[1]=-0.7000; radius[1]=1.5350;
  x[2]=0.7370; y[2]=0.0000; z[2]=-1.1390; charge[2]=0.4300; radius[2]=0.2000;
  natom=3;

  maxx = x[0];
  minx = x[0];
  maxy = y[0];
  miny = y[0];
  maxz = z[0];
  minz = z[0];
  for(int i=0;i<natom;i++){
    if(maxx < x[i]+radius[i]) 
      maxx = x[i]+radius[i];
    if(minx > x[i]-radius[i]) 
      minx = x[i]-radius[i];
    if(maxy < y[i]+radius[i])
      maxy = y[i]+radius[i];
    if(miny > y[i]-radius[i])
      miny = y[i]-radius[i];
    if(maxz < z[i]+radius[i])
      maxz = z[i]+radius[i];
    if(minz > z[i]-radius[i])
      minz = z[i]-radius[i];
  }

  cout<<"Mol. dimensions: "<<maxx-minx<<" "<<maxy-miny<<" "<<maxz-minz<<endl;

  //if we are doing mg-auto calculate recommended grid values
  //including dime, if not specified

  if(((calc_type == 0) || (calc_type == 1)) && dime[0] == 0){
    cglen[0] = 1.7 * (maxx-minx);
    cglen[1] = 1.7 * (maxy-miny);
    cglen[2] = 1.7 * (maxz-minz);
    fglen[0] = 20.0 + (maxx-minx);
    fglen[1] = 20.0 + (maxy-miny);
    fglen[2] = 20.0 + (maxz-minz);

    for(int i=0;i<3;i++){
      if(fglen[i] > cglen[i])
        cglen[i] = fglen[i];
    }

    if (dime[0] == 0 ){
      cout<<"Grid dime not specified, calculating ..."<<endl;
      for(int i=0;i<3;i++){
        dime[i] = 32*(int((int(fglen[i]/grid[i]+0.5)-1)/32 + 0.5))+ 1;
        if (dime[i] < 33) 
          dime[i] = 33;
      }
    }

    cout<<"Grid values: "<<endl;
    cout<<"fglen: "<<" "<<fglen[0]<<" "<<fglen[1]<<" "<<fglen[2]<<endl;
    cout<<"cglen: "<<" "<<cglen[0]<<" "<<cglen[1]<<" "<<cglen[2]<<endl;
    cout<<"dime: "<<" "<<dime[0]<<" "<<dime[1]<<" "<<dime[2]<<endl;
    cout<<"grid: "<<grid[0]<<" "<<grid[1]<<" "<<grid[2]<<endl;
    cout<<"Required memory (in MB): "<<dime[0]*dime[1]*dime[2]*200.0/1024.0/1024.0<<endl;
  }

  i_param[0] = calc_type;
  i_param[1] = nlev ;
  i_param[2] = cmeth;
  i_param[3] = ccmeth;
  i_param[4] = fcmeth;
  i_param[5] = chgm;
  i_param[6] = nonlin;
  i_param[7] = bcfl;
  i_param[8] = srfm;
  i_param[9] = calcenergy;
  i_param[10] = calcforce;
  i_param[11] = wpot;
  i_param[12] = wchg;
  i_param[13] = wsmol;
  i_param[14] = wkappa;
  i_param[15] = wdiel;
  i_param[16] = watompot;
  i_param[17] = rpot;
  i_param[18] = 0;
  i_param[19] = calcnpforce;
  i_param[20] = calcnpenergy;
  i_param[21] = nion;
  i_param[22] = rchg;
  i_param[23] = rkappa;
  i_param[24] = rdiel;

  r_param[0] = pdie;
  r_param[1] = sdie;
  r_param[2] = srad;
  r_param[3] = swin;
  r_param[4] = temp;
  r_param[5] = sdens;
  r_param[6] = gamma;
  r_param[7] = smvolume;
  r_param[8] = smsize;

  //more intialization
  for(int i=0;i<natom;i++){
         apbsdx[i] = 0.0;
         apbsdy[i] = 0.0;
         apbsdz[i] = 0.0;
         apbsqfx[i] = 0.0;
         apbsqfy[i] = 0.0;
         apbsqfz[i] = 0.0;
         apbsibx[i] = 0.0;
         apbsiby[i] = 0.0;
         apbsibz[i] = 0.0;
         apbsdbx[i] = 0.0;
         apbsdby[i] = 0.0;
         apbsdbz[i] = 0.0;
         apbsnpx[i] = 0.0;
         apbsnpy[i] = 0.0;
         apbsnpz[i] = 0.0;
  }

  esenergy[0] = 0.0; //WATCH OUT FOR THIS
  npenergy[0] = 0.0;

  //OK, now we are ready to call the apbs_driver and start the show
  for(int j=0;j<loop;j++){
      rc = apbsdrv_(&natom,x,y,z,radius,charge,r_param,i_param,grid,dime,
           pdime, glen, center, cglen, fglen,
           ccenter, fcenter, &ofrac, &apbs_debug,
           ionq, ionc, ionrr,
           esenergy,npenergy,
           apbsdx, apbsdy, apbsdz,
           apbsqfx, apbsqfy, apbsqfz,
           apbsibx, apbsiby, apbsibz,
           apbsnpx, apbsnpy, apbsnpz,
           apbsdbx, apbsdby, apbsdbz,
           apbsgrid_meta, &apbsgrid);
  }

  cout<<"ElecEnergy :"<<esenergy[0]<<endl;
  cout<<"NP Energy :"<<npenergy[0]<<endl;
  return true;
}
// =========================================================================
