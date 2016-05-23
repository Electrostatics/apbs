/*
 main.cpp
 
 Main for PBAM runs, electrostatics, dynamics and energyforce
 
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

#include <memory>
#include "setup.h"
#include "BD.h"
#include "Electrostatics.h"
#include <time.h>

using namespace std;

int main_dynamics( int poles, double tol, shared_ptr<Setup> setup,
                  shared_ptr<Constants> consts, shared_ptr<System> sys)
{
  int traj, i, j = 0;
  shared_ptr<BesselConstants> bConsta = make_shared<BesselConstants>(2*poles);
  shared_ptr<BesselCalc> bCalcu = make_shared<BesselCalc>(2*poles, bConsta);
  shared_ptr<SHCalcConstants> SHConsta = make_shared<SHCalcConstants>(2*poles);
  shared_ptr<SHCalc> SHCalcu = make_shared<SHCalc>(2*poles, SHConsta);
  
  shared_ptr<ASolver> ASolv = make_shared<ASolver> (bCalcu, SHCalcu, sys,
                                                    consts, poles);
  
  vector<shared_ptr<BaseTerminate > >  terms(setup->get_numterms());
  for (i = 0; i < setup->get_numterms(); i++)
  {
    string type = setup->get_termtype(i);
    string bdtype = type.substr(1,2);
    double val = setup->get_termval(i);
    BoundaryType btype = ( bdtype == "<=" ) ? LEQ : GEQ;
    
    if ( type == "contact" )
    {
      cout << "Contact termination found" << endl;
      double pad = setup->get_conpad(j);
      ContactFile confile (setup->get_confile(j));
      auto conterm = make_shared<ContactTerminate2>(confile, pad);

      terms[i] = make_shared<ContactTerminate2>(confile, pad);
      j += 1;  // j is index of contact termconditions
    } else if (type.substr(0,1) == "x")
    {
      cout << type << " termination found for molecule ";
      cout << setup->get_termMolIDX(i)[0] << " at a distance " << val << endl;
      terms[i] = make_shared<CoordTerminate>( setup->get_termMolIDX(i)[0],
                                             X, btype, val);
    } else if (type.substr(0,1) == "y")
    {
      cout << type << " termination found for molecule ";
      cout << setup->get_termMolIDX(i)[0] << " at a distance " << val << endl;
      terms[i] = make_shared<CoordTerminate>( setup->get_termMolIDX(i)[0],
                                             Y, btype, val);
    } else if (type.substr(0,1) == "z")
    {
      cout << type << " termination found for molecule ";
      cout << setup->get_termMolIDX(i)[0] << " at a distance " << val << endl;
      terms[i] = make_shared<CoordTerminate>( setup->get_termMolIDX(i)[0],
                                             Z, btype, val);
    } else if (type.substr(0,1) == "r")
    {
      cout << type << " termination found for molecule ";
      cout << setup->get_termMolIDX(i)[0] << " at a distance " << val << endl;
      terms[i] = make_shared<CoordTerminate>( setup->get_termMolIDX(i)[0],
                                             R, btype, val);
    } else if (type == "time")
    {
      cout << "Time termination found, at time (ps) " << val << endl;
      terms[i] = make_shared<TimeTerminate>( val);
    } else cout << "Termination type not recognized!" << endl;
  }
  
  cout << "Done making termination conds " << endl;
  HowTermCombine com = (setup->get_andCombine() ? ALL : ONE);
  auto term_conds = make_shared<CombineTerminate> (terms, com);
  
  char buff[100], outb[100];
  sprintf( outb, "%s.stat", setup->getRunName().c_str());
  string statfile = outb;
  
  for (traj = 0; traj < setup->getNTraj(); traj++)
  {
    sprintf( buff, "%s_%d.xyz", setup->getRunName().c_str(), traj);
    string xyztraj = buff;
    sprintf( outb, "%s_%d.dat", setup->getRunName().c_str(), traj);
    string outfile = outb;
    
    string stats = setup->getRunName();
    sys->reset_positions( setup->get_trajn_xyz(traj));
    sys->set_time(0.0);
    BDRun dynamic_run( ASolv, term_conds, outfile);
    dynamic_run.run(xyztraj, statfile);
    cout << "Done with trajectory " << traj << endl;
  }
  
  return 0;
}

int main_electrostatics( int poles, double tol, shared_ptr<Setup> setup,
                        shared_ptr<Constants> consts, shared_ptr<System> sys)
{
  int i;
  shared_ptr<BesselConstants> bConsta = make_shared<BesselConstants>(2*poles);
  shared_ptr<BesselCalc> bCalcu = make_shared<BesselCalc>(2*poles, bConsta);
  shared_ptr<SHCalcConstants> SHConsta = make_shared<SHCalcConstants>(2*poles);
  shared_ptr<SHCalc> SHCalcu = make_shared<SHCalc>(2*poles, SHConsta);
  
  shared_ptr<ASolver> ASolv = make_shared<ASolver> (bCalcu, SHCalcu, sys,
                                                    consts, poles);
  ASolv->solve_A(tol); ASolv->solve_gradA(tol);
  Electrostatic Estat( ASolv, setup->getGridPts());
  
  if ( setup->getDXoutName() != "" )
    Estat.print_dx( setup->getDXoutName());
  
  if ( setup->get_3dmap_name() != "" )
    Estat.print_3d_heat( setup->get_3dmap_name());
  
  for ( i = 0; i < setup->getGridCt(); i++ )
  for ( i = 0; i < setup->getGridCt(); i++ )
  {
    Estat.print_grid(setup->getGridAx(i), setup->getGridAxLoc(i),
                     setup->getGridOutName(i));
  }
  return 0;
}


// Main to solve for A and then print energies forces and torques
int main_energyforce( int poles, double tol, shared_ptr<Setup> setup,
                     shared_ptr<Constants> consts, shared_ptr<System> sys)
{
  clock_t t;
  t = clock();
  shared_ptr<BesselConstants> bConsta = make_shared<BesselConstants>(2*poles);
  shared_ptr<BesselCalc> bCalcu = make_shared<BesselCalc>(2*poles, bConsta);
  shared_ptr<SHCalcConstants> SHConsta = make_shared<SHCalcConstants>(2*poles);
  shared_ptr<SHCalc> SHCalcu = make_shared<SHCalc>(2*poles, SHConsta);
  
  shared_ptr<ASolver> ASolv = make_shared<ASolver> (bCalcu, SHCalcu, sys,
                                                        consts, poles);
  
  ASolv->solve_A(tol); ASolv->solve_gradA(tol);
  PhysCalc calcEnFoTo( ASolv, setup->getRunName(), consts->get_unitsEnum());
  calcEnFoTo.calc_all();
  calcEnFoTo.print_all();
  t = clock() - t;
  printf ("energyforce calc took me %f seconds.\n",
          ((float)t)/CLOCKS_PER_SEC);
  return 0;
}

int main_bodyapprox( int poles, double tol, shared_ptr<Setup> setup,
                  shared_ptr<Constants> consts, shared_ptr<System> sys)
{
    clock_t t3;
  t3 = clock();
  shared_ptr<BesselConstants> bConsta = make_shared<BesselConstants>(2*poles);
  shared_ptr<BesselCalc> bCalcu = make_shared<BesselCalc>(2*poles, bConsta);
  shared_ptr<SHCalcConstants> SHConsta = make_shared<SHCalcConstants>(2*poles);
  shared_ptr<SHCalc> SHCalcu = make_shared<SHCalc>(2*poles, SHConsta);
  
  shared_ptr<ASolver> ASolv = make_shared<ASolver> (bCalcu, SHCalcu, sys,
                                                    consts, poles);
  ThreeBody threeBodTest( ASolv, consts->get_unitsEnum() );
  threeBodTest.solveNmer(2);
  threeBodTest.solveNmer(3);
  t3 = clock() - t3;
  threeBodTest.calcTBDEnForTor();

  
  threeBodTest.printTBDEnForTor(setup->getMBDLoc());
  
  printf ("manybody approx calc took me %f seconds.\n",
          ((float)t3)/CLOCKS_PER_SEC);
  
  return 0;
}

// Function to get and check inputs from file
void get_check_inputs(shared_ptr<Setup> &setFile, shared_ptr<System> &syst,
                           shared_ptr<Constants> &consts)
{
  try {
    setFile->check_inputs();
  } catch (const BadInputException& ex)
  {
    cout << ex.what() << endl;
    exit(0);
  }
  cout << "All inputs okay " << endl;
  
  consts = make_shared<Constants>(*setFile);
  try {
    syst = make_shared<System>(*setFile);
  } catch(const OverlappingMoleculeException& ex1)
  {
    cout << ex1.what() << endl;
    cout << "Provided system has overlapping molecules. ";
    cout << "Please provide a correct system."<< endl;
    exit(0);
  } catch (const NotEnoughCoordsException& ex2)
  {
    cout << ex2.what() << endl;
    exit(0);
  }
  cout << "Molecule setup okay " << endl;
  
  if (setFile->get_randOrient())
  {
    for ( int i = 0; i < syst->get_n(); i++)
      syst->rotate_mol(i, Quat().chooseRandom());
  }
  
  // writing initial configuration out
  syst->write_to_pqr( setFile->getRunName() + ".pqr");
  cout << "Written config" << endl;
}

int main(int argc, const char * argv[])
{
  string input_file = argv[1];
//  string input_file = "/Users/davidbrookes/data/2fgr/electrostatics/run.electrostatic.inp";
//  string input_file = "/Users/davidbrookes/Projects/pb_solvers/pbam/pbam_test_files/dynamics_test/contact_1BRS_nam/run.dyn.hard.refs";
//  string input_file = "/Users/davidbrookes/Projects/pb_solvers/pbam/pbam_test_files/electrostatic_test/run.electrostatic_david.inp";
//  string input_file = "/Users/lfelberg/PBSAM/pb_solvers/pbam/pbam_test_files/dynamics_test/contact_1BRS_nam/run.dyn.hard.refs";

  int poles = 5;
  double solv_tol = 1e-4;
  
  auto setp = make_shared<Setup>(input_file);
  auto syst = make_shared<System> ();
  auto consts = make_shared<Constants> ();
  
  get_check_inputs( setp, syst, consts);
  
  if ( setp->getRunType() == "dynamics")
    main_dynamics( poles, solv_tol, setp, consts, syst);
  else if ( setp->getRunType() == "electrostatics")
    main_electrostatics( poles, solv_tol, setp, consts, syst);
  else if ( setp->getRunType() == "energyforce")
    main_energyforce( poles, solv_tol, setp, consts, syst);
  else if ( setp->getRunType() == "bodyapprox")
    main_bodyapprox( poles, solv_tol, setp, consts, syst);
  else
    cout << "Runtype not recognized! See manual for options" << endl;
    
  return 0;
}

