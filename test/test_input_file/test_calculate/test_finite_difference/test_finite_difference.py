"""Test input file calculate.FiniteDifference grid-related parsing."""
import logging
import json
import pytest
from apbs.input_file.calculate.finite_difference import FiniteDifference


_LOGGER = logging.getLogger(__name__)


GOOD_TEMPLATE = {
    "boundary condition": None,
    "calculate energy": True,
    "calculate forces": True,
    "calculation type": None,
    "calculation parameters": None,
    "charge discretization": None,
    "error tolerance": 1e-6,
    "equation": None,
    "ions": None,
    "molecule": "foo",
    "no-op": False,
    "solute dielectric": 12,
    "solvent dielectric": 80,
    "solvent radius": 1.4,
    "surface method": None,
    "surface spline window": 0.3,
    "temperature": 298.15,
    "use maps": None,
    "write atom potentials": "atom_potentials.txt",
    "write maps": [
        {"property": "potential", "format": "dx.gz", "path": "pot.dx.gz"}
    ],
}
GOOD_BOUNDARY_CONDITIONS = [
    "zero",
    "single sphere",
    "multiple sphere",
    "focus foo",
]
GOOD_BEER = ["White Bluffs NOG"]
GOOD_CALCULATION_TYPES = {
    "manual": {
        "grid center": {"position": [0, 0, 0]},
        "grid dimensions": {
            "counts": [97, 97, 97],
            "spacings": [0.2, 0.2, 0.2],
        },
    },
    "focus": {
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
}
GOOD_CHARGE_DISCRETIZATIONS = ["linear", "cubic", "quintic"]
GOOD_EQUATIONS = [
    "linearized pbe",
    "nonlinear pbe",
    "linearized regularized pbe",
    "nonlinear regularized pbe",
]
GOOD_IONS = [
    {
        "species": [
            {"charge": +2, "radius": 2.0, "concentration": 0.050},
            {"charge": 1, "radius": 1.2, "concentration": 0.100},
            {"charge": -1, "radius": 2.0, "concentration": 0.200},
        ]
    },
    {
        "species": [
            {"charge": 1, "radius": 1.2, "concentration": 0.100},
            {"charge": -1, "radius": 2.0, "concentration": 0.100},
        ]
    },
]
GOOD_SURFACE_METHODS = [
    "molecular surface",
    "smoothed molecular surface",
    "cubic spline",
    "septic spline",
]
GOOD_USEMAP_INPUTS = [
    [{"property": prop, "alias": "foo"}]
    for prop in [
        "dielectric",
        "ion accessibility",
        "charge density",
        "potential",
    ]
]


@pytest.mark.parametrize("test_variable", GOOD_BOUNDARY_CONDITIONS)
def test_boundary_conditions(test_variable):
    """Test Focus calculation type."""
    input_dict = GOOD_TEMPLATE
    input_dict["boundary condition"] = test_variable
    input_dict["calculation type"] = list(GOOD_CALCULATION_TYPES)[0]
    input_dict["calculation parameters"] = GOOD_CALCULATION_TYPES[
        input_dict["calculation type"]
    ]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(json.dumps(input_dict, indent=2))
    obj = FiniteDifference(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = FiniteDifference(dict_=dict_)
    obj.validate()

    with pytest.raises(ValueError):
        input_dict["boundary condition"] = "foo"
        obj = FiniteDifference(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_CALCULATION_TYPES)
def test_calculation_types(test_variable):
    """Test Focus calculation type."""
    input_dict = GOOD_TEMPLATE
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["calculation type"] = test_variable
    input_dict["calculation parameters"] = GOOD_CALCULATION_TYPES[
        input_dict["calculation type"]
    ]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(json.dumps(input_dict, indent=2))
    obj = FiniteDifference(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = FiniteDifference(dict_=dict_)
    obj.validate()

    with pytest.raises(ValueError):
        input_dict["calculation type"] = "foo"
        obj = FiniteDifference(dict_=input_dict)
        obj.validate()

        input_dict["calculation type"] = test_variable
        input_dict["calculation parameters"] = None
        obj = FiniteDifference(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_CHARGE_DISCRETIZATIONS)
def test_charge_discretizations(test_variable):
    """Test Focus calculation type."""
    input_dict = GOOD_TEMPLATE
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["calculation type"] = list(GOOD_CALCULATION_TYPES)[0]
    input_dict["calculation parameters"] = GOOD_CALCULATION_TYPES[
        input_dict["calculation type"]
    ]
    input_dict["charge discretization"] = test_variable
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(json.dumps(input_dict, indent=2))
    obj = FiniteDifference(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = FiniteDifference(dict_=dict_)
    obj.validate()

    with pytest.raises(ValueError):
        input_dict["charge discretization"] = "foo"
        obj = FiniteDifference(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_EQUATIONS)
def test_equations(test_variable):
    """Test Focus calculation type."""
    input_dict = GOOD_TEMPLATE
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["calculation type"] = list(GOOD_CALCULATION_TYPES)[0]
    input_dict["calculation parameters"] = GOOD_CALCULATION_TYPES[
        input_dict["calculation type"]
    ]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["equation"] = test_variable
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(json.dumps(input_dict, indent=2))
    obj = FiniteDifference(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = FiniteDifference(dict_=dict_)
    obj.validate()

    with pytest.raises(ValueError):
        input_dict["equation"] = "foo"
        obj = FiniteDifference(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_IONS)
def test_ions(test_variable):
    """Test Focus calculation type."""
    input_dict = GOOD_TEMPLATE
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["calculation type"] = list(GOOD_CALCULATION_TYPES)[0]
    input_dict["calculation parameters"] = GOOD_CALCULATION_TYPES[
        input_dict["calculation type"]
    ]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = test_variable
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(json.dumps(input_dict, indent=2))
    obj = FiniteDifference(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = FiniteDifference(dict_=dict_)
    obj.validate()

    with pytest.raises((ValueError, TypeError)):
        input_dict["ions"] = {"species": None}
        obj = FiniteDifference(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_SURFACE_METHODS)
def test_surface_methods(test_variable):
    """Test Focus calculation type."""
    input_dict = GOOD_TEMPLATE
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["calculation type"] = list(GOOD_CALCULATION_TYPES)[0]
    input_dict["calculation parameters"] = GOOD_CALCULATION_TYPES[
        input_dict["calculation type"]
    ]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = test_variable
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(json.dumps(input_dict, indent=2))
    obj = FiniteDifference(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = FiniteDifference(dict_=dict_)
    obj.validate()

    with pytest.raises((ValueError, TypeError)):
        input_dict["surface method"] = "foo"
        obj = FiniteDifference(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_USEMAP_INPUTS)
def test_use_maps(test_variable):
    """Test Focus calculation type."""
    input_dict = GOOD_TEMPLATE
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["calculation type"] = list(GOOD_CALCULATION_TYPES)[0]
    input_dict["calculation parameters"] = GOOD_CALCULATION_TYPES[
        input_dict["calculation type"]
    ]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = test_variable
    _LOGGER.debug(json.dumps(input_dict, indent=2))
    obj = FiniteDifference(dict_=input_dict)
    dict_ = obj.to_dict()
    obj = FiniteDifference(dict_=dict_)
    obj.validate()

    with pytest.raises((ValueError, TypeError)):
        input_dict["use maps"] = "foo"
        obj = FiniteDifference(dict_=input_dict)
        obj.validate()
