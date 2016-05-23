//
//  EnergForce.cpp
//  pb_solvers_code
//
//  Created by David Brookes on 11/18/15.
//  Copyright © 2015 David Brookes. All rights reserved.
//

#include "EnergyForce.h"

EnergyCalc::EnergyCalc(shared_ptr<VecOfMats<cmplx>::type> _A,
                       shared_ptr<VecOfMats<cmplx>::type> _L,
                       shared_ptr<Constants> _const, int N, int p)
:N_(N), _const_(_const), p_(p), _A_(_A), _L_(_L)
{
  _omega_ = make_shared<vector<double> > (N_);
  calc_energy();
}

EnergyCalc::EnergyCalc(shared_ptr<ASolver> _asolv)
:_A_(_asolv->get_A()), _L_(_asolv->get_L()), _const_(_asolv->get_consts()),
N_(_asolv->get_N()), p_(_asolv->get_p())
{
  _omega_ = make_shared<vector<double> > (N_);
//  calc_energy();
}

double EnergyCalc::calc_ei(int i)
{
  double ei;
  int n, m;
  
  MyMatrix<cmplx> Li = _L_->operator[](i);
  MyMatrix<cmplx> Ai = _A_->operator[](i);
  
  // calculate inner product (as defined in eq 29 of Lotan 2006):
  ei = 0.0;
  for (n = 0; n < p_; n++)
  {
    for (m = -n; m <= n; m++)
    {
      cmplx unm, vnm;
      unm = Li(n, m + p_);
      vnm = Ai(n, m + p_);
      ei += unm.real()*vnm.real() + unm.imag()*vnm.imag();
    }
  }
  ei *= (1/_const_->get_dielectric_water());
  return ei;
}

void EnergyCalc::calc_energy()
{
  for (int i = 0; i < N_; i++)
  {
//    _omega_->set_val(i, calc_ei(i));
    (*_omega_)[i] = calc_ei(i);
  }
}

ForceCalc::ForceCalc(shared_ptr<VecOfMats<cmplx>::type> _A,
                     shared_ptr<MyMatrix<VecOfMats<cmplx>::type > > _gradA,
                     shared_ptr<VecOfMats<cmplx>::type> _L,
                     shared_ptr<MyVector<VecOfMats<cmplx>::type > > _gradL,
                     shared_ptr<Constants> _con, int N, int p)
:N_(N), _const_(_con), p_(p), _gradA_(_gradA), _A_(_A), _L_(_L),
_gradL_(_gradL)
{
  _F_ = make_shared<vector<Pt> > (N_, Pt());
//  calc_force();
}

ForceCalc::ForceCalc(shared_ptr<ASolver> _asolv)
:_A_(_asolv->get_A()), _gradA_(_asolv->get_gradA()), _L_(_asolv->get_L()),
_gradL_(_asolv->get_gradL()), _const_(_asolv->get_consts()),
N_(_asolv->get_N()), p_(_asolv->get_p())
{
  _F_ = make_shared<vector<Pt> > (N_);
//  calc_force();
}

Pt ForceCalc::calc_fi(int i)
{
  int j, n, m;
  cmplx unm1, vnm1, unm2, vnm2;
  double ip1, ip2, fij;
  VecOfMats<cmplx>::type gLi, gAi;
  MyMatrix<cmplx> Li, Ai;
  Pt fi;

  Li = _L_->operator[](i);
  Ai = _A_->operator[](i);
  
  gLi = _gradL_->operator[](i);
  gAi = _gradA_->operator()(i, i);
  fi = MyVector<double> (3);
  for (j = 0; j < 3; j++)  // for each component of the gradient
  {
    ip1 = 0.0;
    ip2 = 0.0;
    for (n = 0; n < p_; n++)
    {
      for (m = -n; m <= n; m++)
      {
        unm1 = gLi[j](n, m + p_);
        vnm1 = Ai(n, m + p_);
        ip1 += unm1.real()*vnm1.real() + unm1.imag()*vnm1.imag();
        
        unm2 = Li(n, m + p_);
        vnm2 = gAi[j](n, m + p_);
        ip2 += unm2.real()*vnm2.real() + unm2.imag()*vnm2.imag();
      }
    }
    fij = -1.0/_const_->get_dielectric_water() * (ip1 + ip2);
    if (j == 0) fi.set_x(fij);
    else if (j == 1) fi.set_y(fij);
    else if (j == 2) fi.set_z(fij);
  }
  return fi;

}


