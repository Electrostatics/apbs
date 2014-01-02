%This routine writes out dx format files

poteror=zeros(prod(dime),1);
for i=1:dime(1)
    for j=1:dime(2)
        for k=1:dime(3)
            pepe=(i-1)*dime(3)*dime(2)+(j-1)*dime(3)+k;
            poteror(pepe)=dxformat(i,j,k);
        end
    end
end
last_line=mod(prod(dime),3);
num_line=(prod(dime)-last_line)/3;
potelast(1:prod(dime)-last_line)=poteror(1:prod(dime)-last_line);
dxpotential=reshape(potelast, num_line,3);
if last_line>=1
lastline(1:last_line)=poteror(prod(dime)-last_line+1:prod(dime));
end
fid = fopen(outputfile, 'wt');
fprintf(fid,'# Data from APBS 1.0.0\n');
fprintf(fid,'#\n');
fprintf(fid,'# ');
fprintf(fid,'%s\n',namefile);
fprintf(fid,'#\n');
fprintf(fid,'object 1 class gridpositions counts ');
fprintf(fid,'%g %g %g\n',dime);
fprintf(fid,'origin ');
fprintf(fid,' %e %e %e\n',-glen/2);
fprintf(fid,'delta ');
fprintf(fid,'%e', h(1));
fprintf(fid,' 0.000000e+00 0.000000e+00\n');
fprintf(fid,'delta 0.000000e+00 ');
fprintf(fid,'%e', h(2));
fprintf(fid,' 0.000000e+00\n');
fprintf(fid,'delta 0.000000e+00 0.000000e+00 ');
fprintf(fid,'%e\n', h(3));
fprintf(fid,'object 2 class gridconnections counts ');
fprintf(fid,'%g %g %g\n', dime);
fprintf(fid,'object 3 class array type double rank 0 items ' );
fprintf(fid,'%g', prod(dime));
fprintf(fid,' data follows\n');
fprintf(fid, '%e %e %e\n', dxpotential);
if last_line==1
    fprintf(fid, '%e\n', lastline);
end
if last_line==2
    fprintf(fid, '%e %e\n', lastline);
end
fprintf(fid,'attribute "dep" string "positions"\n');
fprintf(fid,'object "regular positions regular connections" class field\n');
fprintf(fid,'component "positions" value 1\n');
fprintf(fid,'component "connections" value 2\n');
fprintf(fid,'component "data" value 3\n');
fclose(fid);