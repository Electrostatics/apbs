README for pka-lig APBS examples
================================

The example input files in this directory calculate the binding energes of a ligand to protein kinase A.

This example was contributed by Chung Wong.

Input File|Description|APBS Version|Results (kJ/mol)|UHBD (kJ/mol)
---|---|---|---|---
[apbs-mol-vdw.in](apbs-mol-vdw.in)|2-level focusing to 0.250 A spacing, VdW surface, srfm mol|**1.5**|**8.08352**|8.876
|||1.4.2|8.08352
|||1.4.1|8.0835
|||1.4|8.0835
|||1.3|8.0835
|||1.2.1|8.0835
|||1.2|8.0835<sup>[4](#4)</sup>
|||1.1.0|8.0858
|||1.0.0|8.0858
|||0.5.1|8.0858<sup>[3](#3)</sup>
|||0.5.0|8.0640
||||0.4.0|8.0640
[apbs-smol-vdw.in](apbs-smol-vdw.in)|2-level focusing to 0.250 A spacing, VdW surface, srfm smol|**1.5**|**20.9630**|8.876
|||1.4.2|20.9630
|||1.4.1|20.9630
|||1.4|20.9630
|||1.3|20.9630
|||1.2.1|20.9630
|||1.2|20.9630<sup>[4](#4)</sup>
|||1.1.0|20.9628
|||1.0.0|20.9628
|||0.5.1|20.9628<sup>[3](#2)</sup>
|||0.5.0|20.9542
|||0.4.0|20.9542<sup>[2](#2)</sup>
|||0.3.2|8.0640<sup>[1](#1)</sup>
|||0.3.1|6.6465
|||0.3.0|6.6465
|||0.2.6|6.6465
|||0.2.5|6.6465
|||0.2.4|6.6465
|||0.2.3|6.6465
|||0.2.2|6.6465
|||0.2.1|6.647
|||0.2.0|6.647
|||0.1.8|6.65
[apbs-mol-surf.in](apbs-mol-surf.in)|2-level focusing to 0.250 A spacing, molecular surface, srfm mol|**1.5**|**119.2610**|86.50
|||1.4.2|119.2610
|||1.4.1|119.2608
|||1.4|119.2608
|||1.3|119.2608
|||1.2.1|119.2608
|||1.2|119.2608<sup>[4](#4)</sup>
|||1.1.0|119.2607
|||1.0.0|119.2607
|||0.5.1|119.2607<sup>[3](#3)</sup>
|||0.5.0|119.2347
|||0.4.0|119.2347
[apbs-smol-surf.in](apbs-smol-surf.in)|2-level focusing to 0.250 A spacing, molecular surface, srfm smol|**1.5**|**108.8770**|86.50
|||1.4.2|108.8770
|||1.4.1|108.8773
|||1.4|108.8773<sup>[5](#5)</sup>
|||1.3|108.8748
|||1.2.1|108.8748
|||1.2|108.8748<sup>[4](#4)</sup>
|||1.1.0|108.8773
|||1.0.0|108.8773
|||0.5.1|108.8773<sup>[3](#3)</sup>
|||0.5.0|108.8540
|||0.4.0|108.8540<sup>[2](#2)</sup>
|||0.3.2|94.8705<sup>[1](#1)</sup>
|||0.3.1|97.0147
|||0.3.0|97.0147
|||0.2.6|97.0147
|||0.2.5|97.0147
|||0.2.4|97.0147
|||0.2.3|97.0147
|||0.2.2|97.0147
|||0.2.1|97.015
|||0.2.0|97.015
|||0.1.8|97.01

<a name=1></a><sup>1</sup> The grid dimensions (dime) changed from 65\^3 to 97\^3 in the 0.3.2 release to give a finer mesh.

<a name=2></a><sup>2</sup> The discrepancy in values between versions 0.4.0 and 0.3.2 is most likely due to three factors:

-   A bug fix in Vacc\_molAcc which removed spurious regions of high internal dielectric values
-   A switch in the algorithm used to compute the dielectric smoothing for srfm smol.
-   The addition of the Vacc sphere density (sdens keyword) as a variable and a change in the default sdens value from 3.0 to 10.0

<a name=3></a><sup>3</sup> The discrepancy in values between versions 0.5.1 and 0.5.0 is most likely due to the following factor(s):

-   Bug fix regarding multipole behavior for neutral proteins

<a name=4></a><sup>4</sup> APBS 1.2 has switched the multigrid smoothing algorithm from standard Gauss-Seidel to Gauss-Seidel red/black in order to facilitate parallelization. This switch has caused small differences in individual calculation energies which, when combined to the final answer, create larger errors (up to 0.04%). These errors can be reduced by resetting the APBS error tolerance to 1e-9 or smaller values. For a more detailed explanation, please see the APBS FAQ, [here](http://www.poissonboltzmann.org/docs/apbs-faq/#sources error calculation).

<a name=5></a><sup>5</sup> The discrepancy in values between versions 1.3 and 1.4 is most likely due to the following factor(s):

-   Translation of contrib/pmgZ library from FORTRAN to C
-   Differences in numerical implementations between FORTRAN and C compilers result in small round-off discrepencies
-   Small margins due to these round-off discrepencies acumulate in the computations

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.


