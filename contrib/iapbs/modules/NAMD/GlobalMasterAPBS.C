/**
***  Copyright (c) 1995, 1996, 1997, 1998, 1999, 2000 by
***  The Board of Trustees of the University of Illinois.
***  All rights reserved.
**/

/**
 *
 *  @file    GlobalMasterAPBS.C
 *  @author Justin Gullingsrud and Robert Konecny
 *  @brief NAMD/APBS module
 *  @note Energy is returned in kcal/mol, forces in  kcal/(mol/A).
 *
 *
 */

/* 
 * This is iAPBS/NAMD module for performing APBS calculations in namd.
 * For more information please see http://mccammon.ucsd.edu/iapbs/
 *
 */

#ifdef NAMD_APBS

#include "GlobalMasterAPBS.h"
#include "InfoStream.h"
#include "Node.h"
#include "SimParameters.h"
#include "Vector.h"
#include "Molecule.h"
#include "ReductionMgr.h"
#include "ParseOptions.h"
#include "Parameters.h"

#include "apbs/apbs.h"
#include "apbs/routines.h" 
#include "apbs/apbscfg.h" 
#include "iapbs/apbs_driver.h"

//#define DEBUGM
#define MIN_DEBUG_LEVEL 1
#include "Debug.h"

// convert from APBS energy units (kJ) to NAMD energy units (kcal)
#define APBS_ENERGY_UNITS 0.2390057

static void parsePQRFile(const char *path, int numAtoms, 
    double *charges, double *radii);

class APBSParameters {
public:
  int debug;
  int calc_type;
  int nonlin;
  int bcfl;
  int srfm;
  int nlev;
  BigReal pdie;
  BigReal sdie;
  BigReal sdens;
  BigReal srad;
  BigReal swin;
  BigReal temp;
  BigReal gamma;
  BigReal smvolume;
  BigReal smsize;
  int chgm;
  int calcenergy;
  int calcforce;
  int calcnpenergy;
  int calcnpforce;
  Bool wpot;
  Bool wchg;
  Bool wsmol;
  Bool wkappa;
  Bool wdiel;
  Bool watompot;
  Bool rpot;
  Bool rchg;
  Bool rkappa;
  Bool rdiel;
  BigReal ionq[MAXION];
  BigReal ionc[MAXION];
  BigReal ionr[MAXION];
  int cmeth;
  Vector center;
  int ccmeth;
  Vector ccenter;
  int fcmeth;
  Vector fcenter;
  Vector grid;
  Vector glen;
  Vector cglen;
  Vector fglen;
  BigReal ofrac;
  Bool sp_apbs;
  int verbose;
  Bool recalculateGrid;

  double r_param[9];
  int i_param[25];

  int dime[3];
  int pdime[3];

  APBSParameters();
  void parse(StringList *script);

private:
  void config_parser(ParseOptions &opts);
  void check_config(ParseOptions &opts, ConfigList &config);
};

APBSParameters::APBSParameters() {
  memset(this, 0, sizeof(APBSParameters));
}

