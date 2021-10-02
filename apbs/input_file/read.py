"""These classes provide information about data input for APBS.

.. todo::

   * Add mmCIF support for :class:`Molecule`
"""
import logging

# from typing import Type
from . import check
from . import InputFile


_LOGGER = logging.getLogger(__name__)


class DielectricMapGroup(InputFile):
    """Input class for a group of three dielectric maps.

    These three maps represent the dielectric function mapped to three meshes,
    shifted by one-half grid spacing in the x, y, and z directions.  The inputs
    are maps of dielectric variables between the solvent and biomolecular
    dielectric constants; these values are unitless.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``alias``:  see :func:`alias`
    * ``format``:  see :func:`format`
    * ``x-shifted path``: string with path to x-shifted map (see :func:`paths`)
    * ``y-shifted path``: string with path to y-shifted map (see :func:`paths`)
    * ``x-shifted path``: string with path to z-shifted map (see :func:`paths`)

    More information about these properties is provided below.
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._paths = None
        self._format = None
        self._alias = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def paths(self) -> tuple:
        """3-tuple of strings.

        Tuple contains paths to the x-, y-, and z-shifted dielectric maps.

        :raises IndexError:  if length of list is not 3
        :raises TypeError:  if list does not contain strings
        """
        return self._paths

    @paths.setter
    def paths(self, value):
        value = tuple(value)
        if len(value) != 3:
            raise IndexError(f"List has length {len(value)}; should be 3")
        for i, elem in enumerate(value):
            if not check.is_string(elem):
                self._paths[i] = elem
        self._paths = value

    @property
    def alias(self) -> str:
        """String used to refer to these maps elsewhere in the input file.

        :raises TypeError: if value is not string
        """
        return self._alias

    @alias.setter
    def alias(self, value):
        if check.is_string(value):
            self._alias = value
        else:
            raise TypeError(f"{value} is not a string")

    @property
    def format(self) -> str:
        """Format for scalar input map.

        One of:

        * ``dx``: :ref:`opendx`
        * ``dx.gz``:  GZip-compressed :ref:`opendx`

        :raises ValueError: if assigned incorrect format value
        """
        return self._format

    @format.setter
    def format(self, value):
        self._format = value.lower()
        if self._format not in ("dx", "dx.gz"):
            raise ValueError(f"{value} is not an allowed format.")

    def from_dict(self, dict_):
        """Load object contents from dictionary.

        :param dict dict_:  dictionary with object contents
        :raises KeyError:  if dictionary elements missing
        """
        try:
            self.alias = dict_["alias"]
            self.format = dict_["format"].lower()
            self.paths = (
                dict_["x-shifted path"],
                dict_["y-shifted path"],
                dict_["z-shifted path"],
            )
        except KeyError as err:
            err = f"Missing key {err} while parsing {dict_}."
            raise KeyError(err)

    def to_dict(self) -> dict:
        return {
            "alias": self._alias,
            "format": self._format,
            "x-shifted path": self._paths[0],
            "y-shifted path": self._paths[1],
            "z-shifted path": self._paths[2],
        }

    def validate(self):
        """Validate object.

        :raises ValueError:  if object invalid
        """
        errors = []
        if self.paths is None:
            errors.append("Paths have not been set.")
        if self.format is None:
            errors.append("Format has not been set.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)


class Map(InputFile):
    """Input class for scalar grid data input.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``alias``:  see :func:`alias`
    * ``format``:  see :func:`format`
    * ``path``:  see :func:`path`
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._alias = None
        self._format = None
        self._path = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def alias(self) -> str:
        """String used to refer to this map elsewhere in the input file.

        :raises TypeError:  if alias is not a string
        """
        return self._alias

    @alias.setter
    def alias(self, value):
        if check.is_string(value):
            self._alias = value
        else:
            raise TypeError(f"{value} is not a string.")

    @property
    def format(self) -> str:
        """Format for scalar input map.

        One of:

        * ``dx``:  :ref:`opendx`
        * ``dx.gz``:  GZip-compressed :ref:`opendx`

        :raises ValueError:  if format is not one of allowed values
        """
        return self._format

    @format.setter
    def format(self, value):
        value = value.lower()
        if value in ["dx", "dx.gz"]:
            self._format = value
        else:
            raise ValueError(f"{value} is not an allowed format.")

    @property
    def path(self) -> str:
        """Path for scalar input map.

        :raises TypeError:  if path is not a string
        """
        return self._path

    @path.setter
    def path(self, value):
        if check.is_string(value):
            self._path = value
        else:
            raise TypeError(value)

    def validate(self):
        """Validate the object.

        :raises ValueError:  if object is not valid
        """
        errors = []
        if self.path is None:
            errors.append("Path is not set.")
        if self.format is None:
            errors.append("Format is not set.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)

    def to_dict(self) -> dict:
        return {
            "alias": self.alias,
            "format": self.format,
            "path": self.path,
        }

    def from_dict(self, input_):
        """Load object contents from dictionary.

        :param dict input_:  input dictionary
        :raises KeyError:  if input is missing keys
        """
        try:
            self.alias = input_["alias"]
            self.format = input_["format"].lower()
            self.path = input_["path"]
        except KeyError as err:
            err = f"Missing key {err} while parsing {input_}"
            raise KeyError(err)


class Molecule(InputFile):
    """Input class for molecule input.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``alias``:  see :func:`alias`
    * ``format``:  see :func:`format`
    * ``path``:  see :func:`path`
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._alias = None
        self._format = None
        self._path = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def alias(self) -> str:
        """String used to refer to this map elsewhere in the input file.

        :raises TypeError:  if alias is not string
        """
        return self._alias

    @alias.setter
    def alias(self, value):
        if check.is_string(value):
            self._alias = value
        else:
            raise ValueError(f"{value} is not a string.")

    @property
    def format(self) -> str:
        """Format of molecule input.

        One of:

        * ``pdb``:  :ref:`pdb`
        * ``pqr``:  :ref:`pqr`

        :raises ValueError:  if format is not one of the above
        """
        return self._format

    @format.setter
    def format(self, value) -> str:
        value = value.lower()
        if value in ["pdb", "pqr"]:
            self._format = value
        else:
            raise ValueError(f"{value} is not a valid format.")

    @property
    def path(self) -> str:
        """Path of molecule input.

        :raises TypeError: if path is not string
        """
        return self._path

    @path.setter
    def path(self, value):
        if check.is_string(value):
            self._path = value
        else:
            raise TypeError(f"{value} is not a string.")

    def validate(self):
        """Validate the object.

        :raises ValueError:  if object is not valid
        """
        errors = []
        if self.path is None:
            errors.append("Path is not set.")
        if self.format is None:
            errors.append("Format is not set.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)

    def to_dict(self) -> dict:
        return {
            "alias": self.alias,
            "format": self.format,
            "path": self.path,
        }

    def from_dict(self, input_):
        """Load object contents from dictionary.

        :param dict input_:  input dictionary
        :raises KeyError:  if input is missing keys
        """
        try:
            self.alias = input_["alias"]
            self.format = input_["format"].lower()
            self.path = input_["path"]
        except KeyError as err:
            err = f"Missing key {err} while parsing {input_}."
            raise KeyError(err)


class Parameter(InputFile):
    """Input class for parameter file input.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``alias``:  see :func:`alias`
    * ``format``:  see :func:`format`
    * ``path``:  see :func:`path`
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._alias = None
        self._format = None
        self._path = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def alias(self) -> str:
        """String used to refer to this map elsewhere in the input file.

        :raises TypeError:  if not a string
        """
        return self._alias

    @alias.setter
    def alias(self, value):
        if check.is_string(value):
            self._alias = value
        else:
            raise TypeError(f"{value} is not a string")

    @property
    def format(self) -> str:
        """Format of the parameter file.

        One of:

        * ``flat``:  :ref:`apbsflatparm`
        * ``xml``:  :ref:`apbsxmlparm`

        :raises ValueError:  if given invalid format
        """
        return self._format

    @format.setter
    def format(self, value):
        value = value.lower()
        if value in ["flat", "xml"]:
            self._format = value
        else:
            raise ValueError(f"{value} is not a valid format.")

    @property
    def path(self) -> str:
        """Path to the parameter file.

        :raises TypeError:  if not a string
        """
        return self._path

    @path.setter
    def path(self, value):
        if check.is_string(value):
            self._path = value
        else:
            raise TypeError(f"{value} is not a string.")

    def validate(self):
        """Validate this object.

        :raises ValueError:  if object is invalid
        """
        errors = []
        if self._format is None:
            errors.append("Format cannot be None.")
        if self._path is None:
            errors.append("Path cannot be None.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)

    def to_dict(self) -> dict:
        return {
            "alias": self.alias,
            "format": self.format,
            "path": self.path,
        }

    def from_dict(self, input_):
        """Load object contents from dictionary.

        :param dict input_:  input dictionary
        :raises KeyError:  if dictionary elements missing
        """
        try:
            self.alias = input_["alias"]
            self.format = input_["format"].lower()
            self.path = input_["path"]
        except KeyError as err:
            err = f"Missing key {err} while parsing {input_}."
            raise KeyError(err)


class Read(InputFile):
    """Class for information about data to be loaded into APBS.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``molecules``:  a list of molecule input objects (see :class:`Molecule`)
    * ``potential maps``:  a list of electrostatic potential map input objects
      (see :class:`Map`)
    * ``charge density maps``:  a list of charge density map input objects (see
      :class:`Map`)
    * ``ion accessibility maps``:  a list of ion accessibility map input
      objects (see :class:`Map`)
    * ``dielectric maps``:  a list of dielectric map input objects (see
      :class:`DielectricMapGroup`)
    * ``parameters``:  a list of parameter files
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._molecules = []
        self._potential_maps = []
        self._charge_density_maps = []
        self._ion_accessibility_maps = []
        self._dielectric_maps = []
        self._parameters = []
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def molecules(self) -> list:
        """List of :class:`Molecule` objects.

        :raises TypeError:  if something other than :class:`Molecule` in list
        """
        return self._molecules

    @molecules.setter
    def molecules(self, list_):
        for elem in list_:
            if not isinstance(elem, Molecule):
                raise TypeError(f"Found {type(elem)} in list.")
        self._molecules = list_

    @property
    def potential_maps(self) -> list:
        """List of electrostatic potential :class:`Map` objects.".

        These maps can be used to set the electrostatic potential at the
        boundary to values outside of the traditional mappings.

        Units of electrostatic potential are :math:`k_b T e_c^{-1}`.

        :raises TypeError: if something other than :class:`Map` in list
        """
        return self._potential_maps

    @potential_maps.setter
    def potential_maps(self, list_):
        for elem in list_:
            if not isinstance(elem, Map):
                raise TypeError(f"Found {type(elem)} in list.")
        self._potential_maps = list_

    @property
    def charge_density_maps(self) -> list:
        """List of charge density :class:`Map` objects..

        These maps can be used to provide charge density (source term)
        distributions outside of the normal atom-based charge distribution
        obtained from a molecular structure.

        Units of charge density are :math:`e_c Å^{-3}`.

        :raises TypeError: if something other than :class:`Map` in list
        """
        return self._charge_density_maps

    @charge_density_maps.setter
    def charge_density_maps(self, list_):
        for elem in list_:
            if not isinstance(elem, Map):
                raise TypeError(f"Found {type(elem)} in list.")
        self._charge_density_maps = list_

    @property
    def ion_accessibility_maps(self) -> list:
        """List of ion accessibility :class:`Map` objects..

        The maps specify ion accessibility values which range between 0
        (inaccessible) and the value of the Debye-Hückel screening parameter;
        these values have units of :math:`Å^{-2}`.

        :raises TypeError: if something other than :class:`Map` in list
        """
        return self._ion_accessibility_maps

    @ion_accessibility_maps.setter
    def ion_accessibility_maps(self, list_):
        for elem in list_:
            if not isinstance(elem, Map):
                raise TypeError(f"Found {type(elem)} in list.")
        self._ion_accessibility_maps = list_

    @property
    def dielectric_maps(self) -> list:
        """List of :class:`DielectricMapGroup` objects.

        These 3-tuples represent the dielectric function mapped to 3 meshes,
        shifted by one-half grid spacing in the x, y, and z directions.  The
        inputs are maps of dielectric variables between the solvent and
        biomolecular dielectric constants; these values are unitless.

        :raises TypeError: if something other than :class:`DielectricMapGroup`
            in list
        """
        return self._dielectric_maps

    @dielectric_maps.setter
    def dielectric_maps(self, list_):
        for elem in list_:
            if not isinstance(elem, DielectricMapGroup):
                raise TypeError(f"Found {type(elem)} in list.")
        self._dielectric_maps = list_

    def validate(self):
        """Validate this input object.

        :raises ValueError:  if invalid
        """
        errors = []
        if not self.molecules and not self.charge_density_maps:
            errors.append(
                "No molecule input provided and no charge density map "
                "specified."
            )
        if not self.molecules and not self.dielectric_maps:
            errors.append(
                "No molecule input provided and no dielectric maps specified."
            )
        for mol in self._molecules:
            if mol.format == "pdb" and not self.parameters:
                errors.append("Have PDB-format molecule but no parameters.")
        for obj in (
            self.molecules
            + self.potential_maps
            + self.charge_density_maps
            + self.ion_accessibility_maps
            + self.dielectric_maps
            + self.parameters
        ):
            try:
                obj.validate()
            except ValueError as err:
                errors.append(f"{err}.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)

    def to_dict(self) -> dict:
        output = dict()
        output["molecules"] = [mol.to_dict() for mol in self.molecules]
        output["potential maps"] = [
            map_.to_dict() for map_ in self.potential_maps
        ]
        output["charge density maps"] = [
            map_.to_dict() for map_ in self.charge_density_maps
        ]
        output["ion accessibility maps"] = [
            map_.to_dict() for map_ in self.ion_accessibility_maps
        ]
        output["dielectric maps"] = [
            map_.to_dict() for map_ in self.dielectric_maps
        ]
        output["parameters"] = [param.to_dict() for param in self.parameters]
        return output

    def from_dict(self, input_):
        """Load object contents from dictionary.

        :param dict input_:  input dictionary
        :raises KeyError:  if input is missing keys
        """
        self.molecules = [
            Molecule(dict_=dict_) for dict_ in input_.get("molecules", [])
        ]
        self.potential_maps = [
            Map(dict_=dict_) for dict_ in input_.get("potential maps", [])
        ]
        self.charge_density_maps = [
            Map(dict_=dict_) for dict_ in input_.get("charge density maps", [])
        ]
        self.ion_accessibility_maps = [
            Map(dict_=dict_)
            for dict_ in input_.get("ion accessibility maps", [])
        ]
        self.dielectric_maps = [
            DielectricMapGroup(dict_=dict_)
            for dict_ in input_.get("dielectric maps", [])
        ]
        self.parameters = [
            Parameter(dict_=dict_) for dict_ in input_.get("parameters", [])
        ]
