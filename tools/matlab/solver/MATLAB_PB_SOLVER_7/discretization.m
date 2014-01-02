
%charge density discretization 

% let's define the min/max domains
xmin=xcent-glen(1)/2.;
ymin=ycent-glen(2)/2.;
zmin=zcent-glen(3)/2.;
xmax=xcent+glen(1)/2.;
ymax=ycent+glen(2)/2.;
zmax=zcent+glen(3)/2.;
% let's convert the atom position to the grid reference system
Px(:,1)=atomP(:,1)-xmin;
Py(:,2)=atomP(:,2)-ymin;
Pz(:,3)=atomP(:,3)-zmin;

% allocating arrays and vectors
charge=zeros(dime(1),dime(2),dime(3));
subcharge=zeros(prod(dime),1);
%
for p=1:atomN
    
    if ((atomP(p,1) >=xmin) && (atomP(p,1)<= xmax)&& (atomP(p,2) >=ymin) && (atomP(p,2)...
            <= ymax) && (atomP(p,3) >=zmin) && (atomP(p,3) <= zmax))

ifloat=Px(p,1)/h(1)+1.;
jfloat=Py(p,2)/h(2)+1.;
kfloat=Pz(p,3)/h(3)+1.;

ihi=ceil(ifloat);
ilo=floor(ifloat);
jhi=ceil(jfloat);
jlo=floor(jfloat);
khi=ceil(kfloat);
klo=floor(kfloat);

dx=ifloat-double(ilo);
dy=jfloat-double(jlo);
dz=kfloat-double(klo);


 partcharge=atomC(p)/prod(h);
     
      eme=(khi-1)*(dime(1))*(dime(2))+(jhi-1)*(dime(1))+ihi;
     subcharge(eme)=subcharge(eme)+dx*dy*dz*partcharge;
      eme=(khi-1)*(dime(1))*(dime(2))+(jlo-1)*(dime(1))+ihi;
     subcharge(eme)=subcharge(eme)+dx*(1.-dy)*dz*partcharge;
      eme=(klo-1)*(dime(1))*(dime(2))+(jlo-1)*(dime(1))+ihi;
     subcharge(eme)=subcharge(eme)+dx*(1.-dy)*(1.-dz)*partcharge;
      eme=(khi-1)*(dime(1))*(dime(2))+(jhi-1)*(dime(1))+ilo;
     subcharge(eme)=subcharge(eme)+(1.-dx)*dy*dz*partcharge;
      eme=(khi-1)*(dime(1))*(dime(2))+(jlo-1)*(dime(1))+ilo;
     subcharge(eme)=subcharge(eme)+(1.-dx)*(1.-dy)*dz*partcharge;
      eme=(klo-1)*(dime(1))*(dime(2))+(jhi-1)*(dime(1))+ihi;
     subcharge(eme)=subcharge(eme)+dx*dy*(1.-dz)*partcharge;
      eme=(klo-1)*(dime(1))*(dime(2))+(jhi-1)*(dime(1))+ilo;
     subcharge(eme)=subcharge(eme)+(1.-dx)*dy*(1.-dz)*partcharge;
      eme=(klo-1)*(dime(1))*(dime(2))+(jlo-1)*(dime(1))+ilo;
     subcharge(eme)=subcharge(eme)+(1.-dx)*(1.-dy)*(1.-dz)*partcharge;

    end
end

% convert vector to array format

for i=1:dime(1)
    for j=1:dime(2)
        for k=1:dime(3)
            pepe=(k-1)*dime(1)*dime(2)+(j-1)*dime(1)+i;
            charge(i,j,k)=subcharge(pepe);
        end
    end
end