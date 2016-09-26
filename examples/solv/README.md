README for solv APBS examples
=============================

The example input files in this directory calculate the solvation energies of methanol and methoxide.

The source for this example is UHBD.


Input File                          | Description | APBS Version | Methanol Results (kJ/mol) | Methoxide Results (kJ/mol) |Difference (kJ/mol)| Methanol UHBD (kJ/mol)|Methoxide UHBD (kJ/mol)|Difference UHBD (kJ/mol)
---------------------------|---------|-------------|----------|----|-----------|------|----------|--------------
[apbs-mol.in](apbs-mol.in)| Focusing to 0.25 A, srfm mol| **1.5** | **-36.2486** | **-390.4120**|**-354.1640**|-35.595|-390.023|-354.424
| | |1.4.2 |-36.2486|-390.4120|-354.1640
| | |1.4.1 |-36.2486|-390.4122|-354.1635 
| | |1.4   |-36.2486|-390.4122<sup>[3](#3)</sup>|-354.1635
| | |1.3 | -36.2486| -390.4121| -354.1635
| | |1.2.1 | -36.2486| -390.4121| -354.1635
| | |1.2<sup>[2](#2)</sup> |-36.2486| -390.4121| -354.1635
| | |1.1.0 |-36.2486| -390.4119	| -354.1632
| | |1.0.0 |-36.2486| -390.4119	| -354.1632
| | |0.5.1 |-36.2486| -390.4119	| -354.1632
| | |0.5.0 |-36.2486| -390.4119	| -354.1632
| | |0.4.0 |-36.2486| -390.4119	| -354.1632
[apbs-smol.in](apbs-smol.in) | Focusing to 0.25 A, srfm smol | **1.5** | **-37.5759** | **-391.2390**| **-353.6630**| -35.595| -390.023| -354.424
| | |1.4.2|-37.5759|-391.2390|-353.6630
| | |1.4.1|-37.5759|-391.2389|-353.6629
| | |1.4 |-37.5759<sup>[3](#3)</sup>|-391.2389<sup>[3](#3)</sup>| -353.6629
| | |1.3 |-37.5760| -391.2388| -353.6629
| | |1.2.1 |-37.5760| -391.2388| -353.6629
| | |1.2<sup>[2](#2)</sup> |-37.5760| -391.2388| -353.6629
| | |1.1.0 |-37.5760| -391.2388| -353.6627
| | |1.0.0 |-37.5760| -391.2388| -353.6627
| | |0.5.1 |-37.5760| -391.2388| -353.6627
| | |0.5.0 |-37.5760| -391.2388| -353.6627
| | |0.4.0<sup>[1](#1)</sup> |-37.5760| -391.2388| -353.6627
| | |0.3.2 |-36.2486| -390.4119| -354.1632
| | |0.3.1 |-36.2486| -390.4119| -354.1632
| | |0.3.0 |-36.2486| -390.4119| -354.1632
| | |0.2.6 |-36.2486| -390.4119| -354.1632
| | |0.2.5 |-36.2486| -390.4119| -354.1632
| | |0.2.4 |-36.2486| -390.4119| -354.1632
| | |0.2.3 |-36.2223| -391.7995| -355.5771
| | |0.2.2 |-36.2223| -391.7995| -355.5771
| | |0.2.1 |-36.222| -391.800| -355.577
| | |0.2.0 |-36.222| -391.800| -355.577
| | |0.1.8 |-36.222| -391.800| -355.577




<a name=1></a><sup>1</sup> The discrepancy in values between versions 0.4.0 and 0.3.2 is most likely due to three factors:
A bug fix in Vacc_molAcc which removed spurious regions of high internal dielectric values
A switch in the algorithm used to compute the dielectric smoothing for srfm smol
The addition of the Vacc sphere density (sdens keyword) as a variable and a change in the default sdens value from 3.0 to 10.0

<a name=2></a><sup>2</sup> APBS 1.2 has switched the multigrid smoothing algorithm from standard Gauss-Seidel to Gauss-Seidel red/black in order to facilitate parallelization. This switch has caused small differences in individual calculation energies which, when combined to the final answer, create larger errors (up to 0.04%). These errors can be reduced by resetting the APBS error tolerance to 1e-9 or smaller values. For a more detailed explanation, please see here.

<a name=3></a><sup>3</sup> The discrepancy in values between versions 1.3 and 1.4 is most likely due to the following factor(s):

Translation of contrib/pmgZ library from FORTRAN to C
Differences in numerical implementations between FORTRAN and C compilers result in small round-off discrepencies
Small margins due to these round-off discrepencies acumulate in the computations

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.