void ForceCalc::calc_force()
{
  for (int i = 0; i < N_; i++)
  {
    (*_F_)[i] = calc_fi(i);
  }
}

TorqueCalc::TorqueCalc(shared_ptr<SHCalc> _shCalc,
                       shared_ptr<BesselCalc> _bCalc,
                       shared_ptr<MyVector<VecOfMats<cmplx>::type> > _gradL,
                       shared_ptr<VecOfMats<cmplx>::type> _gamma,
                       shared_ptr<Constants> _consts,
                       shared_ptr<System> _sys, int p)
: N_(_sys->get_n()), p_(p), _consts_(_consts),
_shCalc_(_shCalc), _bCalc_(_bCalc), _gradL_(_gradL), _gamma_(_gamma)
{
  _tau_ = make_shared<vector<Pt> > (N_);
//  calc_tau();
}

TorqueCalc::TorqueCalc(shared_ptr<ASolver> _asolv)
:N_(_asolv->get_N()), p_(_asolv->get_p()),
_consts_(_asolv->get_consts()), _shCalc_(_asolv->get_sh()),
_bCalc_(_asolv->get_bessel()),
_gamma_(_asolv->get_gamma()),
_sys_(_asolv->get_sys()),
_gradL_(_asolv->get_gradL())
{
  _tau_ = make_shared<vector<Pt> > (N_);
//  calc_tau();
}


VecOfMats<cmplx>::type TorqueCalc::calc_H(int i)
{
  VecOfMats<cmplx>::type H (3);
  int mi = _sys_->get_Mi(i);
  cmplx sh, h, gam;
  double scale, qij;
  double lambda  = _sys_->get_lambda();
  MyMatrix<cmplx> gamma_i = _gamma_->operator[](i);
  vector<double> bessI;
  MyMatrix<cmplx> Hx (p_, 2*p_), Hy (p_, 2*p_), Hz (p_, 2*p_);
  
  int j, n, m;
  for (j = 0; j < mi; j++)
  {
    qij = _sys_->get_qij(i, j);
    Pt pt = _sys_->get_posij(i, j);
    _shCalc_->calc_sh(pt.theta(),pt.phi());
    scale = 1.0;
    
    if (_sys_->get_ai(i) == 0)
      bessI = _bCalc_->calc_mbfI(p_, _consts_->get_kappa()*pt.r());
    else
      bessI = _bCalc_->calc_mbfI(p_, 0.0);

    for (n = 0; n < p_; n++)
    {
      gam = gamma_i(n, n);
      for (m = 0; m <= n; m++)
      {
        sh = _shCalc_->get_result(n, m);
        h = bessI[n] * qij * scale * sh * gam;
        Hx(n, m+p_) += h * pt.x();
        Hy(n, m+p_) += h * pt.y();
        Hz(n, m+p_) += h * pt.z();
      }
      scale *= (pt.r()/lambda);
    }
  }
  H.set_val(0, Hx);
  H.set_val(1, Hy);
  H.set_val(2, Hz);
  
  return H;
}