void APBSParameters::config_parser(ParseOptions &opts) {
  opts.optional("main", "calc_type", "Calculation type: 0: mg-manual; 1: mg-auto; 2: mg-para", &calc_type, 1);
  opts.optional("main", "nlev", " Levels in multigrid hierarchy nlev?", &nlev, 0);
  opts.optional("main", "debug", "APBS Debugging level", &debug, 0);
  opts.optional("main", "nonlin", "Use NLPB?", &nonlin, 0);
  opts.optional("main", "bcfl", "Boundary condition method", &bcfl, 1);
  opts.optional("main", "srfm", "Surface calculation method", &srfm, 2);
  opts.optional("main", "pdie", "Solute dielectric", &pdie, 2.0);
  opts.optional("main", "sdie", "Solvent dielectric", &sdie, 78.4);
  opts.optional("main", "sdens", "Vacc sphere density", &sdens, 10.0);
  opts.optional("main", "srad", "Solvent radius", &srad, 1.4);
  opts.optional("main", "swin", "Cubic spline window", &swin, 0.3);
  opts.optional("main", "temp", "Temperature (in K)", &temp, 298.15);
  opts.optional("main", "gamma", "Surface tension for apolar energies/forces (in kJ/mol/A^2)", &gamma, 0.105);
  opts.optional("main", "smvolume", "smvolume", &smvolume, 10.0);
  opts.optional("main", "smsize", "smsize", &smsize, 1000.0);
  opts.optional("main", "chgm", "Charge discretization method: 0: spl0; 1: spl2", &chgm, 1);
  opts.optional("main", "calcenergy", "Energy calculation flag: 0: Do not perform energy calculation; 1: Calculate total energy only; 2: Calculate per-atom energy components", &calcenergy, 2);
  opts.optional("main", "calcforce", "Atomic forces calculation: 0: Do not perform force calculation; 1: Calculate total force only; 2: Calculate per-atom force components", &calcforce, 2);
  opts.optional("main", "calcnpenergy", "NP energy calculation: 0: Do not perform NP energy calculation; 1: Calculate total NP energy only; 2: Calculate per-atom NP energy components", &calcnpenergy, 1);
  opts.optional("main", "calcnpforce", "NP force calculation: 0: Do not perform NP force calculation; 1: Calculate total NP force only; 2: Calculate per-atom NP force components", &calcnpforce, 2);
  opts.optionalB("main", "wpot", "Write electrostatic potential to iapbs-pot.dx?", &wpot, FALSE);
  opts.optionalB("main", "watompot", "Write electrostatic potential at each atom to iapbs-atompot.dx?", &watompot, FALSE);
  opts.optionalB("main", "wchg", "Write charge data to iapbs-charge.dx?", &wchg, FALSE);
  opts.optionalB("main", "wsmol", "Write molecular surface data to iapbs-smol.dx?", &wsmol, FALSE);
  opts.optionalB("main", "wkappa", "Write kappa data to iapbs-kappa.dx?", &wkappa, FALSE);
  opts.optionalB("main", "wdiel", "Write the dielectric map to iapbs-diel[x,y,z].dx?", &wdiel, FALSE);
  opts.optionalB("main", "rkappa", "Read kappa map from iapbs-kappa.dx?", &rkappa, FALSE);
  opts.optionalB("main", "rchg", "Read charge map from iapbs-charge.dx?", &rchg, FALSE);
  opts.optionalB("main", "rdiel", "Reade the dielectric map from iapbs-diel[x,y,z].dx?", &rdiel, FALSE);
  opts.optionalB("main", "rpot", "Reade pot map from iapbs-pot[x,y,z].dx?", &rpot, FALSE);
  opts.optional("main", "ion", "Counterion charge [e], conc [M], radius [A]", PARSE_MULTIPLES);
  opts.optional("main", "dime", "Grid dimensions (in x, y and z)", PARSE_STRING);
  opts.optional("main", "cmeth", "Centering method: 0: Center on a point 1: Center on a molecule", &cmeth, 1);
  opts.optional("main", "center", "Grid center if cmeth=0", &center);
  opts.optional("main", "ccmeth", "Coarse grid centering method: 0: Center on a point 1: Center on a molecule", &ccmeth, 1);
  opts.optional("main", "ccenter", "Coarse grid center if ccmeth=0", &ccenter);
  opts.optional("main", "fcmeth", "Fine grid centering method: 0: Center on a point 1: Center on a molecule", &fcmeth, 1);
  opts.optional("main", "fcenter", "Fine grid center if fcmeth=0", &fcenter);
  opts.optional("main", "grid", "Grid spacings", &grid);
  opts.optional("main", "glen", "Grid side lengths", &glen);
  opts.optional("main", "cglen", "Coarse grid side lengths", &cglen);
  opts.optional("main", "fglen", "Fine grid side lengths", &fglen);
  opts.optional("main", "pdime", "Grid of processors to be used in calculation", PARSE_STRING);
  opts.optional("main", "ofrac", "Overlap fraction between processors", &ofrac, 0.1);
  opts.optionalB("main", "sp_apbs", "Perform single point energy calculation", &sp_apbs, FALSE);
  opts.optionalB("main", "recalculateGrid", "Recalculate grid size on the fly", &recalculateGrid, FALSE);
  opts.optional("main", "verbose", "APBS verbosity level", &verbose, 0);
}

