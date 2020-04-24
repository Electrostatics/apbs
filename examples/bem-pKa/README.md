README for TABI-PB (Boundary Element Method) pKa Examples
=========================================================

The example input files included in this folder uses a boundary element approach called
TABI-PB to solve the PBE. BEMs have the characteristic that only the boundary of the
domain has to be discretized. This is particularly useful for problems in which the data
of interest is at the boundary of the solution.

This directory contains five example .in files:
        1. 2LZT-ASH66.in
        2. 2LZT-ASP66.in
        3. 2LZT-noASH66.in
        4. 2LZT-noASP66.in
        5. ASH66.in
        6. ASP66.in

These files can be used to demonstrate TABI-PB pKa calculations, documented at:
http://www.poissonboltzmann.org/examples/Lysozyme_pKa_example/

File Input| APBS Version| Result (kJ/mol)| Expected (kJ/mol)
---|---|---|---
[ASH66.in](ASH66.in)| **3.0**| **-4.165**| **-4.165**
| 1.5| -4.165|
[2LTZ-ASH66.in](2LTZ-ASH66.in)| **3.0**| **-360.665**| **-360.665**
| 1.5| -360.665|
[2LTZ-noASH66.in](2LTZ-noASH66.in)| **3.0**| **-359.870**| **-359.870**
| 1.5| -359.870|
