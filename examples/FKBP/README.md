README for FKBP APBS examples
=============================

The example input files in this directory simulate the binding of various (small) ligands to FKBP. Analogous to HCA binding case (except it works).

In order to calculate solvation energy upon binding you will need to take the results from these input files and subtract from them the results obtained from the `coulomb` utility found at `apbs/tools/manip/coulomb`. The values returned from this utility are:

-   1d7h-dmso: -15.0930 kJ/mol (analytical value -15.103 kJ/mol)
-   1d7i-dss: -11.9670 kJ/mol (analytical value -11.975 kJ/mol)

This example was contributed by Jung-Hsin Lin.

Input File|Description|APBS Version|Results (kJ/mol)|UHBD (kJ/mol)
---|---|---|---|---
[1d7h-dmso/apbs-mol.in](1d7h-dmso/apbs-mol.in)|1d7h-dmso, 2-level focusing to 0.225 A, VdW surface, srfm mol|**1.5**|**15.0081**|19.097
|||1.4.2|15.0081
|||1.4.1|15.0081
|||1.4|15.0081<sup>[4](#4)</sup>
|||1.3|15.0077
|||1.2.1|15.0077<sup>[3](#3)</sup>
|||1.2|15.0087<sup>[2](#2)</sup>
|||1.1.0|15.0089
|||1.0.0|15.0089
|||0.5.1|15.0089
|||0.5.0|15.0089
|||0.4.0|15.0089
[1d7h-dmso/apbs-smol.in](1d7h-dmso/apbs-smol.in)|1d7h-dmso, 2-level focusing to 0.225 A, VdW surface, srfm smol|**1.5**|**16.2445**|19.097
|||1.4.2|16.2445
|||1.4.1|16.2445
|||1.4|16.2445<sup>[4](#4)</sup>
|||1.3|16.2446
|||1.2.1|16.2446<sup>[3](#3)</sup>
|||1.2|16.2456<sup>[2](#2)</sup>
|||1.1.0|16.2458
|||1.0.0|16.2458
|||0.5.1|16.2458
|||0.5.0|16.2458
|||0.4.0|16.2458<sup>[1](#1)</sup>
|||0.3.2|15.0089
|||0.3.1|15.0089
|||0.3.0|15.0089
|||0.2.6|15.0089
|||0.2.5|15.0089
|||0.2.4|15.0089
|||0.2.3|15.0097
|||0.2.2|14.5886
|||0.2.1|14.589
|||0.2.0|14.589
|||0.1.8|14.591
[1d7i-dss/apbs-mol.in](1d7i-dss/apbs-mol.in)|1d7i-dss, 2-level focusing to 0.225 A, VdW surface, srfm mol|**1.5**|**14.4250**|16.231
|||1.4.2|14.4250
|||1.4.1|14.4250
|||1.4|14.4250
|||1.3|14.4250
|||1.2.1|14.4250<sup>[3](#3)</sup>
|||1.2|14.4253<sup>[2](#2)</sup>
|||1.1.0|14.4254
|||1.0.0|14.4254
|||0.5.1|14.4254
|||0.5.0|14.4254
|||0.4.0|14.4254
[1d7i-dss/apbs-smol.in](1d7i-dss/apbs-smol.in)|1d7i-dss, 2-level focusing to 0.225 A, VdW surface, srfm smol|**1.5**|**15.45150**|16.231
|||1.4.2|15.4515
|||1.4.1|15.4515
|||1.4|15.4515
|||1.3|15.4515
|||1.2.1|15.4515<sup>[3](#3)</sup>
|||1.2|15.4517
|||1.1.0|15.4517
|||1.0.0|15.4517
|||0.5.1|15.4517
|||0.5.0|15.4517
|||0.4.0|15.4517<sup>[1](#1)</sup>
|||0.3.2|14.4254
|||0.3.1|14.4254
|||0.3.0|14.4254
|||0.2.6|14.4254
|||0.2.5|14.4254
|||0.2.4|14.4254
|||0.2.3|14.4254
|||0.2.2|14.3865
|||0.2.1|14.387
|||0.2.0|14.387
|||0.1.8|15.210

<a name=1></a><sup>1</sup> The discrepancy in values between versions 0.4.0 and 0.3.2 is most likely due to three factors:

-   A bug fix in Vacc\_molAcc which removed spurious regions of high internal dielectric values
-   A switch in the algorithm used to compute the dielectric smoothing for srfm smol
-   The addition of the Vacc sphere density (sdens keyword) as a variable and a change in the default sdens value from 3.0 to 10.0

<a name=2></a><sup>2</sup> APBS 1.2 has switched the multigrid smoothing algorithm from standard Gauss-Seidel to Gauss-Seidel red/black in order to facilitate parallelization. This switch has caused small differences in individual calculation energies which, when combined to the final answer, create larger errors (up to 0.04%). These errors can be reduced by resetting the APBS error tolerance to 1e-9 or smaller values. For a more detailed explanation, please see the APBS FAQ, [here](http://www.poissonboltzmann.org/docs/apbs-faq/#sources error calculation).

<a name=3></a><sup>3</sup> The discrepancy in values between versions 1.2 and 1.2.1 is most likely due to the following factor(s):

-   Fixed a bug in Vpmg\_fillcoCoefMolIon which causes npbe based calculations to return very large energies

<a name=4></a><sup>4</sup> The discrepancy in values between versions 1.3 and 1.4 is most likely due to the following factor(s):

-   Translation of contrib/pmgZ library from FORTRAN to C
-   Differences in numerical implementations between FORTRAN and C compilers result in small round-off discrepencies
-   Small margins due to these round-off discrepencies acumulate in the computations

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.


