function [rmin,dime,h] = gridinfo(filename)

%disp([ filename '...'])

file_id = fopen(filename, 'rt');
flag=[];
while isempty(flag)
    line=fgetl(file_id);
    flag=strfind(line, 'object 1');
end
line=fgetl(file_id);
rmin=sscanf(line,'%*s %e %e %e', [3, inf]);
rmin=rmin';
line=fgetl(file_id);
h(1)=sscanf(line,'%*s %e %*e %*e');
line=fgetl(file_id);
h(2)=sscanf(line,'%*s %*e %e %*e');
line=fgetl(file_id);
h(3)=sscanf(line,'%*s %*e %*e %e');
line=fgetl(file_id);
dime=sscanf(line,'%*s %*s %*s %*s %*s %e %e %e', [3, inf]);
dime=dime';
fclose(file_id);