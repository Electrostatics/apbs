//
//  Constants.cpp
//  pbam
//
//  Created by David Brookes on 9/22/15.
//  Copyright © 2015 David Brookes. All rights reserved.
//

#include "Constants.h"

// [F/m]=[C2/J/m] , F = Farad:
const double Constants::PERMITTIVITY_VAC = 8.854187817e-12;
const double Constants::KB = 1.380658e-23; //!<  [ m^2 kg/ s^2 / K ] = [ J/K ]
const double Constants::LITRE = 1e-3;  // [ m^3/L]
const double Constants::PI = M_PI;
const double Constants::COLOUMB_CONSTANT = 8.988e9;  //!< [ N*m^2/C^2 ]
const double Constants::ELECTRON_CHARGE = 1.60217733e-19;  //!<  [ coulombs ]
const double Constants::E2 = 1.60217733e-19 * 1.60217733e-19;
const double Constants::AVOGADRO_NUM = 6.02209e23;  //[C2]
const double Constants::KCAL = 4184.0;  //!<  [ 1 kCal = 4184 Joules ]
const double Constants::ANGSTROM = 1e-10;  //!<  [ 1A = 1e-10 Meters ]
const double Constants::PICO_SEC = 1e-12;  //!<  [ 1 ps = 1e-12 s ]
const double Constants::MAX_DIST = 1.4e8;
const double Constants::FORCE_CUTOFF = 1e2;

const int Constants::MAX_NUM_POLES = 30;

/*
 Constructor sets default values of independent constants
 */
Constants::Constants(Units units)
:bDist_(100.0), qDist_(500.0), fDist_(100.0), dielectricWater_(78.0),
dielectricProt_(4.0), saltConcentration_(0.0100), temp_(353.0), tol_(2.5),
patchAngle_(6.0), rotateAngle_(20.0), units_(units)
{
	update_all();
}

/*
 Constructor given a Setup object:
 */
Constants::Constants(Setup setup)
:bDist_(100.0), qDist_(500.0), fDist_(100.0), dielectricWater_(78.0),
dielectricProt_(4.0), saltConcentration_(0.0100), temp_(353.0), tol_(2.5),
patchAngle_(6.0), rotateAngle_(20.0)
{
  set_units(setup.getUnits());
  set_dielectric_prot(setup.getIDiel());
  set_dielectric_water(setup.getSDiel());
  set_salt_concentration(setup.getSaltConc());
  set_temp(setup.getTemp());
  update_all();
}

Constants::Constants(Constants &consts)
:bDist_(consts.get_b_dist()), qDist_(consts.get_q_dist()),
fDist_(consts.get_f_dist()), dielectricWater_(consts.get_dielectric_water()),
dielectricProt_(consts.get_dielectric_prot()),
saltConcentration_(consts.get_salt_concentration()),
temp_(consts.get_temp()), tol_(consts.get_tol()),
patchAngle_(consts.get_patch_angle()), rotateAngle_(consts.get_rotate_angle()),
lambda_(consts.get_lambda()), KbT_(consts.get_kbt()), iKbT_(consts.get_ikbt()),
kappa_(consts.get_kappa()), patchSize_(consts.get_patch_size()),
rotateSize_(consts.get_rotate_size()), units_(consts.get_unitsEnum())
{
}

void Constants::update_kbt()
{
  KbT_ = KB * temp_;
  iKbT_ = 1 / KbT_;
}

void Constants::update_kappa()
{
  double kap_num = sqrt(2 * saltConcentration_ * AVOGADRO_NUM * E2);
  double kap_den = sqrt(LITRE * dielectricWater_ *
                        PERMITTIVITY_VAC * KB * temp_);
    
  kappa_ = ANGSTROM * kap_num / kap_den;
}

void Constants::update_patch_size()
{
  patchSize_ = cos((patchAngle_ * PI) / 180.0);
}

void Constants::update_rotate_size()
{
  patchSize_ = cos((rotateAngle_ * PI) / 180.0);
}

void Constants::update_all()
{
  update_kappa();
  update_rotate_size();
  update_kbt();
  update_rotate_size();
}

void Constants::set_units( string units )
{
  units_ = INTERNAL;
  
  if (units == "kcalmol")   units_ = KCALMOL;
  else if (units == "jmol") units_ = JMOL;
  else if (units == "kT")   units_ = kT;
}

string Constants::get_units( )
{
  if (units_ == KCALMOL) return "kcalmol" ;
  else if (units_ == JMOL) return "jmol";
  else if (units_ == kT) return "kT";
  return "internal";
}

const double Constants::get_conv_factor()
{
  double conversion_factor = 1.0;
  
  if (units_ == KCALMOL)   conversion_factor = convert_int_to_kcal_mol(1.0);
  else if (units_ == JMOL) conversion_factor = convert_int_to_jmol(1.0);
  else if (units_ == kT)   conversion_factor = convert_int_to_kT(1.0);
  
  return conversion_factor;
}

const double Constants::convert_int_to_kcal_mol(double val)
{
  double coul_num = E2 * AVOGADRO_NUM;
  double coul_den = PERMITTIVITY_VAC * 4.0 * PI * ANGSTROM * KCAL;
  return val * (coul_num / coul_den );
}

const double Constants::convert_int_to_jmol(double val)
{
  double coul_num = E2 * AVOGADRO_NUM;
  double intj_den = PERMITTIVITY_VAC * 4.0 * PI * ANGSTROM; //IU units density
  return val * (coul_num / intj_den);
}

const double Constants::convert_j_to_int(double val)
{
  double coul_num = E2;
  double intj_den = PERMITTIVITY_VAC * 4.0 * PI * ANGSTROM; //IU units density
  return val / (coul_num / intj_den);
}

const double Constants::convert_int_to_kT(double val)
{
  double intkCal_mol = convert_int_to_kcal_mol( val );
  double iKT_kCal = iKbT_ * ( KCAL / AVOGADRO_NUM );
  return val * intkCal_mol * iKT_kCal;
}
