//
//  setup.cpp
//  pb_solvers_code
//
//  Created by David Brookes on 3/9/16.
//  Copyright © 2016 David Brookes. All rights reserved.
//

#include "setup.h"

const double Setup::MAX_DIST = 1.4e18;

Setup::Setup(string infile)
:
ompThreads_( 1 ),
saltConc_( 0.01 ),
nType_( 2 ),
PBCs_( 0 ),
blen_( MAX_DIST ),
maxtime_( 1000000 ),
ntraj_( 5 ),
gridPts_( 30 ),
gridCt_(0),
idiel_( 4.0 ),
sdiel_( 78.0 ),
temp_( 298.0 ),
srand_( (unsigned)time(NULL) ),
nTypenCount_(2),
typeDef_(2),
typeDiff_(2),
pqr_names_(2),
xyz_names_(2),
isTransRot_(2),
runSpecs_(2),
mbdfile_loc_(2),
termvals_(2),
termtype_(2),
andCombine_(false)
{
  nTypenCount_[0] = 1;
  nTypenCount_[1] = 1;
  
  for (int i = 0; i<nType_; i++)
  {
    typeDiff_[i] = vector<double> (2);
    typeDiff_[i][0] = 0.0;
    typeDiff_[i][1] = 0.0;
  }
  
  typeDef_[0]  = "stat";
  typeDef_[1]  = "stat";
  runSpecs_[0] = "electrostatics";
  runSpecs_[1] = "test";
  
  potOutfnames_.resize(3);
  potOutfnames_[0] = "";
  potOutfnames_[1] = "";
  potOutfnames_[2] = "";
  
  mbdfile_loc_[0] = "";
  mbdfile_loc_[1] = "";
  
  units_ = "internal";
  
  // Initializing file locs to defaults
  // pqr fname, imat path, spol path, spol name
  vector<vector<string> > molfn = {{"../Config/test1.pqr",
    "../Config/test1.xyz"}, {"../Config/test2.pqr", "../Config/test2.xyz"}};
  
  for (int i=0; i<nType_; i++)
  {
    pqr_names_[i] = molfn[i][0];
    xyz_names_[i].resize(1);
    isTransRot_[i].resize(1);
    xyz_names_[i][0] = molfn[i][1];
    isTransRot_[i][0] = false;
  }
  
  confiles_.resize(0);
  
  read_infile(infile);
}

void Setup::read_infile(string fname)
{
  cout << "Reading Input file " << fname << endl ;
  ifstream fin(fname);
  if (!fin.is_open()) throw CouldNotReadException(fname);
  
  string inputLine;
  vector<vector <string> > keywordLines;
  getline(fin,inputLine);
  
  while (!fin.eof())
  {
    findLines(inputLine);
    getline(fin, inputLine);
  }
}

vector<string> Setup::split(string str, char delim)
{
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;
  
  while(getline(ss, tok, delim))
  {
    internal.push_back(tok);
  }
  return internal;
}


void Setup::findLines(string fline)
{
    if (!fline.empty())
    {
      findKeyword( split(fline, ' '));
    }
}

