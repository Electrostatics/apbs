!
#include "../include/dprec.fh"

MODULE apbs_vars
#ifdef APBS

use file_io_dat, only : MAX_FN_LEN
!-----------------------------------------------------------------------
!
!                 iAPBS variables definition
!
! Similar type of varibles are packed together (integer/integer and
! real/real) to mimize fortran/C passing incompatabilities.
!
! For detailed description of individual variables please see
! the iAPBS Programmer's Guide.
!
!-----------------------------------------------------------------------
! max number of counterions
  INTEGER, PARAMETER :: MAXION = 10
! static dimensions (for now) rokFIXME
  INTEGER, PARAMETER :: APBSNATOMS = 500000

!-----------------------------------------------------------------------
!     int ispara;            : 1 => is a parallel calculation,
!                              0 => is not
  INTEGER :: ispara

!-----------------------------------------------------------------------
! r_param memebers
! 1 double pdie;          : Solute dielectric
! 2 double sdie;          : Solvent dielectric
! 3 double srad;          : Solvent radius
! 4 double swin;          : Cubic spline window
! 5 double temp;          : Temperature (in K)
! 6 double sdens;         : Vacc sphere density
! 7 double gamma;         : Surface tension for apolar energies/forces
!                           (in kJ/mol/A^2)
! 8 double smvolume
! 9 double smsize
!
  _REAL_ :: r_param(9)
  
!-----------------------------------------------------------------------
!   i_param members
!
! 1 int type;         : What type of MG calculation?
!                            0: sequential manual
!                            1: sequential auto-focus
!                            2: parallel auto-focus
! 2 int nlev;         : Levels in multigrid hierarchy
! 3 int cmeth;        : Centering method:
!                            0: center on point,
!                            1: center on molecule
! 4 int ccmeth;       : Coarse grid centering method:  0 => center
!                       on point, 1 => center on molecule
! 5 int fcmeth;       : Fine grid centering method:  0 => center on
!                       point, 1 => center on molecule
! 6 int chgm            Types of charge discretization methods
!                            0: Trilinear interpolation of charge to 
!                       8 nearest grid points. The traditional method;
!                       not particularly good to use with PBE forces. 
!                            1: Cubic B-spline across nearest- and
!                       next-nearest-neighbors.  Mainly for use in 
!                       grid-sensitive applications 
!                       (such as force calculations).
!
! 7 int nonlin;           : 0 => LPBE, 1 => NPBE, 2: LRPBE,
!                           3: NRPBE, 4: SMPBE
! 8 int bcfl;             : Boundary condition: 0 => zero, 1 => single
!                           Debye-Huckel sphere, 2 => multiple Debye-
!                           Huckel spheres, 4 => focusing
! 9 int srfm;             : Surface calculation method
!                              0: Mol surface for epsilon; inflated VdW
!                                 for kappa; no smoothing
!                              1: As 0 with harmoic average
!                                 smoothing
!                              2: Cubic spline
!10 int calcenergy;       : Energy calculation
!                              0: don't calculate out energy
!                              1: calculate total energy
!                              2: calculate total energy and all energy
!                                 components
!11 int calcforce;        : Atomic forces I/O
!                              0: don't calculate forces
!                              1: calculate net forces on molecule
!                              2: calculate atom-level forces

!12 int wpot              : write potential map
!13 int wchg              : write charge map
!14 int wsmol             : write smol map
!15 int wkappa            : write kappa map
!16 int wdiel             : write diel maps (x, y and z)
!17 int watompot          : write atom potentials map
!18 int rpot              : read pot map
!19 dummy
!20 calcnpforce
!21 calcnpenergy
!22 int nion;             : Number of counterion species
!23 int rchg;             : read charge map
!24 int rkappa            : read kappa map
!25 int rdiel             : read diel maps (x, y and z)


  INTEGER :: i_param(25)

!-----------------------------------------------------------------------
!   int dime[3];                 : Grid dimensions
!   int pdime[3];                : Grid of processors to be used in
!                                  calculation
!
  INTEGER :: dime(3), pdime(3)

