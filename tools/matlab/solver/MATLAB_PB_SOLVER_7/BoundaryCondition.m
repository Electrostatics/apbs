% This routine evaluates the (Dirichlet) boundary condition


%generating the grid points

X=linspace(-glen(1)/2,glen(1)/2,dime(1));
Y=linspace(-glen(2)/2,glen(2)/2,dime(2));
Z=linspace(-glen(3)/2,glen(3)/2,dime(3));

%memory allocation for arrays

gxcf=zeros(dime(2),dime(3),2);
gycf=zeros(dime(1),dime(3),2);
gzcf=zeros(dime(1),dime(2),2);
potB=zeros(dime(1),dime(2),dime(3));
BC=[1 dime(1);1 dime(2); 1 dime(3)];

%% X Face Boundary
if bx==1
for j=1:dime(2)
    for k=1:dime(3)
        for n=1:2
            i=BC(1,n);
            
            dist=sqrt((X(i)-atomP(:,1)).^2+(Y(j)-atomP(:,2)).^2+(Z(k)-atomP(:,3)).^2);
            val=pre1*atomC./dist.*exp(-xkappa*(dist-atomR))./(1+xkappa*atomR);               
            gxcf(j,k,n)=sum(val);
            potB(i,j,k)=sum(val);
        end
    end
end
end
%% Y Boundary
if by==1 
for i=1:dime(1)
    for k=1:dime(3)
        for n=1:2
            j=BC(2,n);
            dist=sqrt((X(i)-atomP(:,1)).^2+(Y(j)-atomP(:,2)).^2+(Z(k)-atomP(:,3)).^2);
            val=pre1*atomC./dist.*exp(-xkappa*(dist-atomR))./(1+xkappa*atomR);                
            gycf(i,k,n)=sum(val);
            potB(i,j,k)=sum(val);
        end
    end
end
end
%% Z Boundary
if bz==1
for i=1:dime(1)
    for j=1:dime(2)
        for n=1:2
            k=BC(3,n);
            dist=sqrt((X(i)-atomP(:,1)).^2+(Y(j)-atomP(:,2)).^2+(Z(k)-atomP(:,3)).^2);
            val=pre1*atomC./dist.*exp(-xkappa*(dist-atomR))./(1+xkappa*atomR);                
            gzcf(i,j,n)=sum(val);
            potB(i,j,k)=sum(val);
        end
    end
end
end