void APBSParameters::check_config(ParseOptions &opts, ConfigList &config) {
  // parse ions; may be more than one "ion" directive
  int nion = 0;
  if (opts.defined("ion")) {
    StringList *ion = config.find("ion");
    for ( ; ion; ion = ion->next) {
      if (nion >= MAXION) {
        NAMD_die("Too many ions specified");
      }
      if (sscanf(ion->data, "%lf %lf %lf", ionq+nion, ionc+nion, ionr+nion) != 3) {
        NAMD_die("Unable to parse ion line");
      }
      nion++;
    }
  }


  // populate arrays to pass to APBS
  i_param[0]  = calc_type;
  i_param[1]  = nlev;
  i_param[2]  = cmeth;
  i_param[3]  = ccmeth;
  i_param[4]  = fcmeth;
  i_param[5]  = chgm;
  i_param[6]  = nonlin;
  i_param[7]  = bcfl;
  i_param[8]  = srfm;
  i_param[9]  = calcenergy;
  i_param[10] = calcforce;
  i_param[11] = wpot;
  i_param[12] = wchg;
  i_param[13] = wsmol;
  i_param[14] = wkappa;
  i_param[15] = wdiel;
  i_param[16] = watompot;
  i_param[17] = rpot;
  i_param[18] = 0;
  i_param[19] = calcnpforce;
  i_param[20] = calcnpenergy;
  i_param[21] = nion;
  i_param[22] = rchg;
  i_param[23] = rkappa;
  i_param[24] = rdiel;

  r_param[0] = pdie;
  r_param[1] = sdie;
  r_param[2] = srad;
  r_param[3] = swin;
  r_param[4] = temp;
  r_param[5] = sdens;
  r_param[6] = gamma;
  r_param[7] = smvolume;
  r_param[8] = smsize;

   if (opts.defined("dime")) { 
    char s[129];
    opts.get("dime", s); 
    if (sscanf(s, "%d %d %d", dime+0, dime+1, dime+2) != 3) {
      NAMD_die("APBS: Unable to parse dime option");
    }
  }
 
  if (opts.defined("pdime")) {
    char s[129];
    opts.get("pdime", s); 
    if (sscanf(s, "%d %d %d", pdime+0, pdime+1, pdime+2) != 3) {
      NAMD_die("APBS: Unable to parse pdime option");
    }
  }

  // consistency check
  if (opts.defined("grid")) { 
    if (calc_type == 1 && (!recalculateGrid)) {
      NAMD_die("APBS: Wrong combination of options (grid/mg-auto).");
    }
  }
  if ((calcforce == 0) || (calcforce == 1)) {
    iout << "APBS: WARNING: Calculation of all-atom solvation forces is not turned on (calcforce)!\n";
    iout << "APBS: WARNING: This may leed to unpredictable results during minimization or MD.\n";
  }


  // printout some info
  if (verbose > 0) {
    if (calc_type == 0) {
      iout << "APBS: MG-MANUAL calculation.\n";
    } else if (calc_type == 1) {
      iout << "APBS: MG-AUTO calculation.\n";
    } else if (calc_type == 2) {
      iout << "APBS: MG-PARA calculation.\n";
    }
    if (nonlin == 0) {
      iout << "APBS: Linearized traditional PBE.\n";
    } else if (nonlin == 1) {
      iout << "APBS: Nonlinear traditional PBE.\n";
    } else if (nonlin == 2) {
      iout << "APBS: Linearized RPBE.\n";
    } else if (nonlin == 3) {
      iout << "APBS: Nonlinear RPBE.\n";
    } else if (nonlin == 4) {
      iout << "APBS: Size-Modified PBE.\n";
    } else {
      NAMD_die("APBS: Unknown PBE option.");
    }
    if (bcfl == 0) {
      iout << "APBS: Zero boundary conditions.\n";
    } else if (bcfl == 1) {
      iout << "APBS: Single Debye-Huckel sphere boundary conditions.\n";
    } else if (bcfl == 2) {
      iout << "APBS: Multiple Debye-Huckel sphere boundary conditions.\n";
    } else if (bcfl == 3) {
      iout << "APBS: Focusing boundary conditions.\n";
    } else {
      NAMD_die("APBS: Unknown boundary conditions option.");
    }
    if (srfm == 0) {
      iout << "APBS: Using molecular surface definition.\n";
    } else if (srfm == 1) {
      iout << "APBS: Using smoothed molecular surface definition.\n";
    } else if (srfm == 2) {
      iout << "APBS: Using cubic-spline surface definition.\n";
    } else if (srfm == 3) {
      iout << "APBS: Using 7-order polynomial spline surface definition.\n";
    } else {
      NAMD_die("APBS: Unknown surface definition.");
    }
    if (chgm == 0) {
      iout << "APBS: Using trilinear interpolation (linear splines).\n";
    } else if (chgm == 1) {
      iout << "APBS: Using cubic B-spline charge discretization.\n";
    } else if (chgm == 2) {
      iout << "APBS: Using quintic B-spline charge discretization.\n";
    } else {
      NAMD_die("APBS: Unknown charge discretization.");
    }

    iout << "APBS: Solute dielectric (pdie): " << pdie << "\n";
    iout << "APBS: Solvent dielectric (sdie): " << sdie << "\n";
    iout << "APBS: Temperature: " << temp << " K\n";
    iout << "APBS: Surface sphere density (sdens): " << sdens << " grid points/A^2\n";
    iout << "APBS: Surface tension: " << gamma << " kJ/mol/A\n";
    iout << "APBS: Verbosity level: " << verbose << "\n";
    iout << "APBS: Debug level: " << debug << "\n";

    if (recalculateGrid) {
      iout << "APBS: Requesting grid size re-calculation on the fly. \n"  << endi;
    }
    iout << "APBS: Grid values: \n";
    if (opts.defined("fglen")){
      iout << "APBS: Grid lengths (fglen): " << fglen[0] << " "
	   << fglen[1] << " "  << fglen[2] << "\n";
    }
    if (opts.defined("cglen")){
      iout << "APBS: Grid lenghts (cglen): " << cglen[0] << " "
	   << cglen[1]  << " " << cglen[2] << "\n";
    }
    if (opts.defined("dime")){
    iout << "APBS: Grid dimensions: "  << dime[0] << " "
	 << dime[1]   << " " << dime[2] << "\n";
    }
    if (opts.defined("grid")){
      iout << "APBS: Grid spacings (in A): "  << grid[0] << " "
	   << grid[1]   << " " << grid[2] << "\n";
    } else if((opts.defined("fglen")) && (opts.defined("dime"))) {
	iout << "APBS: Grid spacings (in A): "  <<
	  fglen[0]/(dime[0]-1) << " " << fglen[1]/(dime[1]-1) 
	     << " " << fglen[2]/(dime[2]-1) << "\n";   
      }
    if((opts.defined("dime")) && (opts.defined("grid"))) {
      iout << "APBS: Grid lengths: " << dime[0]*grid[0]-grid[0] << " "
           << dime[1]*grid[1]-grid[1] << " "  
	   << dime[2]*grid[2]-grid[2] << "\n";  
    }
    if (opts.defined("dime")){
    iout << "APBS: Required memory: " <<
      dime[0]*dime[1]*dime[2]*200.0/1024/1024 << " MB\n";
    }

    if (calcenergy == 0) {
      iout << "APBS: Electrostatic energies will not be calculated.\n";
    } else if (calcenergy == 1) {
      iout << "APBS: Total electrostatic energies will be calculated.\n";
    } else if (calcenergy == 2) {
      iout << "APBS: All-atom electrostatic energies will be calculated.\n";
    }
    if (calcforce == 0) {
      iout << "APBS: Solvent forces will not be calculated.\n";
    } else if (calcforce == 1) {
      iout << "APBS: Total solvent forces will be calculated.\n";
    } else if (calcforce == 2) {
      iout << "APBS: All-atom solvent forces will be calculated.\n";
    }
    if (calcnpenergy == 0) {
      iout << "APBS: Non-polar energies will not be calculated.\n";
    } else if (calcnpenergy == 1) {
      iout << "APBS: Total non-polar energies will be calculated.\n";
    } else if (calcnpenergy == 2) {
      iout << "APBS: All-atom non-polar energies will be calculated.\n";
    }


    if (wpot == 1){
      iout << "APBS: Writing potential to iapbs-pot.dx.\n";
    }
    if (wchg == 1){
      iout << "APBS: Writing charge distribution to iapbs-charge.dx.\n";
    }
    if (wsmol == 1){
      iout << "APBS: Writing molecular accessibility to iapbs-smol.dx.\n";
    }
    if (wkappa == 1){
      iout << "APBS: Writing kappa map to iapbs-kappa.dx.\n";
    }
    if (wdiel == 1){
      iout << "APBS: Writing dielectric map to iapbs-dielXYZ.dx\n";
    }
    if (rchg == 1){
      iout << "APBS: Reading charge map data from iapbs-charge.dx.\n";
    }
    if (rkappa == 1){
      iout << "APBS: Reading kappa map data from iapbs-kappa.dx.\n";
    }
    if (rdiel == 1){
      iout << "APBS: Reading dielectric map data from iapbs-dielXYZ.dx.\n";
    }
  }
}