Pt TorqueCalc::calc_tau_i(int i)
{
  Pt tau_i;
  VecOfMats<cmplx>::type Hi;
  VecOfMats<cmplx>::type gLi;

  Hi    = calc_H(i);
  tau_i = Pt();
  gLi   = _gradL_-> operator[](i);
  
  //perform cross product:
  tau_i.set_x(1/_consts_->get_dielectric_water()
                * (lotan_inner_prod(gLi[1], Hi[2], p_)
                   - lotan_inner_prod(gLi[2], Hi[1], p_)));
  
  tau_i.set_y(1/_consts_->get_dielectric_water() *
                (lotan_inner_prod(gLi[2], Hi[0], p_)
                 - lotan_inner_prod(gLi[0], Hi[2], p_)));
  
  tau_i.set_z(1/_consts_->get_dielectric_water() *
                (lotan_inner_prod(gLi[0], Hi[1], p_)
                 - lotan_inner_prod(gLi[1], Hi[0], p_)));
  return tau_i;
}

void TorqueCalc::calc_tau()
{
  for (int i = 0; i < N_; i++)
  {
    (*_tau_)[i] = calc_tau_i(i);
  }
}

ThreeBody::ThreeBody( shared_ptr<ASolver> _asolver, Units unt, double cutoff )
: N_(_asolver->get_N()), p_(_asolver->get_p()), cutoffTBD_(cutoff),
_besselCalc_(_asolver->get_bessel()),
_shCalc_(_asolver->get_sh()),
_consts_(_asolver->get_consts()),
_sys_(_asolver->get_sys()),
//energy_approx_(N_, 0),
//force_approx_(N_, Pt(0,0,0)),
//torque_approx_(N_, Pt(0,0,0)),
unit_(unt)
{
  energy_approx_ = make_shared<vector<double> >(N_);
  force_approx_ = make_shared<vector<Pt> >(N_);
  torque_approx_ = make_shared<vector<Pt> >(N_);
  
  dimer_.reserve(N_*N_);
  trimer_.reserve(N_*N_*N_);
  
  generatePairsTrips();
}

void ThreeBody::generatePairsTrips()
{
  int i, j, k;
  vector<double> dist(3); // distances between pairs: [ij, ik, jk]
  vector<int> temp2(2), temp3(3);
  int M2 = N_*(N_-1)/2; int M3 = N_*(N_-1)*(N_-2)/6;
  
  for( i = 0; i < N_; i++)
  {
    for( j = i+1; j < N_; j++)
    {
      dist[0] = _sys_->get_pbc_dist_vec( i, j).norm();
      if (cutoffTBD_ > dist[0])
      {
        temp2[0] = i; temp2[1] = j;
        dimer_.push_back(temp2);
      }
      for( k = j + 1; k < N_; k++)
      {
        dist[1] = _sys_->get_pbc_dist_vec( i, k).norm();
        dist[2] = _sys_->get_pbc_dist_vec( j, k).norm();
        sort( dist.begin(), dist.end());
        if (cutoffTBD_*2.0 > (dist[0] + dist[1]))
        {
          temp3[0] = i; temp3[1] = j; temp3[2] = k;
          trimer_.push_back(temp3);
        }
      }
    }
  }
  
  // Resizing vectors for saving energy/force vals
  energy_di_.resize( dimer_.size());
  force_di_.resize( dimer_.size());
  torque_di_.resize( dimer_.size());
  for ( i = 0; i < dimer_.size(); i++)
  {
    energy_di_[i].resize(2);
    force_di_[i].resize(2);
    torque_di_[i].resize(2);
  }
  
  energy_tri_.resize( trimer_.size());
  force_tri_.resize( trimer_.size());
  torque_tri_.resize( trimer_.size());
  for ( i = 0; i < trimer_.size(); i++)
  {
    energy_tri_[i].resize(3);
    force_tri_[i].resize(3);
    torque_tri_[i].resize(3);
  }
  
  if ( cutoffTBD_ < 1e37 )
    cout << "Cutoffs implemented, using " << cutoffTBD_ << endl;
  cout << "Max # of di: "  << M2 << " Act used: " << dimer_.size() <<endl;
  cout << "Max # of tri: " << M3 << " Act used: " << trimer_.size() <<endl;
} //end cutoffTBD


