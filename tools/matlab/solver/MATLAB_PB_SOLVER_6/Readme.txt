Version number 6

This version includes energetic calculation for linear PB equation. 

I added a line in the inm. input file. To request the evaluation of the energy (in KJ/mol) you have to type "calceneryes". Otherwise, any other word would work. For instances, type "calcenerno" if you don't want to evaluate the energy.

The energy is calculated by the following expression

E=0.5*u(r1,..,rn)*KT*NA*10^-10

in which u(r1,..rn)=ec*phi(r1,..,rn)/KT is the (undimensional) solution of the linear PB eq evaluated at the exact location r1,..,rn of the pointlike charges using linear interpolation. KT is the thermal energy in erg and NA*10^-10 is the Avogadro number times 10^-10 needed to convert erg to KJ/mol. 

I made some changes in the MAPBS corresponding to this new calculation. I added a new matlab file named "energy.m" which evaluates the energy as a function of the electrostatic potential at the exact location of the point like charges. This is performed by using linear interpolation. I also made minor changes in other matlab files.


This version was successfully tested on the point-pmf example provided by the APBS. 

Note that in this case the center of grid is explicitly provided by the user in the apbs calculation. In our case, we always have to include the name of one pqr file from where our algorithm evaluates the center of grid. If you want to specify the coordinates of the center of grid, namely [centx centy centz] instead of calculating from the location of a set of atoms in the system, you should write a qpr file as follows

ATOM      1  I   ION     1       centx   centy  centz  1.00  1.00

In our example, please look at the molex.pqr file. 
