README for Actin-Dimer APBS examples
====================================

The example input files in this directory calculate binding energies for actin dimers. This is an example of a large biomolecule binding energy calculation that often requires parallel focusing.

This example was contributed by Dave Sept.

Input File                          | Description | APBS Version | Results (kJ/mol) | UHBD (kJ/mol)
------------------------------------|-------------|--------------|------------------|--------------
[apbs-mol-auto.in](apbs-mol-auto.in)| Sequential, 2-level focusing to ≤ 0.725 A, NPBE, srfm mol| **1.4.1-binary** | **104.8683** | 106.7 (1.00 A res., NPBE)
| | | 1.4   |104.8683
| | |1.3 | 104.8683<sup>[8](#8)</sup>
| | |1.2.1 | 104.867
| | |1.2 |104.867
| | |1.1.0 |104.867<sup>[5](#5)</sup>
| | |1.0.0 |104.868
| | |0.5.1 |104.868<sup>[3](#3)</sup>
| | |0.5.0 | 105.0338<sup>[2](#2)</sup>
| | |0.4.0 |104.8895
[apbs-smol-auto.in](apbs-smol-auto.in) | Sequential, 2-level focusing to ≤ 0.725 A, NPBE, srfm smol | **1.4.1-binary** | **109.5841** | 106.7 (1.00 A res., NPBE)
| | | 1.4 |109.5841
| | |1.3 |109.5841<sup>[8](#8)</sup>
| | |1.2.1 |109.5829
| | |1.2 |109.5829
| | |1.1.0 |109.5829<sup>[5](#5)</sup>
| | |1.0.0 |109.5841
| | |0.5.1 |109.5841<sup>[3](#3)</sup>
| | |0.5.0 |109.7518<sup>[2](#2)</sup>
| | |0.4.0 |109.6043<sup>[1](#1)</sup>
| | |0.3.2 |90.8704
| | |0.3.1 |88.6101
| | |0.3.0 |88.6101
| | |0.2.6 |88.6101
| | |0.2.5 |88.6101
| | |0.2.4 |88.6101
| | |0.2.3 |88.6064
| | |0.2.2 |90.829
| | |0.2.1 |90.829
| | |0.2.0 |90.829
| | |0.1.8 |90.84
| [apbs-mol-parallel.in](apbs-mol-parallel.in) |Parallel with 8 processors, focusing to \~0.9 A, LPBE, srfm mol |1.4.1-binary|**98.1746**|106.7 (1.00 A res., NPBE)
|||1.4|98.1746
|||1.3|98.1746<sup>[8](#8)</sup>
|||1.2.1|98.1733<sup>[7](#7)</sup>
|||1.2|98.1635<sup>[6](#6)</sup>
|||1.1.0|98.1630<sup>[5](#5)</sup>
|||1.0.0|98.1643<sup>[4](#4)</sup>
|||0.5.1|98.1654<sup>[3](#3)</sup>
|||0.5.0|98.3530<sup>[2](#2)</sup>
|||0.4.0|98.1834
[apbs-smol-parallel.in](apbs-smol-parallel.in)|Parallel with 8 processors, focusing to \~0.9 A, LPBE, srfm smol|**1.4.1-binary**|115.5421|106.7 (1.00 A res., NPBE)
|||1.4|115.5421<sup>[9](#9)</sup>
|||1.3|115.5422<sup>[8](#8)</sup>
|||1.2.1|115.5409<sup>[7](#7)</sup>
|||1.2|115.5563<sup>[6](#6)</sup>
|||1.1.0|115.5560<sup>[5](#5)</sup>
|||1.0.0|115.5573<sup>[4](#4)</sup>
|||0.5.1|115.5584<sup>[3](#3)</sup>
|||0.5.0|115.7492<sup>[2](#2)</sup>
|||0.4.0|115.5751<sup>[1](#1)</sup>
|||0.3.2|87.1121
|||0.3.1|87.1121
|||0.3.0|90.2573
|||0.2.6|90.2573
|||0.2.5|90.2573
|||0.2.4|90.2573
|||0.2.3|90.2543
|||0.2.2|91.9450
|||0.2.1|91.945
|||0.2.0|91.939
|||0.1.8|91.67

<a name=1></a><sup>1</sup> The discrepancy in values between versions 0.4.0 and 0.3.2 is most likely due to three factors:

-   A bug fix in Vacc\_molAcc which removed spurious regions of high internal dielectric values
-   A switch in the algorithm used to compute the dielectric smoothing for srfm smol.
-   The addition of the Vacc sphere density (sdens keyword) as a variable and a change in the default sdens value from 3.0 to 10.0

<a name=2></a><sup>2</sup> The discrepancy in values between versions 0.5.0 and 0.4.0 is most likely due to the following factor(s):

-   A change in the autofocusing routine for APBS

<a name=3></a><sup>3</sup> The discrepancy in values between versions 0.5.1 and 0.5.0 is most likely due to the following factor(s):

-   Bug fix regarding multipole behavior for neutral proteins

<a name=4></a><sup>4</sup> The discrepancy in values between versions 0.5.1 and 1.0.0 was due to the execution of the previous APBS tests on a PowerPC platform with the XLC/XLF compilers. Running with binaries compiled with gcc/gfortran or the Intel compilers gives identical results between versions 0.5.1 and 1.0.0.

<a name=5></a><sup>5</sup> The discrepancy in values between versions 1.0.0 and 1.1.0 is due to a bugfix in the implementation of the boundary conditions. This bug introduces a very small error (generally less than 1%) the calculated results.

<a name=6></a><sup>6</sup> APBS 1.2 has switched the multigrid smoothing algorithm from standard Gauss-Seidel to Gauss-Seidel red/black in order to facilitate parallelization. This switch has caused small differences in individual calculation energies which, when combined to the final answer, create larger errors (up to 0.04%). These errors can be reduced by resetting the APBS error tolerance to 1e-9 or smaller values. For a more detailed explanation, please see [here](http://is.gd/45AzN).

<a name=7></a><sup>7</sup> The discrepancy in values between versions 1.2 and 1.2.1 is most likely due to the following factor(s):

-   Fixed a bug in Vpmg\_fillcoCoefMolIon which causes npbe based calculations to return very large energies

<a name=8></a><sup>8</sup> The discrepancy in values between versions 1.2.1 and 1.3 is most likely due to the following factor(s):

-   Fixed a bug in Vpmg.c which causes zero potential values on boundaries in non-focusing calculations.

<a name=9></a><sup>9</sup> The discrepancy in values between versions 1.3 and 1.4 is most likely due to the following factor(s):

-   Translation of contrib/pmgZ library from FORTRAN to C
-   Differences in numerical implementations between FORTRAN and C compilers result in small round-off discrepencies
-   Small margins due to these round-off discrepencies acumulate in the computations

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.