shared_ptr<System> ThreeBody::make_subsystem(vector<int> mol_idx)
{
  vector<Molecule> sub_mols (mol_idx.size());
  for (int i = 0; i < mol_idx.size(); i++)
  {
    sub_mols[i] = _sys_->get_molecule(mol_idx[i]);
  }
  
  shared_ptr<System> _subsys = make_shared<System>(sub_mols,_sys_->get_cutoff(),
                                                   _sys_->get_boxlength());
  _subsys -> set_time(_sys_->get_time());
  return _subsys;
}

//// Two or three body approximation computation
//void ThreeBody::solveNmer( int num, double preclim )
//{
//  int i, j;
//  shared_ptr<vector<vector<int> > > nmer = (( num == 2 ) ?
//                                            make_shared<vector<vector<int> > >(dimer_) :
//                                            make_shared<vector<vector<int> > >(trimer_));
//  vector< Molecule > mol_temp;
//  shared_ptr<System> _sysTemp = make_subsystem(nmer->operator[](0));
//  shared_ptr<ASolver> _asolvTemp = make_shared<ASolver>(_besselCalc_, _shCalc_,
//                                                        _sysTemp, _consts_, p_);
//  
//  for( i = 0; i < nmer->size(); i++)
//  {
//    shared_ptr<System> _sysTemp = make_subsystem(nmer->operator[](i));
//    
//    _asolvTemp->reset_all(_sysTemp);
//    _asolvTemp->solve_A(preclim);
//    _asolvTemp->solve_gradA(preclim);
//    
//    PhysCalc phys_all( _asolvTemp, outfname_, unit_);
//    phys_all.calc_all();
//    
//    for ( j = 0; j < num; j++)
//    {
//      if ( num == 2 )
//      {
//        energy_di_[i][j] = phys_all.get_omegai_conv(j);
//        force_di_[i][j] = phys_all.get_forcei_conv(j);
//        torque_di_[i][j] = phys_all.get_taui_conv(j);
//      } else
//      {
//        energy_tri_[i][j] = phys_all.get_omegai_conv(j);
//        force_tri_[i][j] = phys_all.get_forcei_conv(j);
//        torque_tri_[i][j] = phys_all.get_taui_conv(j);
//      }
//    }
//  }
//  
//  cout << num << "mers done " << endl;
//}


void ThreeBody::solveNmer( int num, double preclim )
{
  int i, j;
  shared_ptr<vector<vector<int> > > nmer = (( num == 2 ) ?
                                            make_shared<vector<vector<int> > >(dimer_) :
                                            make_shared<vector<vector<int> > >(trimer_));
  
  #pragma omp parallel for
  for( i = 0; i < nmer->size(); i++)
  {
    vector<int> tempmol;
    int poles;
    #pragma omp critical
    {
      tempmol = nmer->operator[](i);
      poles = p_;
//      cout << i << endl;
    }
    shared_ptr<System> _sysTemp = make_subsystem(tempmol);
    shared_ptr<BesselConstants> bConsta = make_shared<BesselConstants>(2*poles);
    shared_ptr<BesselCalc> bCalcu = make_shared<BesselCalc>(2*poles, bConsta);
    shared_ptr<SHCalcConstants> SHConsta = make_shared<SHCalcConstants>(2*poles);
    shared_ptr<SHCalc> SHCalcu = make_shared<SHCalc>(2*poles, SHConsta);
    shared_ptr<Constants> consts =  make_shared<Constants>(*_consts_);
    
    shared_ptr<ASolver> _asolvTemp = make_shared<ASolver>(bCalcu, SHCalcu,
                                                          _sysTemp,
                                                          consts, poles);
    
//    _asolvTemp->reset_all(_sysTemp);
    _asolvTemp->solve_A(preclim);
    _asolvTemp->solve_gradA(preclim);
    
    PhysCalc phys_all( _asolvTemp, outfname_, unit_);
    phys_all.calc_all();
    
    #pragma omp critical
    {
    for ( j = 0; j < num; j++)
    {
      if ( num == 2 )
      {
        energy_di_[i][j] = phys_all.get_omegai_conv(j);
        force_di_[i][j] = phys_all.get_forcei_conv(j);
        torque_di_[i][j] = phys_all.get_taui_conv(j);
      } else
      {
        energy_tri_[i][j] = phys_all.get_omegai_conv(j);
        force_tri_[i][j] = phys_all.get_forcei_conv(j);
        torque_tri_[i][j] = phys_all.get_taui_conv(j);
      }
    }
    }
  }
  
  cout << num << "mers done " << endl;
}

