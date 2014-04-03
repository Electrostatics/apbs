!
! This is iAPBS/sander module for performing APBS calculations in sander.
!
! For more information please see http://mccammon.ucsd.edu/iapbs/
!
#include "../include/dprec.fh"

MODULE apbs
#ifdef APBS

! rokFIXME add timer calls

CONTAINS
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  SUBROUTINE apbs_read()
!
! read in APBS user input parameters and set defaults
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    use apbs_vars
    use file_io_dat, only : mdin_apbs, sp_apbs
    IMPLICIT NONE

    INTEGER :: i
    INTEGER :: nonlin, bcfl, nion, srfm, calcenergy, calcforce
    INTEGER :: calc_type, nlev, cmeth, ccmeth, fcmeth, chgm
    INTEGER :: calcnpenergy, calcnpforce
    INTEGER :: wpot, wchg, wsmol, wkappa, wdiel
    INTEGER :: rchg, rkappa, rdiel, watompot, rpot
    _REAL_ :: pdie, sdie, srad, swin, temp, gamma, sdens
    _REAL_ :: smvolume, smsize
    NAMELIST /apbs/ dime, pdime, cglen, fglen, grid, nlev, &
         nonlin, bcfl, nion, pdie, sdie, srfm, chgm, srad, swin, &
         temp, gamma, sdens, calc_type, calcnpenergy, calcnpforce, &
         cmeth, ccmeth, fcmeth, ionq, ionc, ionrr, &
         smvolume, smsize, &
         calcenergy, calcforce, apbs_debug, sp_apbs, apbs_print, &
         wpot, wchg, wsmol, ispara, radiopt, geom_upd_limit, &
         evdw_upd_limit, pqr, dime_updates, &
         wkappa, wdiel, rchg, rkappa, rdiel, watompot, rpot

! rokFIXME: also add maps-related and centering-related (center) keywords

    ! Default values
    ! PBEparm
    ispara = 0
!   i_pbeparm(1) = 1 ! molid
    nonlin = 0
    bcfl = 1
    nion = 0
    srfm = 2
    pdie = 2.0
    sdie = 78.4
    srad = 1.4
    swin = 0.3
    temp = 298.15
    gamma = 0.105
    sdens = 10.0
    gamma = 0.105
    smvolume = 10.0
    smsize = 1000.0

    calcenergy = 2
    calcforce = 2
    calcnpenergy = 1
    calcnpforce = 2
    wpot = 0
    wchg = 0
    wsmol = 0
    wkappa = 0
    wdiel= 0
    watompot = 0
    rchg = 0
    rkappa = 0
    rdiel = 0
    rpot = 0

    ! MGparm
    calc_type = 1 ! 0 - manual MG, 1- autoMG, 2- parallel MG 
    nlev = 4
    cmeth = 1
    ccmeth = 1
    fcmeth = 1
    chgm = 1

    pdime = (/ 0, 0, 0 /)
    ofrac = 0.1

    !    ionq  = (/ 1.0, -1.0 /)
    !    ionc  = (/ 0.15, 0.15 /)
    !    ionrr = (/ 2.0, 2.0 /)

    dime    = (/ 0, 0, 0 /)
    grid    = (/ 0.0, 0.0, 0.0 /)
    glen    = (/ 0.0, 0.0, 0.0 /)
    center  = (/ 0.0, 0.0, 0.0 /)
    cglen   = (/ 0.0, 0.0, 0.0 /)
    fglen   = (/ 0.0, 0.0, 0.0 /)
    ccenter = (/ 0.0, 0.0, 0.0 /)
    fcenter = (/ 0.0, 0.0, 0.0 /)

    ! is this a single point energy calculation?
    ! default is no
    sp_apbs = .FALSE.
    ! printing verbosity
    apbs_print = 1
    ! debuging flag
    apbs_debug = 0
    ! radii optimization option
    radiopt = 0

    ! number of PB steps
    napbs = 0
    ! APBS update forces schema geom limit
    ! 0.0 means: do updates every apbs_force call
    geom_upd_limit = 0.0
    evdw_upd_limit = 0.0

    ! read in APBS input parameters
    IF (mdin_apbs) THEN
       REWIND 5
       READ(5, nml=apbs)
    ELSE
       WRITE(6, '(a)') 'iAPBS: WARNING: did NOT read in any APBS parameters!'
       WRITE(6, '(a)') 'iAPBS: Exiting ...'
       CALL mexit(6,1)
    END IF

    i_param(1) = calc_type
    i_param(2) = nlev 
    i_param(3) = cmeth
    i_param(4) = ccmeth
    i_param(5) = fcmeth
    i_param(6) = chgm
    i_param(7) = nonlin
    i_param(8) = bcfl
    i_param(9) = srfm
    i_param(10) = calcenergy
    i_param(11) = calcforce
    i_param(12) = wpot
    i_param(13) = wchg
    i_param(14) = wsmol
    i_param(15) = wkappa
    i_param(16) = wdiel
    i_param(17) = watompot
    i_param(18) = rpot
    i_param(19) = 0
    i_param(20) = calcnpforce
    i_param(21) = calcnpenergy
    i_param(22) = nion
    i_param(23) = rchg
    i_param(24) = rkappa
    i_param(25) = rdiel

    r_param(1) = pdie
    r_param(2) = sdie
    r_param(3) = srad
    r_param(4) = swin
    r_param(5) = temp
    r_param(6) = sdens
    r_param(7) = gamma
    r_param(8) = smvolume
    r_param(9) = smsize

  END SUBROUTINE apbs_read

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  SUBROUTINE apbs_init(natom, x, cg, radii)
!
! initialize APBS charges and radii
! in: natom
!     cg (atomic charges)
!     radii (atomi radii)
!
! on exit pbcg and pbradii are modified (via 'use apbs_vars')
!
! these values are then constant during simulation
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    use apbs_vars

    IMPLICIT NONE
    ! passed in variables
    INTEGER, INTENT(in) :: natom
    _REAL_, INTENT(in) :: x(3*natom), cg(natom), radii(natom)

    ! local variables
    INTEGER :: i, j, dummyi, ios, numline=0
    CHARACTER :: dummyc
    CHARACTER (len = 128) string
    _REAL_ :: dummyr,  maxx, minx, maxy, miny, maxz, minz
    _REAL_ :: cx(natom), cy(natom), cz(natom)
    _REAL_ :: tmpcg, tmprad, tot_charge

    WRITE(6, '(a)') 'iAPBS: Initializing APBS interface'


