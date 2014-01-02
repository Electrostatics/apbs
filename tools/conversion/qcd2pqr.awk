#!/usr/bin/env awk -f

# Converts QCD/QCARD files to PQR format. 

# Nathan Baker and Dave Sept

BEGIN{  i=0

    printf "Enter path to input QCD file: "
    getline reffile <"-"

    printf "Enter path to output PQR file: "
    getline outfile <"-"

    prevnum = 0
    anum = 0
    res = 0
    xshift = 0
    yshift = 0
    zshift = 0

    while(getline  <reffile >0){
        resnum = $2
        anum = anum + 1
        res  = $3
        atom = $4
        x = $5 + xshift
        y = $6 + yshift
        z = $7 + zshift
        charge = $8
        radius = $9

        if(length($3)!=4) aname = sprintf(" %s",atom)
        else aname = sprintf("%s",atom)

        printf("ATOM  %5d %-4s %s %5d    %8.3f%8.3f%8.3f %5.3f %5.3f\n",anum,aname,res,resnum,x,y,z,charge,radius) > outfile 
    }
}
