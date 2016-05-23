//
//  BD.h
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

#ifndef BD_h
#define BD_h

#include <stdio.h>
#include <random>
#include <memory>
#include "EnergyForce.h"
#include "readutil.h"

/*
 Base class for implementing termination conditions in BD
 */
class BaseTerminate
{
public:
  BaseTerminate() { }
  
  virtual const bool is_terminated(shared_ptr<System> _sys) const
  {
    return false;
  }
  
  virtual string get_how_term(shared_ptr<System> _sys)
  {
    return "";
  }
};

/*
 Class for contact based termination. This terminates based on whether
 the specified molecule molecule pair is within a given cutoff of each other
 */
class ContactTerminate : public BaseTerminate
{
protected:
  double dist_contact_; //termination time
  int mol1_;
  int mol2_;
  string how_term_;
  
public:
  ContactTerminate(vector<int> mol, double distance)
  :BaseTerminate(), mol1_(mol[0]), mol2_(mol[1]), dist_contact_(distance)
  {
    char buff[400];
    sprintf(buff, "Type %d and Type %d are within %5.2f;\t",
            mol1_, mol2_, dist_contact_);
    how_term_ = "System has fulfilled condition: " + string(buff);
  }
  
  const bool is_terminated(shared_ptr<System> _sys) const
  {
    int i, j, idx1, idx2;
    for ( i = 0; i < _sys->get_typect(mol1_); i++)
    {
      for ( j = 0; j < _sys->get_typect(mol2_); j++)
      {
        idx1 = _sys->get_mol_global_idx( mol1_, i);
        idx2 = _sys->get_mol_global_idx( mol2_, j);

        double mol_c2c = _sys->get_pbc_dist_vec(idx1, idx2).norm();
        double a1 = _sys->get_ai(idx1);
        double a2 = _sys->get_ai(idx2);
        if (mol_c2c <= (dist_contact_+a1+a2)) return true;
      }
    }
    return false;
  }
  
  string get_how_term(shared_ptr<System> _sys)   { return how_term_; }
};

class ContactTerminate2 : public BaseTerminate
{
protected:
  int mol1_;
  int mol2_;
  
  double pad_; // distance between spheres if contact distance cannot be met
  
  vector<vector<int> > atPairs_;  // vector of size two vectors (atom index from each molecule type)
  vector<double> dists_;  // min distance between the above pairs
  
  string how_term_;
  
public:
  ContactTerminate2(vector<int> mol, vector<vector<int> > atpairs,
                    vector<double> dists, double pad)
  :BaseTerminate(), mol1_(mol[0]), mol2_(mol[1]), atPairs_(atpairs),
  dists_(dists), pad_(pad)
  {
    string_create();
  }
  
  ContactTerminate2(ContactFile confile, double pad)
  :pad_(pad), mol1_(confile.get_moltype1()), mol2_(confile.get_moltype2()),
  atPairs_(confile.get_at_pairs()), dists_(confile.get_dists())
  {
    string_create();
  }
  
  void string_create()
  {
    char buff[400];
    sprintf(buff, "Type %d and Type %d are within %5.2f;\t",
            mol1_, mol2_, pad_);
    how_term_ = "System has fulfilled condition: " + string(buff);
  }
  
  string get_how_term(shared_ptr<System> _sys)   { return how_term_; }
  