! possible additions:
! - call pb_aaradi() (from sa_driver.f)
! - call phi_aaradi() (from sa_driver.f)
!    CALL pb_aaradi( natom, nbonh, ibh, jbh, pbradii, acrg, ucrgh, ucrga, &
!         resid, igraph, isymbl, radii )
!
    IF (radiopt == 0) THEN
       WRITE(6, '(a)') & 
            'iAPBS: Using charge/radii definition from prmtop file'
       pbradii = radii
       pbcg = cg / 18.2223d0 ! 18.2223d0 is used to convert amber charge 
                             ! to units of the electron charge
    ELSE IF (radiopt == 1) THEN
       ! read from a file
       ! file format: atom chrg radius
       WRITE(6, '(a)') & 
            'iAPBS: Reading chg/radii definition from pbparamsin file'
       OPEN(21, file="pbparamsin", status="old")
       DO i = 1, natom
          READ(21,*) dummyc, pbcg(i), pbradii(i)
       END DO
       CLOSE(21)

    ELSE IF (radiopt == 2) THEN
       ! read cg and rad from PQR file
       WRITE(6, '(2a)') &
            'iAPBS: Reading charge/radii definition from pqr filename: ', pqr
       OPEN(21, file=pqr, status="old")
       DO
          READ (21, '(a)', iostat=ios) string
          IF ( ios /= 0 ) EXIT
          IF (string(1:4) == 'ATOM' ) THEN
             READ(string, * , iostat=ios) &
                  dummyc,  dummyi, dummyc, dummyc, dummyi, &
                  dummyr, dummyr, dummyr, tmpcg, tmprad
             IF ( ios /= 0 ) EXIT
             numline = numline + 1
             pbcg(numline) = tmpcg
             pbradii(numline) = tmprad
          END IF
       END DO
       IF (numline /= natom) THEN
          WRITE(6, '(a)') 'Wrong number of atoms in PQR file!'
          CALL mexit(6,1)
       END IF
       CLOSE(21)

    ELSE IF (radiopt == 3) THEN
       ! read rad PQR file
       WRITE(6, '(2a)') &
            'iAPBS: Reading radii definition from pqr filename: ', pqr
       OPEN(21, file=pqr, status="old")
       DO
          READ (21, '(a)', iostat=ios ) string
          IF ( ios /= 0 ) EXIT
          IF (string(1:4) == 'ATOM' ) THEN
             READ(string, * , iostat=ios) &
                  dummyc,  dummyi, dummyc, dummyc, dummyi, &
                  dummyr, dummyr, dummyr, dummyr, tmprad
             IF ( ios /= 0 ) EXIT
             numline = numline + 1
             pbradii(numline) = tmprad
          END IF
       END DO
       IF (numline /= natom) THEN
          WRITE(6, '(a)') 'Wrong number of atoms in PQR file!'
          CALL mexit(6,1)
       END IF
       CLOSE(21)
       pbcg = cg / 18.2223d0 ! 18.2223d0 is used to convert amber charge 
                             ! to units of the electron charge
    ELSE
       WRITE(6,'(a)') 'iAPBS: Unknown radiopt option.'
       CALL mexit(6,1)
    END IF

    ! unpack coordinates
    DO i = 1, natom
       j = 3*(i-1)
       cx(i) = x(j+1)
       cy(i) = x(j+2)
       cz(i) = x(j+3)
    END DO

    IF (apbs_print > 1) THEN
       WRITE(6, '(a)') 'iAPBS: natom, x, y, z, PB charge, PB radius'
       DO i = 1, natom
          WRITE(6, '(i4, 5f8.3)') i, cx(i), cy(i), cz(i), pbcg(i), pbradii(i)
       END DO
       WRITE(6,'()')
    END IF

    maxx = cx(1)
    minx = cx(1)
    maxy = cy(1)
    miny = cy(1)
    maxz = cz(1)
    minz = cz(1)
    tot_charge = 0.0
    DO i = 1, natom
       IF(maxx < cx(i)+pbradii(i)) maxx = cx(i)+pbradii(i)
       IF(minx > cx(i)-pbradii(i)) minx = cx(i)-pbradii(i)
       IF(maxy < cy(i)+pbradii(i)) maxy = cy(i)+pbradii(i)
       IF(miny > cy(i)-pbradii(i)) miny = cy(i)-pbradii(i)
       IF(maxz < cz(i)+pbradii(i)) maxz = cz(i)+pbradii(i)
       IF(minz > cz(i)-pbradii(i)) minz = cz(i)-pbradii(i)
       tot_charge = tot_charge + pbcg(i)
    END DO

    IF (apbs_print > 1) THEN
       WRITE(6,'(a, 3f8.3)') 'iAPBS: Molecular dimensions: ', &
            maxx-minx, maxy-miny, maxz-minz
       WRITE(6,'(a, f8.3)') 'iAPBS: Total charge: ', tot_charge
    END IF

! for mg-auto calculate missing grid parameters if dime = 0
    IF ((i_param(1) == 0 .OR. i_param(1) == 1) .AND. dime(1) == 0) THEN
       cglen(1) = 1.7 * (maxx-minx)
       cglen(2) = 1.7 * (maxy-miny)
       cglen(3) = 1.7 * (maxz-minz)
       fglen(1) = 20.0 + (maxx-minx)
       fglen(2) = 20.0 + (maxy-miny)
       fglen(3) = 20.0 + (maxz-minz)

       DO i = 1, 3
          IF (fglen(i) > cglen(i)) cglen(i) = fglen(i)
       END DO

       WRITE(6, '(a)') 'iAPBS: Grid dime not specified, calculating ...'
       WRITE(6, '(a)') &
            'iAPBS: Requesting dime re-calculation on the fly'
       dime_updates = .TRUE.
       DO i = 1, 3
          dime(i) = &
               32*(INT((INT(fglen(i)/grid(i) + 0.5) - 1)/32.0 + 0.5)) + 1
          IF (dime(i) < 33) dime(i) = 33
       END DO
    END IF

    IF (apbs_print > 0) THEN
       WRITE(6, '(a)') 'iAPBS: Grid values: '
       WRITE(6, '(a, 3f8.3)') 'iAPBS: fglen: ', fglen(1), fglen(2), fglen(3)
       WRITE(6, '(a, 3f8.3)') 'iAPBS: cglen: ', cglen(1), cglen(2), cglen(3)
       WRITE(6, '(a, 3i4)')   'iAPBS: dime: ', dime(1), dime(2), dime(3)
       WRITE(6, '(a, 3f8.3)') 'iAPBS: grid: ', grid(1), grid(2), grid(3)
       
       WRITE(6, '(a, f10.3)') 'iAPBS: Required memory (in MB): ', &
            dime(1)*dime(2)*dime(3)*200.0/1024/1024
    END IF

    CALL apbs_print_info()
  
  END SUBROUTINE apbs_init

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  SUBROUTINE apbs_print_info()
!
! Print out APBS calculation parameters
!
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    use apbs_vars
    IMPLICIT NONE

    WRITE(6,'()')
    WRITE(6, '(a)') ' APBS calculation parameters:'
    WRITE(6,'()')

