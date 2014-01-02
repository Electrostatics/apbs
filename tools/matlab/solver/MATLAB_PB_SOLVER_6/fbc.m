
VSMALL=0.0000000000000001;

%  /* Calculate new problem dimensions */
    hxNEW =h(1);
    hyNEW =h(2);
    hzNEW = h(3);
    nxNEW = dime(1);
    nyNEW = dime(2);
    nzNEW = dime(3);
    xminNEW = xcent - (nxNEW-1)*hxNEW/2.0;
    xmaxNEW = xcent + (nxNEW-1)*hxNEW/2.0;
    yminNEW = ycent - (nyNEW-1)*hyNEW/2.0;
    ymaxNEW = ycent + (nyNEW-1)*hyNEW/2.0;
    zminNEW = zcent - (nzNEW-1)*hzNEW/2.0;
    zmaxNEW = zcent + (nzNEW-1)*hzNEW/2.0;

  %  /* Relevant old problem parameters */
    hxOLD = cgh(1);
    hyOLD = cgh(2);
    hzOLD = cgh(3);
    nxOLD = cgdime(1);
    nyOLD = cgdime(2);
    nzOLD = cgdime(3);
    xminOLD = xcent - (nxOLD-1)*hxOLD/2.0;
    xmaxOLD = xcent + (nxOLD-1)*hxOLD/2.0;
    yminOLD = ycent - (nyOLD-1)*hyOLD/2.0;
    ymaxOLD = ycent + (nyOLD-1)*hyOLD/2.0;
    zminOLD = zcent - (nzOLD-1)*hzOLD/2.0;
    zmaxOLD = zcent + (nzOLD-1)*hzOLD/2.0;

