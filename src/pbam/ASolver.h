//
//  ASolver.h
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

#ifndef ASolver_h
#define ASolver_h

#include "ReExpCalc.h"
#include "System.h"
#include <memory>

/*
 This class is designed to compute the vector A defined in Equation 22 
 Lotan 2006, page 544
 */
class ASolver
{
protected:
  
  shared_ptr<VecOfMats<cmplx>::type>      _A_, _prevA_;  // solution
  
  /*
   Gradient of A. Each (i, j) entry in the outer vector is grad_j(A^(i))
   The inner vectors are all of length three and represent dA/dr, dA/dtheta
   and dA/dphi, respectively. This can only be calculated once A has been
   solved for
   */
  shared_ptr<MyMatrix<VecOfMats<cmplx>::type > > _gradT_A_,_gradA_,_prevGradA_;
  
  /*
   enum for telling the ReExpCoeffs which values to retrieve
   in the below methods
   */
  enum WhichReEx { BASE, DDR, DDPHI, DDTHETA };
  
  bool                  solvedA_;  // Boolean of whether or not A is solved
  int                         N_;  // number of molecules
  int                         p_;  // max value for n (2*numVals_ usually)
  double                  a_avg_;  // the average radius of particles in syst

  shared_ptr<VecOfMats<cmplx>::type>      _gamma_, _delta_, _E_, _L_;
  shared_ptr<MyVector<VecOfMats<cmplx>::type > > _gradL_;
  shared_ptr<ReExpCoeffsConstants>        _reExpConsts_;
  shared_ptr<BesselCalc>      _besselCalc_;
  shared_ptr<System>          _sys_;  // system data (radii, charges, etc.)
  shared_ptr<SHCalc>          _shCalc_;
  shared_ptr<Constants>       _consts_;
  
  // re expansion coefficients calculated for every inter molecular vector
  MyMatrix<ReExpCoeffs>  T_;
  
  // pre-computed spherical harmonics matrices for every charge in the system
  // inner vector is all SH for all the charges in a molecule.
  // Outer vector is every molecule
  shared_ptr<vector<vector<MyMatrix<cmplx> > > > _allSh_;
  
  // calculate the SH for all charges in a molecule
  vector<MyMatrix<cmplx> > calc_mol_sh(Molecule mol);
  
  // calculate one index of inner gamma matrix
  cmplx calc_indi_gamma(int i, int n);
  
  // calculate on index of inner delta matrix
  cmplx calc_indi_delta(int i, int n);
  cmplx calc_indi_e(int i, int n, int m);
  
  void copy_to_prevA(); // copy contents of _A_ to _prevA_
  void copy_to_prevGradA();
  
  // pre-compute spherical harmonics matrices for every charge in the system
  void pre_compute_all_sh();
  
  // Compute the T matrix (re expansion coefficients for
  // every inter molecular vector)
  void compute_T();
  
  // compute the gamma matrix (as defined on page 544 of Lotan 2006):
  void compute_gamma();
  
  // compute the delta matrix (as defined on page 544 of Lotan 2006):
  void compute_delta();
  
  // compute the E vector (equations on page 543 of Lotan 2006)
  void compute_E();
  
  // initialize A vector
  void init_A();
  
  // inialize grad(A) matrix to the zero matrix
  void init_gradA();
  
  // re-expand element j of A with element (i, j) of T and return results
  // if prev=True then re-expand prevA
  MyMatrix<cmplx> re_expandA(int i, int j, bool prev=false);
  
  // re-expand element j of grad(A) with element (i, j) of T
  VecOfMats<cmplx>::type re_expand_gradA(int i,int j,int wrt,bool prev=false);
  
  // re-expand element j of A with element (i, j) of grad(T) and return results
  // uses eq 46 to solve eq 47 in Lotan 2006
  VecOfMats<cmplx>::type re_expandA_gradT(int i, int j, bool prev=false);
  
  // perform first part of T*A and return results (see eq 46 in Lotan 2006)
  // input wrt is only used if whichA is not BASE (then we need to know which
  // molecule the gradient is with respect to). If prev=True then expand
  // prevA (or prevGradA_)
  MyMatrix<cmplx> expand_RX(int i, int j, WhichReEx whichR,
                            WhichReEx whichA, bool prev, int wrt=-1);
  
  // perform first part of T*A and return results for singular A wrt THETA
  MyMatrix<cmplx> expand_dRdtheta_sing(int i, int j, double theta,
                                       MyMatrix<cmplx> mat, bool ham);
  MyMatrix<cmplx> expand_dRdtheta_sing(int i, int j, double theta, bool ham);
  
  // perform first part of T*A and return results for singular A wrt PHI
  MyMatrix<cmplx> expand_dRdphi_sing(int i, int j, double theta,
                                     MyMatrix<cmplx> mat, bool ham);
  MyMatrix<cmplx> expand_dRdphi_sing(int i, int j, double theta, bool ham);

  // perform second part of T*A and return results (see eq 46 in Lotan 2006)
  MyMatrix<cmplx> expand_SX(int i, int j, MyMatrix<cmplx> x1,
                            WhichReEx whichS);

  // perform third part of T*A and return results (see eq 46 in Lotan 2006)
  MyMatrix<cmplx> expand_RHX(int i, int j, MyMatrix<cmplx> x2,
                             WhichReEx whichRH);
  
  // precompute gradT times A(i,j) for all pairs of molecules
  void pre_compute_gradT_A();
  
  // convenience method for retrieving values from A and gradA (or their
  // previous values
  cmplx which_aval(WhichReEx whichA, bool prev, int i, int n,
                   int m, int wrt=-1);

