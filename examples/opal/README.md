README for Born APBS examples
=============================

*\$Id: README.html 1264 2008-04-16 16:41:31Z yhuang01 \$*

This is the canonical electrostatics test case: Born ion. A non-polarizable ion with a single embedded point charge; has an analytical solution for the potential. We examine the solvation free energy as a function of ionic radius.

The shell script ./runme.sh will generate ions of several radii and calculate the solvation energies (which will appear in OUTPUT-XXX, where XXX is the particular radius of interest). Please see apbs.in for details on the particular solvation energy calculations. Analytical results are given in pmf.dat. Note: You will need to ensure that the 'apbs' binary is in your path prior to running ./runme.sh. You may also need to edit the path to your Bourne-like shell in the ./runme.sh script.

This example was contributed by Nathan Baker.

Input File|Description|APBS Version|Results (kJ/mol)|Analytical (kJ/mol)
-|-|-|-|-
[apbs-mol-auto.in](apbs-mol-auto.in)|Sequential, 3 A sphere, 3-level focusing to 0.188 A, srfm mol|**1.0.0**|**-229.7735**|-228.57
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
[apbs-smol-auto.in](apbs-smol-auto.in)|Sequential, 3 A sphere, 3-level focusing to 0.188 A, srfm smol|**1.0.0**|**-229.0123**|-228.57
|||0.5.1|-229.0123
|||0.5.0|-229.0123
|||0.4.0|-229.0123
[apbs-mol-parallel.in](apbs-mol-parallel.in)|Parallel with 4 processors, 3 A sphere, focusing to 0.103 A, srfm mol|**1.0.0**|**-230.4916**|-228.57
|||0.5.1|-230.4916
|||0.5.0|-230.4916
|||0.4.0|-230.4916
|||0.2.1|-230.77
[apbs-smol-parallel.in](apbs-smol-parallel.in)|Parallel with 4 processors, 3 A sphere, focusing to 0.103 A, srfm smol|**1.0.0**|**-229.3872**|-228.57
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

<a name=1></a><sup>1</sup> The discrepancy in values between versions 0.4.0 and 0.3.2 is most likely due to three factors:

-   A bug fix in Vacc\_molAcc which removed spurious regions of high internal dielectric values
-   A switch in the algorithm used to compute the dielectric smoothing for srfm smol.
-   The addition of the Vacc sphere density (sdens keyword) as a variable and a change in the default sdens value from 3.0 to 10.0

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.


