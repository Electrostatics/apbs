function data = data_parse(filename, dime)
% e.g. data_parse('pot8.dx', [65 65 65])

disp([ filename '...'])

file_id = fopen(filename, 'rt');
flag=[];
while isempty(flag)
    line=fgetl(file_id);
    flag=strfind(line, 'data follows');
end

data_column=99999*zeros(1,prod(dime));  % 99999 is a flag value.  All values should be replaced.
index=1; % next empty memory location in data_column
for counter=1:ceil(prod(dime)/3)
    line=fgetl(file_id);
    [A, count, errmsg, nextindex]=sscanf(line, '%e');
    data_column(index:(index+length(A)-1))=A;
    index=index+length(A);
end
fclose(file_id);

%% reshape data column into a 3D array
data=permute(reshape(data_column, dime(3), dime(2), dime(1)), [3  2 1]);