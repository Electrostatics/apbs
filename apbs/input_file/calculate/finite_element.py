"""Parameters for a finite-element polar solvation calculation."""
import logging

# from typing import Type
from .. import check
from .. import InputFile
from .generic import MobileIons, UseMap, WriteMap


_LOGGER = logging.getLogger(__name__)


class FiniteElement(InputFile):
    """Parameters for a finite-difference polar solvation Poisson-Boltzmann
    calculation.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``a priori refinement``:  *a priori* mesh refinement strategy; see
      :func:`a_priori_refinement`
    * ``boundary condition``:  :func:`boundary_condition`
    * ``calculate energy``:  see :func:`calculate_energy`
    * ``calculate forces``:  see :func:`calculate_forces`
    * ``charge discretization``:  method used to map charges onto the grid; see
      :func:`charge_discretization`
    * ``domain length``:  see :func:`domain_length`
    * ``error estimation``:  how error is calculated for driving refinement;
      see :func:`error_estimation`
    * ``error-based refinement``:  method for choosing which simplices to
      refine based on error; see :func:`error_based_refinement`
    * ``equation``:  what version of the Poisson-Boltzmann equation to solve;
      see :func:`equation`
    * ``ions``:  information about mobile ion species; see :func:`ions`
    * ``initial mesh resolution``:  target resolution of initial mesh; see
      :func:`initial_mesh_resolution`
    * ``initial mesh vertices``:  target number of vertices in the initial
      finite element mesh; see :func:`initial_mesh_vertices`
    * ``maximum refinement iterations``:  number of times to perform the
      solve-estimate-refine iteration of the finite element solver; see
      :func:`maximum_refinement_iterations`
    * ``maximum vertices``:  target for maximum number of vertices in mesh; see
      :func:`maximum_vertices`
    * ``molecule``:  alias to molecule for calculation; see :func:`molecule`
    * ``no-op``:  determine whether the solver should be run; see :func:`noop`
    * ``solute dielectric``:  see :func:`solute_dielectric`
    * ``solvent dielectric``:  see :func:`solvent_dielectric`
    * ``solvent radius``:  see :func:`solvent_radius`
    * ``surface method``:  see :func:`surface_method`
    * ``surface spline window``:  see :func:`surface_spline_window`
    * ``temperature``:  see :func:`temperature`
    * ``use maps``:  use input map for one or more properties of the system;
      see :func:`use_maps`
    * ``write atom potentials``:  write out atom potentials; see
      :func:`write_atom_potentials`
    * ``write maps``:  write out one or more properties of the system to a map;
      see :func:`write_maps`

    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._a_priori_refinement = None
        self._boundary_condition = None
        self._calculate_energy = None
        self._calculate_forces = None
        self._charge_discretization = None
        self._domain_length = None
        self._error_based_refinement = None
        self._error_tolerance = None
        self._equation = None
        self._ions = None
        self._initial_mesh_resolution = None
        self._initial_mesh_vertices = None
        self._maximum_refinement_iterations = None
        self._maximum_vertices = None
        self._molecule = None
        self._solute_dielectric = None
        self._solvent_dielectric = None
        self._solvent_radius = None
        self._surface_method = None
        self._surface_spline_window = None
        self._temperature = None
        self._use_maps = []
        self._write_atom_potentials = None
        self._write_maps = []
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def from_dict(self, input_):
        """Populate object from dictionary.

        :param dict input_:  dictionary with input data.
        :raises KeyError:  if dictionary missing keys.
        """
        self.a_priori_refinement = input_["a priori refinement"]
        self.boundary_condition = input_["boundary condition"]
        self.calculate_energy = input_["calculate energy"]
        self.calculate_forces = input_["calculate forces"]
        self.charge_discretization = input_["charge discretization"]
        self.domain_length = input_["domain length"]
        self.error_based_refinement = input_["error based refinement"]
        self.error_tolerance = input_["error tolerance"]
        self.equation = input_["equation"]
        self.ions = MobileIons(dict_=input_["ions"])
        self.initial_mesh_resolution = input_["initial mesh resolution"]
        self.initial_mesh_vertices = input_["initial mesh vertices"]
        self.maximum_refinement_iterations = input_[
            "maximum refinement iterations"
        ]
        self.maximum_vertices = input_["maximum vertices"]
        self.molecule = input_["molecule"]
        self.solute_dielectric = input_["solute dielectric"]
        self.solvent_dielectric = input_["solvent dielectric"]
        self.solvent_radius = input_["solvent radius"]
        self.surface_method = input_["surface method"]
        self.surface_spline_window = input_["surface spline window"]
        self.temperature = input_["temperature"]
        self.use_maps = [UseMap(dict_=dict_) for dict_ in input_["use maps"]]
        self.write_atom_potentials = input_["write atom potentials"]
        self.write_maps = [
            WriteMap(dict_=dict_) for dict_ in input_["write maps"]
        ]

    def to_dict(self) -> dict:
        """Dump dictionary from object."""
        dict_ = {}
        dict_["a priori refinement"] = self.a_priori_refinement
        dict_["boundary condition"] = self.boundary_condition
        dict_["calculate energy"] = self.calculate_energy
        dict_["calculate forces"] = self.calculate_forces
        dict_["charge discretization"] = self.charge_discretization
        dict_["domain length"] = self.domain_length
        dict_["error based refinement"] = self.error_based_refinement
        dict_["error tolerance"] = self.error_tolerance
        dict_["equation"] = self.equation
        dict_["ions"] = self.ions.to_dict()
        dict_["initial mesh resolution"] = self.initial_mesh_resolution
        dict_["initial mesh vertices"] = self.initial_mesh_vertices
        dict_[
            "maximum refinement iterations"
        ] = self.maximum_refinement_iterations
        dict_["maximum vertices"] = self.maximum_vertices
        dict_["molecule"] = self.molecule
        dict_["solute dielectric"] = self.solute_dielectric
        dict_["solvent dielectric"] = self.solvent_dielectric
        dict_["solvent radius"] = self.solvent_radius
        dict_["surface method"] = self.surface_method
        dict_["surface spline window"] = self.surface_spline_window
        dict_["temperature"] = self.temperature
        dict_["use maps"] = [map_.to_dict() for map_ in self.use_maps]
        dict_["write atom potentials"] = self.write_atom_potentials
        dict_["write maps"] = [map_.to_dict() for map_ in self.write_maps]
        return dict_

    def validate(self):
        """Validate this object.

        Assumes that all attributes have been set via setters.

        :raises ValueError:  if object is invalid.
        """
        for map_ in self.use_maps + self.write_maps:
            map_.validate()
        self.ions.validate()
        if self.initial_mesh_vertices > self.maximum_vertices:
            raise ValueError(
                f"Initial mesh vertices {self.initial_mesh_vertices} setting "
                f"is greater than maximum mesh vertices setting "
                f"{self.maximum_vertices}."
            )

    @property
    def write_maps(self) -> list:
        """Write out maps related to computed properties.

        See :class:`WriteMap` for more information.

        :raises TypeError:  if set to wrong type
        """
        return self._write_maps

    @write_maps.setter
    def write_maps(self, value):
        if not check.is_list(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a list."
            )
        for elem in value:
            if not isinstance(elem, WriteMap):
                raise TypeError(
                    f"Value {elem} (type {type(elem)}) is not a WriteMap "
                    f"object."
                )
            self._write_maps.append(elem)

    @property
    def write_atom_potentials(self) -> str:
        """Write out the electrostatic potential at each atom location.

        Write out text file with potential at center of atom in units of
        :math:`k_b \\, T \\, e_c^{-1}`.

        .. note::

           These numbers are meaningless by themselves due to the presence of
           "self-energy" terms that are sensitive to grid spacing and position.
           These numbers should be evaluated with respect to a reference
           calculation:  the potentials from that reference calculation should
           be subtracted from the target system.  For example, one calculation
           might include a molecule with a heterogeneous dielectric coefficient
           and the reference system might be exactly the same system setup but
           with a homeogeneous dielectric coefficient.  If the results from the
           reference calculation are substracted from the first calculation,
           then the result will be a physically meaningful reaction field
           potential. However, the results from the first and reference
           calculations are meaningless by themselves.

        :returns:  path to text file for writing atom potential values.
        :raises TypeError:  if not set to string
        """
        return self._write_atom_potentials

    @write_atom_potentials.setter
    def write_atom_potentials(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        self._write_atom_potentials = value

    @property
    def use_maps(self) -> list:
        """Information for (optionally) using maps read into APBS.

        :returns:  list of :class:`UseMap` objects
        :raises TypeError:  if not a list of :class:`UseMap` objects
        """
        return self._use_maps

    @use_maps.setter
    def use_maps(self, value):
        if not check.is_list(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a list."
            )
        for elem in value:
            if not isinstance(elem, UseMap):
                raise TypeError(
                    f"List contains element {elem} of type {type(elem)}."
                )
            self._use_maps.append(elem)

    @property
    def temperature(self) -> float:
        """Temperature for the calculation in Kelvin.

        :raises ValueError:  if not a positive number (no violations of the
            3rd Law!)
        """
        return self._temperature

    @temperature.setter
    def temperature(self, value):
        if check.is_positive_definite(value):
            self._temperature = value
        else:
            raise ValueError(f"{value} is not a positive number.")

    @property
    def surface_spline_window(self) -> float:
        """Window for spline-based surface definitions (not needed otherwise).

        This is the distance (in Å) over which the spline transitions from the
        solvent dielectric value to the solute dielectric value.  A typical
        value is 0.3 Å.

        :returns:  positive number
        :raises TypeError:  if not a positive number
        """
        return self._surface_spline_window

    @surface_spline_window.setter
    def surface_spline_window(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        self._surface_spline_window = value

    @property
    def surface_method(self) -> str:
        """Method used to defined solute-solvent interface.

        One of the following values:

        * ``molecular surface``:  The dielectric coefficient is defined based
          on a molecular surface definition. The problem domain is
          divided into two spaces. The "free volume" space is defined by the
          union of solvent-sized spheres (see :func:`solvent_radius`) which do
          not overlap with the solute atoms. This free volume is assigned bulk
          solvent dielectric values. The complement of this space is assigned
          solute dielectric values. When the solvent radius is set to zero,
          this method corresponds to a van der Waals surface definition. The
          ion-accessibility coefficient is defined by an "inflated" van der
          Waals model. Specifically, the radius of each biomolecular atom is
          increased by the radius of the ion species (as specified with the
          :func:`ion` property). The problem domain is then divided into two
          spaces. The space inside the union of these inflated atomic spheres
          is assigned an ion-accessibility value of 0; the complement space is
          assigned the bulk ion accessibility value. See Connolly ML, J Appl
          Crystallography 16 548-558, 1983 (`10.1107/S0021889883010985
          <https://doi.org/10.1107/S0021889883010985>`_).

        * ``smoothed molecular surface``:  The dielectric and ion-accessibility
          coefficients are defined as for the ``molecular surface`` (see
          above). However, they are then "smoothed" by a 9-point harmonic
          averaging to somewhat reduce sensitivity to the grid setup. See
          Bruccoleri et al. J Comput Chem 18 268-276, 1997
          (`10.1007/s00214-007-0397-0
          <http://dx.doi.org/10.1007/s00214-007-0397-0>`_).

        * ``cubic spline``:  The dielectric and ion-accessibility coefficients
          are defined by a cubic-spline surface as described by Im et al,
          Comp Phys Commun 111 (1-3) 59-75, 1998
          (`10.1016/S0010-4655(98)00016-2
          <https://doi.org/10.1016/S0010-4655(98)00016-2>`_). The width of the
          dielectric interface is controlled by the :func:`spline_window`
          property. These spline-based surface definitions are very stable
          with respect to grid parameters and therefore ideal for calculating
          forces. However, they require substantial reparameterization of the
          force field; interested users should consult Nina et al, Biophys
          Chem 78 (1-2) 89-96, 1999 (`10.1016/S0301-4622(98)00236-1
          <http://dx.doi.org/10.1016/S0301-4622(98)00236-1>`_). Additionally,
          these surfaces can generate unphysical results with non-zero ionic
          strengths.

        * ``septic spline``: The dielectric and ion-accessibility coefficients
          are defined by a 7th order polynomial. This surface definition has
          characteristics similar to the cubic spline, but provides higher
          order continuity necessary for stable force calculations with atomic
          multipole force fields (up to quadrupole).

        :raises TypeError:  if not set to a string
        :raises ValueError:  if set to invalid value
        """
        return self._surface_method

    @surface_method.setter
    def surface_method(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)} is not a string."
            )
        value = value.lower()
        if value not in [
            "molecular surface",
            "smoothed molecular surface",
            "cubic spline",
            "septic spline",
        ]:
            raise ValueError(f"Value {value} is an invalid surface method.")
        self._surface_method = value

    @property
    def solvent_radius(self) -> float:
        """Radius of the solvent molecules.

        This parameter is used to define various solvent-related surfaces and
        volumes (see :func:`surface_method`). This value is usually set to 1.4
        Å for a water-like molecular surface and set to 0 Å for a van der Waals
        surface.

        :raises ValueError:  if value is not a non-negative number
        """
        return self._solvent_radius

    @solvent_radius.setter
    def solvent_radius(self, value):
        if check.is_positive_semidefinite(value):
            self._solvent_radius = value
        else:
            raise ValueError(f"{value} is not a non-negative number.")

    @property
    def solvent_dielectric(self) -> float:
        """Solvent dielectric.

        78.5 is a good choice for water at 298 K.

        :returns:  a floating point number greater than or equal to one
        :raises TypeError:  if not a number
        :raises ValueError:  if not greater than or equal to 1
        """
        return self._solvent_dielectric

    @solvent_dielectric.setter
    def solvent_dielectric(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        if value < 1:
            raise ValueError(f"Value {value} is not >= 1.")
        self._solvent_dielectric = value

    @property
    def solute_dielectric(self) -> float:
        """Solute dielectric.

        The dielectric value of a solute is often chosen using the following
        rules of thumb:

        * 1:  only used to compare calculation results with non-polarizable
          molecular simulation

        * 2-4:  "molecular" dielectric value; used when conformational degrees
          of freedom are modeled explicitly

        * 4-8:  used to mimic sidechain libration and other small-scale
          conformational degrees of freedom

        * 8-12:  used to model larger-scale sidechain rearrangement

        * 20-40:  used to model larger-scale macromolecular conformational
          changes and/or water penetration into interior of molecule

        .. note::

           What does the continuum dielectric value of a non-continuum molecule
           mean?  Hard to say -- this approximation can be very difficult to
           interpret and can significant affect your results.

        :returns:  a floating point number greater than or equal to one
        :raises TypeError:  if not a number
        :raises ValueError:  if not greater than or equal to 1
        """
        return self._solute_dielectric

    @solute_dielectric.setter
    def solute_dielectric(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        if value < 1:
            raise ValueError(f"Value {value} is not >= 1.")
        self._solute_dielectric = value

    @property
    def molecule(self) -> str:
        """Specify which molecule to use for calculation.

        :returns:  alias for molecule read (see :ref:`read_new_input`)
        :raises TypeError:  if not set to a string
        """
        return self._molecule

    @molecule.setter
    def molecule(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        self._molecule = value

    @property
    def maximum_vertices(self) -> int:
        """Maximum number of vertices produced in solve-estimate-refine
        iterations.

        The solve-estimate-refine loop will continue until the mesh has more
        than this number of vertices or :func:`maximum_refinement_iterations`
        has been reached.

        .. todo::  add validation step to make sure this number is greater than
                   or equal to the initial number of vertices.

        :raises TypeError: if not sent to positive integer
        """
        return self._maximum_vertices

    @maximum_vertices.setter
    def maximum_vertices(self, value):
        if not isinstance(value, int):
            raise TypeError(f"{value} (type {type(value)}) is not an integer.")
        if value < 1:
            raise TypeError(f"{value} is not a positive integer.")
        self._maximum_vertices = value

    @property
    def maximum_refinement_iterations(self) -> int:
        """Maximum number of solve-estimate-refine iterations.

        The solve-estimate-refine loop will continue until this number of
        iterations is reached or the mesh has more than
        :func:`maximum_vertices` vertices.

        :raises TypeError: if not sent to positive integer
        """
        return self._maximum_refinement_iterations

    @maximum_refinement_iterations.setter
    def maximum_refinement_iterations(self, value):
        if not isinstance(value, int):
            raise TypeError(f"{value} (type {type(value)}) is not an integer.")
        if value < 1:
            raise TypeError(f"{value} is not a positive integer.")
        self._maximum_refinement_iterations = value

    @property
    def initial_mesh_vertices(self) -> int:
        """Target number of vertices in initial mesh.

        Initial refinement will continue until this number of vertices is
        exceeded or :func:`initial_mesh_resolution` is reached.

        :raises TypeError:  if not set to a positive integer
        """
        return self._initial_mesh_vertices

    @initial_mesh_vertices.setter
    def initial_mesh_vertices(self, value):
        if not isinstance(value, int):
            raise TypeError(f"{value} (type {type(value)}) is not an integer.")
        if value < 1:
            raise TypeError(f"{value} is not a positive integer.")
        self._initial_mesh_vertices = value

    @property
    def initial_mesh_resolution(self) -> float:
        """Target spacing (in Å) of initial mesh.

        Initial refinement will continue until this target spacing is met or
        until :func:`initial_mesh_vertices` is exceeded.

        :raises TypeError:  if not set to a positive number
        """
        return self._initial_mesh_resolution

    @initial_mesh_resolution.setter
    def initial_mesh_resolution(self, value):
        if not check.is_positive_definite(value):
            raise ValueError(f"Value {value} is not a positive number.")
        self._initial_mesh_resolution = value

    @property
    def ions(self) -> MobileIons:
        """Descriptions of mobile ion species.

        :raises TypeError:  if not set to a :class:`Ions` object
        """
        _LOGGER.debug(f"self.ions = {self._ions}.")
        if self._ions is None:
            raise TypeError("Ions are set to None.")
        return self._ions

    @ions.setter
    def ions(self, value):
        if not isinstance(value, MobileIons):
            raise TypeError(
                f"Value {value} (type {type(value)} is not an Ions object."
            )
        self._ions = value

    @property
    def domain_length(self) -> list:
        """Length of rectangular prism domain.

        :returns:  list of non-zero lengths for x, y, and z directions.
        :raises  TypeError: if not list of positive definite numbers.
        :raises IndexError:  if list not length 3
        """
        return self._domain_length

    @domain_length.setter
    def domain_length(self, value):
        if not check.is_list(value):
            raise TypeError(f"{value} (type {type(value)}) is not a list.")
        if len(value) != 3:
            raise IndexError(f"{value} does not have length 3.")
        for elem in value:
            if not check.is_positive_definite(elem):
                raise TypeError(f"{elem} is not a positive number.")
        self._domain_length = value

    @property
    def a_priori_refinement(self) -> str:
        """Strategy for refining the initial very coarse 8-tetrahedron initial
        finite element to a resolution suitable for the main
        solve-estimate-refine iteration.

        Allowed values include:

        * ``geometric``:  geometry-based refinement at molecular surface and
          charges
        * ``uniform``:  uniform refinement of the mesh

        :raises ValueError:  if not set to allowed value
        :raises TypeError:  if not set to string
        """
        return self._a_priori_refinement

    @a_priori_refinement.setter
    def a_priori_refinement(self, value):
        if not check.is_string(value):
            raise TypeError(f"{value} (type {type(value)}) is not a string.")
        value = value.lower()
        if value not in ["geometric", "uniform"]:
            raise ValueError(f"{value} is not a recognized value.")
        self._a_priori_refinement = value

    @property
    def boundary_condition(self) -> str:
        """Boundary condition for Poisson-Boltzmann equation.

        This property can have one of the following values:

        * ``zero``:  Dirichlet condition where the potential at the boundary is
          set to zero. This condition is not commonly used and can result in
          large errors if used inappropriately.

        * ``single sphere``:  Dirichlet condition where the potential at the
          boundary is set to the values prescribed by a Debye-Hückel model for
          a single sphere with a point charge, dipole, and quadrupole. The
          sphere radius in this model is set to the radius of the biomolecule
          and the sphere charge, dipole, and quadrupole are set to the total
          moments of the protein. This condition works best when the boundary
          is sufficiently far (multiple Debye lengths) from the biomolecule.

        * ``multiple sphere``:  Dirichlet condition where the potential at the
          boundary is set to the values prescribed by a Debye-Hückel model for
          multiple, non-interacting spheres with a point charges. The radii of
          the non-interacting spheres are set to the atomic radii of and the
          sphere charges are set to the atomic charges. This condition works
          better than ``single sphere`` for closer boundaries but can be very
          slow for large biomolecules.

        * ``focus`` :c:var:`alias`:  Dirichlet condition where the potential at
          the boundary is set to the values computed by a previous (usually
          lower-resolution) PB calculation with alias :c:var:`alias`. All of
          the boundary points should lie within the domain of the previous
          calculation for best accuracy; if any boundary points lie outside,
          their values are computed using the ``single sphere`` Debye-Hückel
          boundary condition (see above).

        :raises ValueError:  if set to an invalid boundary type
        :raise IndexError:  if an insufficient number of words are present
        """
        return self._boundary_condition

    @boundary_condition.setter
    def boundary_condition(self, value):
        words = value.split()
        if words[0] == "zero":
            self._boundary_condition = "zero"
        elif words[0] == "single" and words[1] == "sphere":
            self._boundary_condition = "single sphere"
        elif words[0] == "multiple" and words[1] == "sphere":
            self._boundary_condition = "multiple sphere"
        elif words[0] == "focus":
            self._boundary_condition = " ".join(words)
        else:
            raise ValueError(f"Unknown boundary condition: {value}.")

    @property
    def calculate_energy(self) -> bool:
        """Indicate whether energy should be calculated.

        :raises TypeError:  if not Boolean
        """
        return self._calculate_energy

    @calculate_energy.setter
    def calculate_energy(self, value):
        if check.is_bool(value):
            self._calculate_energy = value
        else:
            raise ValueError(f"{value} is not Boolean.")

    @property
    def calculate_forces(self) -> bool:
        """Indicate whether forces should be calculated.

        :raises TypeError:  if not Boolean
        """
        return self._calculate_forces

    @calculate_forces.setter
    def calculate_forces(self, value):
        if check.is_bool(value):
            self._calculate_forces = value
        else:
            raise ValueError(f"{value} is not Boolean.")

    @property
    def charge_discretization(self) -> str:
        """The method by which the biomolecular point charges (i.e., Dirac
        delta functions) by which charges are mapped to the grid used for the
        finite difference calculation.

        As we are attempting to model delta functions, the support (domain) of
        these discretized charge distributions is always strongly dependent on
        the grid spacing.

        The following types of discretization are supported:

        * ``linear``: Traditional trilinear interpolation (linear splines). The
          charge is mapped onto the nearest-neighbor grid points. Resulting
          potentials are very sensitive to grid spacing, length, and position.

        * ``cubic``: Cubic B-spline discretization. The charge is mapped onto
          the nearest- and next-nearest-neighbor grid points. Resulting
          potentials are somewhat less sensitive (than ``linear``) to grid
          spacing, length, and position.

        * ``quintic``: Quintic B-spline discretization. Similar to ``cubic``,
          except the charge/multipole is additionally mapped to include
          next-next-nearest neighbors (125 grid points receive charge density).

        :raises TypeError:  if not set to string
        :raises ValueError:  if not one of the allowed values above
        """
        return self._charge_discretization

    @charge_discretization.setter
    def charge_discretization(self, value):
        if not check.is_string(value):
            raise TypeError(f"{value} (type {type(value)}) is not a string.")
        value = value.lower()
        if value not in ["linear", "cubic", "quintic"]:
            raise ValueError(f"{value} is not an allowed value.")
        self._charge_discretization = value

    @property
    def error_based_refinement(self) -> str:
        """Specify error-based refinement strategy for simplices.

        Can be assigned one of the following values:

        * ``global``:  this refines simplices until the global error is less
          than the amount specified by :func:`error_tolerance`.
        * ``simplex``:  this refines simplices until the per-simplex error is
          less than the amount specified by :func:`error_tolerance`.
        * ``fraction``:  this refines the specified fraction of simplices
          with the largest per-simplex error.  The fraction is specified by
          :func:`error_tolerance`.

        :raises TypeError:  if not set to string
        :raises ValueError:  if not set to allowed value
        """
        return self._error_based_refinement

    @error_based_refinement.setter
    def error_based_refinement(self, value):
        if not check.is_string(value):
            raise TypeError(f"{value} (type {type(value)} is not a string.")
        value = value.lower()
        if value not in ["global", "simplex", "fraction"]:
            raise ValueError(f"Invalid value:  {value}.")
        self._error_based_refinement = value

    @property
    def error_tolerance(self) -> float:
        """Error tolerance for error-based refinement of simplices.

        The meaning of this property changes based on the setting of
        :func:`error_based_refinement`.

        raises TypeError:  if not a positive number.
        """
        return self._error_tolerance

    @error_tolerance.setter
    def error_tolerance(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"{value} is not a positive number.")
        self._error_tolerance = value

    @property
    def equation(self) -> str:
        """Specifies which version of the Poisson-Boltzmann equation (PBE) to
        solve:

        * Most users should use one of these:

          * ``linearized pbe``
          * ``nonlinear pbe``

        * These versions are experimental and unstable:

          * ``linearized regularized pbe``
          * ``nonlinear regularized pbe``

        .. todo:: Confirm that all of these forms of the PBE work for FEM.

        :raises TypeError:  if not set to a string.
        :raises ValueError:  if set to an invalid value
        """
        return self._equation

    @equation.setter
    def equation(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        value = value.lower()
        if value not in [
            "linearized pbe",
            "nonlinear pbe",
            "linearized regularized pbe",
            "nonlinear regularized pbe",
        ]:
            raise ValueError(f"Value {value} is invalid.")
        self._equation = value
