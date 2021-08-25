"""Parameters for a boundary-element polar solvation calculation."""
import logging
from math import log2
from .. import check
from .. import InputFile
from .generic import MobileIons, WriteMap


_LOGGER = logging.getLogger(__name__)


class Mesh(InputFile):
    """Boundary element mesh input.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``software``:  software to use when generating the mesh; see
      :func:`software`
    * ``solvent radius``:  see :func:`solvent_radius`.
    * ``surface density``:  see :func:`surface_density`.
    * ``surface method``:  see :func:`surface_method`.

    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._software = None
        self._solvent_radius = None
        self._surface_density = None
        self._surface_method = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def software(self) -> str:
        """Software for generating mesh.

        One of the following values:

        * ``nanoshaper``:  `NanoShaper software
          <https://www.electrostaticszone.eu/downloads>`

        .. note::  The user is responsible for downloading the software and
           ensuring that it is available in their path.

        :raises TypeError:  if not set to a string
        :raises ValueError:  if set to an invalid value
        """
        return self._software

    @software.setter
    def software(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        value = value.lower()
        if value not in ["nanoshaper"]:
            raise ValueError(f"{value} is not a valid value.")
        self._software = value

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
    def surface_density(self) -> float:
        """Number of points per area on surface.

        Units are number per Å\ :superscript:`2` and are used in calculation of
        surface terms (e.g., molecular surface, solvent accessible surface).
        This keyword is ignored when :func:`surface_radius` is 0.0 (e.g., for
        van der Waals surfaces) or if :func:`surface method` refers to
        splines.

        A typical value is 10.0.

        .. todo:: I am not sure how TABI uses this value with NanoShaper

        :raises ValueError:  if value is not a positive number
        """
        return self._surface_density

    @surface_density.setter
    def surface_density(self, value):
        if check.is_positive_definite(value):
            self._surface_density = value
        else:
            raise ValueError(f"{value} is not a positive number.")

    @property
    def surface_method(self) -> str:
        """Method to compute molecular surface (boundary).

        One of the following:

        * ``molecular surface``:  The problem domain is divided into two
          spaces with the boundary defined as the interface between these
          spaces. The "free volume" space is defined by the union of
          solvent-sized spheres (see :func:`solvent_radius`) which do not
          overlap with the solute atoms. This free volume is assigned bulk
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
        * ``skin``:  Edelsbrunner molecular skin as defined in Edelsbrunner, H.
           “Deformable Smooth Surface Design.” Discrete and Computational
           Geometry 21, no. 1 (January 1, 1999): 87–115.
           DOI:`10.1007/PL00009412 <https://doi.org/10.1007/PL00009412>`_.

        :raises TypeError:  if not set to string.
        :raises ValueError:  if not set to valid value.
        """
        return self._surface_method

    @surface_method.setter
    def surface_method(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)} is not a string."
            )
        value = value.lower()
        if value not in ["molecular surface", "skin"]:
            raise ValueError(f"{value} is not a valid value.")
        self._surface_method = value


class TABIParameters(InputFile):
    """Parameters for the TABI solver.

    "TABI" is the the Treecode-Accelerated Boundary Integral (TABI) solver. For
    more information, see the Geng & Krasny `2013 J Comput Phys paper
    <https://doi.org/10.1016/j.jcp.2013.03.056>`_.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``maximum particles``:  the maximum number of particles in a tree leaf;
      see :func:`maximum_particles`.
    * ``multipole acceptance criterion``:  see
      :func:`multipole_acceptance_criterion`.
    * ``tree order``:  see :func:`tree_order`.

    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._maximum_particles = None
        self._multipole_acceptance_criterion = None
        self._tree_order = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def tree_order(self) -> int:
        """Specifies the order of the treecode multipole expansion.

        This is an integer that indicates the Taylor expansion order. Users can
        adjust the order for different accuracy.
        
        A typical choice for this parameter is 3.

        :raises TypeError:  if not an integer
        :raises ValueError:  if not a positive number.
        """
        return self._tree_order

    @tree_order.setter
    def tree_order(self, value):
        if not isinstance(value, int):
            raise TypeError(
                f"Value {value} (type {type(value)} is not an integer."
            )
        if not check.is_positive_definite(value):
            raise ValueError(f"Value {value} is not positive.")
        self._tree_order = value

    @property
    def multipole_acceptance_criterion(self) -> float:
        """The multipole acceptance criterion (MAC) controls the distance ratio
        at which the method uses direct summation or Taylor approximation
        (a particle-cluster interaction) to calculate the integral kernels.

        The MAC is related to cluster characteristics by
        :math:`\\frac{r_c}{R}\leqslant \\theta`, where :math:`r_c` is the
        cluster radius, :math:`R` is the distance of the particle to the
        cluster center, and :math:`0 < \\theta < 1`.
        If the this relationship is satisfied, then the Taylor approximation
        will be used instead of direct summation.

        A typical value for this parameter is 0.8.

        :raises ValueError:  if not a positive number less than 1.
        """
        return self._multipole_acceptance_criterion

    @multipole_acceptance_criterion.setter
    def multipole_acceptance_criterion(self, value):
        if not check.is_positive_definite(value):
            raise ValueError(
                f"Value {value} (type {type(value)}) is not a positive number."
            )
        if value >= 1:
            raise ValueError(f"Value {value} is not less than 1.")
        self._multipole_acceptance_criterion = value

    @property
    def maximum_particles(self) -> int:
        """The maximum number of particles in the tree-code leaf.

        This controls leaf size in the process of building the tree structure.
        A typical value for this parameter is 500.

        :raises ValueError:  if not set to a positive number
        :raise TypeError:  if not set to an integer
        """
        return self._maximum_particles

    @maximum_particles.setter
    def maximum_particles(self, value):
        if not check.is_positive_definite(value):
            raise ValueError(f"Value {value} is not a positive number.")
        if not isinstance(value, int):
            raise TypeError(
                f"Value {value} (type {type(value)} is not an integer."
            )