void APBSParameters::parse(StringList *script) {
  ConfigList config;
  for ( ; script; script = script->next ) {
    const char *block = script->data;
    // split block into lines
    int lastline = 0;
    while (!lastline) {
      const char *newline = strchr(block, '\n');
      if (!newline) {
        lastline = 1;
      }
      // skip initial whitespace
      block += strspn(block, " ");
      // word is everything up to space or '='.  Also skip comments
      size_t wordlen = strcspn(block, " =#\n");
      if (!wordlen) {
        block = newline + 1;
        continue;
      }
      // args are everything after the = or ' ', until a comment or end of line
      // there may be trailing spaces, but that shouldn't hurt anything.
      const char *args = block + wordlen + 1;
      args += strspn(args, " =");
      size_t arglen = strcspn(args, "#\n");
      config.add_element(block, wordlen, args, arglen);

      // advance to next line
      block = newline+1; 
    }
  }

  // we've lex'ed all the options; now parse them.
  ParseOptions opts;
  config_parser(opts);
  if (!opts.check_consistency()) { 
    NAMD_die("Internal error in APBS configuration parser");
  }
  // feed the options to the parser
  if (!opts.set(config)) {
    NAMD_die("ERROR(S) IN THE APBS CONFIGURATION FILE");
  }
  // do other checks that ParseOptions can't do
  check_config(opts, config);
}

