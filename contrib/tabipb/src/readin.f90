!Read protein structure from msms
subroutine readin
use molecule
use comdata
implicit double precision(a-h,o-z)
real*8 pos(3),vector(3)
integer nind(5)
CHARACTER(100) :: FHEAD
character(10) :: c1,c2,c3,c4,c5
integer i,iflag,nremark,MEOF
real*4 xyzqr(5)

!Obtain path
    pathname=''
    lenpath = len(pathname)
    do while (pathname(lenpath:lenpath) .eq. ' ')
        lenpath = lenpath - 1
    enddo  


    lenfname = 0 
    do while (fname(lenfname:lenfname) .ne. ' ')
        lenfname = lenfname + 1
    enddo 
    lenfname = 4;

    nremark=0
    open(102,file=pathname(1:lenpath)//fname(1:lenfname)//".pqr")
    do  
        READ(102,*) fhead
        if (fhead(1:6)=='REMARK') then
            nremark=nremark+1
        else
            exit 
        endif
    enddo 
    !print *,'lines of remarks = ', nremark
    close(102)
    
    open(102,file=pathname(1:lenpath)//fname(1:lenfname)//".pqr")
    open(103,file=pathname(1:lenpath)//fname(1:lenfname)//".xyzr")
    
    do i=1,nremark
        read(102,*) fhead
    enddo
    
    natm=0
    do 
        read(102,*,IOSTAT = MEOF) c1,c2,c3,c4,c5,xyzqr
        
        if ((c1(1:3) .ne. 'END') .and. (MEOF .eq. 0)) then 
            write(103,*) xyzqr(1:3),xyzqr(5)
            natm=natm+1
        endif 
        IF(MEOF .LT. 0) EXIT
    enddo      
    
    close(102)
    close(103)


    !Read atom coordinates and partial charges
    nchr=natm
    allocate(atmpos(3,natm),atmrad(natm),atmchr(nchr),chrpos(3,nchr),STAT=ierr)
    IF (ierr .NE. 0) THEN
        WRITE(6,*) 'Error allocating atmpos, atmrad, atmchr, chrpos!'
        STOP
    END IF

    open(102,file=pathname(1:lenpath)//fname(1:lenfname)//".pqr")
    
    do i=1,nremark
        read(102,*) fhead
    enddo
    
    do i=1,natm
        read(102,*,IOSTAT = MEOF) c1,c2,c3,c4,c5,xyzqr
        atmpos(1:3,i)=xyzqr(1:3)
        atmrad(i)=xyzqr(5)
        chrpos(1:3,i)=xyzqr(1:3)
        atmchr(i)=xyzqr(4)
    enddo      
    
    close(102)

    rslt=system('msms -if '//pathname(1:lenpath)//fname(1:lenfname)//".xyzr"//' -prob 1.4 -de ' &
    //den(1:5)//' -of '//pathname(1:lenpath)//fname(1:lenfname))    
    
    ! read the surface points
    OPEN(102,FILE=pathname(1:lenpath)//FNAME(1:lenfname)//".vert")

    READ(102,*) FHEAD
    READ(102,*) FHEAD
    READ(102,*) NSPT, ppp, qqq, rrr

    ALLOCATE(SPTPOS(3,NSPT), SPTNRM(3,NSPT), NATMAFF(NSPT), NSFTYPE(NSPT), STAT= ierr)
    IF (ierr .NE. 0) THEN
        WRITE(6,*) 'Error allocating SPTPOS, SPTNRM, NATMAFF, NSFTYPE!'
    STOP
    END IF

    SPTPOS=0.D0; SPTNRM=0.D0; NATMAFF=0; NSFTYPE=0;
      
    DO I=1,NSPT
        READ(102,*) POS(1:3), VECTOR(1:3), KK, NAFF, NAFFT 
         
        SPTPOS(:,I) = POS;   SPTNRM(:,I) = VECTOR
        NATMAFF(I)  = NAFF;  NSFTYPE(I)  = NAFFT
    END DO
      
    CLOSE(102)

! read the surface triangulization

    OPEN(103,FILE=pathname(1:lenpath)//FNAME(1:lenfname)//".face")

    READ(103,*) FHEAD
    READ(103,*) FHEAD
    READ(103,*) NFACE, PPP, QQQ, RRR

    ALLOCATE(NVERT(3,NFACE), MFACE(NFACE), STAT=ierr)
    IF (ierr .NE. 0) THEN
        WRITE(6,*) 'Error allocating NVERT, MFACE'
    STOP
    END IF

    NVERT=0; MFACE=0
      
    DO I=1,NFACE 
       READ(103,*) NIND(1:5) 
       NVERT(1:3,I) = NIND(1:3);  MFACE(I) = NIND(4)
    END DO
    CLOSE(103)
    call surface_area(s_area) ! the post-MSMS code
    print *,'surface area=', real(s_area)

    rslt=system('rm '//pathname(1:lenpath)//fname(1:lenfname)//".xyzr")
    rslt=system('rm '//pathname(1:lenpath)//fname(1:lenfname)//".vert")
    rslt=system('rm '//pathname(1:lenpath)//fname(1:lenfname)//".face")

End
!------------------------------------------------------------------------
subroutine surface_area(s_area)
use molecule
implicit double precision(a-h,o-z)
integer iface(3),jface(3),nfacenew,ialert
real*8 face(3,3),s_area,face_old(3,3),xx(3),yy(3),cpu1,cpu2
real*8,allocatable:: nvert_copy(:,:)

        print *,'# of surfaces=',nface,' # of surface points=',nspt
        call cpu_time(cpu1)
        s_area=0.d0
        nfacenew=nface
        allocate(nvert_copy(3,nface))
        nvert_copy=nvert
        do i=1,nface
                iface=nvert(:,i)
                xx=0.d0
                ialert=0;
                do j=1,3
                    face(:,j)=sptpos(:,iface(j))
                    xx=xx+1/3.d0*(face(:,j))
                enddo
                aa=sqrt(dot_product(face(:,1)-face(:,2),face(:,1)-face(:,2)))
                bb=sqrt(dot_product(face(:,1)-face(:,3),face(:,1)-face(:,3)))
                cc=sqrt(dot_product(face(:,2)-face(:,3),face(:,2)-face(:,3)))
                area_local=triangle_area(aa,bb,cc)
                
                do ii=max(1,i-10),i-1
                    jface=nvert(:,ii)
                    yy=0.d0
                    do j=1,3
                        face_old(:,j)=sptpos(:,jface(j))
                        yy=yy+1/3.d0*(face_old(:,j))
                    enddo
                    dist_local=dot_product(xx-yy,xx-yy)
                    if (dist_local<1.d-5) then
                       ialert=1
!                       print *,i,ii,'particles are too close',dist_local
                    endif
                enddo
                if (area_local < 1.d-5 .or. ialert==1) then
!                    print *,i,j,'small area=', area_local
                    ichanged=nface-nfacenew
                    nvert_copy(:,(i-ichanged):(nface-1))=nvert_copy(:,(i-ichanged+1):nface)
                    nfacenew=nfacenew-1
                endif
                s_area=s_area+area_local
        enddo
        print *,nface-nfacenew,' ugly faces are deleted'
        
        nface=nfacenew
        deallocate(nvert,STAT=ierr)
        IF (ierr .NE. 0) THEN
            WRITE(6,*) 'Error deallocating nvert'
            STOP
        EndIF
       
        allocate(nvert(3,nface),STAT=ierr)
        IF (ierr .NE. 0) THEN
            WRITE(6,*) 'Error allocating nvert'
            STOP
        EndIF
        nvert=nvert_copy(:,1:nface)
        deallocate(nvert_copy, STAT=ierr)
        
        IF (ierr .NE. 0) THEN
            WRITE(6,*) 'Error deallocating nvert_copy'
            STOP
        call cpu_time(cpu2)
        EndIF
        print *, 'total MSMS post-processing time =',cpu2-cpu1
end

!------------------------------------------------------------------------
function triangle_area(aa,bb,cc)
implicit double precision(a-h,o-z)
    s=0.5d0*(aa+bb+cc)
    triangle_area=sqrt(s*(s-aa)*(s-bb)*(s-cc))
end 

