% This routine evaluates the (Dirichlet) boundary condition

% read the pqr data
PQRdata=importdata(pqr_str);

atomdata=PQRdata.data;
atomN=length(atomdata(:,1));
atomP=atomdata(:,2:4);
atomC=atomdata(:,5);
atomR=atomdata(:,6);

% define parameters
e_c = 4.8032424e-10;  %statcoulombs
k_B = 1.3806581e-16;  %erg K-1
N_A = 6.0221367e+23;
dielw=bulk(2);
diel=8.8541878e-12; %F m-1 or C^2 N-1 m-2
bulkIonicStrength=bulk(1);
pre1=e_c^2/(dielw*k_B*T)*1e+8;
xkappa = sqrt((bulkIonicStrength*1.0e-16*8*pi*N_A*e_c^2)/(1000*dielw*k_B*T));

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

%% Y Boundary

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

%% Z Boundary

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
