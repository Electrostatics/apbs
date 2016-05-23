//
//  readutil.h
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

#ifndef readutil_h
#define readutil_h

#include "util.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>

#include <string>

using namespace std;

class CouldNotReadException: public exception
{
protected:
  string path_;
  
public:
  CouldNotReadException(string path)
  :path_(path)
  {
  }
  
  virtual const char* what() const throw()
  {
    string ss;
    ss = "Could not read: " + path_;
    return ss.c_str();
  }
};

class NotEnoughCoordsException: public exception
{
protected:
  int infile_;
  int needed_;
  
public:
  NotEnoughCoordsException( int inFile, int needed)
  :infile_(inFile), needed_(needed)
  {
  }
  
  virtual const char* what() const throw()
  {
    string ss;
    ss = "File has "+to_string(infile_)+" lines, need "+to_string(needed_);
    return ss.c_str();
  }
  
};


/*
 Class for storing information in a contact file (for dynamics termination)
 */
class ContactFile
{
protected:
  string path_;
  int moltype1_;
  int moltype2_;
  
  vector<vector<int> > atPairs_;  // vector of size-two vectors (atom index from each molecule type)
  vector<double> dists_;  // min distance between the above pairs
  
  
  void read()
  {
    ifstream file(path_.c_str());
    if (!file.is_open()) throw CouldNotReadException(path_);
    string line;
    
    vector<int> pair (2);
    int mol1, mol2, at1, at2;
    double dist;
    while (getline(file, line))
    {
      stringstream linestream(line);
      
      linestream >> mol1 >> at1 >> mol2 >> at2 >> dist;
      moltype1_ = mol1 - 1;
      moltype2_ = mol2 - 1;

      pair[0] = at1 - 1;
      pair[1] = at2 - 1;
      atPairs_.push_back(pair);
      dists_.push_back(dist);
    }
  }
  
public:
  ContactFile(string path)
  :path_(path)
  {
    read();
  }
  
  const string get_path() const                   { return path_; }
  const int get_moltype1() const                  { return moltype1_; }
  const int get_moltype2() const                  { return moltype2_; }
  const vector<vector<int> > get_at_pairs() const { return atPairs_; }
  const vector<double> get_dists() const          { return dists_; }
  
};


class TransRotFile
{
protected:
  string path_;
  vector<int> mols_;
  int nmols_;
  vector<MyMatrix<double> > rotMats_;
  vector<Pt> transVecs_;
  
public:
  TransRotFile(string path, int nmols)
  :path_(path), rotMats_(nmols, MyMatrix<double>(3, 3, 0.0)), transVecs_(nmols)
  {
    read();
  }
  
  TransRotFile() { }
  
  void read()
  {
    ifstream file(path_.c_str());
    if (!file.is_open()) throw CouldNotReadException(path_);
    string line;
    int i = 0;
    int row;
    MyMatrix<double> rm;
    Pt trans;
    vector<int> pair (2);
    int mol;
    double rot1, rot2, rot3, tr;
    while (getline(file, line))
    {
      stringstream linestream(line);
      linestream >> mol >> rot1 >> rot2 >> rot3 >> tr;
      
      row = i % 3;
//      if (row == 0)
//      {
//        rm = MyMatrix<double>(3, 3, 0.0);
//        trans = Pt();
//        mols_.push_back(mol);
//        rotMats_.push_back(rm);
//        transVecs_.push_back(trans);
//      }
      
      rotMats_[mol-1].set_val(row, 0, rot1);
      rotMats_[mol-1].set_val(row, 1, rot2);
      rotMats_[mol-1].set_val(row, 2, rot3);
      
      if (row == 0) trans.set_x(tr);
      else if (row == 1) trans.set_y(tr);
      else if (row == 2) trans.set_z(tr);
      
      i += 1;
    }
  }
  
  const string get_path() const     { return path_; }
  const Pt get_trans(int j) const   { return transVecs_[j]; }
  const MyMatrix<double> get_rotmat(int j) const { return rotMats_[j]; }
  
};

/*
 Class for reading and storing the info in .pqr files
 */
class PQRFile
{
protected:
  string path_;
  int M_;  // number of charges
  vector<double> charges_;
  vector<double> atomRadii_;
  vector<Pt> atomCenters_;
  
  bool cg_;
  vector<Pt> cgCenters_;  // coarse grain centers
  vector<double> cgRadii_;
  
  void read()
  {
    ifstream fin(path_.c_str());
    if (!fin.is_open()) throw CouldNotReadException(path_);
    
    char buf[600];
    fin.getline(buf, 599);
    Pt cent( 0.0, 0.0, 0.0);
    int iCen = 17;
    int iCoord = 31;
    while (!fin.eof())
    {
      double x, y, z, c, r;
      if (strncmp(&(buf[0]),"ATOM",4) == 0)
      {
        sscanf(&(buf[iCoord]), "%lf %lf %lf %lf %lf", &x, &y, &z, &c, &r);
        // read in as centers that specifies dielectric boundary
        if (strncmp(&(buf[iCen]),"CEN",3) == 0)
        {
          cg_ = true;
          cgRadii_.push_back(r);
          cgCenters_.push_back(Pt(x,y,z));
        }
        
        // read in as atoms
        else
        {
          atomCenters_.push_back(Pt(x,y,z));
          charges_.push_back(c);
          atomRadii_.push_back(r);
          cent = cent + Pt(x,y,z);
        }
      }
      fin.getline(buf,599);
    }
    M_ = int(atomCenters_.size());
    if (cg_ == false) cgCenters_.push_back( cent * (1./(double) M_));
  }
  
public:
  
  PQRFile(string path, int approx_size=1000)
  :path_(path), cg_(false)
  {
    charges_.reserve(approx_size);
    atomRadii_.reserve(approx_size);
    atomCenters_.reserve(approx_size);
    cgCenters_.reserve(10);
    cgRadii_.reserve(10);
    read();
  }
  
  const string get_path() const             { return path_; }
  vector<Pt> get_atom_pts() const           { return atomCenters_; }
  const vector<double> get_charges() const  { return charges_; }
  const vector<double> get_radii() const    { return atomRadii_; }
  const int get_M() const                   { return M_; }
  const vector<double> get_cg_radii() const { return cgRadii_; }
  const vector<Pt> get_cg_centers() const   { return cgCenters_; }
  const bool get_cg() const                 { return cg_; }
  
};


/*
 Class for reading and storing the info in .xyz files
 */
class XYZFile
{
protected:
  int nmols_;  // number of molecules
  vector<Pt> pts_;
  string path_;
  
public:
  XYZFile(string path, int nmols)
  :path_(path), nmols_(nmols)
  {
    pts_.reserve(nmols);
    read();
  }
  
  XYZFile() { }
  
  void read()
  {
    int ctr = 0;
    string s;
    double x,y,z;
    ifstream fin (path_.c_str());
    if (!fin.is_open()) throw CouldNotReadException(path_);
    
    while ( getline( fin, s ) && (ctr < nmols_))
    {
      istringstream ss( s );
      ss >> x >> y >> z;
      pts_.push_back(Pt(x, y, z));
      ctr++;
    }
    if (ctr != nmols_)  throw NotEnoughCoordsException(ctr, nmols_);
  }
  
  const string get_path() const     { return path_; }
  const int get_nmols() const       { return nmols_; }
  vector<Pt> get_pts() const        { return pts_; }
};
    

#endif /* readutil_h */