  const bool is_terminated(shared_ptr<System> _sys) const
  {
    bool contacted = false;
    int i, j, k, idx1, idx2;
    Pt cen1, cen2, pos1, pos2, vc1, vc2;
    double a1, a2, d, dcon;
    double sphdist1, sphdist2;  // distance of atom to edge of sphere
    
    for ( i = 0; i < _sys->get_typect(mol1_); i++)
    {
      for ( j = 0; j < _sys->get_typect(mol2_); j++)
      {
        idx1 = _sys->get_mol_global_idx( mol1_, i);
        idx2 = _sys->get_mol_global_idx( mol2_, j);
        
        cen1 = _sys->get_centeri(idx1);
        cen2 = _sys->get_centeri(idx2);
        
        a1 = _sys->get_ai(idx1);
        a2 = _sys->get_ai(idx2);
        
        for (k = 0; k < atPairs_.size(); k++)
        {
          dcon = dists_[k];
          pos1 = _sys->get_posijreal(idx1, atPairs_[k][0]);
          pos2 = _sys->get_posijreal(idx2, atPairs_[k][1]);
          
          vc1 = pos1 - cen1;
          vc2 = pos2 - cen2;
          
          sphdist1 = a1 - vc1.norm();
          sphdist2 = a2 - vc2.norm();
          
          // if sum of distances to edge of the spheres is > contact distance,
          // then contact can never happen and the new position is closest
          // point on edge of sphere and new contact distance is pad
          if ( (sphdist1 + sphdist2) > dcon)
          {
            pos1 = vc1 * (a1/vc1.norm()); // project onto sphere surface
            pos2 = vc2 * (a2/vc2.norm());
            dcon = pad_;
            
            // get position of atoms relative to box
            // (as opposed to center of molecule)
            pos1 = pos1 + cen1;
            pos2 = pos2 + cen2;
            d = _sys->get_pbc_dist_vec_base(pos1, pos2).norm();
 
            if (d < dcon){ contacted = true; break;}
          }else
          {
            d = _sys->get_pbc_dist_vec_base(pos1, pos2).norm();
            if (d < dcon)
            {
              contacted = true;
              cout << "This is dcon " << dcon << " and d " << d << endl;
              cout << "This is pos1 " << pos1.x() << " " <<
              pos1.y() << " " << pos1.z() << " " << endl;
              cout << "This is pos2 " << pos2.x() << " " <<
              pos2.y() << " " << pos2.z() << " " << endl;
              
              break;
            }
          }
        }
      }
    }
    return contacted;
  }
};


/*
 Class for time based termination
 */
class TimeTerminate : public BaseTerminate
{
protected:
  double endTime_; //termination time
  string how_term_;
  
public:
  TimeTerminate(double end_time)
  :BaseTerminate(), endTime_(end_time)
  {
    char buff[400];
    sprintf(buff, "System has fulfilled condition: time >= %7.1f;\t", endTime_);
    how_term_ = buff;
  }
  
  const bool is_terminated(shared_ptr<System> _sys) const
  {
    bool done = false;
    if (_sys->get_time() >= endTime_) done = true;
    return done;
  }
  
  string get_how_term(shared_ptr<System> _sys)   { return how_term_; }
};

enum CoordType { X, Y, Z, R };
enum BoundaryType { LEQ, GEQ };

/*
 Class for coordinate based termination. This terminates based on whether
 the specified molecule satisfies the BoundaryType condition on the CoordType
 with the given boundary value.
 */
class CoordTerminate : public BaseTerminate
{
protected:
  double boundaryVal_;
  int molIdx_;
  CoordType coordType_;
  BoundaryType boundType_;
  string how_term_;
  
public:
  CoordTerminate(int mol_idx, CoordType coord_type,
                 BoundaryType bound_type, double bound_val)
  :BaseTerminate(), molIdx_(mol_idx), coordType_(coord_type),
  boundType_(bound_type), boundaryVal_(bound_val)
  {
    char buff[400];
    string cord = "r", eq = ">=";
    if (coordType_ == X)      cord = "x";
    else if (coordType_ == Y) cord = "y";
    else if (coordType_ == Z) cord = "z";
    
    if (boundType_ == LEQ)    eq   = "<=";
    
    sprintf(buff, "Molecule type %d has fulfilled condition: %s %s %5.2f;\t",
            molIdx_, cord.c_str(), eq.c_str(), boundaryVal_);
    how_term_ = buff;
  }
  
  const bool is_terminated(shared_ptr<System> _sys) const
  {
    bool done = false;
    int i, idx;
    for ( i = 0; i < _sys->get_typect(molIdx_); i++)
    {
      idx = _sys->get_mol_global_idx( molIdx_, i);
      Pt mol_coord = _sys->get_unwrapped_center(idx);
      double test_val;
      if (coordType_ == X)      test_val = mol_coord.x();
      else if (coordType_ == Y) test_val = mol_coord.y();
      else if (coordType_ == Z) test_val = mol_coord.z();
      else                      test_val = mol_coord.norm();
      
      if ((boundType_ == LEQ) && (test_val <= boundaryVal_))      return true;
      else if ((boundType_ == GEQ) && (test_val >= boundaryVal_)) return true;
    }
    return done;
  }
  
