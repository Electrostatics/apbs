"""Test input file calculate.FiniteElement."""
import logging
import json
import pytest
from apbs.input_file.calculate.finite_element import FiniteElement


_LOGGER = logging.getLogger(__name__)


GOOD_TEMPLATE = {
    "a priori refinement": None,
    "boundary condition": None,
    "calculate energy": True,
    "calculate forces": True,
    "charge discretization": None,
    "domain length": [22.5, 19.5, 0.5],
    "error based refinement": None,
    "error tolerance": 1e-3,
    "equation": None,
    "ions": None,
    "initial mesh resolution": 0.5,
    "initial mesh vertices": 100000,
    "maximum refinement iterations": 10,
    "maximum vertices": 10000000,
    "molecule": "foo",
    "solute dielectric": 12,
    "solvent dielectric": 78.54,
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
GOOD_A_PRIORI_REFINEMNT = ["geometric", "uniform"]
GOOD_BOUNDARY_CONDITIONS = [
    "zero",
    "single sphere",
    "multiple sphere",
    "focus foo",
]
GOOD_BEER = ["White Bluffs FID"]
GOOD_CHARGE_DISCRETIZATIONS = ["linear", "cubic", "quintic"]
GOOD_ERROR_BASED_REFINEMENT = ["global", "simplex", "fraction"]
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


@pytest.mark.parametrize("test_variable", GOOD_A_PRIORI_REFINEMNT)
def test_a_priori_refinement(test_variable):
    input_dict = GOOD_TEMPLATE
    input_dict["a priori refinement"] = test_variable
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["error based refinement"] = GOOD_ERROR_BASED_REFINEMENT[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(f"Input JSON: {json.dumps(input_dict, indent=2)}")
    obj = FiniteElement(dict_=input_dict)
    obj.validate()
    dict_ = obj.to_dict()
    _LOGGER.debug(f"Output JSON: {json.dumps(dict_, indent=2)}")
    obj = FiniteElement(dict_=dict_)
    obj.validate()
    with pytest.raises(ValueError):
        input_dict["a priori refinement"] = "foo"
        obj = FiniteElement(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_BOUNDARY_CONDITIONS)
def test_boundary_conditions(test_variable):
    input_dict = GOOD_TEMPLATE
    input_dict["a priori refinement"] = GOOD_A_PRIORI_REFINEMNT[0]
    input_dict["boundary condition"] = test_variable
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["error based refinement"] = GOOD_ERROR_BASED_REFINEMENT[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(f"Input JSON: {json.dumps(input_dict, indent=2)}")
    obj = FiniteElement(dict_=input_dict)
    obj.validate()
    dict_ = obj.to_dict()
    _LOGGER.debug(f"Output JSON: {json.dumps(dict_, indent=2)}")
    obj = FiniteElement(dict_=dict_)
    obj.validate()
    with pytest.raises(ValueError):
        input_dict["boundary condition"] = "foo"
        obj = FiniteElement(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_CHARGE_DISCRETIZATIONS)
def test_charge_discretization(test_variable):
    input_dict = GOOD_TEMPLATE
    input_dict["a priori refinement"] = GOOD_A_PRIORI_REFINEMNT[0]
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["charge discretization"] = test_variable
    input_dict["error based refinement"] = GOOD_ERROR_BASED_REFINEMENT[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(f"Input JSON: {json.dumps(input_dict, indent=2)}")
    obj = FiniteElement(dict_=input_dict)
    obj.validate()
    dict_ = obj.to_dict()
    _LOGGER.debug(f"Output JSON: {json.dumps(dict_, indent=2)}")
    obj = FiniteElement(dict_=dict_)
    obj.validate()
    with pytest.raises(ValueError):
        input_dict["charge discretization"] = "foo"
        obj = FiniteElement(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_ERROR_BASED_REFINEMENT)
def test_error_based_refinement(test_variable):
    input_dict = GOOD_TEMPLATE
    input_dict["a priori refinement"] = GOOD_A_PRIORI_REFINEMNT[0]
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["error based refinement"] = test_variable
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(f"Input JSON: {json.dumps(input_dict, indent=2)}")
    obj = FiniteElement(dict_=input_dict)
    obj.validate()
    dict_ = obj.to_dict()
    _LOGGER.debug(f"Output JSON: {json.dumps(dict_, indent=2)}")
    obj = FiniteElement(dict_=dict_)
    obj.validate()
    with pytest.raises(ValueError):
        input_dict["error based refinement"] = "foo"
        obj = FiniteElement(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_EQUATIONS)
def test_equation(test_variable):
    input_dict = GOOD_TEMPLATE
    input_dict["a priori refinement"] = GOOD_A_PRIORI_REFINEMNT[0]
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["error based refinement"] = GOOD_ERROR_BASED_REFINEMENT[0]
    input_dict["equation"] = test_variable
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(f"Input JSON: {json.dumps(input_dict, indent=2)}")
    obj = FiniteElement(dict_=input_dict)
    obj.validate()
    dict_ = obj.to_dict()
    _LOGGER.debug(f"Output JSON: {json.dumps(dict_, indent=2)}")
    obj = FiniteElement(dict_=dict_)
    obj.validate()
    with pytest.raises(ValueError):
        input_dict["equation"] = "foo"
        obj = FiniteElement(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_IONS)
def test_ions(test_variable):
    input_dict = GOOD_TEMPLATE
    input_dict["a priori refinement"] = GOOD_A_PRIORI_REFINEMNT[0]
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["error based refinement"] = GOOD_ERROR_BASED_REFINEMENT[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = test_variable
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(f"Input JSON: {json.dumps(input_dict, indent=2)}")
    obj = FiniteElement(dict_=input_dict)
    obj.validate()
    dict_ = obj.to_dict()
    _LOGGER.debug(f"Output JSON: {json.dumps(dict_, indent=2)}")
    obj = FiniteElement(dict_=dict_)
    obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_SURFACE_METHODS)
def test_surface_methods(test_variable):
    input_dict = GOOD_TEMPLATE
    input_dict["a priori refinement"] = GOOD_A_PRIORI_REFINEMNT[0]
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["error based refinement"] = GOOD_ERROR_BASED_REFINEMENT[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = test_variable
    input_dict["use maps"] = GOOD_USEMAP_INPUTS[0]
    _LOGGER.debug(f"Input JSON: {json.dumps(input_dict, indent=2)}")
    obj = FiniteElement(dict_=input_dict)
    obj.validate()
    dict_ = obj.to_dict()
    _LOGGER.debug(f"Output JSON: {json.dumps(dict_, indent=2)}")
    obj = FiniteElement(dict_=dict_)
    obj.validate()
    with pytest.raises(ValueError):
        input_dict["surface method"] = "foo"
        obj = FiniteElement(dict_=input_dict)
        obj.validate()


@pytest.mark.parametrize("test_variable", GOOD_USEMAP_INPUTS)
def test_use_map(test_variable):
    input_dict = GOOD_TEMPLATE
    input_dict["a priori refinement"] = GOOD_A_PRIORI_REFINEMNT[0]
    input_dict["boundary condition"] = GOOD_BOUNDARY_CONDITIONS[0]
    input_dict["charge discretization"] = GOOD_CHARGE_DISCRETIZATIONS[0]
    input_dict["error based refinement"] = GOOD_ERROR_BASED_REFINEMENT[0]
    input_dict["equation"] = GOOD_EQUATIONS[0]
    input_dict["ions"] = GOOD_IONS[0]
    input_dict["surface method"] = GOOD_SURFACE_METHODS[0]
    input_dict["use maps"] = test_variable
    _LOGGER.debug(f"Input JSON: {json.dumps(input_dict, indent=2)}")
    obj = FiniteElement(dict_=input_dict)
    obj.validate()
    dict_ = obj.to_dict()
    _LOGGER.debug(f"Output JSON: {json.dumps(dict_, indent=2)}")
    obj = FiniteElement(dict_=dict_)
    obj.validate()
