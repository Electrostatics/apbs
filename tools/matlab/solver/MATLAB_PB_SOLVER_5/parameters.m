% parameters

% define parameters
e_c = 4.8032424e-10;  %statcoulombs
k_B = 1.3806581e-16;  %erg K-1
N_A = 6.0221367e+23;
dielw=bulk(2);
diel=8.8541878e-12; %F m-1 or C^2 N-1 m-2
bulkIonicStrength=bulk(1);
pre1=e_c^2/(dielw*k_B*T)*1e+8;
xkappa = sqrt((bulkIonicStrength*1.0e-16*8*pi*N_A*e_c^2)/(1000*dielw*k_B*T));

zmagic=4*pi*e_c^2/(k_B*T)*10^8; % should be derived from unit conversions similar to zmagic in VPBE.c of APBS's source code

Squared_Debye_Huckel=25897036486.0*bulk(1)*bulk(2)/T;

