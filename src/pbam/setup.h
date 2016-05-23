//
// setup.h
// pbam
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

#ifndef SETUP_H
#define SETUP_H

#include "readutil.h"

using namespace std;

class Setup
{
protected:
  
  static const double MAX_DIST;  // maximum distance for cutoff, box length
  
  int ompThreads_;
  double saltConc_;
  int nType_;  		// Number of different molecule types
  int PBCs_;			// PBC in 0, pseudo-2, or 3 dimensions
  double blen_; 		// boxlength for PBC
  int maxtime_;
  int ntraj_;
  
  // for electrostatics runtype
  vector< string> potOutfnames_; // Vector of outfiles, [0] = dx, rest = grid
  
  int gridPts_; // number of voxels to compute for each dim
  int gridCt_; // number of grid files to write
  vector<string> axis_;  // For grid print, axis desired
  vector<double> axLoc_; // Location along given axis
  
  // for dynamics runs
  int numTerm_;  //number of termination conditions
  vector<string> termtype_; // type of each term ('time', 'x', 'y', 'z', 'r' or 'contact')
  vector<vector<int> > termmols_; // vector of molecule ids
  vector<double> termvals_; // value for each termination condition
  vector<string> confiles_;  // contact files for contact termination conditions
  vector<double> conpads_;  // pads for contact termination conditions
  bool andCombine_;  //if true, term conds will combine w 'and', otherwise 'or'
  
  double idiel_;
  double sdiel_;  // dielectric constant win molecule and of solvent
  double temp_;
  int srand_;			// random seed
  bool orientRand_; // flag for creating random orientations for mols
  
  vector<int> nTypenCount_; // Array for each of mol types, how many mols
  vector<vector<double> > typeDiff_; // Dtr, Drot each type, size [Ntype][2]
  vector<string> typeDef_; 		// For each type, type is stat, rot or move
  vector<string> runSpecs_;	//include run type [0] (electrost/bd) & runname [1]
  vector<string> pqr_names_;  // PQR file names
  vector<vector<string> > xyz_names_;  // XYZ file names
  vector<vector<bool> > isTransRot_;
  
  vector<string> mbdfile_loc_; // location of names for manybd data output
  
  double kappa_;
  double iKbT_;
  
  string units_; // the units desired for output
  
  // input file reading methods:
  void read_infile(string infile);
  vector<string> split(string str, char delim);
  void findLines(string fline);
  void findKeyword(vector<string> fline);
  
  //'electrostatics' or 'dynamics' (for RunType)
  void setRunType( string runt ) {runSpecs_[0] = runt;}
  void setRunName( string runn ) {runSpecs_[1] = runn;}
  void setUnits( string units )  {units_ = units;}
  void resizeVecs();
  
  // setting values for electrostatics run
  void setDXoutName( string dx) { potOutfnames_[0] = dx;}
  void set_3dmap_name( string ht) { potOutfnames_[1] = ht;}
  void setGridOutName( int i, string grid) { potOutfnames_[i+1] = grid;}
  
  void setGridPts( int gridP ) { gridPts_ = gridP; }
  void setGridCt( int gridC ) { gridCt_ = gridC; }
  void setGridAx( int i, string ax) { axis_[i-1] = ax;}
  void setGridAxLoc( int i, double axLoc) { axLoc_[i-1] = axLoc;}
  
  //dynamics settings
  void set_numterms(int n) { numTerm_ = n; }
  void resize_termcond(int n)
  {
    termtype_.resize(n);
    termmols_.resize(n);
    for ( int i = 0; i < n; i++) termmols_[i].resize(2);
    termvals_.resize(n);
  }
  void add_termcond(int i, string type, vector<int> mol_idx, double val)
  {
    termtype_[i] = type;
    for (int n = 0; n < 2; n++) termmols_[i][n] = mol_idx[n];
    termvals_[i] = val;
  }
  
  void set_term_combine(string type)
  {
    if (type=="and") andCombine_ = true;
    else if (type=="or") andCombine_ = false;
    else andCombine_ = false;
  }
  
  // setting details for 3bd
  void set2BDLoc( string fileloc ) { mbdfile_loc_[0] = fileloc;}
  void set3BDLoc( string fileloc ) { mbdfile_loc_[1] = fileloc;}
  
  //
  void setOMP( int ompT ) { ompThreads_ = ompT ; }
  void setSaltCon( double saltCon )
  { saltConc_ = saltCon; }
  void setNType( int numType ) { nType_ = numType; }
  void setPBCT( int pbc ){ PBCs_ = pbc; }
  void setBoxl( double boxl ){ blen_ = boxl; }
  void setMaxTime( int maxt ){ maxtime_ = maxt; }
  