! nonlin
    IF (i_param(7) == 0) THEN
       WRITE(6, '(a)')'Linearized traditional PBE'
    ELSE IF (i_param(7) == 1) THEN
       WRITE(6, '(a)')'Nonlinear traditional PBE'
    ELSE IF (i_param(7) == 2) THEN
       WRITE(6, '(a)')'Linearized RPBE'
    ELSE IF (i_param(7) == 3) THEN
       WRITE(6, '(a)')'Nonlinear RPBE'
    ELSE IF (i_param(7) == 4) THEN
       WRITE(6, '(a)')'Size-Modified PBE'
    ELSE
       WRITE(6, '(a)')'Unknown PBE option'
       CALL mexit(6,1)
    END IF

! boundary conditions, bcfl
    IF (i_param(8) == 0) THEN
       WRITE(6, '(a)') 'Zero boundary conditions'
    ELSE IF (i_param(8) == 1) THEN
       WRITE(6, '(a)') 'Single Debye-Huckel sphere boundary conditions'
    ELSE IF (i_param(8) == 2) THEN
       WRITE(6, '(a)') 'Multiple Debye-Huckel sphere boundary conditions'
    ELSE IF (i_param(8) == 4) THEN
       WRITE(6, '(a)') 'Focusing boundary conditions'
    ELSE
       WRITE(6, '(a)') 'Unknown boundary conditions option'
       CALL mexit(6,1)
    END IF

! surface definition, srfm
    IF (i_param(9) == 0) THEN
       WRITE (6, '(a)') 'Molecular surface definition'
    ELSE IF (i_param(9) == 1) THEN
       WRITE (6, '(a)') 'Smoothed molecular surface definition'
    ELSE IF (i_param(9) == 2) THEN
       WRITE (6, '(a)') 'Using cubic-spline surface definition'
    ELSE IF (i_param(9) == 3) THEN
       WRITE (6, '(a)') 'Using 7-order polynomial spline surface'
    ELSE
       WRITE(6, '(a)') 'Unknown surface definition'
       CALL mexit(6,1)
    END IF

! charge discretization, chgm
    IF (i_param(6) == 0) THEN
       WRITE(6, '(a)') 'Using trilinear interpolation (linear splines)'
    ELSE IF(i_param(6) == 1) THEN
       WRITE(6, '(a)') 'Using cubic B-spline charge discretization'
    ELSE IF(i_param(6) == 2) THEN
       WRITE(6, '(a)') 'Using quintic B-spline charge discretization'
    ELSE
       WRITE(6, '(a)') 'Unknown charge discretization'
       CALL mexit(6,1)
    END IF

!   WRITE(6,*) 'Number of grid points for grid-based discretization:', dime
   WRITE(6,'(a, 3i4)') 'Grid dimension:', dime(1), dime(2), dime(3)
   IF (cglen(1) > 0.) THEN
      WRITE(6,'(a, 3f8.3, a)') 'Coarse grid lengths:', &
           cglen(1), cglen(2), cglen(3), ' A'
   END IF
   IF (fglen(1) > 0.) THEN
      WRITE(6,'(a, 3f8.3, a)') 'Fine grid lengths:', &
           fglen(1), fglen(2), fglen(3), ' A'
