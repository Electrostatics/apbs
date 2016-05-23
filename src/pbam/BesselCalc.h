//
//  BesselCalc.h
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

#ifndef BesselCalc_h
#define BesselCalc_h

#include <vector>
#include <assert.h>
#include <memory>

#include "Constants.h"

using namespace std;

/*
 Class for storing constants that can be used for multiple runs of 
  bessel calculations
 */
class BesselConstants
{
protected:
  int numVals_;
  vector<double> kConsts_;  // recursion constants for k
  
public:
  
  BesselConstants(const int N=Constants::MAX_NUM_POLES);
  
  const int get_n() const                     { return numVals_; }
  const double get_kconst_val(int i) const    { return kConsts_[i]; }
  
};

/*
 Calculator class for modified bessel functions (spherical and standard)
 */
class BesselCalc
{
protected:
  
  int                          numVals_;  // order of the Bessel function
  shared_ptr<BesselConstants>  _consts_;  // recursion constants Lotan 2006 eq3

public:
  BesselCalc() {}
  
  BesselCalc(int N, shared_ptr<BesselConstants> _consts);

  /*
  Calculate the modified sphereical bessel functions I and K 
  (MBF of the first and second kind, respectively).
  Input is desired number of iterations an output is a vector 
  containing the calculated value at every iteration
  */
  const vector<double> calc_mbfI(const int n, const double z) const;
  const vector<double> calc_mbfK(const int n, const double z) const;
  
  const int get_num_vals() const { return numVals_; }
  
};

#endif /* BesselCalc_h */