gxcf=zeros(dime(2),dime(3),2);
gycf=zeros(dime(1),dime(3),2);
gzcf=zeros(dime(1),dime(2),2);
potB=zeros(dime(1),dime(2),dime(3));
%     /* BOUNDARY CONDITION SETUP FOR POINTS OFF OLD MESH:
%      * For each "atom" (only one for bcfl=1), we use the following formula to
%      * calculate the boundary conditions:
%      *    g(x) = \frac{q e_c}{4*\pi*\eps_0*\eps_w*k_b*T}
%      *          * \frac{exp(-xkappa*(d - a))}{1+xkappa*a}
%      *          * 1/d
%      * where d = ||x - x_0|| (in m) and a is the size of the atom (in m).
%      * We only need to evaluate some of these prefactors once:
%      *    pre1 = \frac{e_c}{4*\pi*\eps_0*\eps_w*k_b*T}
%      * which gives the potential as
%      *    g(x) = pre1 * q/d * \frac{exp(-xkappa*(d - a))}{1+xkappa*a}
%      */
%     eps_w = Vpbe_getSolventDiel(pbe);           /* Dimensionless */
%     T = Vpbe_getTemperature(pbe);               /* K             */
%     pre1 = (Vunit_ec)/(4*VPI*Vunit_eps0*eps_w*Vunit_kb*T);
% 
%     /* Finally, if we convert keep xkappa in A^{-1} and scale pre1 by
%      * m/A, then we will only need to deal with distances and sizes in
%      * Angstroms rather than meters.                                       */
%     xkappa = Vpbe_getXkappa(pbe);              /* A^{-1}        */
%     pre1 = pre1*(1.0e10);
%     size = Vpbe_getSoluteRadius(pbe);
%     apos = Vpbe_getSoluteCenter(pbe);
%     charge = Vunit_ec*Vpbe_getSoluteCharge(pbe);
% 
%     /* Check for rounding error */
%     if (VABS(xminOLD-xminNEW) < VSMALL) xminNEW = xminOLD;
%     if (VABS(xmaxOLD-xmaxNEW) < VSMALL) xmaxNEW = xmaxOLD;
%     if (VABS(yminOLD-yminNEW) < VSMALL) yminNEW = yminOLD;
%     if (VABS(ymaxOLD-ymaxNEW) < VSMALL) ymaxNEW = ymaxOLD;
%     if (VABS(zminOLD-zminNEW) < VSMALL) zminNEW = zminOLD;
%     if (VABS(zmaxOLD-zmaxNEW) < VSMALL) zmaxNEW = zmaxOLD;
%     
% 
%     /* Sanity check: make sure we're within the old mesh */
%     Vnm_print(0, "VPMG::focusFillBound -- New mesh mins = %g, %g, %g\n",
%       xminNEW, yminNEW, zminNEW);
%     Vnm_print(0, "VPMG::focusFillBound -- New mesh maxs = %g, %g, %g\n",
%       xmaxNEW, ymaxNEW, zmaxNEW);
%     Vnm_print(0, "VPMG::focusFillBound -- Old mesh mins = %g, %g, %g\n",
%       xminOLD, yminOLD, zminOLD);
%     Vnm_print(0, "VPMG::focusFillBound -- Old mesh maxs = %g, %g, %g\n",
%       xmaxOLD, ymaxOLD, zmaxOLD);
% 
%     /* The following is obsolete; we'll substitute analytical boundary
%      * condition values when the new mesh falls outside the old */
%     if ((xmaxNEW>xmaxOLD) || (ymaxNEW>ymaxOLD) || (zmaxNEW>zmaxOLD) ||
%         (xminOLD>xminNEW) || (yminOLD>yminNEW) || (zminOLD>zminNEW)) {
% 
%         Vnm_print(2, "Vpmg::focusFillBound -- new mesh not contained in old!\n");
% 		Vnm_print(2, "Vpmg::focusFillBound -- old mesh min = (%g, %g, %g)\n",
% 				  xminOLD, yminOLD, zminOLD);
%         Vnm_print(2, "Vpmg::focusFillBound -- old mesh max = (%g, %g, %g)\n",
% 				  xmaxOLD, ymaxOLD, zmaxOLD);
% 		Vnm_print(2, "Vpmg::focusFillBound -- new mesh min = (%g, %g, %g)\n",
% 				  xminNEW, yminNEW, zminNEW);
%         Vnm_print(2, "Vpmg::focusFillBound -- new mesh max = (%g, %g, %g)\n",
% 				  xmaxNEW, ymaxNEW, zmaxNEW);
% 		fflush(stderr);
%         VASSERT(0);
%     }

    
%    /* Fill the "i" boundaries (dirichlet) */
    for k=1:nzNEW
        for j=1:nyNEW
%            /* Low X face */
            x = xminNEW;
            y = yminNEW + (j-1)*hyNEW;
            z = zminNEW + (k-1)*hzNEW;
            if ((x >= (xminOLD-VSMALL)) && (y >= (yminOLD-VSMALL)) && (z >= (zminOLD-VSMALL)) &&...
                (x <= (xmaxOLD+VSMALL)) && (y <= (ymaxOLD+VSMALL)) && (z <= (zmaxOLD+VSMALL))) 
                ifloat = (x - xminOLD)/hxOLD+0.;
                jfloat = (y - yminOLD)/hyOLD+0.;
                kfloat = (z - zminOLD)/hzOLD+0.;
                ihi = ceil(ifloat);
                if (ihi > (nxOLD-1)) 
                    ihi = nxOLD-1;
                
                end
                ilo = floor(ifloat);
                if (ilo < 1) 
                    ilo = 1;
                
                end
                jhi = ceil(jfloat);
                if (jhi > (nyOLD-1)) 
                    jhi = nyOLD-1;
               
                end
                 jlo = floor(jfloat);
                if (jlo < 1) 
                    jlo = 1;
                
                end
                khi = ceil(kfloat);
                if (khi > (nzOLD-1)) 
                    khi = nzOLD-1;
                
                end
                klo = floor(kfloat);
                if (klo < 1) 
                    klo = 1;
                end
                dx = ifloat - double(ilo);
                dy = jfloat - double(jlo);
                dz = kfloat - double(klo);