!      WRITE(6,'(a, 3f8.3)') ' Grid spacings:', fglen(1)/dime(1), &
!           fglen(2)/dime(2), fglen(3)/dime(3)
   END IF
   IF (grid(1) > 0.) THEN
      WRITE(6,'(a, 3f8.3, a)') 'Grid spacings:', &
           grid(1), grid(2), grid(3), ' A'
   END IF
   WRITE(6,'(a, f8.3)') 'Solute dielectric (pdie):', r_param(1)
   WRITE(6,'(a, f8.3)') 'Solvent dielectric (sdie):', r_param(2)
   WRITE(6,'(a, f8.3, a)') 'Temperature:', r_param(5), ' K'
   WRITE(6,'(a, f8.3, a)') 'Surface sphere density (sdens):', &
        r_param(6), ' grid points/A^2'
   WRITE(6,'(a, f8.3, a)') 'Surface tension:', r_param(7), ' kJ/mol/A'

   IF (radiopt == 0) THEN
      WRITE(6, '(a)') 'Using charge/radii information from prmtop file'
   ELSE IF (radiopt == 1) THEN
      WRITE(6, '(a)') 'Using charge/radii information from pbparamsin file'
   ELSE IF (radiopt == 2) THEN
      WRITE(6, '(a)') 'Using charge/radii information from PQR file'
   ELSE IF (radiopt == 3) THEN
      WRITE(6, '(a)') 'Using radii information from PQR file'
   ELSE
      WRITE(6, '(a)') 'Unknown radiopt selection'
      CALL mexit(6,1)
   END IF

   IF (i_param(10) == 0) THEN
      WRITE(6, '(a)') 'No electrostatic energy will be calculated'
   ELSE IF (i_param(10) == 1) THEN
      WRITE(6, '(a)') 'Total electrostatic energy will be calculated'
   ELSE IF (i_param(10) == 2) THEN
      WRITE(6, '(a)') 'Total and per atom electrostatic energy will be calculated'
   ELSE
      WRITE(6, '(a)') 'Unknown calcenergy selection'
      CALL mexit(6,1)
   END IF

   IF (i_param(21) == 0) THEN
      WRITE(6, '(a)') 'No apolar energy will be calculated'
   ELSE IF (i_param(21) == 1) THEN
      WRITE(6, '(a)') 'Total apolar energy will be calculated'
   ELSE IF (i_param(21) == 2) THEN
      WRITE(6, '(a)') 'Total and per atom apolar energy calculation is not supported'
   ELSE
      WRITE(6, '(a)') 'Unknown calcnpenergy selection'
      CALL mexit(6,1)
   END IF

   IF (i_param(11) == 0) THEN
      WRITE(6, '(a)') 'No electrostatic forces will be calculated'
   ELSE IF (i_param(11) == 1) THEN
      WRITE(6, '(a)') 'Total electrostatic forces will be calculated'
   ELSE IF (i_param(11) == 2) THEN
      WRITE(6, '(a)') 'Total and per atom electrostatic forces will be calculated'
   ELSE
      WRITE(6, '(a)') 'Unknown calcforce selection'
      CALL mexit(6,1)
   END IF

   IF (i_param(20) == 0) THEN
      WRITE(6, '(a)') 'No apolar forces will be calculated'
   ELSE IF (i_param(20) == 1) THEN
      WRITE(6, '(a)') 'Total apolar forces will be calculated'
   ELSE IF (i_param(20) == 2) THEN
      WRITE(6, '(a)') 'Total and per atom apolar forces will be calculated'
   ELSE
      WRITE(6, '(a)') 'Unknown calcnpforce selection'
      CALL mexit(6,1)
   END IF

   IF (i_param(12) == 1) THEN
      WRITE(6, '(a)') 'Writing potential to iapbs-pot.dx.'
   END IF
   IF (i_param(13) == 1) THEN
      WRITE(6, '(a)') 'Writing charge distribution to iapbs-charge.dx.'
   END IF
   IF (i_param(14) == 1) THEN
      WRITE(6, '(a)') 'Writing molecular accessibility to iapbs-smol.dx.'
   END IF
   IF (i_param(15) == 1) THEN
      WRITE(6, '(a)') 'Writing kappa map to iapbs-kappa.dx.'
   END IF
   IF (i_param(16) == 1) THEN
      WRITE(6, '(a)') 'Writing dielectric map to iapbs-dielXYZ.dx'
   END IF
   IF (i_param(17) == 1) THEN
      WRITE(6, '(a)') 'Writing atomic potential map to iapbs-atompot.dx'
   END IF
   IF (i_param(23) == 1) THEN
      WRITE(6, '(a)') 'Reading charge map data from iapbs-charge.dx.'
   END IF
   IF (i_param(24) == 1) THEN
      WRITE(6, '(a)') 'Reading kappa map data from iapbs-kappa.dx.'
   END IF
   IF (i_param(25) == 1) THEN
      WRITE(6, '(a)') 'Reading dielectric map data from iapbs-dielXYZ.dx.'
   END IF
   IF (i_param(18) == 1) THEN
      WRITE(6, '(a)') 'Reading potential map data from iapbs-pot.dx.'
   END IF

   WRITE(6,'()')

  END SUBROUTINE apbs_print_info

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  SUBROUTINE apbs_spenergy(natom, x, f, eelt, enpol)
!
! single point PB energy calculation
!
! in:
!  natom
!  x: coords
!  f: forces
! out:
!  eelt: electrostatic energy (polar)
!  enpol: apolar energy
!
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    use apbs_vars

    IMPLICIT NONE

    ! passed in variables
    INTEGER, INTENT(in) :: natom
    _REAL_, INTENT(in)  :: x(3*natom)
    _REAL_ :: f(3*natom)
    _REAL_, INTENT(out) :: eelt, enpol

    ! local variables
    INTEGER :: i, j, rc, apbsdrv, ncalc(1)
    LOGICAL :: skip
    _REAL_ :: cx(natom), cy(natom), cz(natom)
    _REAL_ :: maxx, minx, maxy, miny, maxz, minz
    _REAL_ :: esenerg(15)
    _REAL_ :: npenerg(15)
    _REAL_ :: apbsdx(natom), apbsdy(natom), apbsdz(natom)
    _REAL_ :: apbsqfx(natom), apbsqfy(natom), apbsqfz(natom)
    _REAL_ :: apbsibx(natom), apbsiby(natom), apbsibz(natom)
    _REAL_ :: apbsnpx(natom), apbsnpy(natom), apbsnpz(natom)
    _REAL_ :: apbsdbx(natom), apbsdby(natom), apbsdbz(natom)
    _REAL_ :: apbsgrid_meta(13), apbsgrid(3*natom)

    !    eelt = 0.d0; enpol = 0.d0 rokFIXME ?

    ! initialization
    DO i = 1, 13
       apbsgrid_meta(i) = 0.0
    END DO
    DO i = 1, 3*natom
       apbsgrid(i) = 0.0
    END DO

    ! unpack coordinates
    DO i = 1, natom
       j = 3*(i-1)
       cx(i) = x(j+1)
       cy(i) = x(j+2)
       cz(i) = x(j+3)
    END DO

    esenerg(1) = 0.0
    npenerg(1) = 0.0

    IF (apbs_debug > 5) THEN
       WRITE(6,  '(a)') 'iAPBS: unpacked coordinates, charge and radius:'
       DO i = 1, natom
          WRITE(6, '(i4, 5f8.3)') i, cx(i), cy(i), cz(i), &
               pbcg(i), pbradii(i)
       END DO
    END IF

    ! adjust dime parameters if mg-manual and grid only is set
    ! dime_updates == .TRUE.
    IF (dime_updates) THEN
       IF (apbs_print > 1) WRITE(6, '(a)') &
            'iAPBS: Grid dime recalculating on the fly ...'
       maxx = cx(1)
       minx = cx(1)
       maxy = cy(1)
       miny = cy(1)
       maxz = cz(1)
       minz = cz(1)
       DO i = 1, natom
          IF(maxx < cx(i)+pbradii(i)) maxx = cx(i)+pbradii(i)
          IF(minx > cx(i)-pbradii(i)) minx = cx(i)-pbradii(i)
          IF(maxy < cy(i)+pbradii(i)) maxy = cy(i)+pbradii(i)
          IF(miny > cy(i)-pbradii(i)) miny = cy(i)-pbradii(i)
          IF(maxz < cz(i)+pbradii(i)) maxz = cz(i)+pbradii(i)
          IF(minz > cz(i)-pbradii(i)) minz = cz(i)-pbradii(i)
       END DO

       IF (apbs_print > 1) THEN
          WRITE(6,'(a, 3f8.3)') 'iAPBS: Molecular dimensions: ', &
               maxx-minx, maxy-miny, maxz-minz
       END IF
       
       cglen(1) = 1.7 * (maxx-minx)
       cglen(2) = 1.7 * (maxy-miny)
       cglen(3) = 1.7 * (maxz-minz)
       fglen(1) = 20.0 + (maxx-minx)
       fglen(2) = 20.0 + (maxy-miny)
       fglen(3) = 20.0 + (maxz-minz)

       DO i = 1, 3
          IF (fglen(i) > cglen(i)) cglen(i) = fglen(i)
       END DO

       DO i = 1, 3
          dime(i) = &
               32*(INT((INT(fglen(i)/grid(i) + 0.5) - 1)/32.0 + 0.5)) + 1
          IF (dime(i) < 33) dime(i) = 33
       END DO

       IF (apbs_print > 1) THEN
          WRITE(6, '(a)') 'iAPBS: New grid values: '
          WRITE(6, '(a, 3f8.3)') 'fglen: ', fglen(1), fglen(2), fglen(3)
          WRITE(6, '(a, 3f8.3)') 'cglen: ', cglen(1), cglen(2), cglen(3)
          WRITE(6, '(a, 3i4)')    'dime: ', dime(1), dime(2), dime(3)
          WRITE(6, '(a, 3f8.3)')  'grid: ', grid(1), grid(2), grid(3)
          WRITE(6, '(a, f10.3)') 'Required memory (in MB): ', &
               dime(1)*dime(2)*dime(3)*200.0/1024/1024
          WRITE(6,'()')
       END IF

    END IF


    IF (apbs_debug > 1) WRITE(6,  '(a)') 'iAPBS: Calling apbsdrv in apbs_spenergy().'

    ! call APBS as a function

    rc = apbsdrv(natom, cx, cy, cz, pbradii, pbcg, &
         r_param, i_param, &
         grid, dime, pdime, glen, center, &
         cglen, fglen, ccenter, fcenter, &
         ofrac, apbs_debug, &
         ionq, ionc, ionrr, &
         esenerg, npenerg, &
         apbsdx, apbsdy, apbsdz, &
         apbsqfx, apbsqfy, apbsqfz, apbsibx, apbsiby, apbsibz, &
         apbsnpx, apbsnpy, apbsnpz, apbsdbx, apbsdby, apbsdbz, &
         apbsgrid_meta, apbsgrid)

    IF (apbs_debug > 1) WRITE(6, '(a, i2)') '  iAPBS> apbs return code: ', rc
    IF (rc > 0) THEN
       WRITE(6, '(a)') 'iAPBS Bomb: apbs failed'
       CALL mexit(6,1)
    END IF

    ! update total returned energy (in kcal/mol)
    eelt = esenerg(1) / 4.184D0
    enpol = npenerg(1) / 4.184D0
    !         write(*,*)'APBSFRC>after energy...'
    IF (apbs_print > 1) THEN
       WRITE (6, '(a, f10.3, a)') &
            "APBSSP> Electrostatic energy: ", &
            eelt, " kcal/mol"
       WRITE (6, '(a, f10.3, a)') &
            "APBSSP> Nonpolar energy:      ", &
            enpol, " kcal/mol"
    END IF

  END SUBROUTINE apbs_spenergy

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  SUBROUTINE apbs_force(natom, x, f, evdw, eelt, enpol)
!
! PB solvation force calculation
!
! in:
!  natom
!  x: coords
!  f: forces
!  evdw: van der Waals energy
! out:
!  eelt: electrostatic solvation energy (polar)
!  enpol: apolar solvation energy
!
! on exit f gets updated with solvation forces
!
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    use apbs_vars

    IMPLICIT NONE

    ! passed in variables
    INTEGER, INTENT(in) :: natom
    _REAL_, INTENT(in) :: evdw
    _REAL_, INTENT(in) :: x(3*natom)
    _REAL_ :: f(3*natom)
    _REAL_, INTENT(out) :: eelt, enpol

    ! local variables
    INTEGER :: i, j, rc, apbsdrv, ncalc(1), iparam12
    LOGICAL :: skip, do_apbs_update
    CHARACTER (len=128) :: dxname, dxn, command
    _REAL_ :: cx(natom), cy(natom), cz(natom)
    _REAL_ :: dx(natom), dy(natom), dz(natom)
    _REAL_ :: maxx, minx, maxy, miny, maxz, minz
    _REAL_ :: esenerg(15)
    _REAL_ :: npenerg(15)
    _REAL_ :: sdie, gamma, ionc1, ionc2

    _REAL_ :: enelec, ennp
    _REAL_ :: esenvac, esensolv, npenvac, npensolv
    _REAL_ :: apbsdx(natom), apbsdy(natom), apbsdz(natom)
    _REAL_ :: solvdx(natom), solvdy(natom), solvdz(natom)
    _REAL_ :: vacdx(natom), vacdy(natom), vacdz(natom)

    _REAL_ :: apbsqfx(natom), apbsqfy(natom), apbsqfz(natom)
    _REAL_ :: apbsibx(natom), apbsiby(natom), apbsibz(natom)
    _REAL_ :: apbsnpx(natom), apbsnpy(natom), apbsnpz(natom)
    _REAL_ :: apbsdbx(natom), apbsdby(natom), apbsdbz(natom)
    _REAL_ :: solvqfx(natom), solvqfy(natom), solvqfz(natom)
    _REAL_ :: solvibx(natom), solviby(natom), solvibz(natom)
    _REAL_ :: solvnpx(natom), solvnpy(natom), solvnpz(natom)
    _REAL_ :: solvdbx(natom), solvdby(natom), solvdbz(natom)
    _REAL_ :: vacqfx(natom), vacqfy(natom), vacqfz(natom)
    _REAL_ :: vacibx(natom), vaciby(natom), vacibz(natom)
    _REAL_ :: vacnpx(natom), vacnpy(natom), vacnpz(natom)
    _REAL_ :: vacdbx(natom), vacdby(natom), vacdbz(natom)

    _REAL_ :: apbsgrid_meta(13), apbsgrid(3*natom)


    !    eelt = 0.d0; enpol = 0.d0 rokFIXME??

    ! initialization
    DO i = 1, 13
       apbsgrid_meta(i) = 0.0
    END DO
    DO i = 1, 3*natom
       apbsgrid(i) = 0.0
    END DO

    ! initialize solv forces
    DO i = 1, natom
       solvdx(i)=0.0
       solvdy(i)=0.0
       solvdz(i)=0.0
       vacdx(i)=0.0
       vacdy(i)=0.0
       vacdz(i)=0.0
       solvqfx(i)=0.0
       solvqfy(i)=0.0
       solvqfz(i)=0.0
       solvibx(i)=0.0
       solviby(i)=0.0
       solvibz(i)=0.0
       solvnpx(i)=0.0
       solvnpy(i)=0.0
       solvnpz(i)=0.0
       solvdbx(i)=0.0
       solvdby(i)=0.0
       solvdbz(i)=0.0
       vacqfx(i)=0.0
       vacqfy(i)=0.0
       vacqfz(i)=0.0
       vacibx(i)=0.0
       vaciby(i)=0.0
       vacibz(i)=0.0
       vacnpx(i)=0.0
       vacnpy(i)=0.0
       vacnpz(i)=0.0
       vacdbx(i)=0.0
       vacdby(i)=0.0
       vacdbz(i)=0.0
    END DO

    esenerg(1) = 0.0
    npenerg(1) = 0.0


    do_apbs_update = .TRUE.

    ! unpack coordinates
    DO i = 1, natom
       j = 3*(i-1)
       cx(i) = x(j+1)
       cy(i) = x(j+2)
       cz(i) = x(j+3)
    END DO

    IF (apbs_debug > 5) THEN
       WRITE(6,  '(a)') 'iAPBS: unpacked coordinates, charge and radius:'
       DO i = 1, natom
          WRITE(6, '(i4, 5f8.3)') i, cx(i), cy(i), cz(i), &
               pbcg(i), pbradii(i)
       END DO
    END IF

    ! adjust dime parameters if mg-auto and grid is 0
    ! dime_updates == .TRUE.
    IF (dime_updates) THEN
       IF (apbs_print > 1) WRITE(6, '(a)') &
            'iAPBS: Grid dime recalculating on the fly ...'
       maxx = cx(1)
       minx = cx(1)
       maxy = cy(1)
       miny = cy(1)
       maxz = cz(1)
       minz = cz(1)
       DO i = 1, natom
          IF(maxx < cx(i)+pbradii(i)) maxx = cx(i)+pbradii(i)
          IF(minx > cx(i)-pbradii(i)) minx = cx(i)-pbradii(i)
          IF(maxy < cy(i)+pbradii(i)) maxy = cy(i)+pbradii(i)
          IF(miny > cy(i)-pbradii(i)) miny = cy(i)-pbradii(i)
          IF(maxz < cz(i)+pbradii(i)) maxz = cz(i)+pbradii(i)
          IF(minz > cz(i)-pbradii(i)) minz = cz(i)-pbradii(i)
       END DO

       IF (apbs_print > 1) THEN
          WRITE(6,'(a, 3f8.3)') 'iAPBS: Molecular dimensions: ', &
               maxx-minx, maxy-miny, maxz-minz
       END IF
       
       cglen(1) = 1.7 * (maxx-minx)
       cglen(2) = 1.7 * (maxy-miny)
       cglen(3) = 1.7 * (maxz-minz)
       fglen(1) = 20.0 + (maxx-minx)
       fglen(2) = 20.0 + (maxy-miny)
       fglen(3) = 20.0 + (maxz-minz)

       DO i = 1, 3
          IF (fglen(i) > cglen(i)) cglen(i) = fglen(i)
       END DO

       DO i = 1, 3
          dime(i) = &
               32*(INT((INT(fglen(i)/grid(i) + 0.5) - 1)/32.0 + 0.5)) + 1
          IF (dime(i) < 33) dime(i) = 33
       END DO

       IF (apbs_print > 1) THEN
          WRITE(6, '(a)') 'iAPBS: New grid values: '
          WRITE(6, '(a, 3f8.3)') 'fglen: ', fglen(1), fglen(2), fglen(3)
          WRITE(6, '(a, 3f8.3)') 'cglen: ', cglen(1), cglen(2), cglen(3)
          WRITE(6, '(a, 3i4)')    'dime: ', dime(1), dime(2), dime(3)
          WRITE(6, '(a, 3f8.3)')  'grid: ', grid(1), grid(2), grid(3)
          WRITE(6, '(a, f10.3)') 'Required memory (in MB): ', &
               dime(1)*dime(2)*dime(3)*200.0/1024/1024
          WRITE(6,'()')
       END IF

    END IF

    ! calcforce parameters should be on!
    IF (apbs_print > 2) THEN
       IF (i_param(11) /= 2 ) THEN
          WRITE(6, *) 'iAPBS: WARNING: calcforce keyword is not set to 2, ', &
               'polar forces will not be calculated.'
       END IF
       IF (i_param(20) /= 2 ) THEN
          WRITE(6, *) 'iAPBS: WARNING: calcnpforce keyword is not set to 2, ', &
               'non-polar forces will not be calculated.'
       END IF
    END IF

    napbs = napbs + 1

    ! check if the geometry or energy
    ! changed enough for PB forces recalculation
    IF (geom_upd_limit > 0.0d0 .OR. evdw_upd_limit > 0.0d0) THEN
       CALL check_apbs_update(cx, cy, cz, natom, evdw, do_apbs_update)
    END IF

    ! if yes, recalculate PB forces and energies
    IF (do_apbs_update) THEN
       IF (apbs_debug > 1) WRITE(6,  '(a)') 'iAPBS: Calling apbsdrv'
       IF (geom_upd_limit > 0.0d0 .OR. evdw_upd_limit > 0.0d0) THEN
          IF (apbs_debug > 0) WRITE(6, '(a)') 'Doing PB forces update'
       END IF

       ! first calculation - in solvent
       IF (apbs_debug > 1) WRITE(6,  '(a)') 'iAPBS: Calling apbsdrv in apbs_force().'
       rc = apbsdrv(natom, cx, cy, cz, pbradii, pbcg, &
            r_param, i_param, &
            grid, dime, pdime, glen, center, &
            cglen, fglen, ccenter, fcenter, &
            ofrac, apbs_debug, &
            ionq, ionc, ionrr, &
            esenerg, npenerg, &
            apbsdx, apbsdy, apbsdz, &
            apbsqfx, apbsqfy, apbsqfz, apbsibx, apbsiby, apbsibz, &
            apbsnpx, apbsnpy, apbsnpz, apbsdbx, apbsdby, apbsdbz, &
            apbsgrid_meta, apbsgrid)


       IF (apbs_debug > 2) WRITE(6, '(a, i2)') &
            'iAPBS: apbs return code: ', rc
       IF (rc > 0) THEN
          WRITE(6, '(a)') 'iAPBS Bomb: apbs failed'
          CALL mexit(6,1)
       END IF

       ! total energy in solvent (in kcal/mol)
       esensolv = esenerg(1) / 4.184D0
       npensolv = npenerg(1) / 4.184D0

       IF (apbs_print > 1) THEN
          WRITE (6, '(a, f10.3, a)') &
               "iAPBS: Electrostatic energy in solvent: ", &
               esensolv, " kcal/mol"
          WRITE (6, '(a, f10.3, a)') &
               "iAPBS: Nonpolar energy in solvent:      ", &
               npensolv, " kcal/mol"
       END IF

       ! get the total forces from the solvent calculation
       IF (i_param(11) == 2) THEN
          DO i = 1, natom
             solvdx(i) = (apbsdx(i) + apbsnpx(i))/ 4.184D0
             solvdy(i) = (apbsdy(i) + apbsnpy(i))/ 4.184D0
             solvdz(i) = (apbsdz(i) + apbsnpz(i))/ 4.184D0

             solvqfx(i) = apbsqfx(i) / 4.184D0
             solvqfy(i) = apbsqfy(i) / 4.184D0
             solvqfz(i) = apbsqfz(i) / 4.184D0

             solvibx(i) = apbsibx(i) / 4.184D0
             solviby(i) = apbsiby(i) / 4.184D0
             solvibz(i) = apbsibz(i) / 4.184D0

             solvdbx(i) = apbsdbx(i) / 4.184D0
             solvdby(i) = apbsdby(i) / 4.184D0
             solvdbz(i) = apbsdbz(i) / 4.184D0
          END DO
       END IF
       IF (i_param(21) > 0) THEN
          DO i = 1, natom
             solvnpx(i) = apbsnpx(i) / 4.184D0
             solvnpy(i) = apbsnpy(i) / 4.184D0
             solvnpz(i) = apbsnpz(i) / 4.184D0
          END DO
       END IF