// Three body approximation
int ThreeBody::find_di( int i, int j)
{
  int di;
  vector<int> dim(2);
  
  for( di = 0; di < dimer_.size(); di++)
  {
    if (( dimer_[di][0] == i ) and ( dimer_[di][1] == j ))
      return di;
  }
  
  return -1;
}

// Three body approximation
void ThreeBody::calcTBDEnForTor( )
{
  int i, j, k;
  vector<int> m(3), di(3); // Given any triplet, di = [01, 02, 12] pairs
  // Matrix of if 0, 1 or 2 mol is in the [01, 02, 12] pairs & loc if so
  vector<vector<int > > loc(3, vector<int> (3));
  vector<vector<double > > incl(3, vector<double> (3));
  incl[0][0] = 1; incl[0][1] = 1; incl[0][2] = 0;
  incl[1][0] = 1; incl[1][1] = 0; incl[1][2] = 1;
  incl[2][0] = 0; incl[2][1] = 1; incl[2][2] = 1;
  
  loc[0][0] = 0; loc[0][1] = 0; loc[0][2] = 0;
  loc[1][0] = 1; loc[1][1] = 0; loc[1][2] = 0;
  loc[2][0] = 0; loc[2][1] = 1; loc[2][2] = 1;
  
  calcTwoBDEnForTor( ); // 2BD contribution
  
  for( i = 0; i < trimer_.size(); i++)
  {
    for( j = 0; j < 3; j++) m[j] = trimer_[i][j];
    
    di[0] = find_di(m[0], m[1]);
    di[1] = find_di(m[0], m[2]);
    di[2] = find_di(m[1], m[2]);
    
    for( j = 0; j < 3; j++)
    {
      (*energy_approx_)[m[j]] += energy_tri_[i][j];
      (*force_approx_)[m[j]]   = (*force_approx_)[m[j]] + force_tri_[i][j];
      (*torque_approx_)[m[j]]  = (*torque_approx_)[m[j]] + torque_tri_[i][j];
      
      for ( k = 0; k < 3; k++)
      {
        (*energy_approx_)[m[j]] -= incl[j][k]*energy_di_[di[k]][loc[j][k]];
        (*force_approx_)[m[j]] =  ((*force_approx_)[m[j]] +
                                force_di_[di[k]][loc[j][k]]*incl[j][k]*-1);
        (*torque_approx_)[m[j]] =  ((*torque_approx_)[m[j]] +
                                 torque_di_[di[k]][loc[j][k]]*incl[j][k]*-1);
      
      } // end k
    } // end j
  } // end i
}

void ThreeBody::printTBDEnForTor( vector<string> outfile )
{
  int j;
  streambuf * buf;
  ofstream of;
  
  if(outfname_ != "")
  {
    of.open(outfname_);
    buf = of.rdbuf();
  } else {
    buf = cout.rdbuf();
  }
  
  ostream out(buf);
  for ( j = 0; j < N_; j++)
  {
    out << "This is mol " << j << endl;
    out << " Energy: " << get_energyi_approx(j) << "\t";
    out << " Force: " << get_forcei_approx(j).norm() << "\t [";
    out << get_forcei_approx(j).x();
    out << ", " << get_forcei_approx(j).y()<< ", ";
    out << get_forcei_approx(j).z()<< "]"<< endl;
  }
  
  if ( outfile[0] != "") printNmer( 2, outfile[0]);
  if ( outfile[1] != "") printNmer( 3, outfile[1]);
}

