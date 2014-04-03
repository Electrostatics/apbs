CHARMM Element source/misc/apbs.src
      SUBROUTINE APBS
c
c
c APBS module for CHARMM/APBS integration
c
c-----------------------------------------------------------------------
##IFN APBS
      CALL WRNDIE(-1,'<CHARMM>','iAPBS module was not compiled.')
##ELSE
c
c implicit none
##INCLUDE '~/charmm_fcm/impnon.fcm'
c
c maxa (max # of atoms), maxaim (including image atoms)
##INCLUDE '~/charmm_fcm/dimens.fcm'
c
c numbers definitions
##INCLUDE '~/charmm_fcm/number.fcm'
c
##INCLUDE '~/charmm_fcm/exfunc.fcm'
c
##INCLUDE '~/charmm_fcm/comand.fcm'
c
c natom, cg (charge)
##INCLUDE '~/charmm_fcm/psf.fcm'
c
##INCLUDE '~/charmm_fcm/stream.fcm'
c
c x, y, z coords, wmain
##INCLUDE '~/charmm_fcm/coord.fcm'
c
##INCLUDE '~/charmm_fcm/stack.fcm'
##INCLUDE '~/charmm_fcm/heap.fcm'
##INCLUDE '~/charmm_fcm/pbeq.fcm'
##INCLUDE '~/charmm_fcm/timer.fcm'
c
c forces common block: dx, dy, dz
c##INCLUDE '~/charmm_fcm/deriv.fcm'
c
c-----------------------------------------------------------------
c apbs include
c
##INCLUDE '~/charmm_fcm/apbs.fcm'
##INCLUDE '~/charmm_fcm/parallel.fcm'
c-----------------------------------------------------------------
c local variables
      integer*4 rc, apbsdrv
!      integer ncalc(1)
      logical skip, sp_apbs

      INTEGER*4 nonlin, bcfl, nion, srfm, calcenergy, calcforce
      INTEGER*4 calc_type, nlev, cmeth, ccmeth, fcmeth, chgm
      INTEGER*4 calcnpenergy, wpot, wchg, wsmol, wkappa, wdiel
      INTEGER*4 watompot, rpot
      INTEGER*4 calcnpforce
      INTEGER*4 rchg, rkappa, rdiel
      INTEGER*4 apbs_print, radiopt

      double precision pdie, sdie, srad, swinapbs, tempapbs, gamma
      double precision sdens,smvolume, smsize
      double precision maxx, minx, maxy, miny, maxz, minz

      double precision esenerg(15), npenerg(15)
      double precision apbsdx(NATOM), apbsdy(NATOM), apbsdz(NATOM)
      double precision apbsqfx(NATOM), apbsqfy(NATOM), apbsqfz(NATOM)
      double precision apbsibx(NATOM), apbsiby(NATOM), apbsibz(NATOM)
      double precision apbsnpx(NATOM), apbsnpy(NATOM), apbsnpz(NATOM)
      double precision apbsdbx(NATOM), apbsdby(NATOM), apbsdbz(NATOM)

      double precision apbsgrid_meta(13), apbsgrid(3*NATOM)

c      character*80 rcsid
c      data rcsid /'$Id: apbs.f rok $'/
c-----------------------------------------------------------------
      INTEGER ISLCT, lstpbi, ntpbi
C Local variables
      INTEGER IMODE, I, LISTR, OLDUSD, NN
      SAVE
c
      IF (TIMER.GT.1)
     $        CALL WRTTIM('PBEQ/APBS solver times:')
      OLDUSD = LSTUSD
      ISLCT  = ALLSTK(INTEG4(NATOM))
      lstpbi = allstk(integ4(NATOM))

c
c     Atom Selection
c     ==============

      IMODE=0 !implies default = all atoms selected
      CALL SELRPN(COMLYN,COMLEN,stack(ISLCT),NATOM,1,IMODE,
     $      .FALSE.,1,' ',0,RESID,RES,IBASE,SEGID,NICTOT,NSEG,
     $      .TRUE.,X,Y,Z,.TRUE.,1,HEAP(PBRAD))

      IF(IMODE.NE.0)THEN
         Write(outu,'(/,3x,a)')
     $   'PREP WARNING: Not all atoms selected, is this what you want?'
c        CALL WRNDIE(-1,'<PREP>','ATOM SELECTION PARSING ERROR')
      ENDIF

      CALL STPRP(NATOM,stack(ISLCT),stack(lstpbi),NTpbi)
      WRITE(OUTU,101)
      WRITE(OUTU,101) 'Calculation with ',NTpbi,' atoms'
 101  FORMAT(3X,A,I6,A)

      call frestk(lstusd-oldusd)

c-----------------------------------------------------------------
c
c     start of APBS section
c
c-----------------------------------------------------------------

c we are in APBS module so set qapbs to .T.
      qapbs = .true.

c get the keywords and assign apbs parameters
      qaparsed = .false.

c   natom  - current number of atoms

c-----------------------------------------------------------------
c
c CHARMM/APBS commands parsing section
c
c-----------------------------------------------------------------

! rokFIXME: also add centering-related (center) keywords

    ! Default values
    ! PBEparm
!      ispara = 0
!   i_pbeparm(1) = 1 ! molid
      nonlin = 0
      bcfl = 1
      nion = 0
      srfm = 2
      pdie = 2.0D0
      sdie = 78.4D0
      srad = 1.4D0
      swinapbs = 0.3D0
      tempapbs = 298.15D0
      gamma = 0.105D0
      sdens = 10.0D0
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
  
      ofrac = 0.1D0

    !    ionq  = (/ 1.0, -1.0 /)
    !    ionc  = (/ 0.15, 0.15 /)
    !    ionrr = (/ 2.0, 2.0 /)

      do i = 1,3
         pdime(i) = 0
         dime(i)  = 0
      end do
      do i = 1, 3
         grid(i)    = 0.5D0
         glen(i)    = 0.0D0
         center(i)  = 0.0D0
         cglen(i)   = 0.0D0
         fglen(i)   = 0.0D0
         ccenter(i) = 0.0D0
         fcenter(i) = 0.0D0
      end do

      do i = 1, 13
         apbsgrid_meta(i) = 0.0
      end do
      do i = 1, 3*NATOM
         apbsgrid(i) = 0.0
      end do

    ! is this a single point energy calculation?
    ! default is no
      sp_apbs = .FALSE.
    ! printing verbosity
      apbs_print = 1
    ! debuging flag
      apbs_debug = 0
    ! radii optimization option
      radiopt = 0
    ! grid dimension recalculation
      qdime_updates = .FALSE.
    ! number of PB steps
      napbs = 0

c-----------------------------------------------------------------
c PBEparm
c-----------------------------------------------------------------

c do we need molID?? probably not
!      i_pbeparm(1) = 1

c pbetype :: LPBE/NPBE
      if (indxa(comlyn, comlen, 'LPBE') .gt. 0) then
         nonlin = 0
      endif
      if (indxa(comlyn, comlen, 'NPBE') .gt. 0) then
         nonlin = 1
      endif

c bcfl :: boundary condition
c defaults to sdh (single Debye-Huckel sphere)
      bcfl = gtrmi(comlyn, comlen, 'BCFL', bcfl)

c pdie :: solute dielectric
c defaults to 2.0
      pdie = gtrmf(comlyn, comlen, 'PDIE', pdie)

c sdie :; solvent dielectric
c defaults to 78.54
      sdie = gtrmf(comlyn, comlen, 'SDIE', sdie)

c srfm :: surface calculation method
c defaults to smol (molecular surface with smoothed harmonic average)
      srfm = gtrmi(comlyn, comlen, 'SRFM', srfm)

c srad :: solvent radius
c defaults to 1.4
      srad = gtrmf(comlyn, comlen, 'SRAD', srad)

c swinapbs :: cubic spline window
c defaults to 0.3
      swinapbs = gtrmf(comlyn, comlen, 'SWIN', swinapbs)

c tempapbs :: temperature (in K)
c defaults to 298.15
      tempapbs = gtrmf(comlyn, comlen, 'TEMP', tempapbs)

c gamma :: surface tension for apolar energies/forces (in kJ/mol/A^2)
c defaults to 0.105
      gamma = gtrmf(comlyn, comlen, 'GAMMA', gamma)

c sdens :: number of grid points per square-angstrom to use in Vacc object
c defaults to 10.0
      sdens = gtrmf(comlyn, comlen, 'SDENS', sdens)

c calcenergy :: energy calculation
c defaults to per atom energy calculation
      calcenergy = gtrmi(comlyn, comlen, 'CALCE', calcenergy)

c calcforce :: atomic forces I/O
c defaults to per atom forces calculation
      calcforce = gtrmi(comlyn, comlen, 'CALCF', calcforce)

c calcnpenergy :: NP energy calculation
c defaults to per atom energy calculation
      calcnpenergy = gtrmi(comlyn, comlen, 'CALNE', calcnpenergy)

c calcnpforce :: NP atomic forces I/O
c defaults to per atom forces calculation
      calcnpforce = gtrmi(comlyn, comlen, 'CALNF', calcnpforce)

c dielMap :: read dielectric maps (x, y and z)
      if (indxa(comlyn, comlen, 'RDIEL') .gt. 0) then
         rdiel = 1
      endif

c kappaMap :: read Kappa map
      if (indxa(comlyn, comlen, 'RKAPPA') .gt. 0) then
         rkappa = 1
      endif

c chargeMap :: read charge map
      if (indxa(comlyn, comlen, 'RCHG') .gt. 0) then
         rchg = 1
      endif

c get ions parameters and set nion (i_pbeparm(4))
c (we are considering 2 ions only - this should be enough for 
c most applications)
      if (indx(comlyn, comlen, 'IONQ1', 5) .gt. 0) then
         ionq(1) = gtrmf(comlyn, comlen, 'IONQ1', zero)
         ionc(1) = gtrmf(comlyn, comlen, 'IONC1', zero)
         ionrr(1) = gtrmf(comlyn, comlen, 'IONR1', zero)
         nion = 1
      endif
      if (indx(comlyn, comlen, 'IONQ2', 5) .gt. 0) then
         ionq(2) = gtrmf(comlyn, comlen, 'IONQ2', zero)
         ionc(2) = gtrmf(comlyn, comlen, 'IONC2', zero)
         ionrr(2) = gtrmf(comlyn, comlen, 'IONR2', zero)
         nion = 2
      endif


c-----------------------------------------------------------------
c external files write section
c-----------------------------------------------------------------
c write potential DX file
      if (indxa(comlyn, comlen, 'WPOT') .gt. 0) then
         wpot = 1
      endif
c write charge DX file
      if (indxa(comlyn, comlen, 'WCHG') .gt. 0) then
         wchg = 1
      endif
c write smol DX file
      if (indxa(comlyn, comlen, 'WSMOL') .gt. 0) then
         wsmol = 1
      endif
c write kappa DX file
      if (indxa(comlyn, comlen, 'WKAPPA') .gt. 0) then
         wkappa = 1
      endif
c write diel DX files (x, y and z)
      if (indxa(comlyn, comlen, 'WDIEL') .gt. 0) then
         wdiel = 1
      endif
c-----------------------------------------------------------------
c MGparm
c-----------------------------------------------------------------

c type mgmanual/mgauto
      if (indxa(comlyn, comlen, 'MGMANUAL') .gt. 0) then
         calc_type = 0
      endif
      if (indxa(comlyn, comlen, 'MGAUTO') .gt. 0) then
         calc_type = 1
      endif

c mg-para 
      if (indxa(comlyn, comlen, 'MGPARA') .gt. 0) then
         calc_type = 2
!         ispara = 1
      endif

c nlev :: levels in multigrid hierarchy
c used in mg-manual only, no default
      nlev = gtrmi(comlyn, comlen, 'NLEV', 4)

c centering methods - defaults to centering on molecule
c 0 is centering on point, 1 is on molecule
c if centering on a point - needs CNT, CCN, FCN

c cmeth for manual only (needs cnt also)
      cmeth = gtrmi(comlyn, comlen, 'CMET', 1)

c ccmeth and fcmeth for auto (need ccn and fcn also)
c defaults to 1 (centered on molecule)
      ccmeth = gtrmi(comlyn, comlen, 'CCME', 1)
      fcmeth = gtrmi(comlyn, comlen, 'FCME', 1)

c dime :: grid dimensions
c no default
      dime(1) = gtrmi(comlyn, comlen, 'DIMX', dime(1))
      dime(2) = gtrmi(comlyn, comlen, 'DIMY', dime(2))
      dime(3) = gtrmi(comlyn, comlen, 'DIMZ', dime(3))

c grid center
      if ((indx(comlyn, comlen, 'CNTX', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'CNTY', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'CNTZ', 4) .gt. 0)) then
         center(1) = gtrmf(comlyn, comlen, 'CNTX', zero)
         center(2) = gtrmf(comlyn, comlen, 'CNTY', zero)
         center(3) = gtrmf(comlyn, comlen, 'CNTZ', zero)
      endif

c ccenter
      if ((indx(comlyn, comlen, 'CCNX', 4) .gt. 0)  .and.
     +    (indx(comlyn, comlen, 'CCNY', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'CCNZ', 4) .gt. 0))then
         ccenter(1) = gtrmf(comlyn, comlen, 'CCNX', zero)
         ccenter(2) = gtrmf(comlyn, comlen, 'CCNY', zero)
         ccenter(3) = gtrmf(comlyn, comlen, 'CCNZ', zero)
      endif

c fcenter
      if ((indx(comlyn, comlen, 'FCNX', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'FCNY', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'FCNZ', 4) .gt. 0)) then
         fcenter(1) = gtrmf(comlyn, comlen, 'FCNX', zero)
         fcenter(2) = gtrmf(comlyn, comlen, 'FCNY', zero)
         fcenter(3) = gtrmf(comlyn, comlen, 'FCNZ', zero)
      endif

c cglen
      if ((indx(comlyn, comlen, 'CGLX', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'CGLY', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'CGLZ', 4) .gt. 0)) then
         cglen(1) = gtrmf(comlyn, comlen, 'CGLX', zero)
         cglen(2) = gtrmf(comlyn, comlen, 'CGLY', zero)
         cglen(3) = gtrmf(comlyn, comlen, 'CGLZ', zero)
      endif

c fglen
      if ((indx(comlyn, comlen, 'FGLX', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'FGLY', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'FGLZ', 4) .gt. 0)) then
         fglen(1) = gtrmf(comlyn, comlen, 'FGLX', zero)
         fglen(2) = gtrmf(comlyn, comlen, 'FGLY', zero)
         fglen(3) = gtrmf(comlyn, comlen, 'FGLZ', zero)
      endif

c glen
      if ((indx(comlyn, comlen, 'GLNX', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'GLNY', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'GLNZ', 4) .gt. 0)) then
      glen(1) = gtrmf(comlyn, comlen, 'GLNX', zero)
      glen(2) = gtrmf(comlyn, comlen, 'GLNY', zero)
      glen(3) = gtrmf(comlyn, comlen, 'GLNZ', zero)
      endif

c grid
      if ((indx(comlyn, comlen, 'GRDX', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'GRDY', 4) .gt. 0) .and.
     +    (indx(comlyn, comlen, 'GRDZ', 4) .gt. 0)) then
         grid(1) = gtrmf(comlyn, comlen, 'GRDX', zero)
         grid(2) = gtrmf(comlyn, comlen, 'GRDY', zero)
         grid(3) = gtrmf(comlyn, comlen, 'GRDZ', zero)
      endif

c chgm/setchgm
c defaults to spl2 (1)
      chgm = gtrmi(comlyn, comlen, 'CHGM', 1)

c setup for mg-para
c pdime :: 
c defaults to 0 which is not correct
      pdime(1) = gtrmi(comlyn, comlen, 'PDIX', pdime(1))
      pdime(2) = gtrmi(comlyn, comlen, 'PDIY', pdime(2))
      pdime(3) = gtrmi(comlyn, comlen, 'PDIZ', pdime(3))

c ofrac :: overlap fraction between procs
c defaults to 0.1
      ofrac = gtrmf(comlyn, comlen, 'OFRA', ofrac)

c do we calculate solvation forces?
      qfapbs = (indxa(comlyn, comlen, 'SFORCE') .gt. 0)

c if yes, skip the first APBS calculation, just set up all variables
      if (qfapbs) then
         skip = .true.
      endif

c how often do we calculate forces in md?
c default is 1 (every step)
      napbs = gtrmi(comlyn, comlen, 'UPDATE', 1)

c forces update method
      umeth = gtrmi(comlyn, comlen, 'UMETHOD', 1)

c get atom positions, radius and charge from a common block (coord.fcm)
c
c      positionx = x
c      positiony = y
c      positionz = z
c      radius = wmain
c      charge = cg

c save wmain radii to a_radius
      do i=1, natom
         a_radius(i) = wmain(i)
      end do

c debug
      apbs_debug = gtrmi(comlyn, comlen, 'DEBUG', apbs_debug)

c ok, we now have all user switches 
      qaparsed = .true.

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
      r_param(4) = swinapbs
      r_param(5) = tempapbs
      r_param(6) = sdens
      r_param(7) = gamma
      r_param(8) = smvolume
      r_param(9) = smsize

c go over atomic coordinates and figure out optimal grid size
      maxx = x(1)
      minx = x(1)
      maxy = y(1)
      miny = y(1)
      maxz = z(1)
      minz = z(1)
      do i = 1, natom
         if(maxx < x(i)+a_radius(i)) maxx = x(i)+a_radius(i)
         if(minx > x(i)-a_radius(i)) minx = x(i)-a_radius(i)
         if(maxy < y(i)+a_radius(i)) maxy = y(i)+a_radius(i)
         if(miny > y(i)-a_radius(i)) miny = y(i)-a_radius(i)
         if(maxz < z(i)+a_radius(i)) maxz = z(i)+a_radius(i)
         if(minz > z(i)-a_radius(i)) minz = z(i)-a_radius(i)
      end do

      write(outu,'(3x, a, 3f8.3)') 'APBS> Molecular dimensions: ',
     +     maxx-minx, maxy-miny, maxz-minz

! for mg-manual calculate missing grid parameters
       if ((i_param(1)==0 .OR. i_param(1)==1) .and. dime(1)==0) then
          cglen(1) = 1.7 * (maxx-minx)
          cglen(2) = 1.7 * (maxy-miny)
          cglen(3) = 1.7 * (maxz-minz)
          fglen(1) = 20.0 + (maxx-minx)
          fglen(2) = 20.0 + (maxy-miny)
          fglen(3) = 20.0 + (maxz-minz)

          do i = 1, 3
             if (fglen(i) > cglen(i)) cglen(i) = fglen(i)
          end do
          write(outu, '(3x, a)')
     +         'APBS> Grid dime not specified, calculating ...'
          write(outu, '(3x, a)')
     +         'APBS> Requesting dime re-calculation on the fly'
          qdime_updates = .TRUE.

          do i = 1, 3
             dime(i) = 
     +        32*(int((int(fglen(i)/grid(i) + 0.5) - 1)/32.0 + 0.5)) + 1
             if (dime(i) < 33) dime(i) = 33
          end do
       end if

       if (apbs_debug .gt. 0) then
          write(outu, '(3x, a)') 'APBS> Grid values: '
          write(outu, '(3x, a, 3f8.3)') 'APBS> fglen: ', fglen(1),
     +         fglen(2), fglen(3)
          write(outu, '(3x, a, 3f8.3)') 'APBS> cglen: ', cglen(1),
     +         cglen(2), cglen(3)
          write(outu, '(3x, a, 3i4)')   'APBS> dime: ', dime(1),
     +         dime(2),dime(3)
          write(outu, '(3x, a, 3f8.3)') 'APBS> grid: ', grid(1),
     +         grid(2),grid(3)
          write(outu, '(3x, a, f10.3)')
     +         'APBS> Required memory (in MB): ',dime(1)*dime(2)
     +         *dime(3)*200.0/1024/1024
       end if

c sanity checks
       do i = 1, natom
          if (ABS(a_radius(i)) .gt. 4.0) then
             write(outu, '(3x, 2a, i6 , a, f8.3)') 
     +            'APBS> WARNING: ',
     +            'Radius of this atom is larger than 4.0 A. Atom: ', 
     +            i, ' Radius: ', a_radius(i)
          end if
          if (ABS(cg(i)) .gt. 5.0) then
             write(outu, '(3x, 2a, i6 ,a, f8.3)') 
     +            'APBS> WARNING: ',
     +            'Charge of this atom is larger than 5 e. Atom: ', 
     +            i, ' Charge: ', cg(i)
          end if
          if (apbs_debug .gt. 10) then
             write(outu, '(3x, i6, 5f8.3)') 
     +            i, x(i), y(i), z(i), cg(i), a_radius(i)
          end if
       end do

       if (apbs_debug .gt. 5) then
          do i = 1, 25
             write(outu, '(3x, a, i4, i4)') 'iAPBS debug i_param(): ', 
     +            i, i_param(i)
          end do
       end if

       do i = 1, 3
          if (grid(i) * dime(i) < fglen(i)) then
             write(outu,  '(3x, a)')
     +            'APBS> WARNING: caclulated grid spacing is larger than
     + requested:'
             write(outu,  '(3x, a, i1, a, f5.3, a, f5.3)')
     +         'APBS> grid(', i, '): requested: ', grid(i), ' actual: ', 
     +            fglen(i)/dime(i)
c             write(outu,  '(3x, a)')
c     +            'APBS> To fix this decrease grd value'
c             call wrndie(-1, '<APBS>',
c     +            'Requested and calculated grid spacing discrapancy')
          end if
       end do

c OK, now we are ready to call the apbs_driver and start the show

       if (skip) then
          write(outu, '(3x, a)')
     +         "APBS> Skiping the first APBS calculation, "
          write(outu, '(3x, a)')
     +         "APBS> Initializing parameters only."
       else
         rc = apbsdrv(natom, x, y, z, a_radius, cg, 
     +       r_param, i_param, 
     +       grid, dime, pdime, glen, center, 
     +       cglen, fglen, ccenter, fcenter, 
     +       ofrac, apbs_debug, 
     +       ionq, ionc, ionrr, 
     +       esenerg, npenerg, 
     +       apbsdx, apbsdy, apbsdz, 
     +       apbsqfx, apbsqfy, apbsqfz, apbsibx, apbsiby, apbsibz, 
     +       apbsnpx, apbsnpy, apbsnpz, apbsdbx, apbsdby, apbsdbz,
     +       apbsgrid_meta, apbsgrid)

         if (apbs_debug .gt. 0) then
            write(outu, '(3x, a, i3)') "APBS> APBS return code: ", rc
         end if

         if ((apbs_debug .gt. 3) .and. (calcforce .gt. 0)) then
            do i = 1, natom
               write(outu, '(3x, a, i8, 3f13.5, a)')
     +              "APBS> Total force on atom", i, apbsdx(i), apbsdy(i)
     +              , apbsdz(i), " kJ/(mol/A)"
            end do
         endif

         if (apbs_debug .gt. 0) then
            write(outu, '(3x, a, f13.5, a)') "APBS> esEnergy: ", 
     +           esenerg(1)/ 4.2D0, " kcal/mol"
            write(outu, '(3x, a, f13.5, a)') "APBS> npEnergy: ", 
     +           npenerg(1)/ 4.2D0, " kcal/mol"
            write(outu, '(3x, a, f13.5, a)') "APBS> Total Energy: ", 
     +           (esenerg(1) + npenerg(1))/ 4.2D0, " kcal/mol"
         endif


c pass the electrostatic energy value back (in kcal/mol)
         call setmsr('ENPB', esenerg(1) / 4.2D0)
         call setmsr('ENNP', npenerg(1) / 4.2D0)
      endif

##ENDIF         ifn/else APBS
      RETURN
      END

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c
      subroutine apbsfrc(f_natom, x, y, z, f_cg, enelec, ennp, 
     +     dx, dy, dz, icall, qprin)
c
c calculates forces by doing two apbs calculation (vacuum/solvent)
c
c this is called by energy.src (SP APBS calculation must be done 
c first, before starting MD to initialize all values)
c-----------------------------------------------------------------------
##IFN APBS
      CALL WRNDIE(-1,'<CHARMM>','APBS module was not compiled.')
##ELSE
c
c implicit none
##INCLUDE '~/charmm_fcm/impnon.fcm'
c maxa (max # of atoms), maxaim (including image atoms)
##INCLUDE '~/charmm_fcm/dimens.fcm'
c numbers definitions
##INCLUDE '~/charmm_fcm/number.fcm'
##INCLUDE '~/charmm_fcm/exfunc.fcm'
##INCLUDE '~/charmm_fcm/comand.fcm'
c natom, cg (charge)
##INCLUDE '~/charmm_fcm/psf.fcm'
##INCLUDE '~/charmm_fcm/stream.fcm'
##INCLUDE '~/charmm_fcm/stack.fcm'
##INCLUDE '~/charmm_fcm/heap.fcm'
##INCLUDE '~/charmm_fcm/pbeq.fcm'
##INCLUDE '~/charmm_fcm/timer.fcm'
c apbs include
##INCLUDE '~/charmm_fcm/apbs.fcm'
##INCLUDE '~/charmm_fcm/parallel.fcm'
c-----------------------------------------------------------------
c local variables
      double precision apbsdx(natom), apbsdy(natom), apbsdz(natom)
      double precision solvdx(natom), solvdy(natom), solvdz(natom)
      double precision vacdx(natom), vacdy(natom), vacdz(natom)
      double precision dx(natom), dy(natom), dz(natom)
      double precision x(natom), y(natom), z(natom)
      double precision enelec, ennp, f_cg(natom) 
      double precision esenvac, esensolv, npenvac, npensolv
      double precision esenerg(15), npenerg(15) 
      double precision sdie, gamma, ionc1, ionc2
      double precision maxx, minx, maxy, miny, maxz, minz
      double precision apbsqfx(natom), apbsqfy(natom), apbsqfz(natom)
      double precision apbsibx(natom), apbsiby(natom), apbsibz(natom)
      double precision apbsnpx(natom), apbsnpy(natom), apbsnpz(natom)
      double precision apbsdbx(natom), apbsdby(natom), apbsdbz(natom)
      double precision solvqfx(natom), solvqfy(natom), solvqfz(natom)
      double precision solvibx(natom), solviby(natom), solvibz(natom)
      double precision solvnpx(natom), solvnpy(natom), solvnpz(natom)
      double precision solvdbx(natom), solvdby(natom), solvdbz(natom)
      double precision vacqfx(natom), vacqfy(natom), vacqfz(natom)
      double precision vacibx(natom), vaciby(natom), vacibz(natom)
      double precision vacnpx(natom), vacnpy(natom), vacnpz(natom)
      double precision vacdbx(natom), vacdby(natom), vacdbz(natom)

      double precision apbsgrid_meta(13), apbsgrid(3*natom)

      integer*4 f_natom, i, apbsdrv, icall, rc
      logical qprin
c--------------------------------------------------------------------
c
c forces: two apbs calculations (one in solvent, the other in vacuum),
c solvation force is the difference 
c

c did we parse options first?
      if (.not. qaparsed) then
         call wrndie(-5, '<APBSFRC>', 'Must have APBS options first!')
      endif

c initialization

      do i = 1, 13
         apbsgrid_meta(i) = 0.0
      end do
      do i = 1, 3*NATOM
         apbsgrid(i) = 0.0
      end do

c we're doing this calculation only every icall/napbs step
      if (mod(icall, napbs) .eq. 0) then

c must turn on forces in apbs!
         if (i_param(11) .ne. 2 ) then
            call wrndie(-5, '<APBSFRC>', 
     +      'Set CALCF to 2 for solvation forces calculation!')
         end if

         if (qdime_updates) then
            maxx = x(1)
            minx = x(1)
            maxy = y(1)
            miny = y(1)
            maxz = z(1)
            minz = z(1)
            do i = 1, natom
               if(maxx < x(i)+a_radius(i)) maxx = x(i)+a_radius(i)
               if(minx > x(i)-a_radius(i)) minx = x(i)-a_radius(i)
               if(maxy < y(i)+a_radius(i)) maxy = y(i)+a_radius(i)
               if(miny > y(i)-a_radius(i)) miny = y(i)-a_radius(i)
               if(maxz < z(i)+a_radius(i)) maxz = z(i)+a_radius(i)
               if(minz > z(i)-a_radius(i)) minz = z(i)-a_radius(i)
            end do

            cglen(1) = 1.7 * (maxx-minx)
            cglen(2) = 1.7 * (maxy-miny)
            cglen(3) = 1.7 * (maxz-minz)
            fglen(1) = 20.0 + (maxx-minx)
            fglen(2) = 20.0 + (maxy-miny)
            fglen(3) = 20.0 + (maxz-minz)

            do i = 1, 3
               if (fglen(i) > cglen(i)) cglen(i) = fglen(i)
            end do

            if (apbs_debug .gt. 0) then
               write(outu,'(3x, a, 3f8.3)')
     +              'APBS> Molecular dimensions: ',maxx-minx, maxy-miny,
     +              maxz-minz
               write(outu, '(3x, a)')
     +              'APBS> Re-calculating grid dimensions ...'
            end if

            do i = 1, 3
             dime(i) = 
     +        32*(int((int(fglen(i)/grid(i) + 0.5) - 1)/32.0 + 0.5)) + 1
               if (dime(i) < 33) dime(i) = 33
            end do
         end if

         if (apbs_debug .gt. 0) then
            write(outu, '(3x, a)') 'APBS> Grid values: '
            write(outu, '(3x, a, 3f8.3)') 'APBS> fglen: ', fglen(1),
     +           fglen(2), fglen(3)
            write(outu, '(3x, a, 3f8.3)') 'APBS> cglen: ', cglen(1),
     +           cglen(2), cglen(3)
            write(outu, '(3x, a, 3i4)')   'APBS> dime: ', dime(1),
     +           dime(2),dime(3)
            write(outu, '(3x, a, 3f8.3)') 'APBS> grid: ', grid(1),
     +           grid(2),grid(3)
            write(outu, '(3x, a, f10.3)')
     +           'APBS> Required memory (in MB): ',dime(1)*dime(2)
     +           *dime(3)*200.0/1024/1024
         end if


c         write(*,*)'APBSFRC> before apbsdrv...'
c first calculation - in solvent 
         rc = apbsdrv(f_natom, x, y, z, a_radius, f_cg, 
     +       r_param, i_param, 
     +       grid, dime, pdime, glen, center, 
     +       cglen, fglen, ccenter, fcenter, 
     +       ofrac, apbs_debug, 
     +       ionq, ionc, ionrr, 
     +       esenerg, npenerg, 
     +       apbsdx, apbsdy, apbsdz, 
     +       apbsqfx, apbsqfy, apbsqfz, apbsibx, apbsiby, apbsibz, 
     +       apbsnpx, apbsnpy, apbsnpz, apbsdbx, apbsdby, apbsdbz,
     +       apbsgrid_meta, apbsgrid)

c         write(*,*)'APBSFRC> after apbsdrv...,ncalc=',ncalc(1)

c total energy in solvent (in kcal/mol)
         esensolv = esenerg(1) / 4.2D0
         npensolv = npenerg(1) / 4.2D0
c         write(*,*)'APBSFRC>after energy...'
         if (apbs_debug .gt. 0) then
            write(outu, '(3x, a, f8.3, a)')
     +           "APBSFRC> Electrostatic energy in solvent: ",
     +           esensolv, " kcal/mol"
            write(outu, '(3x, a, f8.3, a)')
     +           "APBSFRC> Nonpolar energy in solvent: ", 
     +           npensolv, " kcal/mol"
         end if

c get the total forces from the solvent calculation
         do i = 1, f_natom
            solvdx(i) = apbsdx(i) / 4.2D0
            solvdy(i) = apbsdy(i) / 4.2D0
            solvdz(i) = apbsdz(i) / 4.2D0

            solvqfx(i) = apbsqfx(i) / 4.2D0
            solvqfy(i) = apbsqfy(i) / 4.2D0
            solvqfz(i) = apbsqfz(i) / 4.2D0

            solvibx(i) = apbsibx(i) / 4.2D0
            solviby(i) = apbsiby(i) / 4.2D0
            solvibz(i) = apbsibz(i) / 4.2D0

            solvnpx(i) = apbsnpx(i) / 4.2D0
            solvnpy(i) = apbsnpy(i) / 4.2D0
            solvnpz(i) = apbsnpz(i) / 4.2D0

            solvdbx(i) = apbsdbx(i) / 4.2D0
            solvdby(i) = apbsdby(i) / 4.2D0
            solvdbz(i) = apbsdbz(i) / 4.2D0
         end do

c-----------------------------------------------------------------

c save sdie and ion concentration
         sdie = r_param(2)
         ionc1 = ionc(1)
         ionc2 = ionc(2)
c set sdie = 1.0
c salt concentration should be 0.0
         r_param(2) = 1.0D0 ! sdie
         ionc(1) = 0.0D0
         ionc(2) = 0.0D0

c second calculation, now in vacuum
         rc = apbsdrv(f_natom, x, y, z, a_radius, f_cg, 
     +       r_param, i_param, 
     +       grid, dime, pdime, glen, center, 
     +       cglen, fglen, ccenter, fcenter, 
     +       ofrac, apbs_debug, 
     +       ionq, ionc, ionrr, 
     +       esenerg, npenerg, 
     +       apbsdx, apbsdy, apbsdz, 
     +       apbsqfx, apbsqfy, apbsqfz, apbsibx, apbsiby, apbsibz, 
     +       apbsnpx, apbsnpy, apbsnpz, apbsdbx, apbsdby, apbsdbz,
     +       apbsgrid_meta, apbsgrid)



c return back the original sdie and ionc concentration values
         r_param(2) = sdie
         ionc(1) = ionc1
         ionc(2) = ionc2

c total energy in vacuum (in kcal/mol)
         esenvac = esenerg(1) / 4.2D0
c         npenvac = npenerg(1) / 4.2D0
c no NP energy in vacuum
         npenvac = 0.0D0
         if (apbs_debug .gt. 0) then
            print *, "APBSFRC> Electrostatic energy in vacuum: ", 
     +           esenvac, " kcal/mol"
            print *, "APBSFRC> Nonpolar energy in vacuum: ", 
     +           npenvac, " kcal/mol"
         end if

c get the total forces from the vacuum calculation
         do i = 1, f_natom
            vacdx(i) = apbsdx(i) / 4.2D0
            vacdy(i) = apbsdy(i) / 4.2D0
            vacdz(i) = apbsdz(i) / 4.2D0

            vacqfx(i) = apbsqfx(i) / 4.2D0
            vacqfy(i) = apbsqfy(i) / 4.2D0
            vacqfz(i) = apbsqfz(i) / 4.2D0

            vacibx(i) = apbsibx(i) / 4.2D0
            vaciby(i) = apbsiby(i) / 4.2D0
            vacibz(i) = apbsibz(i) / 4.2D0

c the following are zero in vacuum

            vacnpx(i) = 0.0D0
            vacnpy(i) = 0.0D0
            vacnpz(i) = 0.0D0

            vacdbx(i) = 0.0D0
            vacdby(i) = 0.0D0
            vacdbz(i) = 0.0D0

         end do

c-----------------------------------------------------------------

c add calulated total forces to dx, dy, dz in common block
c
         do i = 1, f_natom
            dx(i) = dx(i) - (solvdx(i) - vacdx(i))
            dy(i) = dy(i) - (solvdy(i) - vacdy(i))
            dz(i) = dz(i) - (solvdz(i) - vacdz(i))
         end do

         if (apbs_debug .gt. 1) then
            do i = 1, f_natom
               print *, "APBSFRC> TotalForces:", i, dx(i), dy(i), dz(i)
            end do
            do i = 1, f_natom
               print *, "APBSFRC> SolventForces:", i, solvdx(i) ,
     +              solvdy(i) , solvdz(i) 
            end do
            do i = 1, f_natom
               print *, "APBSFRC> VacuumForces:", i, vacdx(i),
     +              vacdy(i), vacdz(i)
            end do
            do i = 1, f_natom
               print *, "APBSFRC> SolvForces:", i, solvdx(i) - vacdx(i),
     +              solvdy(i) - vacdy(i), solvdz(i) - vacdz(i)
            end do
            do i = 1, f_natom
               print *, "APBSFRC> qfForces:", i, solvqfx(i) - vacqfx(i),
     +              solvqfy(i) - vacqfy(i), solvqfz(i) - vacqfz(i)
            end do
            do i = 1, f_natom
               print *, "APBSFRC> ibForces:", i, solvibx(i) - vacibx(i),
     +              solviby(i) - vaciby(i), solvibz(i) - vacibz(i)
            end do
            do i = 1, f_natom
               print *, "APBSFRC> npForces:", i, solvnpx(i) - vacnpx(i),
     +              solvnpy(i) - vacnpy(i), solvnpz(i) - vacnpz(i)
            end do
            do i = 1, f_natom
               print *, "APBSFRC> dbForces:", i, solvdbx(i) - vacdbx(i),
     +              solvdby(i) - vacdby(i), solvdbz(i) - vacdbz(i)
            end do

         endif

c total, solvatation energy (in kcal/mol)
         enelec = esensolv - esenvac
         if (apbs_debug .gt. 0) then
            print *, "APBSFRC> Total solvation energy: ", 
     +           enelec, " kcal/mol"
         end if

c total, non-polar energy (in kcal/mol)
c         ennp = npensolv - npenvac
         ennp = npensolv
         if (apbs_debug .gt. 0) then
            print *, "APBSFRC> Total non-polar energy: ", 
     +           ennp, " kcal/mol"
            print *, "APBSFRC> Total non-polar energy (vacuum): ", 
     +           npenvac, " kcal/mol"

         end if

         if (qprin) then
            WRITE(outu,'(3X,A,F13.5,A)')
     +           'The Free Energy of Charging in Solvent  = ',
     +           esensolv,' [KCAL/MOL]'
            WRITE(outu,'(3X,A,F13.5,A)')
     +           'The Free Energy of Charging in vacuum   = ',
     +           esenvac,' [KCAL/MOL]'
            WRITE(outu,'(3X,A,F13.5,A)')
     +           'The Electrostatic Solvation Free Energy = ',
     +           enelec,' [KCAL/MOL]'

            WRITE(outu,'(3X,A,F13.5,A)')
     +           'The Nonpolar Solvation Free Energy = ',
     +           ennp,' [KCAL/MOL]'
         end if

c pass the electrostatic energy value back (in kcal/mol)
c but this doesn't make too much sense in here, does it?
c         call setmsr('ENPB', enelec)

c save solvation forces for different update schemes
         do i = 1, f_natom
            solvfrcx(i) = solvdx(i) - vacdx(i)
            solvfrcy(i) = solvdy(i) - vacdy(i)
            solvfrcz(i) = solvdz(i) - vacdz(i)
         end do
         
c save ennp and enelec
         senelec = enelec
         sennp = ennp

c end if section for napbs==icall
c if we don't do APBS calculation just use the old APBS calculated forces
c or update it accordingly
      else
         if (apbs_debug .gt. 0) then
            print *, "APBSFRC> Reusing solvation forces in this step:",
     +           icall
         end if
c select force update method
         if (umeth .eq. 0) then
c     this just uses zero for solvation forces and energies 
c     between updates 
            enelec = 0.0
            ennp = 0.0
         else if (umeth .eq. 1) then
c     this method uses forces from previous APBS step between updates
            do i = 1, f_natom
               dx(i) = dx(i) - solvfrcx(i)
               dy(i) = dy(i) - solvfrcy(i)
               dz(i) = dz(i) - solvfrcz(i)
            end do
            enelec = senelec
            ennp = sennp
         else
            write(outu, '(3x, a)')
     +      "APBSFRC> This method of force updates is not implemented"
         end if

c endif section for napbs/icall comparison
      end if

##ENDIF     ifn/else APBS
      RETURN
      END

c $Id: apbs.f rok $