!-----------------------------------------------------------------

       ! save sdie and ion concentration
       sdie = r_param(2)
       ionc1 = ionc(1)
       ionc2 = ionc(2)
       ! set sdie = 1.0
       ! salt concentration should be 0.0
       r_param(2) = 1.0D0 ! sdie
       ionc(1) = 0.0D0
       ionc(2) = 0.0D0

       ! if we are generating potential DX file turning it
       ! off for calculation in vacuum
       iparam12 = 0
       IF (i_param(12) == 1) THEN
          iparam12 = 1
          i_param(12) = 0
       END IF

       ! redo the calculation in vacuum
       IF (apbs_debug > 1) WRITE(6,  '(a)') 'iAPBS: Calling apbsdrv in apbs_force().'
       rc = apbsdrv(natom, cx, cy, cz, pbradii, pbcg, &
            r_param, i_param, &
            grid, dime, pdime, glen, center, &
            cglen, fglen, ccenter, fcenter, &
            ofrac, apbs_debug, &
            ionq, ionc, ionrr, &
            esenerg, npenerg, &
            apbsdx, apbsdy, apbsdz, &
            apbsqfx, apbsqfy, apbsqfz, apbsibx, apbsiby, apbsibz, &
            apbsnpx, apbsnpy, apbsnpz, apbsdbx, apbsdby, apbsdbz, &
            apbsgrid_meta, apbsgrid)

       IF (apbs_debug > 2) WRITE(6, '(a, i2)') &
            'iAPBS: apbs return code: ', rc
       IF (rc > 0) THEN
          WRITE(6, '(a)') 'iAPBS Bomb: apbs failed'
          CALL mexit(6,1)
       END IF

       r_param(2) = sdie
       ionc(1) = ionc1
       ionc(2) = ionc2
       i_param(12) = iparam12

       ! total energy in vacuum (in kcal/mol)
       esenvac = esenerg(1) / 4.184D0
       ! npenvac = npenerg(ncalc(1)) / 4.184D0
       ! no NP energy in vacuum
       npenvac = 0.0D0
       IF (apbs_print > 1) THEN
          WRITE(6, '(a, f10.3, a)') &
               "iAPBS: Electrostatic energy in vacuum: ", &
               esenvac, " kcal/mol"
          WRITE(6, '(a, f10.3, a)') &
               "iAPBS: Nonpolar energy in vacuum:      ", &
               npenvac, " kcal/mol"
       END IF

       ! get the total forces from the vacuum calculation
       IF (i_param(11) == 2) THEN
          DO i = 1, natom
             vacdx(i) = apbsdx(i) / 4.184D0
             vacdy(i) = apbsdy(i) / 4.184D0
             vacdz(i) = apbsdz(i) / 4.184D0

             vacqfx(i) = apbsqfx(i) / 4.184D0
             vacqfy(i) = apbsqfy(i) / 4.184D0
             vacqfz(i) = apbsqfz(i) / 4.184D0

             vacibx(i) = apbsibx(i) / 4.184D0
             vaciby(i) = apbsiby(i) / 4.184D0
             vacibz(i) = apbsibz(i) / 4.184D0

             ! the following are zero in vacuum
             vacnpx(i) = 0.0D0
             vacnpy(i) = 0.0D0
             vacnpz(i) = 0.0D0

             vacdbx(i) = 0.0D0
             vacdby(i) = 0.0D0
             vacdbz(i) = 0.0D0
          END DO
       END IF