%                 nx = nxOLD; 
%                 ny = nyOLD; 
%                 nz = nzOLD;
                uval =  dx*dy*dz*(MATLAB_pot_coarse(ihi,jhi,khi))...
                  + dx*(1.0-dy)*dz*(MATLAB_pot_coarse(ihi,jlo,khi))...
                  + dx*dy*(1.0-dz)*(MATLAB_pot_coarse(ihi,jhi,klo))...
                  + dx*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ihi,jlo,klo))...
                  + (1.0-dx)*dy*dz*(MATLAB_pot_coarse(ilo,jhi,khi))...
                  + (1.0-dx)*(1.0-dy)*dz*(MATLAB_pot_coarse(ilo,jlo,khi))...
                  + (1.0-dx)*dy*(1.0-dz)*(MATLAB_pot_coarse(ilo,jhi,klo))...
                  + (1.0-dx)*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ilo,jlo,klo));
%                 nx = nxNEW; 
%                 ny = nyNEW; 
%                 nz = nzNEW;
             else 
				disp('problem!!!....')
            end
%             nx = nxNEW; 
%             ny = nyNEW; 
%             nz = nzNEW;
            gxcf(j,k,1) = uval;
            potB(1,j,k)= uval;

%            /* High X face */
            x = xmaxNEW;
            if ((x >= (xminOLD-VSMALL)) && (y >= (yminOLD-VSMALL)) && (z >= (zminOLD-VSMALL)) &&...
                (x <= (xmaxOLD+VSMALL)) && (y <= (ymaxOLD+VSMALL)) && (z <= (zmaxOLD+VSMALL))) 
                ifloat = (x - xminOLD)/hxOLD+0.;
                jfloat = (y - yminOLD)/hyOLD+0.;
                kfloat = (z - zminOLD)/hzOLD+0.;
                ihi = ceil(ifloat);
                if (ihi > (nxOLD-1)) 
                    ihi = nxOLD-1;
                
                end
                ilo = floor(ifloat);
                if (ilo < 1) 
                    ilo = 1;
                
                end
                jhi = ceil(jfloat);
                if (jhi > (nyOLD-1)) 
                    jhi = nyOLD-1;
                
                end
                jlo = floor(jfloat);
                if (jlo < 1) 
                    jlo = 1;
                end
                khi = ceil(kfloat);
                if (khi > (nzOLD-1)) 
                    khi = nzOLD-1;
                end
                klo = floor(kfloat);
                if (klo < 1) 
                    klo = 1;
                end
                dx = ifloat - double(ilo);
                dy = jfloat - double(jlo);
                dz = kfloat - double(klo);
%                 nx = nxOLD; 
%                 ny = nyOLD; 
%                 nz = nzOLD;
                uval =  dx*dy*dz*(MATLAB_pot_coarse(ihi,jhi,khi))...
                  + dx*(1.0-dy)*dz*(MATLAB_pot_coarse(ihi,jlo,khi))...
                  + dx*dy*(1.0-dz)*(MATLAB_pot_coarse(ihi,jhi,klo))...
                  + dx*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ihi,jlo,klo))...
                  + (1.0-dx)*dy*dz*(MATLAB_pot_coarse(ilo,jhi,khi))...
                  + (1.0-dx)*(1.0-dy)*dz*(MATLAB_pot_coarse(ilo,jlo,khi))...
                  + (1.0-dx)*dy*(1.0-dz)*(MATLAB_pot_coarse(ilo,jhi,klo))...
                  + (1.0-dx)*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ilo,jlo,klo));
%                 nx = nxNEW; 
%                 ny = nyNEW; 
%                 nz = nzNEW;
          
            else 
               disp('problem!!!....')
            end
%            nx = nxNEW; ny = nyNEW; nz = nzNEW;
             gxcf(j,k,2) = uval;
             potB(nxNEW,j,k)= uval;
            
