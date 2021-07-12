"""Test input file READ section parsing."""
import logging
import pytest
from apbs.input_file.read import Read


_LOGGER = logging.getLogger(__name__)


GOOD_INPUT = [
    """# Typical input with PQR molecules
molecules:
  - alias:  molecule 1
    format:  pqr
    path:  mol1.pqr
  - alias:  molecule 2
    format:  pqr
    path:  mol2.pqr
  - alias:  complex between 1 & 2
    format:  pqr
    path:  complex.pqr
""",
    """# PDB input with parameter file
molecules:
  - alias:  molecule 1
    format:  pdb
    path:  mol1.pdb
  - alias:  molecule 2
    format:  pdb
    path:  mol2.pdb
  - alias:  the complex
    format:  pdb
    path:  mol1.pdb
parameters:
  - alias:  my parameters
    format:  flat
    path:  custom_parms.txt
""",
    """# PQR input with map for boundary conditions
molecules:
  - alias:  molecule
    format:  pqr
    path:  mol.pqr
potential maps:
  - alias:  potential
    format:  dx
    path:  potential.dx
""",
    """# All-map input
charge density maps:
  - alias:  charge density
    format:  dx
    path:  charge.dx
ion accessibility maps:
  - alias:  ion accessibility
    format:  dx
    path:  ions.dx
dielectric maps:
  - alias:  the dielectric maps
    format:  dx
    x-shifted path: dielx.dx
    y-shifted path: diely.dx
    z-shifted path: dielz.dx
""",
]

BAD_INPUT = [
    """# Typical input with PQR molecules with BAD format
molecules:
  - alias:  molecule 1
    format:  BAD
    path:  mol1.pqr
  - alias:  molecule 2
    format:  pqr
    path:  mol2.pqr
  - alias:  complex between 1 & 2
    format:  pqr
    path:  complex.pqr
""",
    """# PDB input WITHOUT parameter file
molecules:
  - alias:  molecule 1
    format:  pdb
    path:  mol1.pdb
  - alias:  molecule 2
    format:  pdb
    path:  mol2.pdb
  - alias:  the complex
    format:  pdb
    path:  mol1.pdb
""",
    """# PQR input with map for boundary conditions WITHOUT path
molecules:
  - alias:  molecule
    format:  pqr
    path:  mol.pqr
potential maps:
  - alias:  potential
    format:  dx
""",
    """# All-map input with MISSING path
charge density maps:
  - alias:  charge density
    format:  dx
    path:  charge.dx
ion accessibility maps:
  - alias:  ion accessibility
    format:  dx
    path:  ions.dx
dielectric maps:
  - alias:  the dielectric maps
    format:  dx
    x-shifted path: dielx.dx
    z-shifted path: dielz.dx
""",
]


def id_function(test_string) -> str:
    """Turn test strings into labels."""
    label = test_string.splitlines()[0]
    return label[2:]


@pytest.mark.parametrize("input_", GOOD_INPUT, ids=id_function)
def test_good_input(input_):
    """Test input file READ sections."""
    read = Read()
    _LOGGER.debug(f"YAML input: {input_}")
    read.from_yaml(input_)
    read.validate()
    new_dict = read.to_dict()
    _LOGGER.debug(f"Object dictionary: {new_dict}")
    read.from_dict(new_dict)
    read.validate()


@pytest.mark.parametrize("input_", BAD_INPUT, ids=id_function)
def test_bad_input(input_):
    """Test input file READ sections."""
    with pytest.raises((KeyError, ValueError)):
        read = Read()
        _LOGGER.debug(input_)
        read.from_yaml(input_)
        read.validate()
