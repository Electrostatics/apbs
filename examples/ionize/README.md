README for ionize APBS examples
===============================

The example input files in this directory calculate electrostatic contributions to the ionization energy of acetic acid.

Input File|Description|APBS Version|Results (kJ/mol)||||UHBD (kJ/mol)||||
---|---|---|---|---|---|---|---|---|---|---
||||Acetic Acid|Acetate|Proton|Ionization Energy|Acetic Acid|Acetate|Proton|Ionization Energy
[apbs-mol.in](apbs-mol.in)|3-level focusing to 0.094 A, srfm mol|**1.5**|**-22.6788**|**-199.746**|**-297.46**|**-474.527**|-22.22|-198.04|-295.79|-471.61
|||1.4.2|-22.6788|-199.746|-297.46|-474.527
|||1.4.1|-22.6788|-199.7463|-297.4598|-474.5273
|||1.4|-22.6788|-199.7463|-297.4598|-474.5273
|||1.3|-22.6788|-199.7463|-297.4598|-474.5273
|||1.2.1|-22.6788|-199.7463|-297.4598|-474.5273
|||1.2<sup>[1](#1)</sup>|-22.6788|-199.7463|-297.4598|-474.5273
|||1.1.0|-22.6787|-199.7462|-297.4599|-474.5273
|||1.0.0|-22.6787|-199.7462|-297.4599|-474.5273
|||0.5.1|-22.6787|-199.7462|-297.4599|-474.5273
|||0.5.0|-22.6787|-199.7462|-297.4599|-474.5273
|||0.4.0|-22.6787|-199.7462|-297.4599|-474.5273
|||0.3.2|-22.6787|-199.7462|-297.4599|-474.5273
|||0.3.1|-22.6787|-199.7462|-297.4599|-474.5273
|||0.3.0|-22.6787|-199.7462|-297.4599|-474.5273
|||0.2.6|-22.6787|-199.7462|-297.4599|-474.5273
|||0.2.5|-22.6787|-199.7462|-297.4599|-474.5273
|||0.2.4|-22.6787|-199.7462|-297.4599|-474.5273
|||0.2.3|-22.629|-195.3135|-290.8751|-463.5617
|||0.2.2|-22.628|-195.3051|-290.8591|-463.5373
|||0.2.1|-22.627|-195.305|-290.859|-463.537
|||0.2.0|-22.627|-195.305|-290.859|-463.537
|||0.1.8|-22.63|-195.31|-290.86|-463.54
[apbs-smol.in](apbs-smol.in)|3-level focusing to 0.094 A, srfm smol|**1.5**|**-22.3305**|**-198.488**|**-295.967**|**-472.125**|-22.22|-198.04|-295.79|-471.61
|||1.4.2|-22.3305|-198.488|-295.967|-472.125
|||1.4.1|-22.3305|-198.4883|-295.9669|-472.1247
|||1.4|-22.3305|-198.4883|-295.9669|-472.1247
|||1.3|-22.3305|-198.4883|-295.9669|-472.1247
|||1.2.1|-22.3305|-198.4883|-295.9669|-472.1247
|||1.2<sup>[1](#1)</sup>|-22.3305|-198.4883|-295.9669|-472.1247
|||1.1.0|-22.3304|-198.4881|-295.9670|-472.1247
|||1.0.0|-22.3304|-198.4881|-295.9670|-472.1247
|||0.5.1|-22.3304|-198.4881|-295.9670|-472.1247
|||0.5.0|-22.3304|-198.4881|-295.9670|-472.1247
|||0.4.0|-22.3304|-198.4881|-295.9670|-472.1247

<a name=1></a><sup>1</sup> APBS 1.2 has switched the multigrid smoothing algorithm from standard Gauss-Seidel to Gauss-Seidel red/black in order to facilitate parallelization. This switch has caused small differences in individual calculation energies which, when combined to the final answer, create larger errors (up to 0.04%). These errors can be reduced by resetting the APBS error tolerance to 1e-9 or smaller values. For a more detailed explanation, please see the APBS FAQ, [here](http://www.poissonboltzmann.org/docs/apbs-faq/#sources error calculation).

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.