  // perform one iteration of the solution for A (eq 51 in Lotan 2006)
  void iter();
  
  // perform one iterations of the solution for grad(A) (eq53 in Lotan 2006)
  void grad_iter(int j);
  
  // calculate the change in A_ from prevA_ (eq 52 in Lotan 2006)
  double calc_change(WhichReEx whichA=BASE, int wrt=-1);
  
  // sum of many calls to the above
  double calc_grad_change(int wrt);

public:
  
  ASolver() { }
  
  ASolver(shared_ptr<BesselCalc> _bcalc,
          shared_ptr<SHCalc> shCalc,
          shared_ptr<System> _sys,
          shared_ptr<Constants> _consts,
          const int p=Constants::MAX_NUM_POLES);
  
  shared_ptr<VecOfMats<cmplx>::type>  get_gamma() { return _gamma_; }
  shared_ptr<VecOfMats<cmplx>::type>  get_delta() { return _delta_; }
  shared_ptr<VecOfMats<cmplx>::type>  get_E()     { return _E_; }
  shared_ptr<VecOfMats<cmplx>::type>  get_A()     { return _A_; }
  shared_ptr<MyMatrix<VecOfMats<cmplx>::type > > get_gradA() {return _gradA_ ; }
  shared_ptr<MyVector<VecOfMats<cmplx>::type > > get_gradL() {return _gradL_ ; }
  shared_ptr<VecOfMats<cmplx>::type>  get_L() {return  _L_; }
  
  int get_p() { return p_; }
  int get_N() { return N_; }
  
  shared_ptr<BesselCalc> get_bessel() { return _besselCalc_; }
  shared_ptr<System> get_sys()        { return _sys_; }
  shared_ptr<SHCalc> get_sh()         { return _shCalc_; }
  shared_ptr<Constants> get_consts()  { return _consts_; }
  
  void calc_L();
  void calc_gradL();
  
  cmplx get_gamma_ni( int i, int n)       {return _gamma_->operator[](i)(n,n);}
  cmplx get_delta_ni( int i, int n)       {return _delta_->operator[](i)(n,n);}
  cmplx get_SH_ij(int i, int j, int n, int m)
  {return (*_allSh_)[i][j](n,abs(m));}
  cmplx get_E_ni(int i, int n, int m)     {return _E_->operator[](i)(n,m+p_);}
  cmplx get_A_ni(int i, int n, int m)     {return _A_->operator[](i)(n,m+p_);}
  cmplx get_prevA_ni(int i, int n, int m)
        {return _prevA_->operator[](i)(n,m+p_ ); }
  
  VecOfMats<cmplx>::type get_gradT_Aij( int i, int j)
        {return _gradT_A_->operator()(i,j);}
  
  // convert derivs in matrix to cartesian
  VecOfMats<cmplx>::type conv_to_cart(VecOfMats<cmplx>::type dZ, int i, int j);
  
  // get elements of grad_j(A^(i))
  cmplx get_dAdr_ni(int i, int j, int n, int m)
  { return _gradA_->operator()(i, j)[0](n, m+p_);}
  cmplx get_dAdtheta_ni(int i, int j, int n, int m)
  { return _gradA_->operator()(i, j)[1](n, m+p_);}
  cmplx get_dAdphi_ni(int i, int j, int n, int m)
  { return _gradA_->operator()(i, j)[2](n, m+p_);}
  
  cmplx get_prev_dAdr_ni(int i, int j, int n, int m)
  { return _prevGradA_->operator()(i, j)[0](n, m+p_);}
  cmplx get_prev_dAdtheta_ni(int i, int j, int n, int m)
  { return _prevGradA_->operator()(i, j)[1](n, m+p_);}
  cmplx get_prev_dAdphi_ni(int i, int j, int n, int m)
  { return _prevGradA_->operator()(i, j)[2](n, m+p_);}
  
  // get elements of grad_j(A^(i))
  cmplx get_dAdx_ni(int i, int j, int n, int m)
  { return _gradA_->operator()(i, j)[0](n, m+p_);}
  cmplx get_dAdy_ni(int i, int j, int n, int m)
  { return _gradA_->operator()(i, j)[1](n, m+p_);}
  cmplx get_dAdz_ni(int i, int j, int n, int m)
  { return _gradA_->operator()(i, j)[2](n, m+p_);}
  
  cmplx get_prev_dAdx_ni(int i, int j, int n, int m)
  { return _prevGradA_->operator()(i, j)[0](n, m+p_);}
  cmplx get_prev_dAdy_ni(int i, int j, int n, int m)
  { return _prevGradA_->operator()(i, j)[1](n, m+p_);}
  cmplx get_prev_dAdz_ni(int i, int j, int n, int m)
  { return _prevGradA_->operator()(i, j)[2](n, m+p_);}
  
  void set_A_ni(int i, int n, int m, cmplx val)
  {_A_->operator[](i).set_val( n, m+p_, val);}

  void print_Ei( int i, int p);
  void print_Ai( int i, int p);
  void print_dAidx( int i, int j, int p);
  void print_dAidy( int i, int j, int p);
  void print_dAidz( int i, int j, int p);
  void print_dAi( int i, int j, int p);

  //numerically solve for A given the desired precision
  void solve_A(double prec);
  
  // numerically solve for grad(A) given the desired precision
  // must solve for A before this
  void solve_gradA(double prec);
  
  /*
   Reset all relevant members given a new system
   */
  void reset_all(shared_ptr<System> _sys);
  
}; // End ASolver

#endif /* ASolver_h */
