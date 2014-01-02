/**
***  Copyright (c) 1995, 1996, 1997, 1998, 1999, 2000 by
***  The Board of Trustees of the University of Illinois.
***  All rights reserved.
**/

/**
 *
 *  @file    GlobalMasterAPBS.h
 *  @author Justin Gullingsrud and Robert Konecny
 *  @brief NAMD/APBS module header file
 *
 *  $Revision: 265 $
 *  $Id: GlobalMasterAPBS.h 265 2009-05-05 04:21:00Z rok $
 *
 */


#ifdef NAMD_APBS

#ifndef GLOBALMASTERAPBS_H
#define GLOBALMASTERAPBS_H

#include "GlobalMaster.h"
class SubmitReduction;

class APBSParameters;

class GlobalMasterAPBS : public GlobalMaster {
 public: 
  /* initializes this according to the simulation parameters */
  GlobalMasterAPBS();
  ~GlobalMasterAPBS();

 protected:
  virtual void calculate();

 private:
  APBSParameters *params;
  SubmitReduction *reduction;
  int outputFreq;
  int step;
  int numAtoms;
  double *radii;
  double *charges;
  double *positionx, *positiony, *positionz;
  // returned force and energy components
  double solvent_elec_energy;
  double solvent_np_energy;
  double vacuum_elec_energy;
  double *solvent_forces[3];
  double *vacuum_forces[3];
  // space for other force components that we ignore
  double *qfForce[3];
  double *ibForce[3];
  double *dbForce[3];
  double *npForce[3];
  // in-memory grid data, ignored now
  double apbsgrid_meta[13];
  double *apbsgrid[0];

  // perform the actual APBS call.  If in_vacuum is true, ion concentrations
  // will be set to zero, and solvent dielectric will be set to the value of
  // the solute (protein) dielectric.
  void call_apbs(int in_vacuum);
};

#endif 

#endif NAMD_APBS