%             /* Zero Neumann conditions */             
%             nx = nxNEW; ny = nyNEW; nz = nzNEW;
%             thee->gxcf[IJKx(j,k,2)] = 0.0;
%             nx = nxNEW; ny = nyNEW; nz = nzNEW;
%             thee->gxcf[IJKx(j,k,3)] = 0.0;
        end
    end

%    /* Fill the "j" boundaries (dirichlet) */
    for k=1:nzNEW 
        for i=1:nxNEW
%            /* Low Y face */
            x = xminNEW + (i-1)*hxNEW;
            y = yminNEW;
            z = zminNEW + (k-1)*hzNEW;
            if ((x >= (xminOLD-VSMALL)) && (y >= (yminOLD-VSMALL)) && (z >= (zminOLD-VSMALL)) &&...
                (x <= (xmaxOLD+VSMALL)) && (y <= (ymaxOLD+VSMALL)) && (z <= (zmaxOLD+VSMALL))) 
                ifloat = (x - xminOLD)/hxOLD+0.;
                jfloat = (y - yminOLD)/hyOLD+0.;
                kfloat = (z - zminOLD)/hzOLD+0.;
                ihi = ceil(ifloat);
                if (ihi > (nxOLD-1)) 
                    ihi = nxOLD-1;
                end
                ilo = floor(ifloat);
                if (ilo < 1) 
                    ilo = 1;
                end
                jhi = ceil(jfloat);
                if (jhi > (nyOLD-1)) 
                    jhi = nyOLD-1;
                end
                jlo = floor(jfloat);
                if (jlo < 1) 
                    jlo = 1;
                end
                khi = ceil(kfloat);
                if (khi > (nzOLD-1)) 
                    khi = nzOLD-1;
                end
                klo = floor(kfloat);
                if (klo < 1)
                    klo = 1;
                end
                dx = ifloat - double(ilo);
                dy = jfloat - double(jlo);
                dz = kfloat - double(klo);
%                 nx = nxOLD; 
%                 ny = nyOLD; 
%                 nz = nzOLD;
                uval =  dx*dy*dz*(MATLAB_pot_coarse(ihi,jhi,khi))...
                  + dx*(1.0-dy)*dz*(MATLAB_pot_coarse(ihi,jlo,khi))...
                  + dx*dy*(1.0-dz)*(MATLAB_pot_coarse(ihi,jhi,klo))...
                  + dx*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ihi,jlo,klo))...
                  + (1.0-dx)*dy*dz*(MATLAB_pot_coarse(ilo,jhi,khi))...
                  + (1.0-dx)*(1.0-dy)*dz*(MATLAB_pot_coarse(ilo,jlo,khi))...
                  + (1.0-dx)*dy*(1.0-dz)*(MATLAB_pot_coarse(ilo,jhi,klo))...
                  + (1.0-dx)*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ilo,jlo,klo));
%                 nx = nxNEW; 
%                 ny = nyNEW; 
%                 nz = nzNEW;
            else 
                 disp('problem!!!....')
            end
%             nx = nxNEW; 
%             ny = nyNEW; 
%             nz = nzNEW;
            gycf(i,k,1) = uval;
             potB(i,1,k)= uval;

%            /* High Y face */
            y = ymaxNEW;
            if ((x >= (xminOLD-VSMALL)) && (y >= (yminOLD-VSMALL)) && (z >= (zminOLD-VSMALL)) && ...
                (x <= (xmaxOLD+VSMALL)) && (y <= (ymaxOLD+VSMALL)) && (z <= (zmaxOLD+VSMALL))) 
                ifloat = (x - xminOLD)/hxOLD+0.;
                jfloat = (y - yminOLD)/hyOLD+0.;
                kfloat = (z - zminOLD)/hzOLD+0.;
                ihi = ceil(ifloat);
                if (ihi > (nxOLD-1)) 
                    ihi = nxOLD-1;
                end
                ilo = floor(ifloat);
                if (ilo < 1) 
                    ilo = 1;
                end
                jhi = ceil(jfloat);
                if (jhi > (nyOLD-1)) 
                    jhi = nyOLD-1;
                end
                jlo = floor(jfloat);
                if (jlo < 1) 
                    jlo = 1;
                end
                khi = ceil(kfloat);
                if (khi > (nzOLD-1))
                    khi = nzOLD-1;
                end
                klo = floor(kfloat);
                if (klo < 1)
                    klo = 1;
                end
                dx = ifloat - double(ilo);
                dy = jfloat - double(jlo);
                dz = kfloat - double(klo);
