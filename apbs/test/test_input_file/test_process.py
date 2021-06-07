"""Test input file PROCESS section parsing."""
import logging
import pytest
from apbs.input_file.process import Process


_LOGGER = logging.getLogger(__name__)


GOOD_INPUT = [
    """# Do all the processes
sums:
  - alias: my sum 1
    elements:
      - alias:  foo 1
        coefficient:  -2
      - alias:  foo 2
        coefficient:  3.14
  - alias: my sum 2
    elements:
      - alias:  bar 1
        coefficient:  -3
      - alias:  bar 2
        coefficient:  1.12
products:
  - alias:  my product
    elements:
      - alias:  foo 1
        coefficient:  -20
      - alias:  foo 2
        coefficient:  +3.14
exps:
  - alias:  my exponential
    elements:
      - alias:  foo 1
        coefficient:  0.02
    """,
]


BAD_INPUT = [
    """# Missing coefficient
sums:
  - alias: my sum 1
    elements:
      - alias:  foo 1
      - alias:  foo 2
        coefficient:  3.14
  - alias: my sum 2
    elements:
      - alias:  bar 1
        coefficient:  -3
      - alias:  bar 2
        coefficient:  1.12
products:
  - alias:  my product
    elements:
      - alias:  foo 1
        coefficient:  -20
      - alias:  foo 2
        coefficient:  +3.14
exps:
  - alias:  my exponential
    elements:
      - alias:  foo 1
        coefficient:  0.02
    """,
    """# Missing alias
sums:
  - alias: my sum 1
    elements:
      - coefficient:  -2
      - alias:  foo 2
        coefficient:  3.14
  - alias: my sum 2
    elements:
      - alias:  bar 1
        coefficient:  -3
      - alias:  bar 2
        coefficient:  1.12
products:
  - alias:  my product
    elements:
      - alias:  foo 1
        coefficient:  -20
      - alias:  foo 2
        coefficient:  +3.14
exps:
  - alias:  my exponential
    elements:
      - alias:  foo 1
        coefficient:  0.02
    """,
    """# Bad coefficient
sums:
  - alias: my sum 1
    elements:
      - alias:  foo 1
        coefficient:  -2
      - alias:  foo 2
        coefficient:  pi
  - alias: my sum 2
    elements:
      - alias:  bar 1
        coefficient:  -3
      - alias:  bar 2
        coefficient:  1.12
products:
  - alias:  my product
    elements:
      - alias:  foo 1
        coefficient:  -20
      - alias:  foo 2
        coefficient:  +3.14
exps:
  - alias:  my exponential
    elements:
      - alias:  foo 1
        coefficient:  0.02
    """,
]


def id_function(test_string) -> str:
    """Turn test strings into labels."""
    label = test_string.splitlines()[0]
    return label[2:]


@pytest.mark.parametrize("input_", GOOD_INPUT, ids=id_function)
def test_good_input(input_):
    """Test input file PROCESS sections."""
    process = Process()
    _LOGGER.debug(f"YAML input: {input_}")
    process.from_yaml(input_)
    process.validate()
    new_dict = process.to_dict()
    _LOGGER.debug(f"Object dictionary: {new_dict}")
    _LOGGER.debug(new_dict)
    process.from_dict(new_dict)
    process.validate()


@pytest.mark.parametrize("input_", BAD_INPUT, ids=id_function)
def test_bad_input(input_):
    """Test input file PROCESS sections."""
    with pytest.raises((KeyError, ValueError)):
        process = Process()
        _LOGGER.debug(input_)
        process.from_yaml(input_)
        process.validate()