class BoundaryElement(InputFile):
    """Parameters for a boundary element linearized Poisson-Boltzmann polar
    solvation calculation.

    Boundary element methods offer the ability to focus numerical effort on a
    much smaller region of the problem domain:  the interface between the
    molecule and the solvent.  In this method, two coupled integral equations
    defined on the solute-solvent boundary define a mathematical relationship
    between the electrostatic surface potential and its normal derivative with
    a set of integral kernels consisting of Coulomb and screened Coulomb
    potentials with their normal derivatives. The boundary element method
    requires a surface triangulation, generated by a program such as
    `NanoShaper <https://www.electrostaticszone.eu/downloads>`_, on which to
    discretize the integral equations.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``calculate energy``:  see :func:`calculate_energy`
    * ``calculate forces``:  see :func:`calculate_forces`
    * ``error tolerance``:  solver error tolerance; see :func:`error_tolerance`
    * ``ions``:  information about mobile ion species; see :func:`ions`
    * ``molecule``:  alias to molecule for calculation; see :func:`molecule`
    * ``solute dielectric``:  see :func:`solute_dielectric`
    * ``solvent dielectric``:  see :func:`solvent_dielectric`
    * ``solver``:  see :func:`solver`
    * ``solver parameters``:  see func:`solver_parameters`
    * ``temperature``:  see :func:`temperature`
    * ``mesh``:  specify how the mesh is obtained; see :func:`mesh`
    * ``write atom potentials``:  write out atom potentials; see
        :func:`write_atom_potentials`

    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._calculate_energy = None
        self._calculate_forces = None
        self._error_tolerance = None
        self._ions = None
        self._mesh = None
        self._molecule = None
        self._solute_dielectric = None
        self._solvent_dielectric = None
        self._solver = None
        self._solver_parameters = None
        self._temperature = None
        self._write_atom_potentials = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def validate(self):
        if self.solver == "tabi":
            if not isinstance(self.solver_parameters, TABIParameters):
                raise TypeError(
                    f"Solver parameters (type {type(self.solver_parameters)})"
                    f" are not from the TABIParameters class."
                )
        raise NotImplementedError()

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
    def solver_parameters(self) -> InputFile:
        """Parameters for specified solver.

        :returns:  an object derived from :class:`InputFile` based on the
            :func:`solver` property:

            * ``tabi``:  returns object of type :class:`TABIParameters`

        :raises TypeError:  if object is not derived from :class:`InputFile`
        """
        return self._solver_parameters

    @solver_parameters.setter
    def solver_parameters(self, value):
        if not isinstance(value, InputFile):
            raise TypeError(
                f"Value {value} (type {type(value)} is not derived from "
                f"InputClass."
            )
        self._solver_parameters = value

    @property
    def solver(self) -> str:
        """Boundary element solver.
        
        Allowed values are one of the following:
        
        * ``tabi``:  the Treecode-Accelerated Boundary Integral (TABI) solver.
          For more information, see the Geng & Krasny `2013 J Comput Phys paper
          <https://doi.org/10.1016/j.jcp.2013.03.056>`_.
        
        Each value must be accompanied by the corresponding solver parameters,
        set via :func:`solver_parameters`.

        :raises TypeError:  if not set to string.
        :raises ValueError:  if set to invalid string.
        """
        return self._solver

    @solver.setter
    def solver(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        value = value.lower()
        if value not in ["tabi"]:
            raise ValueError(f"Value {value} is not an allowed value.")
        self._solver = value

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
    def mesh(self) -> Mesh:
        """Parameters for the boundary mesh.

        :raises TypeError:  if not set to :class:`Mesh` object.
        """
        return self._mesh

    @mesh.setter
    def mesh(self, value):
        if not isinstance(value, Mesh):
            raise TypeError(
                f"Value {value} (type {type(value)} is not class Mesh."
            )
        self._mesh = value

    @property
    def ions(self) -> MobileIons:
        """Descriptions of mobile ion species.

        :raises TypeError:  if not set to a :class:`Ions` object
        """
        return self._ions

    @ions.setter
    def ions(self, value):
        if not isinstance(value, MobileIons):
            raise TypeError(
                f"Value {value} (type {type(value)} is not an Ions object."
            )
        self._ions = value

    @property
    def error_tolerance(self) -> float:
        """Relative error tolerance for iterative solver.

        If not specified, the default value is :const:`ERROR_TOLERANCE`.

        :raises TypeError:  if not set to positive number
        :raises ValueError:  if not set to number less than 1
        """
        return self._error_tolerance

    @error_tolerance.setter
    def error_tolerance(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        if value >= 1.0:
            raise ValueError(f"Value {value} is not less than 1.0.")
        self._error_tolerance = value

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
