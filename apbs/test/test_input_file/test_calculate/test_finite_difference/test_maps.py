"""Test input file calculate.FiniteDifference map input and output."""
import logging
import pytest
from apbs.input_file.calculate.finite_difference import UseMap, WriteMap


_LOGGER = logging.getLogger(__name__)


GOOD_USEMAP_INPUTS = [
    {"property": prop, "alias": "foo"}
    for prop in [
        "dielectric",
        "ion accessibility",
        "charge density",
        "potential",
    ]
]


@pytest.mark.parametrize("input_dict", GOOD_USEMAP_INPUTS)
def test_good_usemap(input_dict):
    """Test UseMap class with good values."""
    obj = UseMap(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = UseMap(dict_=dict_)
    obj.validate()


BAD_USEMAP_INPUTS = [
    {"property": "fnord", "alias": "foo"},
    {"property": "dielectric"},
]


@pytest.mark.parametrize("input_dict", BAD_USEMAP_INPUTS)
def test_bad_usemap(input_dict):
    """Test UseMap class with good values."""
    with pytest.raises((KeyError, ValueError)):
        obj = UseMap(dict_=input_dict)
        dict_ = obj.to_dict()
        obj = UseMap(dict_=dict_)
        obj.validate()


GOOD_WRITEMAP_INPUTS = []
for prop in [
    "charge density",
    "potential",
    "solvent accessibility",
    "ion accessibility",
    "laplacian",
    "energy density",
    "ion number density",
    "ion charge density",
    "dielectric x",
    "dielectric y",
    "dielectric z",
]:
    for fmt in ["dx", "dx.gz", "flat", "uhbd"]:
        GOOD_WRITEMAP_INPUTS.append(
            {"property": prop, "format": fmt, "path": f"foo.{fmt}"}
        )


@pytest.mark.parametrize("input_dict", GOOD_WRITEMAP_INPUTS)
def test_good_writemap(input_dict):
    """Test UseMap class with good values."""
    obj = WriteMap(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = WriteMap(dict_=dict_)
    obj.validate()


BAD_WRITEMAP_INPUTS = [
    {"property": "foo", "format": "gz", "path": "foo.gz"},
    {"property": "potential", "format": "foo", "path": "foo.foo"},
    {"format": "gz", "path": "potential.gz"},
    {"property": "potential", "path": "potential.gz"},
    {"property": "potential", "format": "gz"},
]


@pytest.mark.parametrize("input_dict", BAD_WRITEMAP_INPUTS)
def test_bad_writemap(input_dict):
    """Test UseMap class with bad values."""
    with pytest.raises((KeyError, ValueError)):
        obj = WriteMap(dict_=input_dict)
        dict_ = obj.to_dict()
        obj = WriteMap(dict_=dict_)
        obj.validate()
