//
//  System.cpp
//  pb_solvers_code
//
//  Created by David Brookes on 9/28/15.
//  Copyright © 2015 David Brookes. All rights reserved.
//

#include "System.h"

// user specified radius and center
Molecule::Molecule(string movetype, double a, vector<double> qs, vector<Pt> pos,
                   vector<double> vdwr, Pt cen, int type, int typeIdx,
                   double drot, double dtrans)
:moveType_(movetype), drot_(drot), dtrans_(dtrans),
qs_(qs), pos_(pos), vdwr_(vdwr), type_(type), typeIdx_(typeIdx),
M_((int) pos.size()),
a_(a), center_(cen), unwrappedCenter_(cen)
{
  set_Dtr_Drot(movetype);
  reposition_charges();
}

// user specified radius
Molecule::Molecule(string movetype, double a, vector<double> qs,
                   vector<Pt> pos, vector<double> vdwr, int type, int typeIdx,
                   double drot, double dtrans)
:moveType_(movetype), drot_(drot), dtrans_(dtrans),
qs_(qs), pos_(pos), vdwr_(vdwr), type_(type), typeIdx_(typeIdx),
M_((int) pos.size()),
a_(a)
{
  set_Dtr_Drot(movetype);
  calc_center();
  reposition_charges();
}

// user specified center
Molecule::Molecule(string movetype, vector<double> qs, vector<Pt> pos,
                   vector<double> vdwr, Pt cen, int type, int typeIdx,
                   double drot, double dtrans)
:moveType_(movetype), drot_(drot), dtrans_(dtrans),
qs_(qs), pos_(pos), vdwr_(vdwr), type_(type), typeIdx_(typeIdx),
M_((int) pos.size()), center_(cen), unwrappedCenter_(cen), a_(0)
{
  set_Dtr_Drot(movetype);
  reposition_charges();
}

// neither the center or radius are specified
Molecule::Molecule(string movetype, vector<double> qs, vector<Pt> pos,
                   vector<double> vdwr,  int type, int typeIdx,
                   double drot, double dtrans)
:moveType_(movetype), drot_(drot), dtrans_(dtrans),
qs_(qs), pos_(pos), vdwr_(vdwr), type_(type), typeIdx_(typeIdx),
M_((int) pos.size()), a_(0)
{
  set_Dtr_Drot(movetype);
  calc_center();
  reposition_charges();
}


void Molecule::set_Dtr_Drot(string type)
{
  if ((type == "stat") or (type == "rot"))  dtrans_ = 0.0;
  if (type == "stat") drot_ = 0.0;
}

void Molecule::calc_center()
{
  // calculate the center of the molecule (for now using center):
  double xc, yc, zc;
  xc = 0;
  yc = 0;
  zc = 0;
  for (int i = 0; i < M_; i++)
  {
    xc += pos_[i].x();
    yc += pos_[i].y();
    zc += pos_[i].z();
  }
  xc /= (double) M_;
  yc /= (double) M_;
  zc /= (double) M_;
  
  center_ = Pt(xc, yc, zc);
  unwrappedCenter_ = center_;
}

void Molecule::calc_a()
{
  a_ = 0;
  double dist;
  for (int i = 0; i < M_; i++)
  {
    dist = pos_[i].norm() + vdwr_[i];
    if (dist > a_) a_ = dist;
  }
}

void Molecule::reposition_charges()
{
  bool recalc_a = false;
  // repositioning the charges WRT center of charge
  for (int i = 0; i < M_; i++)
  {
    // check that the charge is encompassed by the the center and radius:
    if (pos_[i].dist(center_)+vdwr_[i] > a_)   recalc_a = true;
    pos_[i] = pos_[i] - center_;
  }
  
  if (recalc_a) calc_a();
}

void Molecule::translate(Pt dr, double boxlen)
{
  Pt dv  = center_ + dr;
  
  unwrappedCenter_ = unwrappedCenter_ + dr; // unwrapped position
  center_ = Pt(dv.x() - round(dv.x()/boxlen)*boxlen,
            dv.y() - round(dv.y()/boxlen)*boxlen,
            dv.z() - round(dv.z()/boxlen)*boxlen);
}

void Molecule::rotate(Quat qrot)
{
  for (int i = 0; i < M_; i++)
  {
    pos_[i] = qrot.rotate_point(pos_[i]);
  }
}

