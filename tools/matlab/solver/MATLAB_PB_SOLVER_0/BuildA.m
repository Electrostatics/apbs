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

disp('constructing A Matrix....')

%boundary components are brought to the right-hand-side of the equation
for i=2:2
    for j=2:dime(2)-1
        for k=2:dime(3)-1
             fc(i,j,k)=fc(i,j,k)+dielx(i-1,j,k)*ch(1)*gxcf(j,k,1);
         end
    end
end
%boundary components are brought to the right-hand-side of the equation
for i=2:dime(1)-1
    for j=2:2
        for k=2:dime(3)-1
             fc(i,j,k)=fc(i,j,k)+diely(i,j-1,k)*ch(2)*gycf(i,k,1);  
         end
    end
end
%boundary components are brought to the right-hand-side of the equation
for i=2:dime(1)-1
    for j=2:dime(2)-1
        for k=2:2
             fc(i,j,k)=fc(i,j,k)+dielz(i,j,k-1)*ch(3)*gzcf(i,j,1); 
        end
    end
end 
%boundary components are brought to the right-hand-side of the equation
for i=dime(1)-1:dime(1)-1
    for j=2:dime(2)-1
        for k=2:dime(3)-1
             fc(i,j,k)=fc(i,j,k)+dielx(i,j,k)*ch(1)*gxcf(j,k,2);
        end
    end
end
%boundary components are brought to the right-hand-side of the equation
for i=2:dime(1)-1
            for j=dime(2)-1:dime(2)-1
                for k=2:dime(3)-1
             fc(i,j,k)=fc(i,j,k)+diely(i,j,k)*ch(2)*gycf(i,k,2);
                end
            end
end
%boundary components are brought to the right-hand-side of the equation
for i=2:dime(1)-1
    for j=2:dime(2)-1
        for k=dime(3)-1:dime(3)-1
            fc(i,j,k)=fc(i,j,k)+dielz(i,j,k)*ch(3)*gzcf(i,j,2); 
         end
    end
end 

zmagic=4*pi*e_c^2/(k_B*T)*10^8; % should be derived from unit conversions similar to zmagic in VPBE.c of APBS's source code


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

%array to column converter
for i=2:dime(1)-1
    for j=2:dime(2)-1
        for k=2:dime(3)-1
             pe=(k-2)*(dime(1)-2)*(dime(2)-2)+(j-2)*(dime(1)-2)+i-1;
             AAxB(pe)=AxB(i,j,k);
             AAyB(pe)=AyB(i,j,k);
             AAzB(pe)=AzB(i,j,k);
             AAMain(pe)=(dielx(i,j,k)+dielx(i-1,j,k))*ch(1)+(diely(i,j,k)+diely(i,j-1,k))*ch(2)...
          +(dielz(i,j,k)+dielz(i,j,k-1))*ch(3)+ prod(h)*kappa(i,j,k);
              bb(pe)=charge(i,j,k)*prod(ch)*zmagic+fc(i,j,k);
        end
    end
end

% Generating array by placing each diagonal column in the following order
diagonals=horzcat(AAMain,AAxB,AAyB,AAzB);

% Diagonal displacement vector corresponding to the pevious array
d=[0 -1 -(dime(1)-2) -(dime(1)-2)*(dime(2)-2)];

% Generating lower Sparse Matrix AA
AA=spdiags(diagonals, d, prod(dime-2), prod(dime-2));

% Generating the transpose Sparse Matrix AA
B=AA';

%Generating the symmetric A Matrix
A=tril(AA,0)+triu(B,1);

disp('Done!....')