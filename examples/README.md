# APBS version validation and test cases

## APBS examples and test cases

This directory serves as the root directory for the APBS test suite.
In each directory you will find example input files to use with APBS and a README file displaying the results for different versions of APBS.

Executing <code>make test</code> in each directory will run the examples for that directory and log the results to <code>TESTRESULTS.log</code>.
Executing <code>make test</code> from the root examples directory will run all the tests listed below.
Tests will either pass, pass with rounding error (within 10<sup>-9</sup> of the expected result), or fail outright.

| Example | README file | Source | Description | 
| ---- | ---- | ---- | ---- | 
| Actin dimer (actin-dimer) | [actin-dimer/README.html](actin-dimer/README.html) | Dave Sept | Calculate binding energies for actin dimers. This is an example of a large biomolecule binding energy calculation that often requires parallel focusing. | 
| Alkane nonpolar solvation energies (alkanes) | [alkanes/README.html](alkanes/README.html) | Nathan Baker, Jason Wagoner | Calculate nonpolar solvation energies for various alkanes.  Taken from Wagoner JA, Baker NA. Assessing implicit models for nonpolar mean solvation forces: the importance of dispersion and volume terms. [Proc Natl Acad Sci USA, 103, 8331-8336, 2006.](http://dx.doi.org/10.1073/pnas.0600118103) |
| Born ion (born) | [born/README.html](born/README.html) | Nathan Baker | Calculate solvation energies for ions of various sizes and compare to the analytical results. |
| FKBP (FKBP) | [FKBP/README.html](FKBP/README.html) | Jung-Hsin Lin | Binding of various (small) ligands to FKBP.  Analogous to HCA binding case (except it works). |
| HCA ligand binding (hca-bind) | [hca-bind/README.html](hca-bind/README.html) | UHBD | Calculate the binding of a small molecule (acetazolamide) to a medium-sized protein (human carbonic anhydrase). |
| Acetic acid ionization (ionize) | [ionize/README.html](ionize/README.html) | UHBD | Calculate electrostatic contributions to the ionization energy of acetic acid. | 
| Ion-ion PMF (ion-pmf) | [ion-pmf/README.html](ion-pmf/README.html) | Nathan Baker | Calculate solvation energies and solvation force components for ion pairs. |
| Ion-protein interaction energies (ion-protein) | [ion-protein/README.html](ion-protein/README.html) | Dave Sept | Calculate polar energy of placing an ion near a macromolecule. |
| PKA-balanol binding (pka-lig) | [pka-lig/README.html](pka-lig/README.html) | Chung Wong | Calculate binding energies of a ligand to protein kinase A. |
| Coulomb's law (point-pmf) | [point-pmf/README.html](point-pmf/README.html) | Nathan Baker | See how well we do reproducing Coulomb's law. |
| Methanol solvation (solv) | [solv/README.html](solv/README.html) | UHBD | Calculate the solvation energies of methanol and methoxide. | 
| Protein-RNA interactions (protein-rna) | [protein-rna/README.html](protein-rna/README.html) | David Draper | Calculate the salt dependence of protein interactions with box B RNA hairpin. |

