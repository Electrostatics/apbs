//
//  EnergyForce.h
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

#ifndef EnergyForce_h
#define EnergyForce_h

#include <stdio.h>
#include <memory>
#include "ASolver.h"
//#include <omp.h>

using namespace std;

/*
 Class for calculating the energy of molecules in the system given
 an ASolver object
 */
class EnergyCalc
{
protected:
  shared_ptr<VecOfMats<cmplx>::type> _A_;
  shared_ptr<VecOfMats<cmplx>::type> _L_;
  int N_;  // number of molecules
  int p_;  // max number of poles
  shared_ptr<Constants> _const_;

  // result of energy calculation, internal units
  shared_ptr<vector<double > > _omega_;
public:
  EnergyCalc() { }
  
  EnergyCalc(shared_ptr<VecOfMats<cmplx>::type> _A,
             shared_ptr<VecOfMats<cmplx>::type> _L,
             shared_ptr<Constants> _const, int N, int p);
  
  EnergyCalc(shared_ptr<ASolver> _asolv);
  
//  EnergyCalc(ASolver asolv, Constants consts, int p);
  
  // fill omega_
  void calc_energy();
  
  // calculate energy of one molecule
  double calc_ei(int i);
  
  // get the energy for a specific molecule:
  double get_omega_i_int(int i)  { return _omega_->operator[](i); }
  // get all energy:
  shared_ptr<vector<double> > get_omega_int() { return _omega_; }
  
  // energy in kCal/mol:
  double get_omega_i_kcal(int i)
  { return _const_->convert_int_to_kcal_mol(_omega_->operator[](i)); }
  
  MyVector<double> get_omega_kcal()
  {
    MyVector<double> omeg(N_);
    for (int n = 0; n < N_; n++)
      omeg[n] = _const_->convert_int_to_kcal_mol(_omega_->operator[](n));
    return omeg;
  }
  
  // energy in kT:
  double get_omega_i_kT(int i)
  { return _const_->convert_int_to_kT(_omega_->operator[](i)); }
  MyVector<double> get_omega_kT()
  {
    MyVector<double> omeg(N_);
    for (int n = 0; n < N_; n++)
      omeg[n] = _const_->convert_int_to_kT(_omega_->operator[](n));
    return omeg;
  }
  
  // energy in joules/mol:
  double get_omega_i_jmol(int i)
  { return _const_->convert_int_to_jmol(_omega_->operator[](i)); }
  MyVector<double> get_omega_jmol()
  {
    MyVector<double> omeg(N_);
    for (int n = 0; n < N_; n++)
      omeg[n] = _const_->convert_int_to_jmol(_omega_->operator[](n));
    return omeg;
  }
};

/*
 Class for calculating the forces on molecules in the system given
 an ASolver object
 */
class ForceCalc
{
protected:
  shared_ptr<VecOfMats<cmplx>::type> _A_;
  shared_ptr<VecOfMats<cmplx>::type> _L_;
  
  shared_ptr< MyMatrix<VecOfMats<cmplx>::type > > _gradA_;
  shared_ptr< MyVector<VecOfMats<cmplx>::type > > _gradL_;
  
  double epsS_;
  int N_;
  int p_;
  shared_ptr<Constants> _const_;
  
//  shared_ptr<VecOfVecs<double>::type> _F_;
  shared_ptr<vector<Pt> > _F_;
  
public:
  ForceCalc() { }
  
  ForceCalc(shared_ptr<VecOfMats<cmplx>::type> _A,
            shared_ptr<MyMatrix<VecOfMats<cmplx>::type > > _gradA,
            shared_ptr<VecOfMats<cmplx>::type> _L,
            shared_ptr<MyVector<VecOfMats<cmplx>::type > > _gradL,
            shared_ptr<Constants> con, int N, int p);
  
  ForceCalc(shared_ptr<ASolver> _asolv);
  
  void calc_force();  // fill F_
  
  // calculate force on one molecule
  Pt calc_fi(int i);
  
  Pt get_fi(int i)     { return (*_F_)[i]; }
  shared_ptr<vector<Pt> > get_F()    { return _F_; }
  
};

/*
 Class for calculating the torque on every molecule in the system
 */
class TorqueCalc
{
protected:
  
  // outer vector has an entry for every molecule. Inner vector is the torque
  // on that molecule
  shared_ptr<vector<Pt> > _tau_;
  
  shared_ptr<SHCalc> _shCalc_;
  shared_ptr<BesselCalc> _bCalc_;
  shared_ptr< MyVector<VecOfMats<cmplx>::type > > _gradL_;
  
