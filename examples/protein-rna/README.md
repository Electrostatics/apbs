README for protein-RNA APBS example
===================================

This is example is taken directly from

> García-García C, Draper DE. Electrostatic interactions in a peptide-RNA complex. Journal of Molecular Biology. 331 (1), 75-88, 2003. <http://dx.doi.org/10.1016/S0022-2836(03)00615-6>

with the minimized PDB files kindly provided by David Draper. It uses the change in binding free energy with ionic strength to estimate the change in number of ions "bound" (diffusively) to RNA upon protein binding.

A more comprehensive walkthrough of this example is available [online](http://www.poissonboltzmann.org/examples/Protein-Rna_Tutorial/).

The APBS results agree reasonably well with Draper's calculations; there are some differences in parameterization, surface definitions, etc. that lead to small differents in the results.

To run this example, set the environmental variable APBS to your current APBS executable; e.g.,

    export APBS=/path/to/apbs/executable
            

in bash or

    setenv APBS /path/to/apbs/executable
            

in tcsh. Then simply type 'make' to run the example.
<center>
APBS Version|Input file|APBS Result
---|---|---
1.5  |apbs-0.025.in|8.674120000000e+01
     |apbs-0.050.in|9.606840000000e+01
     |apbs-0.075.in|1.011540000000e+02
     |apbs-0.100.in|1.046140000000e+02
     |apbs-0.125.in|1.072230000000e+02
     |apbs-0.150.in|1.093080000000e+02
     |apbs-0.175.in|1.110410000000e+02
     |apbs-0.200.in|1.125200000000e+02
     |apbs-0.225.in|1.138070000000e+02
     |apbs-0.250.in|1.149440000000e+02
     |apbs-0.275.in|1.159620000000e+02
     |apbs-0.300.in|1.168800000000e+02
     |apbs-0.325.in|1.177170000000e+02
     |apbs-0.400.in|1.198460000000e+02
     |apbs-0.500.in|1.220610000000e+02
     |apbs-0.600.in|1.238080000000e+02
     |apbs-0.700.in|1.252360000000e+02
     |apbs-0.800.in|1.264340000000e+02
1.4.2|apbs-0.025.in|8.674120000000e+01
     |apbs-0.050.in|9.606840000000e+01
     |apbs-0.075.in|1.011540000000e+02
     |apbs-0.100.in|1.046140000000e+02
     |apbs-0.125.in|1.072230000000e+02
     |apbs-0.150.in|1.093080000000e+02
     |apbs-0.175.in|1.110410000000e+02
     |apbs-0.200.in|1.125200000000e+02
     |apbs-0.225.in|1.138070000000e+02
     |apbs-0.250.in|1.149440000000e+02
     |apbs-0.275.in|1.159620000000e+02
     |apbs-0.300.in|1.168800000000e+02
     |apbs-0.325.in|1.177170000000e+02
     |apbs-0.400.in|1.198460000000e+02
     |apbs-0.500.in|1.220610000000e+02
     |apbs-0.600.in|1.238080000000e+02
     |apbs-0.700.in|1.252360000000e+02
     |apbs-0.800.in|1.264340000000e+02
</center>
<!---
%%%%%%
Commented this out since is the result of running all the example. Reather, I am putting the results of the test in here. JB.
%%%%%%
Input file|Description|APBS version|APBS results||Draper PB results||Draper experimental results||
---|---|---|---|---|---|---|---|---
||||n = -(d Δ G)/(d ln [KCl])/RT|-d( Δ G)/(d log<sub>10</sub> [KCl]) (kcal)|n = -(d Δ G)/(d ln [KCl])/RT|-d( Δ G)/(d log<sub>10</sub> [KCl]) (kcal)|n = -(d Δ G)/(d ln [KCl])/RT|-d( Δ G)/(d log<sub>10</sub> [KCl]) (kcal)
'make all'|Run a series of binding energy calculations at different ionic strengths|**1.4.2**|**-(4.52831 ± 0.0758878)**|**6.1561 ± 0.109612**|-(4.3 ± 0.2)|5.9 ± 0.2|-(4.4 ± 0.2)|6.0 ± 0.2
          |                                                                        |1.0.0           |-(4.52 ± 0.08)|6.2 ± 0.1
--->
Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.


