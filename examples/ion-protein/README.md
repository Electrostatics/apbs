README for ion-protein APBS examples
====================================

The example input files in this directory calculate the energy of placing an ion near a macromolecule.

This example was contributed by Dave Sept.

Input File|Description|APBS Version|Results (kJ/mol)|UHBD (kJ/mol)
---|---|---|---|---|
[apbs-mol-pdiel2.in](apbs-mol-pdiel2.in)|0.53 A resolution, pdie 2, srfm mol|**1.4.2-binary**|**15.5916**|23.58
|||1.4.1|15.5916
|||1.4|15.5916
|||1.3|15.5916<sup>[3](#3)</sup>
|||1.2.1|15.5819
|||1.2|15.5819
|||1.1.0|15.5819<sup>[2](#2)</sup>
|||1.0.0|15.5916
|||0.5.1|15.5916
|||0.5.0|15.5916
|||0.4.0|15.5916
[apbs-smol-pdiel2.in](apbs-smol-pdiel2.in)|0.53 A resolution, pdie 2, srfm smol|**1.4.2-binary**|**23.5554**|23.58
|||1.4.1|23.5554
|||1.4|23.5554
|||1.3|23.5554<sup>[3](#3)</sup>
|||1.2.1|23.5458
|||1.2|23.5458
|||1.1.0|23.5458<sup>[2](#2)</sup>
|||1.0.0|23.5554
|||0.5.1|23.5554
|||0.5.0|23.5554
|||0.4.0|23.5554<sup>[1](#1)</sup>
|||0.3.2|21.4763
|||0.3.1|19.8794
|||0.3.0|19.8794
|||0.2.6|19.8794
|||0.2.5|19.8794
|||0.2.4|19.8794
|||0.2.3|19.8652
|||0.2.2|21.4530
|||0.2.1|21.453
|||0.2.0|21.453
|||0.1.8|21.45
[apbs-mol-pdiel12.in](apbs-mol-pdiel12.in)|0.53 A resolution, pdie 12, srfm mol|**1.4.2-binary**|**18.0272**|23.58
|||1.4.1|18.0272
|||1.4|18.0272
|||1.3|18.0272<sup>[3](#3)</sup>
|||1.2.1|18.0176
|||1.2|18.0176
|||1.1.0|18.0176<sup>[2](#2)</sup>
|||1.0.0|18.0272
|||0.5.1|18.0272
|||0.5.0|18.0272
|||0.4.0|18.0272
[apbs-smol-pdiel12.in](apbs-smol-pdiel12.in)|0.53 A resolution, pdie 12, srfm smol|**1.4.2-binary**|**19.2825**|23.58
|||1.4.1|19.2825
|||1.4|19.2825
|||1.3|19.2825<sup>[3](#3)</sup>
|||1.2.1|19.2728
|||1.2|19.2728
|||1.1.0|19.2728<sup>[2](#2)</sup>
|||1.0.0|19.2825
|||0.5.1|19.2825
|||0.5.0|19.2825
|||0.4.0|19.2825<sup>[1](#1)</sup>
|||0.3.2|21.4763
|||0.3.1|18.9205
|||0.3.0|17.4207
|||0.2.6|17.4207
|||0.2.5|17.4207
|||0.2.4|17.4207
|||0.2.3|17.4049
|||0.2.2|18.8953
|||0.2.1|18.895
|||0.2.0|18.895
|||0.1.8|18.90

<a name=1></a><sup>1</sup> The discrepancy in values between versions 0.4.0 and 0.3.2 is most likely due to three factors:

-   A bug fix in Vacc\_molAcc which removed spurious regions of high internal dielectric values
-   A switch in the algorithm used to compute the dielectric smoothing for srfm smol.
-   The addition of the Vacc sphere density (sdens keyword) as a variable and a change in the default sdens value from 3.0 to 10.0

<a name=2></a><sup>2</sup> The discrepancy in values between versions 1.0.0 and 1.1.0 is due to a bugfix in the implementation of the boundary conditions. This bug introduces a very small error (generally less than 1%) the calculated results. This error is most prominent when the molecule substantially overlaps the boundary (e.g., in the current example) and is often symptomatic of insufficiently-large problem domains.

<a name=3></a><sup>3</sup> The discrepancy in values between versions 1.2.1 and 1.3 is most likely due to the following factor(s):

-   Fixed a bug in Vpmg.c which causes zero potential values on boundaries in non-focusing calculations.

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.

