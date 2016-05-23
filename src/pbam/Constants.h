//
//  Constants.h
//  pb_solvers
/*
 Copyright (c) 2015, Teresa Head-Gordon, Lisa Felberg, Enghui Yap, David Brookes
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of UC Berkeley nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "setup.h"

enum Units { INTERNAL, KCALMOL, JMOL, kT };

/**
 Class for storing all relevant constants. Many have default values that are 
 defined inline.
 Others are dependent on each and are defined in the constructor. 
 Absolute constants (i.e. those with values that will never change, are const 
 and public)
 */
class Constants
{
protected:
    
  //Independent constants:
  double bDist_;  //!< Initial distance between 2 proteins for BD run
  double qDist_;  //!< Distance for molecules to be considered escaped
  double fDist_;  //!< Cutoff for protein force interactions
  double dielectricWater_;  //!< The dielectric constant of water (solvent, sDiel)
  double dielectricProt_;  //!< Dielectric constant of the protein (internal, iDiel)
  double saltConcentration_;  //!< [ Molar ]
  double temp_;  //!<  [ Kelvin ]
  double tol_;
  double patchAngle_;
  double rotateAngle_;
  double lambda_; //uniform scaling 
  
  //Dependent constants:
  double KbT_;  //!< [kCal/mol] = [J/K]*[#/mol]*[K]*[kCal/J]
  double iKbT_;  //!<  1/KbT = [kCal/mol]-1
  double kappa_;  //!< Inverse debye length [1/A]
  double patchSize_;  //(cos(PATCH_ANGLE * M_PI / 180.0))
  double rotateSize_;  //(cos(ROTATE_ANGLE * M_PI / 180.0))
  
  // Enum for units of the system
  Units units_;
    
public:
    
  static const int MAX_NUM_POLES;
  
  //Absolute constants:
  static const double PERMITTIVITY_VAC;  // [F/m]=[C2/J/m] , F = Farad
  static const double KB; //!<  [ m^2 kg/ s^2 / K ] = [ J/K ]
  static const double LITRE; // [ m^3/L]
  static const double PI;
  static const double COLOUMB_CONSTANT; //!< [ N*m^2/C^2 ]
  static const double ELECTRON_CHARGE; //!<  [ coulombs ]
  static const double E2;
  static const double AVOGADRO_NUM;  //[C2]
  static const double KCAL; //!<  [ 1 kCal = 4184 Joules ]
  static const double ANGSTROM; //!<  [ 1A = 1e-10 Meters ]
  static const double PICO_SEC;  //!<  [ 1 ps = 1e-12 s ]
  static const double MAX_DIST;  // maximum distance for cutoff, box length
  static const double FORCE_CUTOFF;  // default distance for cutoff
  
  Constants(Units units = INTERNAL);
  Constants(Setup setup);
  Constants(Constants &other);
  
  //The methods below update dependent constants (called when others are set in
  //setter methods)
  void update_kbt();
  void update_kappa();
  void update_patch_size();
  void update_rotate_size();
  void update_all(); // calls all of the above methods
  
  void set_units( string units );
  
  //Setter methods. Some call update methods if necessary
  void set_b_dist(double val)                 { bDist_ = val; }
  void set_q_dist(double val)                 { qDist_ = val; }
  void set_f_dist(double val)                 { fDist_ = val; }
  void set_dielectric_water(double val)
  {
    dielectricWater_ = val;
    update_kappa();
  }
  void set_dielectric_prot(double val)        { dielectricProt_ = val; }
  void set_salt_concentration(double val)
  {
    saltConcentration_ = val;
    update_kappa();
  }
  void set_temp(double val)
  {
    temp_ = val;
    update_kappa(); update_kbt();
  }
  void set_tol(double val)                    { tol_ = val; }
  void set_patch_angle(double val)
  {
    patchAngle_ = val;
    update_patch_size();
  }
  void set_rotate_angle(double val)
  {
    rotateAngle_ = val;
    update_rotate_size();
  }
  
  const double get_b_dist() const             { return bDist_; }
  const double get_q_dist() const             { return qDist_; }
  const double get_f_dist() const             { return fDist_; }
  const double get_dielectric_water() const   { return dielectricWater_; }
  const double get_dielectric_prot()  const   { return dielectricProt_; }
  const double get_salt_concentration() const { return saltConcentration_; }
  const double get_temp() const               { return temp_; }
  const double get_tol() const                { return tol_; }
  const double get_kbt() const                { return KbT_; }
  const double get_ikbt() const               { return iKbT_; }
  const double get_kappa() const              { return kappa_; }
  const double get_patch_size() const         { return patchSize_; }
  const double get_rotate_size() const        { return rotateSize_; }
  const double get_patch_angle() const        { return patchAngle_; }
  const double get_rotate_angle() const       { return rotateAngle_; }
  const double get_lambda() const             { return lambda_; }
  const Units get_unitsEnum() const                    { return units_; }
  string get_units();
  
  const double get_conv_factor();
  
  //convert a value from international units to mol units:
  static const double convert_int_to_kcal_mol(double val);
  static const double convert_int_to_jmol(double val);
  static const double convert_j_to_int(double val);
  const double convert_int_to_kT(double val);
  
};

#endif