!-----------------------------------------------------------------

       ! add calulated total forces to f in common block
       DO i = 1, natom
          j = 3*(i-1)
          f(j+1) = f(j+1) + (solvdx(i) - vacdx(i))
          f(j+2) = f(j+2) + (solvdy(i) - vacdy(i))
          f(j+3) = f(j+3) + (solvdz(i) - vacdz(i))
       END DO

       IF (apbs_debug > 6) THEN
          DO i = 1, natom
             j = 3*(i-1)
             WRITE(6, '(a, 4x, i4, 3f8.3)') "iAPBS: TotalForces:", &
                  i, f(j+1), f(j+2), f(j+3)
          END DO
          DO i = 1, natom
             WRITE(6,'(a, 2x, i4, 3f8.3)') "iAPBS: SolventForces:", &
                  i, solvdx(i), solvdy(i) , solvdz(i)
          END DO
          DO i = 1, natom
             WRITE(6,'(a, 3x, i4, 3f8.3)') "iAPBS: VacuumForces:", &
                  i, vacdx(i), vacdy(i), vacdz(i)
          END DO
          DO i = 1, natom
             WRITE(6,'(a, 5x, i4, 3f8.3)') "iAPBS: SolvForces:", &
                  i, solvdx(i) - vacdx(i), solvdy(i) - vacdy(i), &
                  solvdz(i) - vacdz(i)
          END DO
          DO i = 1, natom
             WRITE(6,'(a, 7x, i4, 3f8.3)') "iAPBS: qfForces:", &
                  i, solvqfx(i) - vacqfx(i), &
                  solvqfy(i) - vacqfy(i), solvqfz(i) - vacqfz(i)
          END DO
          DO i = 1, natom
             WRITE(6,'(a, 7x, i4, 3f8.3)') "iAPBS: ibForces:", &
                  i, solvibx(i) - vacibx(i), &
                  solviby(i) - vaciby(i), solvibz(i) - vacibz(i)
          END DO
          DO i = 1, natom
             WRITE(6,'(a, 7x, i4, 3f8.3)') "iAPBS: npForces:", &
                  i, solvnpx(i) - vacnpx(i), &
                  solvnpy(i) - vacnpy(i), solvnpz(i) - vacnpz(i)
          END DO
          DO i = 1, natom
             WRITE(6,'(a, 7x, i4, 3f8.3)') "iAPBS: dbForces:", &
                  i, solvdbx(i) - vacdbx(i), &
                  solvdby(i) - vacdby(i), solvdbz(i) - vacdbz(i)
          END DO

       END IF ! if apbs_debug >

       ! total, solvatation energy (in kcal/mol)
       enelec = esensolv - esenvac
       IF (apbs_print > 1) THEN
          WRITE(6, '(a, f10.3, a)') &
               "iAPBS: Total solvation energy: ", enelec, " kcal/mol"
       END IF

       ! total, non-polar energy (in kcal/mol)
       !         ennp = npensolv - npenvac
       ennp = npensolv
       IF (apbs_print > 1) THEN
          WRITE(6, '(a, f10.3, a)') &
               "iAPBS: Total non-polar energy: ", ennp, " kcal/mol"
          WRITE(6, '(a, f10.3, a)') &
               "iAPBS: Total non-polar energy (vacuum): ", npenvac, " kcal/mol"
       END IF

       ! assign calculated  energies for export
       eelt = enelec
       enpol = ennp


       ! printing options rokFIXME
       IF (apbs_print > 1) THEN
          WRITE(6, '(a, f10.3, a)') &
               'The Free Energy of Charging in Solvent  = ', &
               esensolv,' kcal/mol'
          WRITE(6, '(a, f10.3, a)') &
               'The Free Energy of Charging in Vacuum   = ', &
               esenvac,' kcal/mol'
          WRITE(6, '(a, f10.3, a)') &
               'The Electrostatic Solvation Free Energy = ', &
               enelec,' kcal/mol'
          WRITE(6, '(a, f10.3, a)') &
               'The Nonpolar Solvation Free Energy      = ', &
               ennp,' kcal/mol'
       END IF

       ! save solvation forces for different update schemes
       DO i = 1, natom
          solvfrcx(i) = solvdx(i) - vacdx(i)
          solvfrcy(i) = solvdy(i) - vacdy(i)
          solvfrcz(i) = solvdz(i) - vacdz(i)
       END DO

       ! save ennp and enelec
       senelec = enelec
       sennp = ennp

       IF (i_param(12) == 1) THEN
          ! if we are generating a potential DX file, rename it
          ! from iapbs-pot.dx to iapbs-pot.#.dx
          ! where # is the MD frame
          !
          ! this is OS specific (using /bin/mv)
          WRITE (dxn, '(i8)') napbs
          dxn = adjustl(dxn)
          dxname = 'iapbs-pot.'// trim(dxn) //'.dx'
          dxname = adjustl(dxname)
          command = '/bin/mv iapbs-pot.dx ' // trim(dxname) 
          CALL system ( command )
       END IF

    ELSE
       IF (apbs_debug > 0) WRITE(6, '(a)') 'iAPBS: Skipping PB forces update'
       ! we are not updating solv forces in this cycle, so just
       ! return energies and forces calculated during the
       ! previous update
       eelt = senelec
       enpol = sennp
       DO i = 1, natom
          j = 3*(i-1)
          f(j+1) = f(j+1) + solvfrcx(i)
          f(j+2) = f(j+2) + solvfrcy(i)
          f(j+3) = f(j+3) + solvfrcz(i)
       END DO
    END IF ! if (do_apbs_update)

    RETURN
  END SUBROUTINE apbs_force

