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

Input File| APBS Version| Result (kJ/mol)| Expected (kJ/mol)
---|---|---|---
[1d30.in](1d30.in)| **3.0**| **-22113.098**| **-22113.098**
[1d30_monomer1.in](1d30_monomer1.in)| **3.0**| **-26225.275**| **-26225.275**
[1d30_monomer2.in](1d30_monomer2.in)| **3.0**| **-779.948**| **-779.948**