GlobalMasterAPBS::GlobalMasterAPBS() {
  DebugM(3,"Constructing\n");

  // fetch input parameters
  SimParameters *simparams = Node::Object()->simParameters;
  StringList *script = Node::Object()->configList->find("apbsForcesConfig");

  params = new APBSParameters;
  params->parse(script);

  // indicate that we will submit an energy calculation each step
  reduction = ReductionMgr::Object()->willSubmit(REDUCTIONS_BASIC);

  outputFreq = simparams->outputEnergies;
  step = simparams->firstTimestep;

  // fetch charges and radii
  Molecule *mol = Node::Object()->molecule;
  numAtoms = mol->numAtoms;
  charges = new double[numAtoms];
  radii = new double[numAtoms];
  positionx = new double[numAtoms];
  positiony = new double[numAtoms];
  positionz = new double[numAtoms];

  {
    for (int i=0; i<3; i++) {
      solvent_forces[i] = new double[numAtoms];
      vacuum_forces[i] = new double[numAtoms];
      qfForce[i] = new double[numAtoms];
      ibForce[i] = new double[numAtoms];
      dbForce[i] = new double[numAtoms];
      npForce[i] = new double[numAtoms];
    }
  }
  for (int i=0; i<13; i++) {
    apbsgrid_meta[i] = 0;
      }
  apbsgrid[0] = 0;

  // we always read charges and radii from a pqr file
  // NAMD stores vdw parameters for _pairs_ of atoms, and I don't
  // understand how to map those back to single atom radii.  Note that
  // we have to deal with at least four different force fields here as
  // well (Charmm, Amber, Gromacs, OPLS...)

  parsePQRFile(simparams->apbsPQRFile, numAtoms, charges, radii);
  double totalcharge = 0;

  // request coordinates of all atoms on each update
  for (int i=0; i<numAtoms; i++) {
    modifyRequestedAtoms().add(i);
    totalcharge += charges[i];
  }
  iout << "APBS: Total charge: " << totalcharge << "\n" << endi;
}