  shared_ptr<Constants> _consts_;
  shared_ptr<System> _sys_;
  shared_ptr<VecOfMats<cmplx>::type> _gamma_;
  
//  double epsS_;
  int N_;
  int p_;

//  /*
//   Calculate H vector (eq 42 and 43 in Lotan 2006)
//   */
//  VecOfMats<cmplx>::type calc_H(int i);
  
public:
  TorqueCalc() { }
  
  TorqueCalc(shared_ptr<SHCalc> _shCalc,
             shared_ptr<BesselCalc> _bCalc,
             shared_ptr<MyVector<VecOfMats<cmplx>::type> > _gradL,
             shared_ptr<VecOfMats<cmplx>::type> _gamma,
             shared_ptr<Constants> _consts,
             shared_ptr<System> sys, int p);
  
  TorqueCalc(shared_ptr<ASolver> _asolv);
  
  void calc_tau();  // fill tau_
  
  // calculate torque on one molecule
  Pt calc_tau_i(int i);
  
  /*
   Calculate H vector (eq 42 and 43 in Lotan 2006)
   */
  VecOfMats<cmplx>::type calc_H(int i);
  
  Pt get_taui(int i)     { return _tau_->operator[](i); }
  shared_ptr<vector<Pt> > get_Tau()    { return _tau_; }
  
  /*
   Calculate inner product of two matrices as defined in equation 29 of Lotan
   2006
   */
  double lotan_inner_prod(MyMatrix<cmplx> U, MyMatrix<cmplx> V, int p)
  {
    double ip = 0;
    int n, m, mT;
    for (n = 0; n < p; n++)
    {
      for (m = -n; m <= n; m++)
      {
        mT = (m < 0) ? -1*m : m;
        ip += U(n, mT+p_).real()*V(n, mT+p_).real()
               + U(n, mT+p_).imag()*V(n, mT+p_).imag();
      }
    }
    return ip;
  }
  
};


class ThreeBody
{
protected:
  int N_; // number of molecules in the system
  int p_; // number of poles
  
  double cutoffTBD_;  // distance for cutoff of tbd approx
  
  Units unit_; // String value of units
  
  vector<vector<int > > dimer_;   // list of all pairs by their index #
  vector<vector<int > > trimer_;  // list of all triplets by their index #
  
  vector<vector< double > > energy_di_;
  vector<vector< double > > energy_tri_;
  
  vector<vector< Pt > > force_di_;
  vector<vector< Pt > > force_tri_;
  
  vector<vector< Pt > > torque_di_;
  vector<vector< Pt > > torque_tri_;
  
  shared_ptr<vector<double> > energy_approx_;
  shared_ptr<vector< Pt > >  force_approx_;
  shared_ptr<vector< Pt > >  torque_approx_;
  
  shared_ptr<BesselCalc>      _besselCalc_;
  shared_ptr<System>          _sys_;  // system data (radii, charges, etc.)
  shared_ptr<SHCalc>          _shCalc_;
  shared_ptr<Constants>       _consts_;
  
  string outfname_;
  
  shared_ptr<System> make_subsystem(vector<int> mol_idx);
  
  int find_di( int i, int j);
  void generatePairsTrips();
  
public:
  ThreeBody(shared_ptr<ASolver> _asolver, Units unt = INTERNAL,
            double cutoff = 1e48 );
  
  // Solve the N body problem, only 2 or 3 right now
  void solveNmer( int num, double preclim = 1e-4);
//  void solveNmerParallel( int num, double preclim = 1e-4);
  
  void printNmer( int num, string outfile);
  
  void calcTBDEnForTor( );
  void calcTwoBDEnForTor( );
  
  void printTBDEnForTor( vector<string> outfile );
  
  vector<vector<int > > getDimers()  { return dimer_; }
  vector<vector<int > > getTrimers() { return trimer_; }
  
  vector<vector<double > > getDiEn() { return energy_di_; }
  vector<vector<double > > getTrEn() { return energy_tri_; }
  
  vector<vector<Pt > > getDiFo()     { return force_di_; }
  vector<vector<Pt > > getTrFo()     { return force_tri_; }
  
  vector<vector<Pt > > getDiTo()     { return torque_di_; }
  vector<vector<Pt > > getTrTo()     { return torque_tri_; }
  
  shared_ptr<vector<double> > get_energy_approx() { return energy_approx_; }
  shared_ptr<vector<Pt> > get_force_approx()      { return force_approx_; }
  shared_ptr<vector<Pt> > get_torque_approx()     { return torque_approx_; }
  
