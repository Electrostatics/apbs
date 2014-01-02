Welcome!!!!!!....
 
This code will solve the PB equation
to obtain an approximate solution for the electrostatic potential
for the model defined in the following input file
C:\Users\Marce\Documents\temp\MATLAB_PB_SOLVER_4\Input_Files\ligand.inm
 
Coarse grained calculation
 
Reading the input files
coarse-ligand-dielx.dx...
coarse-ligand-diely.dx...
coarse-ligand-dielz.dx...
coarse-ligand-kappa.dx...

dime =

    97    97    97


glen =

    70    70    70

Done!....
 
Calculating boundary condition....
Done!....
 
Generating the charge map....
Done!....
 
Constructing the sparse matrix A....
Done!....
 
Performing the LU decomposition....
Done!....
 
Solving the linear equation system using the
Biconjugated gradient method stabilized by LU matrices

error =

  9.6391e-007


iteration_number =

  181.5000

Done!....
 

computing_time =

  224.9678

Warning: Directory already exists.
> In <a href="matlab: opentoline('C:\Users\Marce\Documents\temp\MATLAB_PB_SOLVER_4\MAPBS.m',302,1)">MAPBS at 302</a>
Converting to dx format...
the file coarse_grid_MATLAB_pot.dx was generated
the file coarse_grid_MATLAB_rho.dx was generated
Done!
 
Generating plots!....
the file coarse_grid_MATLAB_pot.fig was generated
the file coarse_grid_MATLAB_pot.jpg was generated
Done!....
 
Calculating
the electrostatic potential in the target grid....
 
reading the solution obtained previously in the coarse grid....
 
MATLAB_Solution.dx...
 
Reading the input files
ligand-dielx.dx...
ligand-diely.dx...
ligand-dielz.dx...
ligand-kappa.dx...

dime =

    97    97    97


glen =

    24    24    24

Done!....
 
Calculating boundary condition....
Done!....
 
Generating the charge map....
Done!....
 
Constructing the sparse matrix A....
Done!....
 
Performing the LU decomposition....
Done!....
 
Solving the linear equation system using the
Biconjugated gradient method stabilized by LU matrices

error =

  6.5759e-007


iteration_number =

   193

Done!....
 

computing_time =

  239.4675

Converting to dx format...
the file target_grid__MATLAB_pot.dx was generated
the file target_grid_MATLAB_rho.dx was generated
Done!
 
Generating plots!....
the file target_grid_MATLAB_pot.fig was generated
the file target_grid_MATLAB_pot.jpg was generated
Done!....
 
Thanks for using our PB solver!!!!....
