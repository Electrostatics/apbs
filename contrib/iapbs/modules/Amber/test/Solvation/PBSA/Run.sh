#!/bin/sh


$AMBERHOME/exe/sander -O -i solvation-pbsa.in \
    -c 2ala.prmcrd -p 2ala.prmtop -o solvation-pbsa.out