System::System(const vector<Molecule>& mols, double cutoff,
               double boxlength)
:molecules_(mols), N_((int) mols.size()), cutoff_(cutoff),
boxLength_(boxlength), t_(0)
{
  int i, j, k, maxi = 0;
  vector<int> maxj, keys(2);
  for ( k = 0; k < N_; k++)
  {
    i = molecules_[k].get_type();
    j = molecules_[k].get_type_idx();
    keys = {i,j};
    typeIdxToIdx_[keys] = k;
    maxi = ( maxi > i ) ? maxi : i;
    
    if ( i >= maxj.size() ) maxj.push_back(0);
    maxj[i] = ( maxj[i] > j ) ? maxj[i] : j;
  }
  
  maxi++;
  for ( j = 0; j < maxj.size(); j++) maxj[j]++;
  
  ntype_ = maxi;
  typect_ = maxj;
  
  check_for_overlap();
  lambda_ = calc_average_radius();
  if (boxLength_/2. < cutoff_)  compute_cutoff();
}

System::System(Setup setup, double cutoff)
:t_(0), ntype_(setup.get_ntype()), typect_(setup.get_type_nct())
{
  vector<Molecule> mols;
  int chg, i, j, k=0;
  string pqrpath;
  Molecule mol;
  vector<int> keys(2);
  for (i = 0; i < setup.get_ntype(); i++)
  {
    
    PQRFile pqrI (setup.getTypeNPQR(i));
    TransRotFile transrot;
    XYZFile xyzI;
    if (setup.getTypeIsTransRot(i))
    {
      transrot = TransRotFile(setup.getTypeNXYZ(i), setup.getTypeNCount(i));
    }
    else
    {
      xyzI = XYZFile (setup.getTypeNXYZ(i), setup.getTypeNCount(i));
    }
    for (j = 0; j < setup.getTypeNCount(i); j++)
    {
      Pt trans;
      MyMatrix<double> rot;
      
      Pt com = pqrI.get_cg_centers()[0];
      
      if (setup.getTypeIsTransRot(i))
      {
        trans = transrot.get_trans(j);
        rot = transrot.get_rotmat(j);
      }
      else
      {
        trans = xyzI.get_pts()[j] + com * -1.0;
        rot = MyMatrix<double> (3, 3, 0.0);
        rot.set_val(0, 0, 1.0);
        rot.set_val(1, 1, 1.0);
        rot.set_val(2, 2, 1.0);
      }
      
      keys = { i, j };
      vector<Pt> repos_charges(pqrI.get_M());
      Pt new_pt;
      
      for ( chg = 0; chg < pqrI.get_M(); chg ++)
      {
        repos_charges[chg] = pqrI.get_atom_pts()[chg].rotate(rot) + trans;
      }
      
      if (pqrI.get_cg())  // coarse graining is in pqr
      {
        mol  = Molecule(setup.getTypeNDef(i), pqrI.get_cg_radii()[0],
                        pqrI.get_charges(), repos_charges,
                        pqrI.get_radii(), xyzI.get_pts()[j], i, j,
                        setup.getDrot(i), setup.getDtr(i));
      }
      else if (! setup.getTypeIsTransRot(i))
      {
        mol = Molecule(setup.getTypeNDef(i), pqrI.get_charges(),
                       repos_charges, pqrI.get_radii(),
                       xyzI.get_pts()[j], i, j,
                       setup.getDrot(i), setup.getDtr(i));
        
      }
      else
      {
        mol = Molecule(setup.getTypeNDef(i), pqrI.get_charges(),
                       repos_charges, pqrI.get_radii(), i, j,
                       setup.getDrot(i), setup.getDtr(i));
      }
      
      molecules_.push_back(mol);
      typeIdxToIdx_[keys] = k;
      k++;
    } // end j
  } // end i
  N_ = (int) molecules_.size();
  boxLength_ = setup.getBLen();
  cutoff_ = cutoff;
  
  if (boxLength_/2. < cutoff_)  compute_cutoff();
  check_for_overlap();
  lambda_ = calc_average_radius();
}

const double System::calc_average_radius() const
{
  double ave = 0;
  for (int i = 0; i < N_; i++)
  {
    ave += get_ai(i);
  }
  ave  =  ave / N_;
  return ave;
}


