% memory allocation for arrays

fc=zeros(dime(1)-1,dime(2)-1,dime(3)-1);
AxB=zeros(dime(1)-1,dime(2)-1,dime(3)-1);
AyB=zeros(dime(1)-1,dime(2)-1,dime(3)-1);
AzB=zeros(dime(1)-1,dime(2)-1,dime(3)-1);
AAMain=zeros(prod(dime-2),1);
AAxB=zeros(prod(dime-2),1);
AAyB=zeros(prod(dime-2),1);
AAzB=zeros(prod(dime-2),1);
bb=zeros(prod(dime-2),1);
%step size

ch=prod(h)./(h.^2);

if bx==1
%boundary components are brought to the right-hand-side of the equation
for i=2:2
    for j=2:dime(2)-1
        for k=2:dime(3)-1
             fc(i,j,k)=fc(i,j,k)+dielx(i-1,j,k)*ch(1)*gxcf(j,k,1);
         end
    end
end
end
if by==1
%boundary components are brought to the right-hand-side of the equation if
%Dirichlet boundary condition is required
for i=2:dime(1)-1
    for j=2:2
        for k=2:dime(3)-1
             fc(i,j,k)=fc(i,j,k)+diely(i,j-1,k)*ch(2)*gycf(i,k,1);  
         end
    end
end
end
if bz==1
%boundary components are brought to the right-hand-side of the equation if
%Dirichlet boundary condition is required
for i=2:dime(1)-1
    for j=2:dime(2)-1
        for k=2:2
             fc(i,j,k)=fc(i,j,k)+dielz(i,j,k-1)*ch(3)*gzcf(i,j,1); 
        end
    end
end 
if bx==1
%boundary components are brought to the right-hand-side of the equation if
%Dirichlet boundary condition is required
for i=dime(1)-1:dime(1)-1
    for j=2:dime(2)-1
        for k=2:dime(3)-1
             fc(i,j,k)=fc(i,j,k)+dielx(i,j,k)*ch(1)*gxcf(j,k,2);
        end
    end
end
end
end
if by==1
%boundary components are brought to the right-hand-side of the equation if
%Dirichlet boundary condition is required
for i=2:dime(1)-1
            for j=dime(2)-1:dime(2)-1
                for k=2:dime(3)-1
             fc(i,j,k)=fc(i,j,k)+diely(i,j,k)*ch(2)*gycf(i,k,2);
                end
            end
end
end
if bz==1
%boundary components are brought to the right-hand-side of the equation if
%Dirichlet boundary condition is required
for i=2:dime(1)-1
    for j=2:dime(2)-1
        for k=dime(3)-1:dime(3)-1
            fc(i,j,k)=fc(i,j,k)+dielz(i,j,k)*ch(3)*gzcf(i,j,2); 
         end
    end
end 
end

%first band, x-backward direction AxB
for i=2:dime(1)-1
    for j=1:dime(2)-1
        for k=1:dime(3)-1
             AxB(i-1,j,k)=-dielx(i-1,j,k)*ch(1);
        end
    end
end
%second band, y-backward direction AyB
for i=1:dime(1)-1
    for j=2:dime(2)-1
        for k=1:dime(3)-1
             AyB(i,j-1,k)=-diely(i,j-1,k)*ch(2);
        end
    end
end
%third band, z-backward direction AzB
for i=1:dime(1)-1
    for j=1:dime(2)-1
        for k=2:dime(3)-1
             AzB(i,j,k-1)=-dielz(i,j,k-1)*ch(3);
        end
    end
end

%ka=8.486902807*2.*78.54;

%array to column converter
for i=2:dime(1)-1
    for j=2:dime(2)-1
        for k=2:dime(3)-1
             pe=(k-2)*(dime(1)-2)*(dime(2)-2)+(j-2)*(dime(1)-2)+i-1;
             AAxB(pe)=AxB(i,j,k);
             AAyB(pe)=AyB(i,j,k);
             AAzB(pe)=AzB(i,j,k);
             AAMain(pe)=(dielx(i,j,k)+dielx(i-1,j,k))*ch(1)+(diely(i,j,k)+diely(i,j-1,k))*ch(2)...
          +(dielz(i,j,k)+dielz(i,j,k-1))*ch(3)+ prod(ch)*kappa(i,j,k)*Squared_Debye_Huckel;
              bb(pe)=charge(i,j,k)*prod(ch)*zmagic+fc(i,j,k);
        end
    end
end

% additional diagonals are added if periodic boundary condition is required
if bx==2
    AAxPeriod=zeros(prod(dime-2),1);
    for k=2:dime(3)-1
        for j=2:dime(2)-1
             pe=(k-2)*(dime(1)-2)*(dime(2)-2)+(j-2)*(dime(1)-2)+1;
             AAxPeriod(pe)=-dielx(dime(1),j,k)*ch(1);
        end
    end 
end
if by==2
AAyPeriod=zeros(prod(dime-2),1);
    for k=2:dime(3)-1
        for i=2:dime(1)-1
             pe=(k-2)*(dime(1)-2)*(dime(2)-2)+i-1;
             AAyPeriod(pe)=-diely(i,dime(2),k)*ch(2);
        end
    end 
end
if bz==2
AAzPeriod=zeros(prod(dime-2),1);
    for j=2:dime(2)-1
        for i=2:dime(1)-1
             pe=(j-2)*(dime(1)-2)+i-1;
             AAzPeriod(pe)=-dielz(i,j,dime(3))*ch(3);
        end
    end 
end
% Generating array by placing each diagonal column in the following order
% the number of diagonals depends on the required boundary conditions
% if Dirichlet along the three Cartesian axis
dcdiag=0;
dcx=-1;
dcy=-(dime(1)-2);
%dcy=-((dime(1)-2)-1);
dcz=-(dime(1)-2)*(dime(2)-2);
%dcz=-((dime(1)-2)*(dime(2)-2)-1);
pcx=-(dime(1)-3);
pcy=-((dime(1)-2)*(dime(2)-3));
pcz=-((dime(1)-2)*(dime(2)-2)*(dime(3)-3));
if bx==1 && by==1 && bz==1
diagonals=horzcat(AAMain,AAxB,AAyB,AAzB);
% Diagonal displacement vector corresponding to the pevious array
d=[dcdiag dcx dcy dcz];
end
% Periodicity along x and y axis and Dirichlet along z axis
if bx==2 && by==2 && bz==1
diagonals=horzcat(AAMain,AAxB,AAxPeriod,AAyB,AAyPeriod,AAzB);
% Diagonal displacement vector corresponding to the pevious array
d=[dcdiag dcx pcx dcy pcy dcz];
end
% Periodic conditions along the x , y and z axis
if bx==2 && by==2 && bz==2
diagonals=horzcat(AAMain,AAxB,AAxPeriod,AAyB,AAyPeriod,AAzB,AAzPeriod);
% Diagonal displacement vector corresponding to the pevious array
d=[dcdiag dcx pcx dcy pcy dcz pcz];
end

% Generating lower Sparse Matrix AA
AA=spdiags(diagonals, d, prod(dime-2), prod(dime-2));

% Generating the transpose Sparse Matrix AA
B=AA';

%Generating the symmetric A Matrix
A=tril(AA,0)+triu(B,1);