!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  SUBROUTINE check_apbs_update(cx, cy, cz, natom, evdw, do_apbs_update)
!
! check if change in geometry warrants new PB energy and forces update
!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    use apbs_vars

    IMPLICIT NONE

    INTEGER :: i, natom
    LOGICAL :: do_apbs_update
    _REAL_ :: dx, dy, dz, dis, maxdis, evdw, evdwDelta
    _REAL_ :: cx(natom), cy(natom), cz(natom)

    do_apbs_update = .TRUE.

    IF (napbs > 1) THEN ! do this starting with second apbs call
       maxdis = 0.d0
       DO i = 1, natom
          dx = cx(i) - savedx(i)
          dy = cy(i) - savedy(i)
          dz = cz(i) - savedz(i)
          dis = dx*dx + dy*dy + dz*dz
          maxdis = MAX(dis, maxdis)
       END DO

       maxdis = SQRT(1.d0/3.d0 * maxdis)
       ! consider vdW energy change
       evdwDelta = evdw - saveevdw

       do_apbs_update = (maxdis > geom_upd_limit) .or. &
            (ABS(evdwDelta) > evdw_upd_limit)

       IF (apbs_debug > 0) THEN
          WRITE(6,'()')
          WRITE(6,'()')
          WRITE(6, '(a, 2f8.4)') &
               'iAPBS: Max. change in geometry, maxdis, geom_upd_limit:', &
               maxdis, geom_upd_limit
          WRITE(6,'(a, 4f10.4)') &
               'iAPBS: evdw, saveevdw, evdwDelta, evdw_upd_limit:', &
               evdw, saveevdw, evdwDelta, evdw_upd_limit
       END IF
    END IF

    ! save coords for the next call
    DO i = 1, natom
       savedx(i) = cx(i)
       savedy(i) = cy(i)
       savedz(i) = cz(i)
    END DO
    ! save vdW energy for the next call
    saveevdw = evdw

   RETURN
  END SUBROUTINE check_apbs_update

#endif /* APBS */
END MODULE apbs