void System::compute_cutoff()
{
  cutoff_ = boxLength_/2.0;
  cout << " The desired cutoff is larger than half the box length";
  cout << ". Resetting cutoff to 1/2 the boxlength: " << cutoff_ << endl;
}


void System::check_for_overlap()
{
  int i, j;
  double dist, ai, aj;
  for (i = 0; i < N_; i++)
  {
    ai = molecules_[i].get_a();
    for (j = i+1; j < N_; j++)
    {
      aj = molecules_[j].get_a();
      dist = get_pbc_dist_vec(i, j).norm();
      if (dist < (ai + aj))
      {
        throw OverlappingMoleculeException(i, j);
      }
    }
  }
}

Pt System::get_pbc_dist_vec(int i, int j)
{
  Pt ci = get_centeri(i);
  Pt cj = get_centeri(j);
  return get_pbc_dist_vec_base(ci, cj);
}

Pt System::get_pbc_dist_vec_base(Pt p1, Pt p2)
{
  Pt dv  = p1 - p2;
  
  Pt v = Pt(dv.x() - round(dv.x()/boxLength_)*boxLength_,
            dv.y() - round(dv.y()/boxLength_)*boxLength_,
            dv.z() - round(dv.z()/boxLength_)*boxLength_);
  
  return v;
}

vector<Pt> System::get_allcenter() const
{
  vector< Pt> mol_cen(N_);
  for ( int i = 0; i < N_; i++)
    mol_cen[i] = molecules_[i].get_center();
  
  return mol_cen;
}

bool System::less_than_cutoff(Pt v)
{
  if (v.norm() < cutoff_) return true;
  else return false;
}

void System::reset_positions( vector<string> xyzfiles )
{
  int i, j, k;
  vector<int> keys(2);
  for (i = 0; i < ntype_; i++)
  {
    XYZFile xyzI (xyzfiles[i], typect_[i]);
    for (j = 0; j < typect_[i]; j++)
    {
      keys = { i, j};
      k = typeIdxToIdx_[keys];
      Pt dist_to_new = get_centeri(k) - xyzI.get_pts()[j];
      molecules_[k].translate(dist_to_new*-1, boxLength_);
    }
  }
  
}

void System::write_to_pqr(string outfile)
{
  int i, j, ct = 0;
  ofstream pqr_out;
  char pqrlin[400];
  
  pqr_out.open( outfile );
  
  for ( i = 0; i < N_; i++ )
  {
    for ( j = 0; j < get_Mi(i); j++)
    {
      sprintf(pqrlin,"%6d  C   CHG A%-5d    %8.3f%8.3f%8.3f %7.4f %7.4f",ct,i,
              get_posijreal(i, j).x(),
              get_posijreal(i, j).y(),
              get_posijreal(i, j).z(),
              get_qij(i, j), get_radij(i, j));
      pqr_out << "ATOM " << pqrlin << endl;
      ct++;
    }
    sprintf(pqrlin,"%6d  X   CEN A%-5d    %8.3f%8.3f%8.3f %7.4f %7.4f",ct,i,
            get_centeri(i).x(), get_centeri(i).y(), get_centeri(i).z(),
            0.0, get_radi(i));
    pqr_out << "ATOM " << pqrlin << endl;
    ct++;
  }
}

void System::write_to_xyz(ofstream & xyz_out)
{
  int i, j, at_tot = 0;
  char xyzlin[400];
  
  for ( i = 0; i < N_; i++ )
    for ( j = 0; j < get_Mi(i); j++)
      at_tot++;
  at_tot += N_; // for adding CG centers
  
  xyz_out << at_tot << endl;
  xyz_out << "Atoms. Timestep (ps): " << t_ << endl;
  for ( i = 0; i < N_; i++ )
  {
    for ( j = 0; j < get_Mi(i); j++)
    {
      sprintf(xyzlin,"N %8.3f %8.3f %8.3f", get_posijreal(i, j).x(),
              get_posijreal(i, j).y(), get_posijreal(i, j).z());
      xyz_out << xyzlin << endl;
    }
    sprintf(xyzlin,"X %8.3f %8.3f %8.3f", get_centeri(i).x(),
            get_centeri(i).y(), get_centeri(i).z());
    xyz_out << xyzlin << endl;
  }
}
