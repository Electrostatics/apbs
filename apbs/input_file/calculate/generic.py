"""Generic objects for polar solvation calculations.

This only contains common properties that are complex objects (e.g., not
strings, lists, etc.)
"""
import logging
from .. import check
from .. import InputFile


_LOGGER = logging.getLogger(__name__)


class Ion(InputFile):
    """Description of a single mobile ion species.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``charge``:  charge of ion; see :func:`charge`
    * ``radius``:  radius of ion; see :func:`radius`
    * ``concentration``:  concentration of ion species;
      see :func:`concentration`

    """

    def __init__(self, dict_=None, json=None, yaml=None):
        self._charge = None
        self._radius = None
        self._concentration = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def to_dict(self) -> dict:
        return {
            "charge": self.charge,
            "radius": self.radius,
            "concentration": self.concentration,
        }

    def from_dict(self, dict_):
        """Populate object from dictionary.

        :raises KeyError:  if dictionary missing information
        """
        self.charge = dict_["charge"]
        self.radius = dict_["radius"]
        self.concentration = dict_["concentration"]

    def validate(self):
        _ = self.charge
        _ = self.radius
        _ = self.concentration

    @property
    def charge(self) -> float:
        """The charge (in electrons) of the ion.

        :raises TypeError:  if set to something that is not a number
        """
        if self._charge is None:
            raise TypeError("None is not a number.")
        return self._charge

    @charge.setter
    def charge(self, value):
        if not check.is_number(value):
            raise TypeError(f"Value {value} is not a number.")
        self._charge = value

    @property
    def radius(self) -> float:
        """The radius (in Å) of the ion.

        :raises TypeError:  if set to something that is not a positive number
        """
        if self._radius is None:
            raise TypeError("None is not a number.")
        return self._radius

    @radius.setter
    def radius(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        self._radius = value

    @property
    def concentration(self) -> float:
        """Concentration (in M) of ion species.

        :raises TypeError:  if not a positive number or zero
        """
        if self._concentration is None:
            raise TypeError("None is not a number.")
        return self._concentration

    @concentration.setter
    def concentration(self, value):
        if not check.is_positive_semidefinite(value):
            raise TypeError(
                f"Value {value} is not a positive semi-definite number."
            )
        self._concentration = value


class MobileIons(InputFile):
    """Provide information about mobile ion species in system.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``species``:  list of ion species; see :func:`species`

    """

    def __init__(self, dict_=None, json=None, yaml=None):
        self._species = []
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def species(self) -> list:
        """List of mobile ion species.

        :returns:  list of :class:`Ion` objects
        :raises TypeError:  if set to something other than a list of
            :class:`Ion` objects
        """
        return self._species

    @species.setter
    def species(self, value):
        if not isinstance(value, (list, tuple)):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a list."
            )
        for elem in value:
            if not isinstance(elem, Ion):
                raise TypeError(
                    f"List element {elem} (type {type(elem)}) is not of class "
                    f"Ion."
                )
            self._species.append(elem)

    def from_dict(self, dict_):
        for elem in dict_.get("species", []):
            self._species.append(Ion(dict_=elem))

    def to_dict(self) -> dict:
        return {"species": [elem.to_dict() for elem in self.species]}

    def validate(self):
        net_charge = 0.0
        for ion in self.species:
            ion.validate()
            net_charge += ion.charge * ion.concentration
        if net_charge != 0.0:
            raise ValueError(
                f"The net mobile ion charge ({net_charge} e) is not zero."
            )