GlobalMasterAPBS::~GlobalMasterAPBS() {
    delete [] charges;
    delete [] radii;
    delete [] positionx;
    delete [] positiony;
    delete [] positionz;
  {
    for (int i=0; i<3; i++) {
      delete solvent_forces[i];
      delete vacuum_forces[i];
      delete qfForce[i];
      delete ibForce[i];
      delete dbForce[i];
      delete npForce[i];
    }
  }
}

void GlobalMasterAPBS::calculate() {
  // fetch the coordinates.  It's complicated like this because atoms
  // don't always arrive in the same order
  AtomIDList::const_iterator a_i = getAtomIdBegin();
  AtomIDList::const_iterator a_e = getAtomIdEnd();
  PositionList::const_iterator p_i = getAtomPositionBegin();
  for ( ; a_i != a_e; ++a_i, ++p_i ) {
      size_t index = *a_i;
      positionx[index] = p_i->x;
      positiony[index] = p_i->y;
      positionz[index] = p_i->z;
  }
  solvent_elec_energy = solvent_np_energy = vacuum_elec_energy = 0;

  call_apbs(0); 
  // For sp (single-point) calculation, that's all we need.  Otherwise, 
  // for dynamics, we do a second calculation in vacuum and store forces.
  if (!params->sp_apbs) {
    call_apbs(1); 
    // store forces back to nodes
    // clear out the requested forces first!
    modifyAppliedForces().resize(0);
    modifyForcedAtoms().resize(0);

    modifyAppliedForces().resize(numAtoms);
    modifyForcedAtoms().resize(numAtoms);

    for (int i=0; i<numAtoms; i++) {
        modifyForcedAtoms().item(i) = i;
        BigReal f[3];
        for (int j=0; j<3; j++) {
          f[j] = (solvent_forces[j][i] - vacuum_forces[j][i])*APBS_ENERGY_UNITS;
        }
        modifyAppliedForces().item(i) = Force(f[0], f[1], f[2]);
    }
  }
  
  // store solvation energy.  A separate category could be created for this.
  BigReal solvationEnergy = solvent_elec_energy+solvent_np_energy-vacuum_elec_energy;
  reduction->item(REDUCTION_MISC_ENERGY) += solvationEnergy;
  reduction->submit();

  // if needed, output separate elec and np energies.
  if (outputFreq && !(step % outputFreq)) {
    BigReal elec = solvent_elec_energy - vacuum_elec_energy;
    iout << "APBS ENERGIES: ELEC: " << elec 
                  << " NONPOLAR: " << solvent_np_energy << "\n" << endi;
  }
  ++step;
}

