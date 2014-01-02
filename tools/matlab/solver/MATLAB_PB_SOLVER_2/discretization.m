
%discretization charge density
XI=linspace(-glen(1)/2,glen(1)/2,dime(1));
YI=linspace(-glen(2)/2,glen(2)/2,dime(2));
ZI=linspace(-glen(3)/2,glen(3)/2,dime(3));

% read the position of each charge point like
ii=zeros(atomN,1);
jj=zeros(atomN,1);
kk=zeros(atomN,1);
for p=1: atomN
i=1;
    while XI(i)<atomP(p,1) && i<=dime(1)
        i=i+1;
    end
     if XI(i)==atomP(p,1)
        ii(p)=i;
     else
         ii(p)=i-1;
     end
     j=1;
    while YI(j)<atomP(p,2) && j<=dime(2)
        j=j+1;
    end
     if YI(j)==atomP(p,2)
        jj(p)=j;
     else
         jj(p)=j-1;
     end
k=1;
    while ZI(k)<atomP(p,3) && k<=dime(3)
        k=k+1;
    end
     if ZI(k)==atomP(p,3)
        kk(p)=k;
     else
         kk(p)=k-1;
     end
end
% define nearest neighbor (8 corners per point like charge)
% first corner: ii jj kk
%ssecond ii+1 jj kk
% ii+1 jj+1 kk
%ii jj+1 kk
%ii jj kk+1
% ii+1 jj kk+1
% ii+1 jj+1 kk+1
% II JJ+1 KK+1
charge=zeros(dime(1),dime(2),dime(3));
%
for p=1:atomN

    IXI(1)=XI(ii(p));
    IYI(1)=YI(jj(p));
    IZI(1)=ZI(kk(p));

[XA YA ZA]=meshgrid(IXI(1):h(1):IXI(1)+h(1),IYI(1):h(2):IYI(1)+h(2),IZI(1):h(3):IZI(1)+h(3));
%
[XX YY ZZ]=meshgrid(atomP(p,1)-h(1):h(1):atomP(p,1)+h(1),atomP(p,2)-h(2):h(2):atomP(p,2)+h(2),atomP(p,3)-h(3):h(3):atomP(p,3)+h(3));
 rho=zeros(3,3,3);
 rho(2,2,2)=atomC(p)/prod(h);
%
    rhogrid = interp3(XX,YY,ZZ,rho,XA,YA,ZA,'linear');
%    
    charge(ii(p),jj(p),kk(p))=rhogrid(1,1,1);
    charge(ii(p)+1,jj(p),kk(p))=rhogrid(2,1,1);
    charge(ii(p)+1,jj(p)+1,kk(p))=rhogrid(2,2,1);
    charge(ii(p),jj(p)+1,kk(p))=rhogrid(1,2,1);
    charge(ii(p),jj(p),kk(p)+1)=rhogrid(1,1,2);
    charge(ii(p)+1,jj(p),kk(p)+1)=rhogrid(2,1,2);
    charge(ii(p),jj(p)+1,kk(p)+1)=rhogrid(1,2,2);
    charge(ii(p)+1,jj(p)+1,kk(p)+1)=rhogrid(2,2,2);
end
we=nonzeros(charge);