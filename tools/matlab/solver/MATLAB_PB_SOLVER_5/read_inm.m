function [dime, glen, T, bulk, bc,digpres, dielx_str, diely_str, dielz_str, kappa_str, pqr_str, pqr_cent_str, in_nam_str, out_nam_str] = read_inm(inputfile)

%addpath('data')
%addpath('C:/Users/Marce/Documents/MATLAB/Gradwohl_DFT/Gradwohl_Electrostatic_Solver/examples')

file_id = fopen(inputfile, 'rt'); % open the file

%% dime
line=fgetl(file_id);
[dime, count, errmsg, nextindex]=sscanf(line, '%e');
dime=dime';

%% glen
line=fgetl(file_id);
[glen, count, errmsg, nextindex]=sscanf(line, '%e');
glen=glen';

%% T
line=fgetl(file_id);
[T, count, errmsg, nextindex]=sscanf(line, '%e');

%% bulk properties
line=fgetl(file_id);
[bulk, count, errmsg, nextindex]=sscanf(line, '%e');
bulk=bulk';

%% bc
bc=fgetl(file_id);

%% significant digits of rpecision
line=fgetl(file_id);
[digpres, count, errmsg, nextindex]=sscanf(line, '%e');

%% dielx_str
dielx_str=fgetl(file_id);

%% diely_str
diely_str=fgetl(file_id);

%% dielz_str
dielz_str=fgetl(file_id);

%% kappa_str
kappa_str=fgetl(file_id);

%% pqr_str
pqr_str=fgetl(file_id);

%% pqr_cent_str
pqr_cent_str=fgetl(file_id);

%% input path nam_str
in_nam_str=fgetl(file_id);

%% output path nam_str
out_nam_str=fgetl(file_id);

fclose(file_id); % close the file