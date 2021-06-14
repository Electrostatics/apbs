"""Test input file CALCULATE section parsing."""
import logging
import pytest
from apbs.input_file.calculate import Calculate


_LOGGER = logging.getLogger(__name__)


GOOD_INPUT = [
    """# Nonpolar calculation without forces
    alias:  nonpolar 1
    type:  nonpolar
    parameters:
      calculate energy:  True
      calculate forces:  False
      grid spacings:  [0.2, 0.2, 0.2]
      molecule:  the molecule
      pressure:  0.2394
      solvent density:  0.033428
      solvent radius:  1.4
      surface density:  100
      surface method:  solvent-accessible
      surface tension:  0.0085
      temperature:  298.15
    """,
    """# Nonpolar calculation with forces
    alias:  nonpolar 1
    type:  nonpolar
    parameters:
      calculate energy:  True
      calculate forces:  True
      grid spacings:  [0.2, 0.2, 0.2]
      displacement:  0.05
      molecule:  the molecule
      pressure:  0.2394
      solvent density:  0.033428
      solvent radius:  1.4
      surface density:  100
      surface method:  solvent-accessible
      surface tension:  0.0085
      temperature:  298.15
    """,
]


BAD_INPUT = [
    """# Nonpolar calculation with forces MISSING displacement
    alias:  nonpolar 1
    type:  nonpolar
    parameters:
      calculate energy:  True
      calculate forces:  True
      grid spacings:  [0.2, 0.2, 0.2]
      molecule:  the molecule
      pressure:  0.2394
      solvent density:  0.033428
      solvent radius:  1.4
      surface density:  100
      surface method:  solvent-accessible
      surface tension:  0.0085
      temperature:  298.15
    """,
    """# Nonpolar calculation with bad surface method
    alias:  nonpolar 1
    type:  nonpolar
    parameters:
      calculate energy:  True
      calculate forces:  False
      grid spacings:  [0.2, 0.2, 0.2]
      molecule:  the molecule
      pressure:  0.2394
      solvent density:  0.033428
      solvent radius:  1.4
      surface density:  100
      surface method:  COMPLETELY BOGUS
      surface tension:  0.0085
      temperature:  298.15
    """,
]


def id_function(test_string) -> str:
    """Turn test strings into labels."""
    label = test_string.splitlines()[0]
    return label[2:]


@pytest.mark.parametrize("input_", GOOD_INPUT, ids=id_function)
def test_good_input(input_):
    """Test input file CALCULATE sections."""
    calculate = Calculate()
    _LOGGER.debug(f"YAML input: {input_}")
    calculate.from_yaml(input_)
    calculate.validate()
    new_dict = calculate.to_dict()
    _LOGGER.debug(f"Object dictionary: {new_dict}")
    calculate.from_dict(new_dict)
    calculate.validate()


@pytest.mark.parametrize("input_", BAD_INPUT, ids=id_function)
def test_bad_input(input_):
    """Test input file CALCULATE sections."""
    with pytest.raises((KeyError, ValueError)):
        calculate = Calculate()
        _LOGGER.debug(input_)
        calculate.from_yaml(input_)
        calculate.validate()