%                 nx = nxOLD; 
%                 ny = nyOLD; 
%                 nz = nzOLD;
                uval =  dx*dy*dz*(MATLAB_pot_coarse(ihi,jhi,khi))...
                  + dx*(1.0-dy)*dz*(MATLAB_pot_coarse(ihi,jlo,khi))...
                  + dx*dy*(1.0-dz)*(MATLAB_pot_coarse(ihi,jhi,klo))...
                  + dx*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ihi,jlo,klo))...
                  + (1.0-dx)*dy*dz*(MATLAB_pot_coarse(ilo,jhi,khi))...
                  + (1.0-dx)*(1.0-dy)*dz*(MATLAB_pot_coarse(ilo,jlo,khi))...
                  + (1.0-dx)*dy*(1.0-dz)*(MATLAB_pot_coarse(ilo,jhi,klo))...
                  + (1.0-dx)*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ilo,jlo,klo));
%                 nx = nxNEW;
%                 ny = nyNEW;
%                 nz = nzNEW;
            else
                disp('problem!!!....')
            end
%             nx = nxNEW; 
%             ny = nyNEW;
%             nz = nzNEW;
            gycf(i,k,2) = uval;
            potB(i,nyNEW,k)= uval;
%             /* Zero Neumann conditions */
%             nx = nxNEW; ny = nyNEW; nz = nzNEW;
%             thee->gycf[IJKy(i,k,2)] = 0.0;
%             nx = nxNEW; ny = nyNEW; nz = nzNEW;
%             thee->gycf[IJKy(i,k,3)] = 0.0;
        end
    end

%    /* Fill the "k" boundaries (dirichlet) */
    for j=1:nyNEW 
        for i=1:nxNEW
%            /* Low Z face */
            x = xminNEW + (i-1)*hxNEW;
            y = yminNEW + (j-1)*hyNEW;
            z = zminNEW;
            if ((x >= (xminOLD-VSMALL)) && (y >= (yminOLD-VSMALL)) && (z >= (zminOLD-VSMALL)) &&...
                (x <= (xmaxOLD+VSMALL)) && (y <= (ymaxOLD+VSMALL)) && (z <= (zmaxOLD+VSMALL))) 
                ifloat = (x - xminOLD)/hxOLD+0.;
                jfloat = (y - yminOLD)/hyOLD+0.;
                kfloat = (z - zminOLD)/hzOLD+0.;
                ihi = ceil(ifloat);
                if (ihi > (nxOLD-1)) 
                    ihi = nxOLD-1;
                end
                ilo = floor(ifloat);
                if (ilo < 1) 
                    ilo = 1;
                end
                jhi = ceil(jfloat);
                if (jhi > (nyOLD-1)) 
                    jhi = nyOLD-1;
                end
                jlo = floor(jfloat);
                if (jlo < 1) 
                    jlo = 1;
                end
                khi = ceil(kfloat);
                if (khi > (nzOLD-1)) 
                    khi = nzOLD-1;
                end
                klo = floor(kfloat);
                if (klo < 1) 
                    klo = 1;
                end
                dx = ifloat - double(ilo);
                dy = jfloat - double(jlo);
                dz = kfloat - double(klo);