// this routine is currently reentrant as long as in_vacuum is different
// for each call.  In principle we could perform the needed two APBS calls
// simultaneously, storing the solvent and vacuum results in different arrays.
void GlobalMasterAPBS::call_apbs(int in_vacuum) {

  //
  // do computation with radii, charges, and current positions
  //
  double esEnergy[10];
  double npEnergy[10];

  // in vacuum, ion concentrations get set to zero, and solvent dielectric
  // gets set to 1.0.

  BigReal ionc[MAXION];
  int nion = params->i_param[21];
  for (int i=0; i<nion; i++) {
    ionc[i] = in_vacuum ? 0 : params->ionc[i];
  }
  double **forcePtr = in_vacuum ? vacuum_forces : solvent_forces;

  double r_param[9];
  memcpy(r_param, params->r_param, 9*sizeof(double));
  if (in_vacuum) {
    // set solvent dielectric to 1.0
    r_param[1] = 1.0;
  }

  int dime [3];
  double grid[3], cglen[3], fglen[3], glen[3], center[3], ccenter[3], fcenter[3];
  double maxx = 0, minx = 0, maxy = 0, miny = 0, maxz = 0, minz = 0;

  for (int i=0; i<3; i++) {
    dime[i]  = params->dime[i];
    grid[i]  = params->grid[i];
    cglen[i] = params->cglen[i];
    fglen[i] = params->fglen[i];
    glen[i] = params->glen[i];
    center[i] = params->center[i];
    ccenter[i] = params->ccenter[i];
    fcenter[i] = params->fcenter[i];
  }

  for (int i=0; i<numAtoms; i++) {
    if (maxx < positionx[i] + radii[i]) maxx = positionx[i] + radii[i];
    if (minx > positionx[i] - radii[i]) minx = positionx[i] - radii[i];
    if (maxy < positiony[i] + radii[i]) maxy = positiony[i] + radii[i];
    if (miny > positiony[i] - radii[i]) miny = positiony[i] - radii[i];
    if (maxz < positionz[i] + radii[i]) maxz = positionz[i] + radii[i];
    if (minz > positionz[i] - radii[i]) minz = positionz[i] - radii[i];
  }

  if (params->verbose > 1) {
    iout << "APBS: Molecular dimensions: " << maxx-minx << " "
	 << maxy-miny << " "
	 << maxz-minz << "\n" << endi;
  }

  // for mg-auto calculate missing grid parameters if dime = 0
  //  if ((params->i_param[0] == 0 || params->i_param[0] == 1) && dime[0] == 0) {
  if (params->recalculateGrid) {
    cglen[0] = 1.7 * (maxx-minx);
    cglen[1] = 1.7 * (maxy-miny);
    cglen[2] = 1.7 * (maxz-minz);
    fglen[0] = 20.0 + (maxx-minx);
    fglen[1] = 20.0 + (maxy-miny);
    fglen[2] = 20.0 + (maxz-minz);

    for (int i=0; i<3; i++) {
      if (fglen[i] > cglen[i]) cglen[i] = fglen[i];
    }

    if (params->verbose > 1) {
      iout << "APBS: Requesting dime re-calculation on the fly. \n"  << endi;
    }

    for (int i=0; i<3; i++) {
      dime[i] = 
	32*(int((int(fglen[i]/grid[i] + 0.5) - 1)/32.0 + 0.5)) + 1;
      if (dime[i] < 33) dime[i] = 33;
    }

    // correction to get the exact grid spacing requested
    for (int i=0; i<3; i++) {
      fglen[i] = (dime[i] - 1) * grid[i];
      if (fglen[i] > cglen[i]) cglen[i] = fglen[i];
    }
  }

  if (params->verbose > 1) {
    iout << "APBS: Grid values: \n" << endi;
    if (params->i_param[0] == 1 || params->i_param[0] == 2){
      iout << "APBS: Grid lengths (fglen): " << fglen[0] << " "
	   << fglen[1] << " "  << fglen[2] << "\n" << endi;
      iout << "APBS: Grid lenghts (cglen): " << cglen[0] << " "
	   << cglen[1]  << " " << cglen[2] << "\n" << endi;
    } else {
      iout << "APBS: Grid lengths: " << dime[0]*grid[0]-grid[0] << " "
           << dime[1]*grid[1]-grid[1] << " "  
	   << dime[2]*grid[2]-grid[2] << "\n" << endi;  
    }
    iout << "APBS: Grid dimensions: "  << dime[0] << " "
	 << dime[1]   << " " << dime[2] << "\n" << endi;
    if (grid[0] > 0) {
      iout << "APBS: Grid spacings (in A): "  << grid[0] << " "
	   << grid[1]   << " " << grid[2] << "\n" << endi;
    } else {
	iout << "APBS: Grid spacings (in A): "  <<
	  fglen[0]/(dime[0]-1) << " " << fglen[1]/(dime[1]-1) 
	     << " " << fglen[2]/(dime[2]-1) << "\n" << endi;   
      }
    iout << "APBS: Required memory: " <<
      dime[0]*dime[1]*dime[2]*200.0/1024/1024 << " MB\n" << endi;
  }

  if (params->verbose > 5) { iout << "APBS: Calling apbsdrv_ \n" << endi; }

  if (params->verbose > 9) {
    iout << "APBS: apbsdrv_ input params:\n" << endi;
    iout << numAtoms << "\n" << endi;
    for (int i=0; i<numAtoms; i++) {
      iout << positionx[i] << " " << positiony[i] << " " 
	   << positionz[i] << " " << radii[i] << " " 
	   << charges[i] << "\n" << endi;
    }
    for (int i=0; i<3; i++) {
      iout << grid[i] << " " 
	   << dime[i] << " "
	   << glen[i] << " "
	   << cglen[i] << " "
	   << fglen[i] << " "
	   << center[i] << " "
	   << ccenter[i] << " "
	   << fcenter[i] << "\n" << endi;
    }
    for (int i=0; i<9; i++) {
      iout << r_param[i] << " " << endi;
    }
    iout << "\n" << endi;
    for (int i=0; i<25; i++) {
      iout << params->i_param[i] << " " << endi;
    }
    iout << "\n" << endi;
    for (int i=0; i<params->i_param[21]; i++) {
      iout << params->ionq[i] << " "
	   << ionc[i] << " "
	   << params->ionq[i] << "\n" << endi;
    }
    iout << params->debug << "\n" << endi;

  }

  int result = apbsdrv_(
      &numAtoms,
      positionx,
      positiony,
      positionz,
      radii,
      charges,
      r_param,
      params->i_param,
      //(double *)&params->grid,
      grid,
      //params->dime,
      dime,
      params->pdime,
      //(double *)&params->glen,
      glen,
      //(double *)&params->center,
      center,
      //(double *)&params->cglen,
      cglen,
      //(double *)&params->fglen,
      fglen,
      //(double *)&params->ccenter,
      ccenter,
      //(double *)&params->fcenter,
      fcenter,
      &params->ofrac,
      &params->debug,
      params->ionq,
      ionc,
      params->ionr,
      esEnergy,
      npEnergy,
      forcePtr[0], forcePtr[1], forcePtr[2],
      qfForce[0], qfForce[1], qfForce[2],
      ibForce[0], ibForce[1], ibForce[2],
      npForce[0], npForce[1], npForce[2],
      dbForce[0], dbForce[1], dbForce[2],
      apbsgrid_meta, apbsgrid
			);

  if (in_vacuum) {
    vacuum_elec_energy = esEnergy[0] * APBS_ENERGY_UNITS;
  } else {
    solvent_elec_energy = esEnergy[0] * APBS_ENERGY_UNITS;
    solvent_np_energy = npEnergy[0] * APBS_ENERGY_UNITS;
  }
}

static void parsePQRFile(const char *path, int numAtoms, 
    double *charges, double *radii) {
  
    iout << "APBS: Reading PQR file " << path << "\n" << endi;
    FILE *fd = fopen(path, "rt");
    if (!fd) {
        NAMD_die("Error opening PQR file.\n");
    }
    int index = 0;
    char buf[200];
    double x, y, z;
    while (fgets(buf, 200, fd)) {
        if (strncmp(buf, "ATOM", 4)) continue;
        if (index >= numAtoms) break;
        if (sscanf(buf+30, "%lf%lf%lf%lf%lf", &x, &y, &z, charges+index, radii+index) != 5) {
            NAMD_die("Error parsing PQR file.\n");
        }
        index++;
    }
    if (index != numAtoms) {
        NAMD_die("Incorrect atom count found in PQR file.\n");
    }
    fclose(fd);
}

#endif
