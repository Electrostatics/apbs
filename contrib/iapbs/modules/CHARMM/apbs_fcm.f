CHARMM Element source/fcm/apbs.fcm
##IF APBS (apbs_fcm)
c
c


c-----------------------------------------------------------------------
c
c                 iAPBS variables definition
c
c Similar type of varibles are bunched together (integer/integer and
c real/real) to mimize fortran/C passing incompatability.
c
c For detailed description of individual variables please see
c APBS Programmer's Guide.
c
c-----------------------------------------------------------------------

      integer*4 MAXION, NATOMS
      parameter (MAXION = 2, NATOMS = MAXAIM)

c-----------------------------------------------------------------------
c     int ispara;          /**< 1 => is a parallel calculation,
c                           *  0 => is not */
!      integer ispara

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
!13 int wchg
!14 int wsmol
!15 wkappa
!16 wdiel
!17 dummy
!18 dummy
!19 dummy
!20 dummy
!21 calcnpenergy
!22 int nion;             : Number of counterion species
!23 int rchg;             : read charge map
!24 int rkappa            : read kappa map
!25 int rdiel             : read diel maps (x, y and z)

      integer*4 i_param(25)
c-----------------------------------------------------------------------
c     r_pbeparm members
c
c 1 double pdie;        /**< Solute dielectric */
c 2 double sdie;        /**< Solvent dielectric */
c 3 double srad;        /**< Solvent radius */
c 4 double swin;        /**< Cubic spline window */
c 5 double temp;        /**< Temperature (in K) */
c 6 double gamma;       /**< Surface tension for apolar energies/forces
c                        * (in kJ/mol/A^2) */
c 7 double sdens;       /**< Vacc sphere density */
c

c-----------------------------------------------------------------------
c r_param memebers
c 1 double pdie;          : Solute dielectric
c 2 double sdie;          : Solvent dielectric
c 3 double srad;          : Solvent radius
c 4 double swin;          : Cubic spline window
c 5 double temp;          : Temperature (in K)
c 6 double sdens;         : Vacc sphere density
c 7 double gamma;         : Surface tension for apolar energies/forces
c                           (in kJ/mol/A^2)
c 8 double smvolume
c 9 double smsize
c

      double precision r_param(9)

c-----------------------------------------------------------------------
c   set_pbeparm members
c
c 1   cpbeparm->setnonlin = 1;
c 2   cpbeparm->setbcfl = 1;
c 3   cpbeparm->setnion = 1;
c 4   cpbeparm->setpdie = 1;
c 5   cpbeparm->setsdie = 1;
c 6   cpbeparm->setsrfm = 1;
c 7   cpbeparm->setsrad = 1;
c 8   cpbeparm->setswin = 1;
c 9   cpbeparm->settemp = 1;
c 10  cpbeparm->setgamma = 1;
c 11  cpbeparm->setcalcenergy = 1;
c 12  cpbeparm->setcalcforce = 0;
c 13  cpbeparm->setsdens = 1;
c       

!      integer set_pbeparm(13)

c-----------------------------------------------------------------------
c   i_mgparm members
c
c 1 int type;       /**< What type of MG calculation?
c                    *   \li 0: sequential manual
c                    *   \li 1: sequential auto-focus
c                    *   \li 2: parallel auto-focus */
c 2 int nlev;       /**< Levels in multigrid hierarchy
c 3 int cmeth;      /**< Centering method:
c                    *   \li 0: center on point,
c                    *   \li 1: center on molecule */
c 4 int ccmeth;     /**< Coarse grid centering method:  0 => center
c                    * on point, 1 => center on molecule */
c 5 int fcmeth;     /**< Fine grid centering method:  0 => center on
c                    * point, 1 => center on molecule */
c 6 int chgm        /**  Types of charge discretization methods
c                    *   \li 0: Trilinear interpolation of charge to 
c                    *    8 nearest grid points. The traditional method;
c                    *    not particularly good to use with PBE forces. 
c                    *   \li 1: Cubic B-spline across nearest- and
c                    *    next-nearest-neighbors.  Mainly for use in 
c                    *   grid-sensitive applications 
c                    *   (such as force calculations).
c
!      integer i_mgparm(6)

c-----------------------------------------------------------------------
c  set_mgparm members
c
c 1    cmgparm->setdime = 1;
c 2    cmgparm->setgcent = 1;
c 3    cmgparm->setcgcent = 1;
c 4    cmgparm->setfgcent = 1;
c
c    /* not using grid */
c 5    cmgparm->setcglen = 1;
c 6    cmgparm->setfglen = 1;
c 7    cmgparm->setglen = 1;
c 8    cmgparm->setgrid = 0;
c 9    cmgparm->setchgm = ;
c
!      integer set_mgparm(9)