void ThreeBody::printNmer( int num, string outfile)
{
  int i, j;
  ofstream nmer_deets;
  double dist, en_nrm;
  Pt fo_nrm;
  vector< double > print_all(4);
  vector<int>  mol(3);
  shared_ptr<vector<vector<int> > > nmer = ( num == 2 ) ?
  make_shared<vector<vector<int> > >(dimer_) :
  make_shared<vector<vector<int> > >(trimer_);
  
  shared_ptr<vector<vector<double > > > en = ( num == 2 ) ?
  make_shared<vector<vector<double> > >(energy_di_) :
  make_shared<vector<vector<double > > >(energy_tri_);
  shared_ptr<vector<vector<Pt> > > frc = ( num == 2 ) ?
  make_shared<vector<vector<Pt> > >(force_di_) :
  make_shared<vector<vector<Pt> > >(force_tri_);
  shared_ptr<vector<vector<Pt> > > tor = ( num == 2 ) ?
  make_shared<vector<vector<Pt> > >(torque_di_) :
  make_shared<vector<vector<Pt> > >(torque_tri_);
  
  nmer_deets.open( outfile );
  
  for( i = 0; i < nmer->size(); i++)
  {
    mol[0] = nmer->operator[](i)[0];
    mol[1] = nmer->operator[](i)[1];
    mol[2] = nmer->operator[](i)[2];
    if (num == 2 ) dist = _sys_->get_pbc_dist_vec(mol[0], mol[1]).norm();
    else
    {
      vector<double> dists(3);
      Pt com = (_sys_->get_centeri(mol[0]) + _sys_->get_centeri(mol[1]) +
                _sys_->get_centeri(mol[1]))*(1/3.);
      dists[0] = _sys_->get_centeri(mol[0]).dist(com);
      //_sysTemp->get_pbc_dist_vec(0, 1).norm();
      dists[1] = _sys_->get_centeri(mol[1]).dist(com);
      //_sysTemp->get_pbc_dist_vec(0, 2).norm();
      dists[2] = _sys_->get_centeri(mol[2]).dist(com);
      //_sysTemp->get_pbc_dist_vec(1, 2).norm();
      //sort( dists.begin(), dists.end());
      dist = dists[0] + dists[1] + dists[2];
    }
    for ( j = 0; j < num; j++)
    {
      en_nrm = 1.0; //energy_approx_[mol[j]];
      fo_nrm = Pt( 1, 1, 1); //force_approx_[mol[j]];
      print_all[0] = ((abs(en->operator[](i)[j]/en_nrm)<1e-15) ?
                      0 : en->operator[](i)[j]/en_nrm);
      print_all[1] = ((abs(frc->operator[](i)[j].x()/fo_nrm.x())<1e-15) ?
                      0 : frc->operator[](i)[j].x()/fo_nrm.x());
      print_all[2] = ((abs(frc->operator[](i)[j].y()/fo_nrm.y())<1e-15) ?
                      0 : frc->operator[](i)[j].y()/fo_nrm.y());
      print_all[3] = ((abs(frc->operator[](i)[j].z()/fo_nrm.z())<1e-15) ?
                      0 : frc->operator[](i)[j].z()/fo_nrm.z());
      nmer_deets << dist << "\t" << print_all[0] << "\t" << print_all[1];
      nmer_deets << ", " << print_all[2] << ", "<< print_all[3] << endl;
    }
  }
  
  nmer_deets.close();
}


// Two body approximation
void ThreeBody::calcTwoBDEnForTor( )
{
  int i, j, mol;
  
  for( mol = 0; mol < N_; mol++)
  {
    for( i = 0; i < dimer_.size(); i++)
    {
      for( j = 0; j < 2; j++)
      {
        if ( dimer_[i][j] == mol )
        {
          (*energy_approx_)[mol] += energy_di_[i][j];
          (*force_approx_)[mol]  = (*force_approx_)[mol] + force_di_[i][j];
          (*torque_approx_)[mol]  = (*torque_approx_)[mol] + torque_di_[i][j];
        }
      }
    }
  }
  
}


