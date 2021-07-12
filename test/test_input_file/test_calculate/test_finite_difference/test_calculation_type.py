"""Test input file calculate.FiniteDifference grid-related parsing."""
import logging
import pytest
from apbs.input_file.calculate.finite_difference import Focus, Manual


_LOGGER = logging.getLogger(__name__)


GOOD_MANUAL_INPUTS = [
    {
        "grid center": {"position": [0, 0, 0]},
        "grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.2, 0.2, 0.2],
        },
    }
]


@pytest.mark.parametrize("input_dict", GOOD_MANUAL_INPUTS)
def test_good_manual(input_dict):
    """Test Manual calculation type."""
    manual = Manual(dict_=input_dict)
    dict_ = manual.to_dict()
    manual = Manual(dict_=dict_)
    manual.validate()


BAD_MANUAL_INPUTS = [
    {
        "grid center": None,
        "grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.2, 0.2, 0.2],
        },
    },
    {"grid center": {"position": [0, 0, 0]}, "grid dimensions": 4},
    {"grid dimensions": {"counts": [97, 97, 97], "spacings": [0.2, 0.2, 0.2]}},
]


@pytest.mark.parametrize("input_dict", BAD_MANUAL_INPUTS)
def test_bad_manual(input_dict):
    """Test bad Manual calculation type inputs."""
    with pytest.raises((KeyError, AttributeError, ValueError)):
        manual = Manual(dict_=input_dict)
        manual.to_dict()
        manual.validate()


GOOD_FOCUS_INPUTS = [
    {
        "coarse grid center": {"position": [0, 0, 0]},
        "coarse grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.2, 0.2, 0.2],
        },
        "fine grid center": {"position": [0.2, -0.1, 0.13]},
        "fine grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.05, 0.05, 0.05],
        },
        "parallel": False,
    },
    {
        "coarse grid center": {"position": [0, 0, 0]},
        "coarse grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.2, 0.2, 0.2],
        },
        "fine grid center": {"position": [0.2, -0.1, 0.13]},
        "fine grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.05, 0.05, 0.05],
        },
        "parallel": True,
        "parallel parameters": {
            "overlap fraction": 0.1,
            "processor array": [8, 8, 8],
            "asynchronous rank": 4,
        },
    },
]


@pytest.mark.parametrize("input_dict", GOOD_FOCUS_INPUTS)
def test_good_focus(input_dict):
    """Test Focus calculation type."""
    focus = Focus(dict_=input_dict)
    dict_ = focus.to_dict()
    focus = Focus(dict_=dict_)
    focus.validate()


BAD_FOCUS_INPUTS = [
    {
        "coarse grid center": {"position": [0, 0, 0]},
        "coarse grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.2, 0.2, 0.2],
        },
        "fine grid center": {"position": [0.2, -0.1, 0.13]},
        "fine grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.3, 0.3, 0.3],
        },
        "parallel": False,
    },
    {
        "coarse grid center": {"position": [0, 0, 0]},
        "coarse grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.2, 0.2, 0.2],
        },
        "fine grid center": {"position": [0.2, -0.1, 0.13]},
        "fine grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.05, 0.05, 0.05],
        },
        "parallel": True,
    },
    {
        "coarse grid center": {"position": [0, 0, 0]},
        "coarse grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.2, 0.2, 0.2],
        },
        "fine grid center": {"position": [0.2, -0.1, 0.13]},
        "fine grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.05, 0.05, 0.05],
        },
        "parallel": True,
        "parallel parameters": {
            "overlap fraction": 0.1,
            "processor array": [2, 2, 2],
            "asynchronous rank": 10,
        },
    },
]


@pytest.mark.parametrize("input_dict", BAD_FOCUS_INPUTS)
def test_bad_focus(input_dict):
    """Test bad Focus calculation type inputs."""
    with pytest.raises((KeyError, AttributeError, ValueError)):
        focus = Focus(dict_=input_dict)
        focus.to_dict()
        focus.validate()