  void setIDiel( double idiel ) { idiel_ = idiel; }
  void setSDiel( double sdiel ) { sdiel_ = sdiel;}
  void setTemp( double temp ) { temp_ = temp;}
  void setRand( int rand ) { srand_ = rand; }
  void setRandOrient()          { orientRand_ = true; }
  void setNTraj( int ntraj ){ ntraj_ = ntraj; }
  void setKappa( double kappa ) { kappa_ = kappa; }
  
  void setTypeNCount( int typeCount, int count )
  { nTypenCount_[typeCount] = count; }
  void setTypeNDef( int typeCount, string definit )
  { typeDef_[typeCount] = definit; }
  void setTypeNDtr( int typeCount, double dTR )
  { typeDiff_[typeCount][0] = dTR; }
  void setTypeNDrot( int typeCount, double dRot )
  { typeDiff_[typeCount][1] = dRot; }
  
  void setTypeNPQR( int typeCount, string pqr )
  { pqr_names_[typeCount] = pqr; }
  
  void setTypeNXYZ( int typeCount, int traj, string xyz )
  { xyz_names_[typeCount][traj] = xyz; }
  
  void setTypeNisTransRot( int typeCount, int traj, bool istr )
  { isTransRot_[typeCount][traj] = istr; }
  
public:
  Setup(string infile);
  
  string getRunType()              { return runSpecs_[0]; }
  string getRunName()              { return runSpecs_[1]; }
  string getUnits()                { return units_; }
  
  // electrostatics
  string getDXoutName(  )  { return potOutfnames_[0];}
  string get_3dmap_name( ) { return potOutfnames_[1];}
  string getGridOutName( int i ) { return potOutfnames_[i+2];}
  
  int getGridPts() { return gridPts_; }
  int getGridCt() { return gridCt_; }
  string getGridAx( int i ) { return axis_[i];}
  double getGridAxLoc( int i ) { return axLoc_[i];}
  
  // Dynamics termination conds
  int get_numterms( )              { return numTerm_; }
  string get_termtype( int i)      { return termtype_[i]; }
  vector<int> get_termMolIDX( int i) { return termmols_[i]; }
  double get_termval( int i)       { return termvals_[i];}
  string get_confile(int j)        { return confiles_[j]; }
  double get_conpad(int j)         { return conpads_[j]; }
  bool get_andCombine( )           { return andCombine_; }
  
  
  // threebody
  string get2BDLoc()               { return mbdfile_loc_[0]; }
  string get3BDLoc()               { return mbdfile_loc_[1]; }
  vector<string> getMBDLoc()       { return mbdfile_loc_; }
  
  int getThreads()                 { return ompThreads_; }
  int getNType()                   { return nType_; }
  int get_ntype()                  { return nType_; } // repeat
  vector<int> get_type_nct()       { return nTypenCount_;}
  int getPBCs()                    { return PBCs_; }
  double getBLen()                 { return blen_; }
  double getIDiel()                { return idiel_; }
  double getSDiel()                { return sdiel_; }
  double getSaltConc()             { return saltConc_; }
  double getTemp()                 { return temp_; }
  int getMaxTime()                 { return maxtime_; }
  int getNTraj()                   { return ntraj_; }
  double getDtr( int n )           { return typeDiff_[n][0]; }
  double getDrot( int n )          { return typeDiff_[n][1]; }
  int getTypeNCount(int type)      { return nTypenCount_[type]; }
  string getTypeNDef(int type)     { return typeDef_[type]; }
  string getTypeNPQR(int type)     { return pqr_names_[type]; }
  string getTypeNXYZ(int type, int traj) { return xyz_names_[type][traj]; }
  bool getTypeIsTransRot(int type, int traj)  { return isTransRot_[type][traj]; }
  vector<string> get_trajn_xyz(int traj)
  {
    vector<string> traj_xyz;
    for (int i = 0; i < nType_; i ++) traj_xyz.push_back(xyz_names_[i][traj]);
    return traj_xyz;
  }
  
  string getTypeNXYZ(int type)     { return xyz_names_[type][0]; }
  bool getTypeIsTransRot(int type)     { return isTransRot_[type][0]; }
  double getKappa()                { return kappa_; }
  double getIKbT()                 { return iKbT_; }
  
  bool get_randOrient()            { return orientRand_; }
  
  // check input arguments and throw BadInputException if they dont check out
  void check_inputs();
  
};

class BadInputException: public exception
{
protected:
  vector<string> problems_;
  
public:
  BadInputException(vector<string> problems)
  :problems_(problems)
  {
  }
  
  virtual const char* what() const throw()
  {
    string ss;
    ss = "The following problems were found in your input file:\n";
    for (int i = 0; i < problems_.size(); i++)
    {
      ss += to_string(i) + ": " + problems_[i] + "\n";
    }
    return ss.c_str();
  }
};

#endif
