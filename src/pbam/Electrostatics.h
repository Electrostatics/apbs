//
//  Electrostatics.h
//  pb_solvers_code
//
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
#ifndef Electrostatics_h
#define Electrostatics_h

#include "ASolver.h"
#include <time.h> 
//#include <omp.h>



// Exception class to ensure that molecule
class ValueOutOfRange: public exception
{
protected:
  string ax_;
  double value_;
  double range_;
  
public:
  ValueOutOfRange( string ax, double val, double ran)
  :ax_(ax), value_(val), range_(ran)
  {
  }
  
  virtual const char* what() const throw()
  {
    string ss;
    ss = ax_ + " value " + to_string(value_)+ " out of range. It is";
    if (value_ < range_)
      ss += " less than ";
    else
      ss += " greater than ";
    ss += to_string(range_);
    return ss.c_str();
  }
};

/*
 Class for printing out electrostatics of system
 */
class Electrostatic
{
protected:
  int p_; // Npoles
  double units_; // A conversion factor to user desired units
  
  double pot_min_; // A minimum value of the pot
  double pot_max_; // A max value of the potential
  
  double lam_; // Average radius of molecules in system
  
  vector<double> range_min_;  // Origin of grid in each dim
  vector<double> range_max_;  // Origin of grid in each dim
  vector<int> npts_;   // number of grid pts in each dimension
  vector<double> step_;  // step of grid in each dimension

  vector<vector<vector<double > > > esp_; // vector of ESP values
  vector<vector<double > > grid_;  // 2D cross section of ESP
  
  shared_ptr<VecOfMats<cmplx>::type> _A_;
  shared_ptr<System> _sys_;
  shared_ptr<SHCalc> _shCalc_;
  shared_ptr<BesselCalc> _bCalc_;
  shared_ptr<Constants> _consts_;
  
  void find_range();
  void find_bins();
  
  void compute_units();
  
  void compute_pot();
  double compute_pot_at( Pt point );
  
  MyMatrix<cmplx> get_local_exp( Pt dist );
  
  double lotan_inner_prod(MyMatrix<cmplx> U, MyMatrix<cmplx> V, int p);
  
public:
  Electrostatic(shared_ptr<VecOfMats<cmplx>::type> _A, shared_ptr<System> _sys,
                shared_ptr<SHCalc> _shCalc, shared_ptr<BesselCalc> _bCalc,
                shared_ptr<Constants> _consts,
                int p, int npts = 150);
  
  Electrostatic(shared_ptr<ASolver> _asolv, int npts=150);
  
  // print APBS file
  void print_dx(string ifname);
  
  // print out 3D heatmap data for surface of each sphere
  void print_3d_heat( string td_name );

  // print Grid file, given an axis and a value on that axis
  void print_grid(string axis, double value, string fname);
  
  // return potential grid
  vector<vector<vector<double > > > get_potential() { return esp_; }
  vector<vector<double > > get_pot2d()              { return grid_; }
  
  vector<double> get_mins()  { return range_min_; }
  vector<double> get_maxs()  { return range_max_; }
  vector<int> get_npts()     { return npts_; }
  vector<double> get_bins()  { return step_; }
  
};


#endif /* Electrostatics_h */
