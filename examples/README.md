# APBS version validation and test cases

## APBS examples and test cases

This directory serves as the root directory for the APBS test suite.
In each directory you will find example input files to use with APBS and a README file displaying the results for different versions of APBS.

Executing <code>make test</code> in each directory will run the examples for that directory and log the results to <code>TESTRESULTS.log</code>.
Executing <code>make test</code> from the root examples directory will run all the tests listed below.
Tests will either pass, pass with rounding error (within 10<sup>-9</sup> of the expected result), or fail outright.

| Example | README file | Source | Description | 
| ---- | ---- | ---- | ---- | 
| Actin dimer (actin-dimer) | [actin-dimer/README.md](actin-dimer/README.md) | Dave Sept | Calculate binding energies for actin dimers. This is an example of a large biomolecule binding energy calculation that often requires parallel focusing. | 
| Alkane nonpolar solvation energies (alkanes) | [alkanes/README.md](alkanes/README.md) | Nathan Baker, Jason Wagoner | Calculate nonpolar solvation energies for various alkanes.  Taken from Wagoner JA, Baker NA. Assessing implicit models for nonpolar mean solvation forces: the importance of dispersion and volume terms. [Proc Natl Acad Sci USA, 103, 8331-8336, 2006.](http://dx.doi.org/10.1073/pnas.0600118103) |
| Born ion (born) | [born/README.md](born/README.md) | Nathan Baker | Calculate solvation energies for ions of various sizes and compare to the analytical results. |
| FKBP (FKBP) | [FKBP/README.md](FKBP/README.md) | Jung-Hsin Lin | Binding of various (small) ligands to FKBP.  Analogous to HCA binding case (except it works). |
| HCA ligand binding (hca-bind) | [hca-bind/README.md](hca-bind/README.md) | UHBD | Calculate the binding of a small molecule (acetazolamide) to a medium-sized protein (human carbonic anhydrase). |
| Acetic acid ionization (ionize) | [ionize/README.md](ionize/README.md) | UHBD | Calculate electrostatic contributions to the ionization energy of acetic acid. | 
| Ion-ion PMF (ion-pmf) | [ion-pmf/README.md](ion-pmf/README.md) | Nathan Baker | Calculate solvation energies and solvation force components for ion pairs. |
| Ion-protein interaction energies (ion-protein) | [ion-protein/README.md](ion-protein/README.md) | Dave Sept | Calculate polar energy of placing an ion near a macromolecule. |
| PKA-balanol binding (pka-lig) | [pka-lig/README.md](pka-lig/README.md) | Chung Wong | Calculate binding energies of a ligand to protein kinase A. |
| Coulomb's law (point-pmf) | [point-pmf/README.md](point-pmf/README.md) | Nathan Baker | See how well we do reproducing Coulomb's law. |
| Methanol solvation (solv) | [solv/README.md](solv/README.md) | UHBD | Calculate the solvation energies of methanol and methoxide. | 
| Protein-RNA interactions (protein-rna) | [protein-rna/README.md](protein-rna/README.md) | David Draper | Calculate the salt dependence of protein interactions with box B RNA hairpin. |
| Geometric flow solvation model | [geoflow/README.md](geoflow/README.md) | Elizabeth Jurrus | Calculate the dielectric interface profile across the solute-solvent boundary in a thermodynamically sef-consistent fashion. |
