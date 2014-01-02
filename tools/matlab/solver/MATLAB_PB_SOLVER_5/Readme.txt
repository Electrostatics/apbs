Version number 5

I made sigiificant changes with respect to the previous version.

NOw we are able to use focus boundary condition. Dirichlet condition where the potential at the boundary is set to the values computed by the previous (usually lower-resolution) PB calculation. 
I added a new line in the inm file for boundary conditions. If it is set equal to "sdh" our code will use Single Debye-Hückel" boundary condition and it will work on the target grid.
If it is set equal to "focusname.inm" our code will use a new matlab file "fbc.m" to evalaute the boundary condition. It use linear interpolation to calculate the dirichlet bc at the faces od the new boxside from the elect pot solution obtained in the coarse grid.

In this case, the user has to provide a second inm file named "focusname.inm" with the same structure than the one for the other inm file. This file will contain the the set of parameters to solve the PB in the coarse grid. It should define a larger domain with probably few grid points. 

Netx, I added a line by which the user have to specify the number od digits of presicion to be reached in the solution for the elect pot (residual error value for the conjugated gradient solver). 

I also added other line in the inm file structure. Now user has to provide two pqr filenames. The first one is to define the molecule that I will solve the PB equation for. The second one is to calcualte the center of the grid. This is needed to calcualte the elec pot in complex systems. 

I finally added two more lines in the inm file struccture. These last two lines correspond to the full path for the input and output files respectively. 

For instances in a pc we have

'c:\Users\Marce\Matlab_work_space\Input_Files'
'c:\Users\Marce\Matlab_work_space\pka_model'

Our code also works in unix format. I mean you can also use fro instances

'/Users/Marce/Matlab_work_space/Input_Files'
'/Users/Marce/Matlab_work_space/pka_model'

(The only worry in both cases is about avoiding to add the slash symbol at the end of such path). If the output directory doesn't exist, a warning message will appear in the command window notifying this and our code will create it.

As a result, now the code will create two folders, one containing the results for the coarse grain calculation (main domain) and the other for the fine (target) grid (subdomain at higher resolution). Note: you don't have to run twice the code (one for the coarse and the other for the fine grid). It will do it automatically. 

Of course, if you don't use focus boundary condition but sdh, you will obtain the same results than the previous version. I mean, you only have to provide one inm file and the code will create just one folder. The only difference is that now you always have to provide two pqr filenames and specify "sdh" in the line corresponding to the boundary condition set up. 

I included two other matlab files. "parameters.m" contains the definition of all parameters invovled with the calculations. "centerofgrid.m" evaluates the center of the grid por an specific pqr file.

I made many other changes. For instances, this version also generate the file "MATLAB_screen.io" containing all the information that is it printed on teh screen during the calculation. It is saved in the same folder created to save the other results.

In This version the user don't have to edit source files. Now the main matlab file is not an script but a function with only one argument  corresponding to the name of the inm. inputfile including the corresponding fullpath.

For instances in a pc we have 

'c:\Users\Marce\Matlab_work_space\pka.inm'.

In such case, the user only have to go to the directory where are source files are located if it is notthe starting one and type on the matlab command window

>>MAPBS('c:\Users\Marce\Matlab_work_space\pka.inm')

and then press enter.

I made many other minor changes in almost all the matlab files in our code. 

I included plenty of comments in our source files to clarify the algorithm. 

I tested this version on the pka example provided by the APBS.

