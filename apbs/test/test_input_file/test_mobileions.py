"""Test input file generic.MobileIons."""
from apbs import input_file
import logging
import pytest
from apbs.input_file.calculate.generic import MobileIons, Ion


_LOGGER = logging.getLogger(__name__)


GOOD_IONS = [
    {"charge": -1, "radius": 2.1, "concentration": 0.100},
    {"charge": +1, "radius": 2, "concentration": 0.100},
]


@pytest.mark.parametrize("input_dict", GOOD_IONS)
def test_good_ions(input_dict):
    """Test good Ion objects."""
    ion = Ion(dict_=input_dict)
    ion.validate()
    ion = Ion(json=ion.to_json())
    ion.validate()
    ion = Ion(yaml=ion.to_yaml())
    ion.validate()


BAD_IONS = [
    {"charge": 2, "radius": 2},
    {"charge": 1, "radius": 0, "concentration": 0.1},
    {"charge": "negative one", "radius": 2, "concentration": 0.1},
    {"charge": -1, "radius": 2.1, "concentration": -0.100},
]


@pytest.mark.parametrize("input_dict", BAD_IONS)
def test_bad_ions(input_dict):
    """Test bad Ion objects."""
    with pytest.raises((ValueError, TypeError, KeyError)):
        Ion(dict_=input_dict)


GOOD_SOLUTIONS = [
    {
        "species": [
            {"charge": +2, "radius": 2.0, "concentration": 0.050},
            {"charge": 1, "radius": 1.2, "concentration": 0.100},
            {"charge": -1, "radius": 2.0, "concentration": 0.200},
        ]
    },
    {"species": GOOD_IONS},
]


@pytest.mark.parametrize("input_dict", GOOD_SOLUTIONS)
def test_good_solutions(input_dict):
    """Test good MobileIons data."""
    obj = MobileIons(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = MobileIons(dict_=dict_)
    obj.validate()


BAD_SOLUTIONS = [
    {
        "species": [
            {"charge": +2, "radius": 2.0, "concentration": 0.051},
            {"charge": 1, "radius": 1.2, "concentration": 0.100},
            {"charge": -1, "radius": 2.0, "concentration": 0.200},
        ]
    },
    {"species": BAD_IONS},
]


@pytest.mark.parametrize("input_dict", BAD_SOLUTIONS)
def test_bad_solutions(input_dict):
    """Test good MobileIons data."""
    with pytest.raises((KeyError, ValueError)):
        obj = MobileIons(dict_=input_dict)
        dict_ = obj.to_dict()
        obj = MobileIons(dict_=dict_)
        obj.validate()