PhysCalc::PhysCalc(shared_ptr<ASolver> _asolv, string outfname, Units unit)
: N_(_asolv->get_N()), outfname_(outfname), BasePhysCalc()
{
  _eCalc_ = make_shared<EnergyCalc>(_asolv);
  _fCalc_ = make_shared<ForceCalc>(_asolv);
  _torCalc_ = make_shared<TorqueCalc>(_asolv);
  
  _sys_ = _asolv->get_sys();
  
  compute_units(_asolv->get_consts(), unit);
}

void PhysCalc::compute_units( shared_ptr<Constants> cst, Units unit)
{
  if (unit==INTERNAL)
  {
    unit_ = "Internal";
    unit_conv_ = 1.0;
  } else if (unit == KCALMOL)
  {
    unit_  = "kCal/Mol";
    unit_conv_ = cst->convert_int_to_kcal_mol(1.0);
  } else if (unit == JMOL)
  {
    unit_  = "Joules/Mol";
    unit_conv_ = cst->convert_int_to_jmol(1.0);
  } else if (unit == kT)
  {
    unit_  = "kT";
    unit_conv_ = cst->convert_int_to_kT(1.0);
  }
  
}

void PhysCalc::print_all()
{
  int i, j;
  Pt force_i, torque_i;
  double force_norm, torque_norm;
  streambuf * buf;
  ofstream of;
  vector<Pt> mol_pos = _sys_->get_allcenter();
  
  if(outfname_ != "")
  {
    of.open(outfname_, fstream::in | fstream::out | fstream::app);
    buf = of.rdbuf();
  } else {
    buf = cout.rdbuf();
  }
  
  ostream out(buf);
  out << "My units are " << unit_ << ". Time: " << _sys_->get_time() << endl;
  
  for ( i = 0; i < N_; i++)
  {
    force_norm = 0;
    torque_norm = 0;
    out << "MOLECULE #" << i + 1 << " radius: " << _sys_->get_radi(i) << endl;
    out << "\tPOSITION: [" << mol_pos[i].x() << ", " << mol_pos[i].y();
    out << ", " << mol_pos[i].z() << "]" << endl;
    out << "\tENERGY: " << unit_conv_ * get_omega()->operator[](i) << endl;
    force_i = get_forcei(i); torque_i = get_taui(i);

    force_i.set_x(force_i.x() * unit_conv_);
    torque_i.set_x(torque_i.x() * unit_conv_);
    force_norm += force_i.x() * force_i.x();
    torque_norm += torque_i.x() * torque_i.x();
    
    force_i.set_y(force_i.y() * unit_conv_);
    torque_i.set_y(torque_i.y() * unit_conv_);
    force_norm += force_i.y() * force_i.y();
    torque_norm += torque_i.y() * torque_i.y();
    
    force_i.set_z(force_i.z() * unit_conv_);
    torque_i.set_z(torque_i.z() * unit_conv_);
    force_norm += force_i.z() * force_i.z();
    torque_norm += torque_i.z() * torque_i.z();

    out << "\tFORCE: " << sqrt(force_norm) << ", [";
    out << force_i.x() <<" " << force_i.y() << " " << force_i.z()<< "]"<<endl;
    out << "\tTORQUE: " << sqrt(torque_norm) << ", [";
    out << torque_i.x() << " " << torque_i.y()<<" "<<torque_i.z()<< "]"<<endl;
  }
  
}


ThreeBodyPhysCalc::ThreeBodyPhysCalc(shared_ptr<ASolver> _asolv, int num,
                                     string outfname, Units unit, double cutoff)
:BasePhysCalc(), ThreeBody(_asolv, unit, cutoff), solved_(false), num_(num),
outfname_(outfname)
{
}


