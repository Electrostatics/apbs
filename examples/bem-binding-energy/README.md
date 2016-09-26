README for TABI-PB (Boundary Element Method) Binding Energy Examples
====================================================================

The example input files included in this folder uses a boundary element approach called
TABI-PB to solve the PBE. BEMs have the characteristic that only the boundary of the 
domain has to be discretized. This is particularly useful for problems in which the data
of interest is at the boundary of the solution.

This directory contains three example .in files:
        1. 1d30.in
        2. 1d30_monomer1.in
        3. 1d30_monomer2.in

These files provide an example to demonstrate the calculation of binding energy on 1d30.
More details are available on the APBS website contributions section.

Input File| APBS Version| Result (kCal/mol)| Expected (kCal/mol)
---|---|---|---
[1d30.in](1d30.in)| **1.5**| **-5249.040**| -5249.030
[1d30_monomer1.in](1d30_monomer1.in)| **1.5**| **-6232.160**| -6232.150
[1d30_monomer2.in](1d30_monomer2.in)| **1.5**| **-182.1470**| -182.1471