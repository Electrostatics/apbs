README for Born APBS examples
=============================

This is the canonical electrostatics test case: Born ion. A non-polarizable ion with a single embedded point charge; has an analytical solution for the potential. We examine the solvation free energy as a function of ionic radius.

Please see apbs.in for details on the particular solvation energy calculations. Analytical results are given in pmf.dat.

This example was contributed by Nathan Baker.

Input File|Description|APBS Version|Results (kJ/mol)|Analytical (kJ/mol)
---|---|---|---|---
[apbs-mol-auto.in](apbs-mol-auto.in)|Sequential, 3 A sphere, 3-level focusing to 0.188 A, srfm mol|**1.4.1-binary**|**-229.7736**|-230.62
|||1.4|-229.7736<sup>[3](#3)</sup>
|||1.3|-229.7735
|||1.2.1|-229.7735
|||1.2|-229.7735
|||1.1.0|-229.7735
|||1.0.0|-229.7735
|||0.5.1|-229.7735
|||0.5.0|-229.7735
|||0.4.0|-229.7735<sup>[1](#1)</sup>
|||0.3.2|-229.7248
|||0.3.1|-229.7248
|||0.3.0|-229.7248
|||0.2.6|-229.7248
|||0.2.5|-229.7248
|||0.2.4|-227.1859
|||0.2.3|-227.1589
|||0.2.2|-227.186
|||0.2.1|-227.19
|||0.2.0|-227.19
|||0.1.8|-227.19
[apbs-smol-auto.in](apbs-smol-auto.in)|Sequential, 3 A sphere, 3-level focusing to 0.188 A, srfm smol|**1.4.1-binary**|**-229.0124**|-230.62
---|---|---|---|---
|||1.4|-229.0124
|||1.3|-229.0124
|||1.2.1|-229.0124
|||1.2|-229.0124<sup>[2](#2)</sup>
|||1.1.0|-229.0123
|||1.0.0|-229.0123
|||0.5.1|-229.0123
|||0.5.0|-229.0123
|||0.4.0|-229.0123
[apbs-mol-parallel.in](apbs-mol-parallel.in)|Parallel with 4 processors, 3 A sphere, focusing to 0.103 A, srfm mol|**1.4.1-binary**|**-230.4918<sup>[4](#4)</sup>**|-230.62
---|---|---|---|---
|||1.4|-230.4919<sup>[3](#3)</sup>
|||1.3|-230.4918
|||1.2.1|-230.4918
|||1.2|-230.4918<sup>[2](#2)</sup>
|||1.1.0|-230.4916
|||1.0.0|-230.4916
|||0.5.1|-230.4916
|||0.5.0|-230.4916
|||0.4.0|-230.4916
|||0.2.1|-230.77
[apbs-smol-parallel.in](apbs-smol-parallel.in)|Parallel with 4 processors, 3 A sphere, focusing to 0.103 A, srfm smol|**1.4.1-binary**|**-229.3871**|-230.62
---|---|---|---|---
|||1.4|-229.3871
|||1.3|-229.3871
|||1.2.1|-229.3871
|||1.2|-229.3871<sup>[2](#2)</sup>
|||1.1.0|-229.3872
|||1.0.0|-229.3872
|||0.5.1|-229.3872
|||0.5.0|-229.3872
|||0.4.0|-229.3872<sup>[1](#1)</sup>
|||0.3.2|-226.3529
|||0.3.1|-226.3529
|||0.3.0|-229.5849
|||0.2.6|-229.5849
|||0.2.5|-229.5849
|||0.2.4|-226.2276
|||0.2.3|-226.2276
|||0.2.2|-226.2276
|||0.2.0|-226.228
|||0.1.8|-226.23
[apbs-smol-parallel.in](apbs-mol-fem.in)|Finite Element Method, 3 A sphere, 3-level focusing to 0.188 A, srfm mol|**1.4.1-binary**|**-231.9550**|-230.62
[apbs-smol-parallel.in](apbs-smol-fem.in)|Finite Element Method, 3 A sphere, 3-level focusing to 0.188 A, srfm smol|**1.4.1-binary**|**-230.9760**|-230.62

<a name=1></a><sup>1</sup> The discrepancy in values between versions 0.4.0 and 0.3.2 is most likely due to three factors:

-   A bug fix in Vacc\_molAcc which removed spurious regions of high internal dielectric values
-   A switch in the algorithm used to compute the dielectric smoothing for srfm smol.
-   The addition of the Vacc sphere density (sdens keyword) as a variable and a change in the default sdens value from 3.0 to 10.0

<a name=2></a><sup>2</sup> APBS 1.2 has switched the multigrid smoothing algorithm from standard Gauss-Seidel to Gauss-Seidel red/black in order to facilitate parallelization. This switch has caused small differences in individual calculation energies which, when combined to the final answer, create larger errors (up to 0.04%). These errors can be reduced by resetting the APBS error tolerance to 1e-9 or smaller values. For a more detailed explanation, please see [here](http://is.gd/45AzN).

<a name=3></a><sup>3</sup> The discrepancy in values between versions 1.3 and 1.4 is most likely due to the following factor(s):

-   Translation of contrib/pmgZ library from FORTRAN to C
-   Differences in numerical implementations between FORTRAN and C compilers result in small round-off discrepencies
-   Small margins due to these round-off discrepencies acumulate in the computations

<a name=4></a><sup>4</sup> The discrepancy in the result between versions 1.4 and 1.4.1-binary is most likely due to a reporting error.

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.


