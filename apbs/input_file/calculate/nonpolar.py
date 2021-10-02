"""Parameters for a grid-based nonpolar solvation calculation."""
import logging
from .. import check
from .. import InputFile

_LOGGER = logging.getLogger(__name__)


class Nonpolar(InputFile):
    """Parameters for a grid-based nonpolar solvation calculation.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``calculate energy``:  see :func:`calculate_energy`
    * ``calculate forces``:  see :func:`calculate_forces`
    * ``displacement``:  finite difference displacement for force calculation
    * ``grid spacings``:  grid spacings for integral calculation; see
      :func:`grid_spacings`
    * ``molecule``:  alias string for molecule to use in calculation; see
      :func:`molecule`
    * ``pressure``:  solvent hard sphere pressure; see :func:`pressure`
    * ``solvent density``:  see :func:`solvent_density`
    * ``solvent radius``:  see :func:`solvent_radius`
    * ``surface density``:  density of points to use for surface integrals;
      see :func:`surface_density`
    * ``surface method``:  method used to calculate the solvent-solute
      interface; see :func:`surface_method`
    * ``surface tension``: see :func:`surface_tension`
    * ``temperature``:  see :func:`temperature`

    APBS apolar calculations follow the very generic framework described in
    Wagoner JA, Baker NA. Assessing implicit models for nonpolar mean solvation
    forces: the importance of dispersion and volume terms. Proc Natl Acad Sci
    USA, 103, 8331-8336, 2006.
    doi:`10.1073/pnas.0600118103 <http://dx.doi.org/10.1073/pnas.0600118103>`_.

    Nonpolar solvation potentials of mean force (energies) are calculated
    according to:

    .. math::

       {W}^{(\mathrm{np})}(x) = \gamma A(x) + pV(x)
       + \\bar \\rho \sum^N_{i=1}
       \int _{\Omega} u_i^{(\mathrm{att})} (x_i, y) \\theta (x,y) \,
       \mathrm{d}y

    and mean nonpolar solvation forces are calculated according to:

    .. math::

       \mathbf{F}_i^{(\mathrm{np})}(x) =
       -\gamma \\frac{\partial A (x)}{\partial x_i}
       - p \int _{\Gamma _i (x)} \\frac{y-x_i}{\lVert y - x_i \\rVert} \,
       \mathrm{d}y - \\bar \\rho \sum _{i=1}^N \int _{\Omega}
       \\frac{\partial u_i^{(\mathrm{att})}(x_i,y)}{\partial x_i}
       \\theta (x,y) \, \mathrm{d}y

    In these equations, :math:`\gamma` is the repulsive (hard sphere) solvent
    surface tension (see :func:`surface_tension`), *A* is the
    conformation-dependent solute surface area (see :func:`solvent_radius` and
    :func:`surface_method`), *p* is the repulsive (hard sphere) solvent
    pressure (see :func:`pressure`), *V* is the conformation-dependent solute
    volume (see :func:`solvent_radius` and :func:`surface_method`),
    :math:`\\rho` (see :func:`solvent_density` keywords) is the bulk solvent
    density, and the integral involves the attractive portion (defined in a
    Weeks-Chandler-Andersen sense) of the Lennard-Jones interactions between
    the solute and the solvent integrated over the region of the problem domain
    outside the solute volume *V*. Lennard-Jones parameters are taken from APBS
    parameter files as read in through an APBS input file READ statement (see
    :ref:`read_new_input`).

    .. note::

       The above expressions can easily be reduced to simpler apolar solvation
       formalisms by setting one or more of the coefficients to zero through
       the keywords.

    .. warning::

       All APOLAR calculations require a parameter file which contains
       Lennard-Jones radius and well-depth parameters for all the atoms in the
       solute PDB. This parameter file must also contain radius and well-depth
       parameters for water (specifically: residue "WAT" and atom "OW").
       Complete parameter files for protein and nucleic acid parameters are not
       currently available; we prefer geometric flow calculations (coupled
       polar and apolar components) rather than this model.
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._calculate_energy = None
        self._calculate_forces = None
        self._displacement = None
        self._grid_spacings = []
        self._molecule = None
        self._pressure = None
        self._solvent_density = None
        self._solvent_radius = None
        self._surface_density = None
        self._surface_method = None
        self._surface_tension = None
        self._temperature = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def displacement(self) -> float:
        """Displacement used for finite difference calculation of force.

        Units are Å.

        :raises ValueError:  if not a number greater than zero
        """
        return self._displacement

    @displacement.setter
    def displacement(self, value):
        if check.is_positive_definite(value):
            self._displacement = value
        else:
            raise ValueError(f"{value} is not a number greater than zero.")

    @property
    def grid_spacings(self) -> tuple:
        """Grid spacings for integral quadrature.

        Units are Å.

        :raises ValueError:  if not a 3-tuple with elements greater than zero
        """
        return self._grid_spacings

    @grid_spacings.setter
    def grid_spacings(self, list_):
        if check.is_list(list_, 3):
            tuple_ = tuple(list_)
            for elem in tuple_:
                if not check.is_positive_definite(elem):
                    raise ValueError(
                        f"Grid spacings {tuple_} are not greater than zero."
                    )
            self._grid_spacings = tuple_
        else:
            raise ValueError(f"Grid spacings {list_} is not 3-tuple.")

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
    def molecule(self) -> str:
        """Alias for molecule to be used in calculation.

        :raises TypeError:  if alias is not string.
        """
        return self._molecule

    @molecule.setter
    def molecule(self, value):
        if check.is_string(value):
            self._molecule = value
        else:
            raise ValueError(f"{value} is not a string.")

    @property
    def pressure(self) -> float:
        """Hard-sphere pressure for solvent in kJ mol\ :superscript:`-1`
        Å\ :superscript:`-3`.

        This coefficient multiplies the volume term of the apolar model and can
        be set to zero to eliminate volume contributions to the apolar
        solvation calculation.

        See the documentation for a discussion of units for this property.

        :raises ValueError:  if not a non-negative number
        """
        return self._pressure

    @pressure.setter
    def pressure(self, value):
        if check.is_positive_semidefinite(value):
            self._pressure = value
        else:
            raise ValueError(f"{value} is not a non-negative number.")

    @property
    def solvent_density(self) -> float:
        """Bulk solvent density.

        A floating point number giving the bulk solvent density in
        Å\ :superscript:`-3`

        This coefficient multiplies the integral term of the apolar model
        discussed above and can be set to zero to eliminate integral
        contributions to the apolar solvation calculation.

        :raises ValueError:  if value is not a non-negative number
        """
        return self._solvent_density

    @solvent_density.setter
    def solvent_density(self, value):
        if check.is_positive_semidefinite(value):
            self._solvent_density = value
        else:
            raise ValueError(f"{value} is not a non-negative number.")

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
        """Number of quadrature points per area on surface.

        Units are number per Å\ :superscript:`2` and are used in calculation of
        surface terms (e.g., molecular surface, solvent accessible surface).
        This keyword is ignored when :func:`surface_radius` is 0.0 (e.g., for
        van der Waals surfaces) or if :func:`surface method` refers to
        splines.

        A typical value is 10.0.

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
        """Specifies the model used to construct the solvent-related surface
        and volume.

        This is a string with a value of:

        * ``solvent-accessible``:  solvent-accessible (also called
            "probe-inflated") surface and volume.  See Connolly ML, J Appl
            Crystallography 16 548-558, 1983 (`10.1107/S0021889883010985
            <https://doi.org/10.1107/S0021889883010985>`_).

        :raises ValueError:  if surface method is not a valid value
        :raises TypeError:  if surface method is not a string
        """
        return self._surface_method

    @surface_method.setter
    def surface_method(self, value):
        value = value.lower()
        if check.is_string(value):
            if value in ["solvent-accessible"]:
                self._surface_method = value
            else:
                raise ValueError(f"{value} is not a valid surface method.")
        else:
            raise TypeError(f"{value} is not a string.")

    @property
    def surface_tension(self) -> float:
        """Surface tension coefficient for apolar solvation models.

        The value is a floating point number designating the surface tension
        in units of kJ mol\ :superscript:`-1` Å\ :superscript:`-2`. This term
        can be set to zero to eliminate the :abbr:`SASA (solvent-accessible
        surface area)` contributions to the apolar solvation calculations.

        See the documentation for a discussion of units for this property.

        :raises ValueError:  if not a non-negative number
        """
        return self._surface_tension

    @surface_tension.setter
    def surface_tension(self, value):
        if check.is_positive_semidefinite(value):
            self._surface_tension = value
        else:
            raise ValueError(f"{value} is not a non-negative number.")

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

    def validate(self):
        """Validate the contents of this object.

        :raises ValueError:  if contents are invalid.
        """
        errors = []
        if self.calculate_energy is None:
            errors.append("calculate energy not set.")
        if self.calculate_forces is None:
            errors.append("calculate forces not set.")
        elif self.displacement is None:
            errors.append(
                "displacement not set and is required for force calculations."
            )
        if self.grid_spacings is None:
            errors.append("grid spacings are not set.")
        if self.molecule is None:
            errors.append("molecule is not set.")
        if self.pressure is None:
            errors.append("pressure is not set.")
        if self.solvent_density is None:
            errors.append("solvent density is not set.")
        if self.solvent_radius is None:
            errors.append("solvent radius is not set.")
        if self.surface_density is None:
            errors.append("surface density is not set.")
        if self.surface_method is None:
            errors.append("surface method is not set.")
        if self.surface_tension is None:
            errors.append("surface tension is not set.")
        if self.temperature is None:
            errors.append("temperature is not set.")

    def from_dict(self, input_):
        """Populate object from dictionary.

        :param dict input_: input dictionary
        :raises KeyError:  if input is missing keys
        """
        self.calculate_energy = input_["calculate energy"]
        self.calculate_forces = input_["calculate forces"]
        if self.calculate_forces:
            self.displacement = input_["displacement"]
        self.grid_spacings = input_["grid spacings"]
        self.molecule = input_["molecule"]
        self.pressure = input_["pressure"]
        self.solvent_density = input_["solvent density"]
        self.solvent_radius = input_["solvent radius"]
        self.surface_density = input_["surface density"]
        self.surface_method = input_["surface method"]
        self.surface_tension = input_["surface tension"]
        self.temperature = input_["temperature"]

    def to_dict(self) -> dict:
        dict_ = {}
        dict_["calculate energy"] = self.calculate_energy
        dict_["calculate forces"] = self.calculate_forces
        if self.calculate_forces:
            dict_["displacement"] = self.displacement
        dict_["grid spacings"] = self.grid_spacings
        dict_["molecule"] = self.molecule
        dict_["pressure"] = self.pressure
        dict_["solvent density"] = self.solvent_density
        dict_["solvent radius"] = self.solvent_radius
        dict_["surface density"] = self.surface_density
        dict_["surface method"] = self.surface_method
        dict_["surface tension"] = self.surface_tension
        dict_["temperature"] = self.temperature
        return dict_
