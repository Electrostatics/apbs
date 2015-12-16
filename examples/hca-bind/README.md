README for hca-bind APBS examples
=================================

The example input files in this directory calculate the binding of a small molecule (acetazolamide) to a medium-sized protein (human carbonic anhydrase).

The UHBD calculations where performed using a van der Waals surface definition for the dielectric coefficient. This is simulated in the APBS input files by setting srad to 0.0.

Input File|Description|APBS Version|Results (kJ/mol)|UHBD (kJ/mol)
---|---|---|---|---
[apbs-mol.in](apbs-mol.in)|2-level focusing to 0.225 A, VdW surface, srfm mol|**1.4.1-binary**|**-52.4648<sup>[6](#6)</sup>**|-70.00
|||1.4|-51.4648<sup>[5](#5)</sup>
|||1.3|-52.4647
|||1.2.1|-52.4647
|||1.2|-52.4647<sup>[4](#4)</sup>
|||1.1.0|-52.4669
|||1.0.0|-52.4669
|||0.5.1|-52.4669<sup>[3](#3)</sup>
|||0.5.0|-52.1062<sup>[2](#2)</sup>
|||0.4.0|-52.4414
[apbs-smol.in](apbs-smol.in)|2-level focusing to 0.225 A, VdW surface, srfm smol|**1.4.1-binary**|**-54.0598**|-70.00
|||1.3|-54.0598
|||1.2.1|-54.0598
|||1.2|-54.0598<sup>[4](#4)</sup>
|||1.1.0|-54.0587
|||1.0.0|-54.0587
|||0.5.1|-54.0587<sup>[3](#3)</sup>
|||0.5.0|-54.7039<sup>[2](#2)</sup>
|||0.4.0|-54.0393<sup>[1](#1)</sup>
|||0.3.2|-57.1192
|||0.3.1|-57.1192
|||0.3.0|-57.1192
|||0.2.6|-57.1192
|||0.2.5|-57.1192
|||0.2.4|-57.1192
|||0.2.3|-57.1123
|||0.2.2|-57.1123
|||0.2.1|-57.112
|||0.2.0|-57.711
|||0.1.8|-58.51

<a name=1></a><sup>1</sup> The discrepancy in values between versions 0.4.0 and 0.3.2 is most likely due to three factors:

-   A bug fix in Vacc\_molAcc which removed spurious regions of high internal dielectric values
-   A switch in the algorithm used to compute the dielectric smoothing for srfm smol
-   The addition of the Vacc sphere density (sdens keyword) as a variable and a change in the default sdens value from 3.0 to 10.0

<a name=2></a><sup>2</sup> The discrepancy in values between versions 0.5.0 and 0.4.0 is most likely due to the following factor(s):

-   A change in the autofocusing routine for APBS

<a name=3></a><sup>3</sup> The discrepancy in values between versions 0.5.1 and 0.5.0 is most likely due to the following factor(s):

-   Bug fix regarding multipole behavior for neutral proteins

<a name=4></a><sup>4</sup> APBS 1.2 has switched the multigrid smoothing algorithm from standard Gauss-Seidel to Gauss-Seidel red/black in order to facilitate parallelization. This switch has caused small differences in individual calculation energies which, when combined to the final answer, create larger errors (up to 0.04%). These errors can be reduced by resetting the APBS error tolerance to 1e-9 or smaller values. For a more detailed explanation, please see [here](http://is.gd/45AzN).

<a name=5></a><sup>5</sup> The discrepancy in values between versions 1.3 and 1.4 is most likely due to the following factor(s):

-   Translation of contrib/pmgZ library from FORTRAN to C
-   Differences in numerical implementations between FORTRAN and C compilers result in small round-off discrepencies
-   Small margins due to these round-off discrepencies acumulate in the computations

<a name=6></a><sup>6</sup> The discrepancy in the result between versions 1.4 and 1.4.1-binary is most likely due to a reporting error.

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.


