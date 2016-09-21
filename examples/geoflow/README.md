README for geometric flow APBS examples
=============================

The example input files in this directory use a differential
geometry-based geometric flow solvation model to describes a smooth
dielectric interface profile across the solvent-solute boundary in a
thermodynamically self-consistent fashion.  The main parameters of the
model are the solute/solvent dielectric coefficients, solvent pressure on
the solute, microscopic surface tension, solvent density, and molecular
force-field parameters.

More information can be found here in [Chen et al.](http://www.ncbi.nlm.nih.gov/pubmed/20938489) and [Thomas et
al](http://www.ncbi.nlm.nih.gov/pubmed/23212974).

The parameters used in the input files (imidazol and glycerol) came from Thomas et al.

| Input File  | Description                 | APBS Version     | Global Net ELEC Energy | Global Net APOL Energy |
|-------------|-----------------------------|------------------|------------------------|------------------------|
| imidazol.in | Serial, mdh boundary condts.| **1.5** | **-10.3022** | **0.541742** |
|||1.4.2|-10.3022|0.541742
