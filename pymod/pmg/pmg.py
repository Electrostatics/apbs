import sys
sys.path.insert(0, '..')
from vtypes import *

import numpy as np

'''

Port of class Vpmg.

'''

class PMG:
    '''
  Vmem *vmem;  /**< Memory management object for this class
  Vpmgp *pmgp;  /**< Parameters
  Vpbe *pbe;  /**< Information about the PBE system

#ifdef BURY_FORTRAN
  Vpde *pde;            /**< @todo doc
  Vmgdriver *mgdriver;  /**< @todo doc
#endif

  double *epsx;  /**< X-shifted dielectric map
  double *epsy;  /**< Y-shifted dielectric map
  double *epsz;  /**< Y-shifted dielectric map
  double *kappa;  /**< Ion accessibility map (0 <= kappa(x) <= 1)
  double *pot;  /**< Potential map
  double *charge;  /**< Charge map

  int *iparm;  /**< Passing int parameters to FORTRAN
  double *rparm;  /**< Passing real parameters to FORTRAN
  int *iwork;  /**< Work array
  double *rwork;  /**< Work array
  double *a1cf;  /**< Operator coefficient values (a11) -- this array can be * overwritten
  double *a2cf;  /**< Operator coefficient values (a22) -- this array can be overwritten
  double *a3cf;  /**< Operator coefficient values (a33) -- this array can be overwritten
  double *ccf;  /**< Helmholtz term -- this array can be overwritten
  double *fcf;  /**< Right-hand side -- this array can be overwritten
  double *tcf;  /**< True solution
  double *u;  /**< Solution
  double *xf;  /**< Mesh point x coordinates
  double *yf;  /**< Mesh point y coordinates
  double *zf;  /**< Mesh point z coordinates
  double *gxcf;  /**< Boundary conditions for x faces
  double *gycf;  /**< Boundary conditions for y faces
  double *gzcf;  /**< Boundary conditions for z faces
  double *pvec;  /**< Partition mask array
  double extDiEnergy;  /**< Stores contributions to the dielectric energy from regions outside the problem domain
  double extQmEnergy;  /**< Stores contributions to the mobile ion energy from regions outside the problem domain
  double extQfEnergy;  /**< Stores contributions to the fixed charge energy from regions outside the problem domain
  double extNpEnergy;  /**< Stores contributions to the apolar energy from regions outside the problem domain
  Vsurf_Meth surfMeth;  /**< Surface definition method
  double splineWin;  /**< Spline window parm for surf defs
  Vchrg_Meth chargeMeth;  /**< Charge discretization method
  Vchrg_Src chargeSrc;  /**< Charge source

  int filled;  /**< Indicates whether Vpmg_fillco has been called

  int useDielXMap;  /**< Indicates whether Vpmg_fillco was called with an external x-shifted dielectric map
  Vgrid *dielXMap;  /**< External x-shifted dielectric map
  int useDielYMap;  /**< Indicates whether Vpmg_fillco was called with an external y-shifted dielectric map
  Vgrid *dielYMap;  /**< External y-shifted dielectric map
  int useDielZMap;  /**< Indicates whether Vpmg_fillco was called with an external z-shifted dielectric map
  Vgrid *dielZMap;  /**< External z-shifted dielectric map
  int useKappaMap;  /**< Indicates whether Vpmg_fillco was called with an
                     * external kappa map
  Vgrid *kappaMap;  /**< External kappa map
  int usePotMap;    /**< Indicates whether Vpmg_fillco was called with an
                       * external potential map
  Vgrid *potMap;    /**< External potential map

  int useChargeMap;  /**< Indicates whether Vpmg_fillco was called with an external charge distribution map
  Vgrid *chargeMap;  /**< External charge distribution map
    '''

    def __init__(self):
        self.mpgp:   MPGP
        self.epsx:   FloatVec 
        self.epsy:   FloatVec 
        self.epsz:   FloatVec 
        self.kappa:  FloatVec 
        self.pot:    FloatVec 
        self.charge: FloatVec 