void Setup::findKeyword(vector<string> fline)
{
  string keyword = fline[0];
  if (keyword == "runname")
  {
    cout << "Runname command found" << endl;
    setRunName( fline[1] );
  } else if (keyword == "runtype")
  {
    cout << "Runtype command found" << endl;
    setRunType( fline[1] );
    if (fline[1] == "dynamics")
    {
      if (fline.size() > 2)
      {
        setNTraj( atoi( fline[2].c_str() ) );
      }
      if (fline.size() > 3)
      {
        setMaxTime( atoi( fline[3].c_str() ));
      }
    } else if (fline[1] == "electrostatics")
    {
      if (fline.size() > 2)
      {
        setGridPts( atoi( fline[2].c_str() ));
      }
    }
  } else if (keyword == "dx")
  {
    cout << "DX command found" << endl;
    setDXoutName( fline[1].c_str());
  } else if (keyword == "3dmap")
  {
    cout << "3D map command found" << endl;
    set_3dmap_name( fline[1].c_str());
  } else if (keyword == "gridct")
  {
    cout << "Grid count command found" << endl;
    setGridCt( atoi(fline[1].c_str()));
    axis_.resize( gridCt_); axLoc_.resize(gridCt_);
    potOutfnames_.resize(gridCt_+2);
  } else if (keyword == "grid2D")
  {
    cout << "Grid command found" << endl;
    setGridOutName( atoi(fline[1].c_str()), fline[2].c_str());
    setGridAx( atoi(fline[1].c_str()), fline[3].c_str());
    setGridAxLoc( atoi(fline[1].c_str()), atof(fline[4].c_str()));
  } else if (keyword == "3bdloc")
  {
    cout << "3BD loc command found" << endl;
    set3BDLoc(fline[1].c_str());
  } else if (keyword == "2bdloc")
  {
    cout << "2BD loc command found" << endl;
    set2BDLoc(fline[1].c_str());
  } else if (keyword == "omp")
  {
    cout << "OMP command found" << endl;
    setOMP(atoi(fline[1].c_str()));
  } else if (keyword == "pbc")
  {
    cout << "PBC command found" << endl;
    setPBCT( atoi(fline[1].c_str()) );
    if ( getPBCs() > 0 )
      setBoxl(atof(fline[2].c_str()));
  } else if (keyword == "salt")
  {
    cout << "Salt command found" << endl;
    setSaltCon( atof(fline[1].c_str()) );
  } else if (keyword == "temp")
  {
    cout << "Temperature command found" << endl;
    setTemp( atof(fline[1].c_str()) );
  } else if (keyword == "idiel")
  {
    cout << "Interior dielectric command found" << endl;
    setIDiel( atof(fline[1].c_str()) );
  } else if (keyword == "sdiel")
  {
    cout << "Solvent dielectric command found" << endl;
    setSDiel( atof(fline[1].c_str()) );
  }else if (keyword == "termct")
  {
    cout << "Termination count command found" << endl;
    set_numterms(atoi(fline[1].c_str()));
    resize_termcond(atoi(fline[1].c_str()));
  } else if (keyword == "termcombine")
  {
    cout << "Termination combine command found" << endl;
    set_term_combine(fline[1]);
  } else if (keyword == "term")
  {
    cout << "Termination condition command found" << endl;
    int idx = atoi(fline[1].c_str()) - 1;
    if (idx > get_numterms()-1)
    {
      cout << "WARNING: trying to add more term types than specified" << endl;
      return;
    }
    string type = fline[2];
    double val = atof(fline[3].c_str());
    vector<int> mol_idx(2);
    if (type == "contact")
    {
      string confile = fline[3];
      double pad  = atof(fline[4].c_str());
      // placeholders:
      mol_idx = {-1};
      val = -1;
      confiles_.push_back(fline[3]);
      cout << "Contact size " << confiles_.size() << endl;
      conpads_.push_back(pad);
      cout << "This is my first contact file " << confiles_[0] << endl;
      cout << "This is my first contact file " << fline[3] << endl;
    }
    else
    {
      mol_idx[0] = atoi(fline[4].c_str()) - 1;
    }
    add_termcond(idx, type, mol_idx, val);
    
  } else if (keyword == "attypes")
  {
    cout << "Atom Types command found" << endl;
    setNType( atoi(fline[1].c_str()) );
    resizeVecs();
    cout << "done with attypes" << endl;
  } else if (keyword == "type")
  {
    cout << "Type def command found" << endl;
    int typeNo = atoi(fline[1].c_str())-1;
    if (typeNo > getNType()-1)
    {
      cout << "WARNING: trying to add more mol types than specified" << endl;
      return;
    }
    if (fline.size() > 2)
      setTypeNCount( typeNo, atoi(fline[2].c_str()) );
    if (fline.size() > 3)
    {
      setTypeNDef( typeNo, fline[3].c_str() );
      if (getTypeNDef(typeNo) == "move")
      {
        setTypeNDtr( typeNo, atof(fline[4].c_str()));
        setTypeNDrot( typeNo, atof(fline[5].c_str()));
      } else if (getTypeNDef(typeNo) == "rot")
      {
        setTypeNDtr( typeNo, 0.0);
        setTypeNDrot( typeNo, atof(fline[4].c_str()));
      } else
      {
        setTypeNDtr( typeNo, 0.0);
        setTypeNDrot( typeNo, 0.0);
      }
    }
  } else if (keyword == "pqr")
  {
    cout << "PQR command found" << endl;
    int typeNo = atoi(fline[1].c_str())-1;
    if (typeNo > getNType()-1)
      return;
    setTypeNPQR( typeNo, fline[2].c_str() );
  } else if (keyword == "xyz")
  {
    string xyz;
    int traj, typeNo = atoi(fline[1].c_str())-1;
    cout << "XYZ command found" << endl;
    
    if ( fline.size() == 4 )
    {
      traj = atoi(fline[2].c_str())-1;
      xyz = fline[3];
    } else
    {
      traj = 0;
      xyz = fline[2];
    }
    if (typeNo > getNType()-1)
      return;
    setTypeNXYZ( typeNo, traj, xyz );
    setTypeNisTransRot(typeNo, traj, false);
  } else if (keyword == "transrot")
  {
    string transrot;
    int traj, typeNo = atoi(fline[1].c_str())-1;
    cout << "transrot command found" << endl;
    
    if ( fline.size() == 4 )
    {
      traj = atoi(fline[2].c_str())-1;
      transrot = fline[3];
    } else
    {
      traj = 0;
      transrot = fline[2];
    }
    if (typeNo > getNType()-1)
      return;
    setTypeNXYZ( typeNo, traj, transrot );
    setTypeNisTransRot(typeNo, traj, true);
  } else if (keyword == "randorient")
  {
    cout << "Random orientation command found" << endl;
    setRandOrient();
  } else if (keyword == "random")
  {
    cout << "RNG Seed command found" << endl;
    setRand( atoi(fline[1].c_str()) );
  } else if (keyword == "units")
  {
    cout << "Units command found" << endl;
    setUnits( fline[1].c_str() );
  } else
    cout << "Keyword not found, read in as " << fline[0] << endl;
}