  double get_energyi_approx( int i) { return (*energy_approx_)[i]; }
  Pt get_forcei_approx( int i)      { return (*force_approx_)[i]; }
  Pt get_torquei_approx( int i)     { return (*torque_approx_)[i]; }
  
};

/*
 Base class for calculations of physical quantities
 */
class BasePhysCalc
{
public:
  BasePhysCalc() { }
  
  virtual void calc_force() {}
  virtual void calc_energy() {}
  virtual void calc_torque() {}
  
  virtual void print_all() { }
  
  virtual shared_ptr<vector<Pt> > get_Tau()
    { return make_shared<vector<Pt> > ();  }
  virtual shared_ptr<vector<Pt> > get_F()
    { return make_shared<vector<Pt> > ();   }
  virtual shared_ptr<vector<double> > get_omega()
    { return make_shared<vector<double> > (); }
  
  virtual Pt get_taui(int i) { return Pt(); }
  virtual Pt get_forcei(int i) { return Pt (); }
  virtual double get_omegai(int i) {return 0; }
  
  virtual Pt get_moli_pos(int i) { return Pt(); }
  
};

/*
 Class for calculating energy force and torque in one place
 */
class PhysCalc : public BasePhysCalc
{
protected:
  int N_; // number of particles
  double unit_conv_; // Conversion factor for units
  string unit_; // String of the type of units
  shared_ptr<System> _sys_; // System
  
  shared_ptr<EnergyCalc> _eCalc_;
  shared_ptr<ForceCalc> _fCalc_;
  shared_ptr<TorqueCalc> _torCalc_;
  
  string outfname_; // where you want the info printed to
  
  void compute_units( shared_ptr<Constants> cst, Units unit);
  
public:
  
  // constructor just requires an asolver
  PhysCalc(shared_ptr<ASolver> _asolv, string outfname, Units unit = INTERNAL);
  
  Pt calc_force_i(int i)  { return _fCalc_->calc_fi(i); }
  Pt calc_tau_i(int i)    { return _torCalc_->calc_tau_i(i); }
  MyVector<double> calc_ei(int i)       { return _eCalc_->calc_ei(i); }
  
  void calc_force()   { _fCalc_->calc_force(); }
  void calc_energy()  { _eCalc_->calc_energy(); }
  void calc_torque()  { _torCalc_->calc_tau(); }
  void calc_all()     { calc_energy(); calc_force(); calc_torque(); }
  
  void print_all();
  
  shared_ptr<vector<Pt> > get_Tau() { return _torCalc_->get_Tau();}
  shared_ptr<vector<Pt> > get_F()    { return _fCalc_->get_F();}
  shared_ptr<vector<double> > get_omega() {return _eCalc_->get_omega_int();}
  
  Pt get_taui(int i) { return _torCalc_->get_taui(i); }
  Pt get_forcei(int i) { return _fCalc_->get_fi(i); }
  double get_omegai(int i) {return _eCalc_->get_omega_i_int(i);}
  
  Pt get_taui_conv(int i)
  { return _torCalc_->get_taui(i)*unit_conv_; }
  Pt get_forcei_conv(int i)
  { return _fCalc_->get_fi(i)*unit_conv_; }
  double get_omegai_conv(int i)
  {return _eCalc_->get_omega_i_int(i)*unit_conv_;}
  
  Pt get_moli_pos( int i) { return _sys_->get_centeri(i); }

};

class ThreeBodyPhysCalc : public BasePhysCalc, ThreeBody
{
protected:
//  shared_ptr<ThreeBody> _threeBody_;
  bool solved_;  // whether the three body problem has been solved
  int num_;  // number of bodies (2 or 3 right now)
  string outfname_;
  
public:
  ThreeBodyPhysCalc(shared_ptr<ASolver> _asolv, int num=3, string outfname = "",
                    Units unit = INTERNAL, double cutoff=1e48);
  
  void calc_force() { if (!solved_) solveNmer(num_); solved_ = true; }
  void calc_energy() { if (!solved_) solveNmer(num_); solved_ = true; }
  void calc_torque() { if (!solved_) solveNmer(num_); solved_ = true; }
  
//  void print_all() { }
  
  virtual shared_ptr<vector<Pt> > get_Tau()
  { return get_torque_approx(); }
  virtual shared_ptr<vector<Pt> > get_F()
  { return get_force_approx();   }
  virtual shared_ptr<vector<double> > get_omega()
  { return get_energy_approx(); }
  
  virtual Pt get_taui(int i) { return get_torquei_approx(i); }
  virtual Pt get_forcei(int i) { return get_forcei_approx(i); }
  virtual double get_omegai(int i) {return get_energyi_approx(i); }
  
};

#endif /* EnergyForce_h */