%                 nx = nxOLD;
%                 ny = nyOLD; 
%                 nz = nzOLD;
                uval =  dx*dy*dz*(MATLAB_pot_coarse(ihi,jhi,khi))...
                  + dx*(1.0-dy)*dz*(MATLAB_pot_coarse(ihi,jlo,khi))...
                  + dx*dy*(1.0-dz)*(MATLAB_pot_coarse(ihi,jhi,klo))...
                  + dx*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ihi,jlo,klo))...
                  + (1.0-dx)*dy*dz*(MATLAB_pot_coarse(ilo,jhi,khi))...
                  + (1.0-dx)*(1.0-dy)*dz*(MATLAB_pot_coarse(ilo,jlo,khi))...
                  + (1.0-dx)*dy*(1.0-dz)*(MATLAB_pot_coarse(ilo,jhi,klo))...
                  + (1.0-dx)*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ilo,jlo,klo));
%                 nx = nxNEW;
%                 ny = nyNEW;
%                 nz = nzNEW;
             else 
				disp('problem!!!....')
            end
%             nx = nxNEW; 
%             ny = nyNEW; 
%             nz = nzNEW;
            gzcf(i,j,1) = uval;
             potB(i,j,1)= uval;

 %           /* High Z face */
            z = zmaxNEW;
            if ((x >= (xminOLD-VSMALL)) && (y >= (yminOLD-VSMALL)) && (z >= (zminOLD-VSMALL)) &&...
                (x <= (xmaxOLD+VSMALL)) && (y <= (ymaxOLD+VSMALL)) && (z <= (zmaxOLD+VSMALL))) 
                ifloat = (x - xminOLD)/hxOLD+0.;
                jfloat = (y - yminOLD)/hyOLD+0.;
                kfloat = (z - zminOLD)/hzOLD+0.;
                ihi = ceil(ifloat);
                if (ihi > (nxOLD-1)) 
                    ihi = nxOLD-1;
                end
                ilo = floor(ifloat);
                if (ilo < 1) 
                    ilo = 1;
                end
                jhi = ceil(jfloat);
                if (jhi > (nyOLD-1)) 
                    jhi = nyOLD-1;
                end
                jlo = floor(jfloat);
                if (jlo < 1) 
                    jlo = 1;
                end
                khi = ceil(kfloat);
                if (khi > (nzOLD-1)) 
                    khi = nzOLD-1;
                end
                klo = floor(kfloat);
                if (klo < 1) 
                    klo = 1;
                end
                dx = ifloat - double(ilo);
                dy = jfloat - double(jlo);
                dz = kfloat - double(klo);
%                 nx = nxOLD; 
%                 ny = nyOLD; 
%                 nz = nzOLD;
                uval =  dx*dy*dz*(MATLAB_pot_coarse(ihi,jhi,khi))...
                  + dx*(1.0-dy)*dz*(MATLAB_pot_coarse(ihi,jlo,khi))...
                  + dx*dy*(1.0-dz)*(MATLAB_pot_coarse(ihi,jhi,klo))...
                  + dx*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ihi,jlo,klo))...
                  + (1.0-dx)*dy*dz*(MATLAB_pot_coarse(ilo,jhi,khi))...
                  + (1.0-dx)*(1.0-dy)*dz*(MATLAB_pot_coarse(ilo,jlo,khi))...
                  + (1.0-dx)*dy*(1.0-dz)*(MATLAB_pot_coarse(ilo,jhi,klo))...
                  + (1.0-dx)*(1.0-dy)*(1.0-dz)*(MATLAB_pot_coarse(ilo,jlo,klo));
%                 nx = nxNEW; 
%                 ny = nyNEW; 
%                 nz = nzNEW;
             else 
				disp('problem!!!....')
            end
            nx = nxNEW; 
            ny = nyNEW; 
            nz = nzNEW;
            gzcf(i,j,2) = uval;
             potB(i,j,nzNEW)= uval;

%             /* Zero Neumann conditions */
%             nx = nxNEW; ny = nyNEW; nz = nzNEW;
%             thee->gzcf[IJKz(i,j,2)] = 0.0;
%             nx = nxNEW; ny = nyNEW; nz = nzNEW;
%             thee->gzcf[IJKz(i,j,3)] = 0.0;
        end
    end