!-----------------------------------------------------------------------
!   double grid[3];             : Grid spacings
!   double glen[3];             : Grid side lengths.
!   double center[3];           : Grid center. If ispart = 0, then this is
!                                 only meaningful if cmeth = 0.  However, if
!                                 ispart = 1 and cmeth = 0, then this is the
!                                 center of the non-disjoint (overlapping)
!                                 partition.  If ispart = 1 and cmeth = 1, then
!                                 this is the vector that must be added to the
!                                 center of the molecule to give the center of
!                                 the non-disjoint partition. 
!   double cglen[3];            : Coarse grid side lengths
!   double fglen[3];            : Fine grid side lengths
!   double ccenter[3];          : Coarse grid center. 
!   double fcenter[3];          : Fine grid center. 
!   double ofrac;               : Overlap fraction between procs
!
  _REAL_ :: grid(3), glen(3), center(3), cglen(3), fglen(3)
  _REAL_ :: ccenter(3), fcenter(3), ofrac

!-----------------------------------------------------------------------
! mobile ion definition
!
!   double ionq[MAXION];   : Counterion charges (in e)
!   double ionc[MAXION];   : Counterion concentrations (in M)
!   double ionr[MAXION];   : Counterion radii (in A)
!
  _REAL_ :: ionq(MAXION), ionc(MAXION), ionrr(MAXION)

!-----------------------------------------------------------------------
! internal PB radii and charges
  _REAL_ :: pbradii(APBSNATOMS)    ! PB radii
  _REAL_ :: pbcg(APBSNATOMS)       ! PB charges

!-----------------------------------------------------------------------
! solvation energy and forces saved for use later
  _REAL_ :: senelec, sennp
  _REAL_ :: solvfrcx(APBSNATOMS)
  _REAL_ :: solvfrcy(APBSNATOMS)
  _REAL_ :: solvfrcz(APBSNATOMS)

  _REAL_ :: geom_upd_limit, evdw_upd_limit, saveevdw
  _REAL_ :: savedx(APBSNATOMS)
  _REAL_ :: savedy(APBSNATOMS)
  _REAL_ :: savedz(APBSNATOMS)

!-----------------------------------------------------------------------
! some integer variables
!
! napbs - how often we calculate APBS forces during MD/minimization
! umeth - forces update method
! apbs_debug - debug/verbosity value [0-5]
! apbs_print - debug/verbosity value [0-5]
! radiopt - optimization of radii method (see the source for details)
!                             
  INTEGER :: napbs, umeth, apbs_debug, apbs_print, radiopt

!-----------------------------------------------------------------------
! logical variables
!
! qapbs: were all parameters parsed?
! qfapbs: do we want to calculate solvation forces?
! qaparsed: did we parse all user options?
! dime_updates: update grid dimensions on the fly
  LOGICAL :: qapbs, qfapbs, qaparsed, dime_updates

!-----------------------------------------------------------------------
! string variables
!
! pqr: file name for an external pqr file (reading radii and charges)
!
  CHARACTER(len=MAX_FN_LEN) pqr


!-----------------------------------------------------------------------
!
!  commons
!
!-----------------------------------------------------------------------
!
! disable COMMONs, we are using modules
!
!-----------------------------------------------------------------------
! integer common
!  COMMON / apbs_int / ispara, i_pbeparm, set_pbeparm, i_mgparm, &
!       dime, pdime, set_mgparm, napbs, umeth, apbs_debug

!-----------------------------------------------------------------------
! double precision common
!  COMMON / apbs_real / r_pbeparm, ionq, ionc, ionrr, &
!       grid, glen, center, cglen, fglen, &
!       ccenter, fcenter, ofrac, pbcg, pbradii, &
!       senelec, sennp, solvfrcx, solvfrcy, solvfrcz

!-----------------------------------------------------------------------
! logical common
!  COMMON / apbs_logical/ qapbs, qfapbs, qaparsed
!
!
!  SAVE / apbs_int /
!  SAVE / apbs_real /
!  SAVE / apbs_logical /

#endif /* APBS */
END MODULE apbs_vars
