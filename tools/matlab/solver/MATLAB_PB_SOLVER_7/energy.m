% Energy calculation for point liek charge systems
% borrowed from Vpmg_qfAtomEnergy
    ener = 0.0;
for p=1: atomN
%    /* Figure out which vertices we're next to */

 partcharge=atomC(p);
 
 
    ifloat = (atomP(p,1) - xmin)/h(1)+1.;
    jfloat = (atomP(p,2) - ymin)/h(2)+1.;
    kfloat = (atomP(p,3) - zmin)/h(3)+1.;
    
ihi=ceil(ifloat);
ilo=floor(ifloat);
jhi=ceil(jfloat);
jlo=floor(jfloat);
khi=ceil(kfloat);
klo=floor(kfloat);

        if ((ihi<=dime(1)) && (jhi<=dime(2)) && (khi<=dime(3)) &&...
            (ilo>=1) && (jlo>=1) && (klo>=1))
                dx = ifloat - double(ilo);
                dy = jfloat - double(jlo);
                dz = kfloat - double(klo);
%                 nx = nxOLD; 
%                 ny = nyOLD; 
%                 nz = nzOLD;
                uval =  dx*dy*dz*(MATLAB_pot(ihi,jhi,khi))...
                  + dx*(1.0-dy)*dz*(MATLAB_pot(ihi,jlo,khi))...
                  + dx*dy*(1.0-dz)*(MATLAB_pot(ihi,jhi,klo))...
                  + dx*(1.0-dy)*(1.0-dz)*(MATLAB_pot(ihi,jlo,klo))...
                  + (1.0-dx)*dy*dz*(MATLAB_pot(ilo,jhi,khi))...
                  + (1.0-dx)*(1.0-dy)*dz*(MATLAB_pot(ilo,jlo,khi))...
                  + (1.0-dx)*dy*(1.0-dz)*(MATLAB_pot(ilo,jhi,klo))...
                  + (1.0-dx)*(1.0-dy)*(1.0-dz)*(MATLAB_pot(ilo,jlo,klo));
            ener = ener + (uval*partcharge);
        end
end

% becuase u = ec*phi/KT and Energy=0.5*q*phi=0.5*z*ec*phi=0.5*u*KT
%because KT is in erg units and theenergy is in KJ/mol we hace to convert from
%erg to KJ/mol as follows
ergtokj=N_A/10^10;
%Then the energy reads
enerT=0.5*ener*T*k_B*ergtokj