c-----------------------------------------------------------------------
c   int dime[3];               /**< Grid dimensions */
c   int pdime[3];              /**< Grid of processors to be used in
c                               * calculation */
c
      integer*4 dime(3), pdime(3)

c-----------------------------------------------------------------------
c   double grid[3];            /**< Grid spacings */
c   double glen[3];            /**< Grid side lengths. */
c   double center[3];          /**< Grid center. If ispart = 0, then this is
c                               * only meaningful if cmeth = 0.  However, if
c                               * ispart = 1 and cmeth = 0, then this is the
c                               * center of the non-disjoint (overlapping)
c                               * partition.  If ispart = 1 and cmeth = 1, then
c                               * this is the vector that must be added to the
c                               * center of the molecule to give the center of
c                               * the non-disjoint partition.  */c
c   double cglen[3];           /**< Coarse grid side lengths */
c   double fglen[3];           /**< Fine grid side lengths */
c   double ccenter[3];         /**< Coarse grid center.  */
c   double fcenter[3];         /**< Fine grid center.  */
c   double ofrac;              /**< Overlap fraction between procs */
c
      double precision grid(3), glen(3), center(3), cglen(3), fglen(3)
      double precision ccenter(3), fcenter(3), ofrac

c-----------------------------------------------------------------------
c mobile ion definition
c
c   double ionq[MAXION]; /**< Counterion charges (in e) */
c   double ionc[MAXION]; /**< Counterion concentrations (in M) */
c   double ionr[MAXION]; /**< Counterion radii (in A) */
c
      double precision ionq(MAXION), ionc(MAXION), ionrr(MAXION)

c-----------------------------------------------------------------------
c atom
c    double position[3];     /**< Atomic position */
c    double radius;          /**< Atomic radius   */
c    double charge;          /**< Atomic charge   */
c these are defined inside of CHARMM

c-----------------------------------------------------------------------
c radii from wmain get saved in a_radius
      double precision a_radius(NATOMS)

c-----------------------------------------------------------------------
c solvation energy and forces saved for use later
      double precision senelec, sennp
      double precision solvfrcx(NATOMS), solvfrcy(NATOMS)
      double precision solvfrcz(NATOMS)

c-----------------------------------------------------------------------
c some integer variables
c
c napbs - how often we calculate APBS forces during MD/minimization
c umeth - forces update method
c a_debug - debug/verbosity value [0-5]
c
      integer*4 napbs, umeth, apbs_debug

c-----------------------------------------------------------------------
c logical variables
c
c qapbs: were all parameters parsed?
c qfapbs: do we want to calculate solvation forces?
c qaparsed: did we parse all user options?
c dime_updates: update grid dimensions on the fly
      logical qapbs, qfapbs, qaparsed, qdime_updates

c-----------------------------------------------------------------------
c
c  commons
c
c-----------------------------------------------------------------------

c-----------------------------------------------------------------------
c integer common
      common / apbs1 / i_param, 
     +     dime, pdime, napbs, umeth, apbs_debug

c-----------------------------------------------------------------------
c double precision common
      common / apbs2 / r_param, ionq, ionc, ionrr, 
     +     grid, glen, center, cglen, fglen,
     +     ccenter, fcenter, ofrac, a_radius,
     +     senelec, sennp, solvfrcx, solvfrcy, solvfrcz

c-----------------------------------------------------------------------
c logical common
      common / apbs3 / qapbs, qfapbs, qaparsed, qdime_updates


##IF SAVEFCM (save)
      SAVE / APBS1 /
      SAVE / APBS2 /
      SAVE / APBS3 /
##ENDIF (save)
##ENDIF (apbs_fcm)

