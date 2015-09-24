README for protein-RNA APBS example
===================================

*\$Id\$*

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

Input file|Description|APBS version|APBS results||Draper PB results||Draper experimental results||
-|-|-|-|-|-|-|-|-
||||n = -(d Δ G)/(d ln [KCl])/RT|-d( Δ G)/(d log<sub>10</sub> [KCl]) (kcal)|n = -(d Δ G)/(d ln [KCl])/RT|-d( Δ G)/(d log<sub>10</sub> [KCl]) (kcal)|n = -(d Δ G)/(d ln [KCl])/RT|-d( Δ G)/(d log<sub>10</sub> [KCl]) (kcal)
'make all'|Run a series of binding energy calculations at different ionic strengths|APBS 1.0.0|-(4.52 ± 0.08)|6.2 ± 0.1|-(4.3 ± 0.2)|5.9 ± 0.2|-(4.4 ± 0.2)|6.0 ± 0.2

Please see the ChangeLog or the [APBS website](http://www.poissonboltzmann.org/) for more information.


