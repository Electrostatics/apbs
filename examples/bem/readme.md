README for TABI-PB (Boundary Element Method)
============================================

The example input files included in this folder uses a boundary element approach called
TABI-PB to solve the PBE. BEMs have the characteristic that only the boundary of the 
domain has to be discretized. This is particularly useful for problems in which the data
of interest is at the boundary of the solution.

This directory contains five example .in files.

Three examples calculate surface potentials for 1a63 in a 0.15 M salt solution:
        1. 1a63_msms.in uses MSMS to create a solvent excluded surface (SES) triangulation.
        2. 1a63_NanoShaper_SES.in uses NanoShaper to create an SES triangulation.
        3. 1a63_NanoShaper_Skin.in uses NanoShaper to create a Skin surface triangulation.

Two examples calculate surface potentials for 451c in a 0.15 M salt solution:
        1. 451c_order1.in uses a 1st order Taylor series expansion for the treecode.
        2. 451c_order5.in uses a 5th order Taylor series expansion for the treecode.

binding_energy is an example to calculate binding energy on 1d30.

pKa is an example to use TABI-PB to calculate pKa as http://www.poissonboltzmann.org/examples/Lysozyme_pKa_example/.

More details are on apbs website contributions.

Additionally, more pqr files are available in the test_proteins directory.