class WriteMap(InputFile):
    """Write the specified property to a map.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``property``:  what property is being written to the map; see
      :func:`property`
    * ``format``:  output format; see :func:`format`
    * ``path``:  a suggested path and file name for the map; see :func:`path`

    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._property = None
        self._format = None
        self._path = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def from_dict(self, input_):
        self.property = input_["property"]
        self.format = input_["format"]
        self.path = input_["path"]

    def to_dict(self) -> dict:
        return {
            "property": self.property,
            "format": self.format,
            "path": self.path,
        }

    def validate(self):
        errors = []
        if self.property is None:
            errors.append("property not set.")
        if self.format is None:
            errors.append("format not set.")
        if self.path is None:
            errors.append("path not set.")
        if errors:
            raise ValueError(" ".join(errors))

    @property
    def path(self) -> str:
        """Suggested path for writing results.

        This path is only a suggestion; if parallel calculations are performed,
        then the filename will be modified to include the processor number for
        the output.

        :raises TypeError:  if not set to string.
        """
        return self._path

    @path.setter
    def path(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        self._path = value

    @property
    def format(self) -> str:
        """Format for writing output.

        Allowed formats (see documentation for details) include:

        * ``dx``:  OpenDX-format data. This is the preferred format for APBS
          input/output.

        * ``dx.gz``: GZipped OpenDX-format data.

        * ``flat``: Write out data as a plain text file.

        * ``uhbd``:  UHBD-format data.

        :raises TypeError:  if not set to a strinng
        :raises ValueError:  if invalid format specified
        """
        return self._format

    @format.setter
    def format(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        value = value.lower()
        if value not in ["dx", "dx.gz", "flat", "uhbd"]:
            raise ValueError(f"Value {value} is not an allowed format.")
        self._format = value

    @property
    def property(self) -> str:
        """Property to write to map.

        See the documentation for a discussion of units for these properties.

        One of:

        * ``charge density``: Write out the biomolecular charge distribution in
          units of :math:`e_c` (electron charge) per Å\\ :sup:`3`.

        * ``potential``: Write out the electrostatic potential over the entire
          problem domain in units of :math:`k_b \\, T \\, e_c^{-1}`.

        * ``solvent accessibility``:  Write out the solvent accessibility
          defined by the molecular surface definition (see
          :func:`FiniteDifference.surface_definition`). Values are unitless and
          range from 0 (inaccessible) to 1 (accessible).

        * ``ion accessibility``: Write out the inflated van der Waals-based ion
          accessibility (see :func:`FiniteDifference.surface_definition`).
          Values are unitless and range from 0 (inaccessible) to 1
          (accessible).

        * ``laplacian``: Write out the Laplacian of the potential
          :math:`\\nabla^2 \\phi` in units of
          k\\ :sub:`B` T e\\ :sub:`c`\\ :sup:`-1` Å\\ :sup:`-2`.

        * ``energy density``:  Write out the "energy density"
          :math:`-\\nabla \\cdot \\epsilon \\nabla \\phi` in units of
          k\\ :sub:`B` T e\\ :sub:`c`\\ :sup:`-1` Å\\ :sup:`-2`.

        * ``ion number density``:  Write out the total mobile ion number
          density for all ion species in units of M. The output is calculated
          according to the formula (for nonlinear PB calculations):
          :math:`\\rho(x) =
          \\sum_i^N {\\bar{\\rho}_i e^{-q_i\\phi(x) - V_i (x)}}`, where
          :math:`N` is the number of ion species, :math:`\\bar{\\rho}_i` is the
          bulk density of ion species :math:`i`, :math:`q_i` is the charge of
          ion species :math:`i`, :math:`\\phi(x)` is the electrostatic
          potential, and :math:`V_i` is the solute-ion interaction potential
          for species :math:`i`.

        * ``ion charge density``: Write out the total mobile ion charge density
          for all ion species in units of e\\ :sub:`c` M. The output is
          calculated according to the formula (for nonlinear PB calculations):
          :math:`\\rho(x) = \\sum_i^N {\\bar{\\rho}_i q_i e^{-q_i\\phi(x) -
          V_i(x)}}`, where :math:`N` is the number of ion species,
          :math:`\\bar{\\rho}_i` is the bulk density of ion species :math:`i`,
          :math:`q_i` is the charge of ion species :math:`i`, :math:`\\phi(x)`
          is the electrostatic potential, and :math:`V_i` is the solute-ion
          interaction potential for species :math:`i`.

        * ``dielectric x`` or ``dielectric y`` or ``dielectric z``: Write out
          the dielectric map shifted by 1/2 grid spacing in the {``x``, ``y``,
          ``z``}-direction. The values are unitless.

        :raises TypeError:  if not a string
        :raises ValueError:  if invalid value
        """
        return self._property

    @property.setter
    def property(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        value = value.lower()
        if value not in [
            "charge density",
            "potential",
            "atom potential",
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
            raise ValueError(f"Property {value} is invalid.")
        self._property = value


class UseMap(InputFile):
    """Use a previously read in map.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``property``:  what property being loaded from the map; see
      :func:`property`
    * ``alias``:  alias assigned when reading in map; see :func:`alias`

    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._property = None
        self._alias = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def from_dict(self, input_):
        """Load object from dictionary.

        :raises KeyError:  if missing items
        """
        self.property = input_["property"]
        self.alias = input_["alias"]

    def to_dict(self) -> dict:
        return {"property": self.property, "alias": self.alias}

    def validate(self):
        errors = []
        if self.property is None:
            errors.append("property not set.")
        if self.alias is None:
            errors.append("alias not set.")
        if errors:
            raise ValueError(" ".join(errors))

    @property
    def property(self) -> str:
        """Specify the property being read from the map.

        One of the following values:

        * ``dielectric``: Dielectric function map (as read in
          :ref:`read_new_input`); this causes the
          :func:`FiniteDifference.solute_dielectric`,
          :func:`FiniteDifference.solvent_dielectric`,
          :func:`FiniteDifference.solvent_radius`,
          :func:`FiniteDifference.surface_method`, and
          :func:`FiniteDifference.surface spline window` properties to be
          ignored, along with the radii of the solute atoms.
          Note that :func:`FiniteDifference.solute_dielectric` and
          :func:`FiniteDifference.solvent_dielectric` are still used for some
          boundary condition calculations (see
          :func:`FiniteDifference.boundary_condition`)

        * ``ion accessibility``:  Mobile ion-accessibility function map (as
          read in :ref:`read_new_input`); this causes the
          :func:`FiniteDifference.surface_method`, and
          :func:`FiniteDifference.surface spline window` properties to be
          ignored, along with the radii of the solute atoms.  The
          :func:`FiniteDifference.ions` property is not ignored and will still
          be used.

        * ``charge density``:  Charge distribution map (as read in
          :ref:`read_new_input`); this causes the :func:`charge discretization`
          parameter and the charges of the biomolecular atoms to be ignored
          when assembling the fixed charge distribution for the
          Poisson-Boltzmann equation.

        * ``potential``:  Potential map (as read in :ref:`read_new_input`);
          this is used to set the boundary condition and causes the
          :func:`boundary_condition` property to be ignored.

        :raises TypeError:  if not string
        :raises ValueError:  if not valid value
        """
        return self._property

    @property.setter
    def property(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        value = value.lower()
        if value not in [
            "dielectric",
            "ion accessibility",
            "charge density",
            "potential",
        ]:
            raise ValueError(f"Value {value} is not valid.")
        self._property = value
