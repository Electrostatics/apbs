clear
clc

%% This is a testing code for our MATLAB PB solver

% It basically evaluates the residual AND absolute
% error between both the APBS and MATLAB electrostatic potential solutions. 
% FOR VISUAL PORPUSE, IT ALSO GENERATES THE CORRESPONDING TWO PLOTS, ONE  
% TO THE ABSOLUTE ERROR BETWEEN APBS AND MATLAB SOLUTIONS, AND THE OTHER
% FOR THE APBS AND THE MATLAB SOLUTION FOR THE ELECTROSTATIC POTENTIAL.
% IT ALSO write out THE ABSOLUTE ERROR DX FILE. 

% WARNING!!!!!!!!!!!!!!! BEFORE USING IT: 

% PLEASE, ADD THE PATH OF THE CORRESPONDING TWO DX FILES FOR THE ELECTROSTATIC 
% POTNETIAL SOLTIONS TO THE MATLAB SEARCH PATH
% AS IT IS DONE IN THE EXAMPLE PROVIDED IN THE NEXT TWO LINES 

  MYPATH='C:\Users\Marce\Documents\temp\MATLAB_PB_SOLVER_5\Potential';
  addpath(MYPATH)

% COMMENT: NO MORE THAN THESE TWO DX FILES MUST BE IN SUCH DIRECTORY.

% YOU ARE DONE. NOW YOU ARE READY TO USE THIS CODE!!!!!! THANKS !!!!
disp('Welcome!!!!!!....')
disp(' ')
disp('This code will calculate the absolute error for the elect potential between')
disp('MATLAB and APBS solutions of the PB equation....')
disp(' ')

% CALLED MATLAB FILES: 
% data_parse.m (read dx files and convert them to data arrays)
% dx_export.m (convert data arrays to dx format)
% gridinf.m (read number of grid points, center of the grid and mesh size from input file)

%% Part 1.  Read the data

files=dir(fullfile(MYPATH,'*.dx'));

[rmin,dime,h]=gridinf(files(1).name);
disp('Reading the input files....')

MATLAB_pot=data_parse(files(1).name,dime); % data_parse loads the file, and displays the reading ... message
APBS_pot=data_parse(files(2).name,dime); 

disp('Done!....')

%% Part 3: Calculating the absolute error between the APBS and MATLAB solutions
diffe=APBS_pot-MATLAB_pot;
%
[me,ne,pe] = size(MATLAB_pot);
if me~=dime(1)|ne~=dime(2)|pe~=dime(3)
    disp('mismatching dimensions')
    return
end
[me,ne,pe] = size(APBS_pot);
if me~=dime(1)|ne~=dime(2)|pe~=dime(3)
    disp('mismatching dimensions')
    return
end
aberror=zeros(prod(dime),1);
for i=1:dime(1)
    for j=1:dime(2)
        for k=1:dime(3)
             pe=(k-1)*(dime(1))*(dime(2))+(j-1)*(dime(1))+i;
             aberror(pe)=diffe(i,j,k);
        end
    end
end

absolute_error=norm(aberror,2)

average_error=absolute_error/numel(aberror)

% calculating the absoute error at each point on the grid

aberror2=abs(diffe);

dire='COMPARATIVE_ANALYSIS';
mkdir(dire)

%% Part 4: Write out the absolute error in dx format
dxformat=aberror2;
namefile='Absolute Error between MATLAB and APBS solutions';
outputfile=strcat(namefile,'.dx');
run dx_export
movefile (outputfile,dire)
%

%% Part 5: Generating the surface Plots

disp('generating plots!....')

disp('generating plots!....')

% potential surface
n=(dime(3)+1)/2;

% plotting the absolute error
%name1=strcat(files(1).name,'_and_',files(2).name,'_absolute_error');
name1='absolute_error';
plot1=surf(aberror2(:,:,n),'facecolor','interp');
saveas(plot1,name1,'fig');
movefile (strcat(name1,'.fig'),dire)
saveas(gcf,strcat(name1,'.tiff'),'tiffn');
movefile (strcat(name1,'.tiff'),dire)

figure

%plotting the electrostatic potential solutions
name2= strcat(files(1).name,'_and_',files(2).name);
subplot(1,2,1)
surf(MATLAB_pot(:,:,n),'facecolor','interp');
subplot(1,2,2)
surf(APBS_pot(:,:,n),'facecolor','interp');
saveas(gcf,strcat(name2,'.fig'))
movefile (strcat(name2,'.fig'),dire)
saveas(gcf,strcat(name2,'.tiff'),'tiffn');
movefile (strcat(name2,'.tiff'),dire)

close
disp('Done!....')

disp('Thanks for using our code!!!!....')
%end