  string get_how_term(shared_ptr<System> _sys)   { return how_term_; }
};


/*
 Combine termination conditions
 */
enum HowTermCombine { ALL, ONE };

class CombineTerminate: public BaseTerminate
{
protected:
  vector<shared_ptr<BaseTerminate> > terms_;
  HowTermCombine howCombine_;
  
public:
  CombineTerminate(vector<shared_ptr<BaseTerminate> > terms, HowTermCombine how_combine)
  :BaseTerminate(), terms_(terms), howCombine_(how_combine)
  {
  }
  
  const bool is_terminated(shared_ptr<System> _sys) const
  {
    bool done;
    howCombine_== ALL ? done=true : done=false;
    for (int i = 0; i < terms_.size(); i++)
    {
      if (terms_[i]->is_terminated(_sys) == ! done)
      {
        done=!done;
        break;
      }
    }
    return done;
  }
  
  string get_how_term(shared_ptr<System> _sys)
  {
    string how_term = "";
    bool done;
    howCombine_== ALL ? done=true : done=false;
    for (int i = 0; i < terms_.size(); i++)
    {
      if (terms_[i]->is_terminated(_sys) == true)
      {
        how_term += terms_[i]->get_how_term(_sys);
      }
    }
    return how_term;
  }
};

/*
 Class for performing a brownian dynamics step
 */
class BDStep
{
protected:
  vector<double> transDiffConsts_;  // translational diffusion constants
  vector<double> rotDiffConsts_;  // rotational diffusion constants
  
  bool diff_; // include random kicks in dynamics
  bool force_; // include force calcs in dynamics
  double dt_;
  double min_dist_;
  
  // random number generator object:
  mt19937 randGen_;
  shared_ptr<System> _sys_;
  shared_ptr<Constants> _consts_;
  
  // check if a molecule's new point causes it to collide with any other
  bool check_for_collision(int mol, Pt new_pt);
  
  // updates on individual molecules:
  void indi_trans_update(int i, Pt fi);
  void indi_rot_update(int i, Pt tau_i);
  
  // compute timestep for BD
  double compute_dt( );
  
  // compute the smallest distance between two molecule centers
  void compute_min_dist( );
  
  // return a random vector with each element drawn from a Gaussian
  Pt rand_vec(double mean, double var);
  
  // update System time
  void update_sys_time(double dt) { _sys_->set_time(_sys_->get_time() + dt); }
  
public:
  BDStep(shared_ptr<System> _sys, shared_ptr<Constants> _consts,
     vector<double> trans_diff_consts,
     vector<double> rot_diff_consts,
     bool diff = true, bool force = true);
  
  // Constructor where diffusion constants are read from system:
  BDStep(shared_ptr<System> _sys, shared_ptr<Constants> _consts,
         bool diff = true, bool force = true);
  
  // update the system with Brownian dynamics given forces and torques on every
  // molecule
  void bd_update(shared_ptr<vector<Pt> > _F,
                 shared_ptr<vector<Pt> > _tau);
  
  shared_ptr<System> get_system() { return _sys_; }
  double get_dt()                 { return dt_; }
  double get_min_dist()           { return min_dist_; }
  
};


/*
 Class for running a full BD simulation
 */
class BDRun
{
protected:
  shared_ptr<BDStep> _stepper_;
  shared_ptr<ASolver> _asolver_;
  shared_ptr<BasePhysCalc> _physCalc_;
  shared_ptr<BaseTerminate> _terminator_;
  
  string outfname_; //outputfile
  
  int maxIter_;
  double prec_;
  
public:
  // num is the number of bodies to perform calculations on (2, 3 or all).
  // If num=0, then the equations will be solved exactly
  BDRun(shared_ptr<ASolver> _asolv, shared_ptr<BaseTerminate> _terminator,
        string outfname, int num=0, bool diff = true, bool force = true,
        int maxiter=1e8, double prec=1e-4);
  
  void run(string xyzfile = "test.xyz", string statfile = "stats.dat");
};




#endif /* BD_h */