void Setup::resizeVecs()
{
  int ntraj= (getRunType() == "dynamics") ? getNTraj() : 1;
  nTypenCount_.resize(nType_);
  typeDef_.resize(nType_);

  typeDiff_.resize(nType_);
  for (int i = 0; i<nType_; i++)
  {
    typeDiff_[i].resize(2);
  }

  pqr_names_.resize(nType_);
  xyz_names_.resize(nType_);
  isTransRot_.resize(nType_);
  for(int i = 0; i < nType_; i++)
  {
    xyz_names_[i].resize(ntraj);
    isTransRot_[i].resize(nType_);
  }

} // end resizeVecs

void Setup::check_inputs()
{
  vector<string> problems;
  if (typeDiff_.size() < nType_ )
  {
    problems.push_back("Number of molecular input type parameters is less \
                       than the specified number of molecule types");
  }
  if (typeDef_.size() < nType_)
  {
    problems.push_back("Number of movement types is less \
                        than the specified number of molecule types");
  }
  if (pqr_names_.size() < nType_)
  {
    problems.push_back("Number of provided PQR files is less than the\
                       specified number of molecular types");
  }
  if (xyz_names_.size() < nType_)
  {
    problems.push_back("Number of provided XYZ files is less than the\
                       specified number of molecular types");
    for (int i = 0; i < nType_; i++)
      if (xyz_names_[i].size() < ntraj_)
      {
        problems.push_back("Number of provided XYZ files is less than the\
                           specified number of trajectories");
      }
  }
  if (runSpecs_[0] == "electrostatics")
  {
    if (axis_.size() < gridCt_ || axLoc_.size() < gridCt_)
    {
      problems.push_back("Number grids provided is less than specified \
                          grid count");
    }
  }
  if (runSpecs_[0] == "dynamics")
  {
    if (termtype_.size() < numTerm_ || termtype_.size() < numTerm_)
    {
      problems.push_back("Number of termination conditions provided is less \
                              than specified termination count");
    }
  }
  if (problems.size() > 0) throw BadInputException(problems);
}

