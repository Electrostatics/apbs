"""These classes provide information about data input for APBS.

.. todo::

   * Add mmCIF support for :class:`Molecule`
"""
import logging
from .input_file import InputFile


_LOGGER = logging.getLogger(__name__)


class DielectricMapGroup(InputFile):
    """Input class for a group of three dielectric maps.

    These three maps represent the dielectric function mapped to three meshes,
    shifted by one-half grid spacing in the x, y, and z directions.  The inputs
    are maps of dielectric variables between the solvent and biomolecular
    dielectric constants; these values are unitless.

    Objects can be initialized with dictionary/JSON/YAML data with the following
    keys:

    * ``alias``:  see :func:`alias`
    * ``format``:  see :func:`format`
    * ``x-shifted path``: string with path to x-shifted map
    * ``y-shifted path``: string with path to y-shifted map
    * ``x-shifted path``: string with path to z-shifted map

    More information about these properties is provided below.
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._paths = [None, None, None]
        self._format = None
        self._alias = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def paths(self) -> tuple:
        """3-tuple of strings.
        
        Tuple contains paths to the x-, y-, and z-shifted dielectric maps.
        """
        return tuple(self.maps)

    @paths.setter
    def paths(self, value):
        for i in range(3):
            self._paths[i] = value[i]

    @property
    def alias(self) -> str:
        """String used to refer to these maps elsewhere in the input file."""
        return self._alias

    @alias.setter
    def alias(self, value):
        self._alias = value

    @property
    def format(self) -> str:
        """Format for scalar input map.

        One of:

        * ``dx``: :ref:`opendx` format
        * ``dx.gz``:  GZip-compressed :ref:`opendx` format
        """
        return self._format

    @format.setter
    def format(self, value):
        self._format = value.lower()

    def from_dict(self, dict_):
        try:
            self._alias = dict_["alias"]
            self._format = dict_["format"].lower()
            self._paths[0] = dict_["x-shifted path"]
            self._paths[1] = dict_["y-shifted path"]
            self._paths[2] = dict_["z-shifted path"]
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
        errors = []
        if None in self._paths:
            errors.append(f"One or more paths is null: {self._paths}.")
        if self._format not in ["dx", "dx.gz"]:
            errors.append(f"Unknown format: {self._format}.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)


class Map(InputFile):
    """Input class for scalar grid data input.

    Objects can be initialized with dictionary/JSON/YAML data with the following
    keys:

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
        """String used to refer to this map elsewhere in the input file."""
        return self._alias

    @alias.setter
    def alias(self, value):
        self._alias = value

    @property
    def format(self) -> str:
        """Format for scalar input map.

        One of:

        * ``dx``:  :ref:`opendx` format
        * ``dx.gz``:  GZip-compressed :ref:`opendx` format
        """
        return self._format

    @format.setter
    def format(self, value):
        self._format = value.lower()

    @property
    def path(self) -> str:
        """Path for scalar input map."""
        return self._path

    @path.setter
    def path(self, value):
        self._path = value

    def validate(self):
        """Validate the object.

        :raises ValueError:  if object is not valid
        """
        errors = []
        if self._path is None:
            errors.append("path is not set.")
        if self._format not in ["dx", "dx.gz"]:
            errors.append(f"{self._format} is not a valid format.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)

    def to_dict(self) -> dict:
        """Convert object to dictionary."""
        return {
            "alias": self._alias,
            "format": self._format,
            "path": self._path,
        }

    def from_dict(self, input_):
        """Load object contents from dictionary.

        :param dict input_:  input dictionary
        """
        try:
            self._alias = input_["alias"]
            self._format = input_["format"].lower()
            self._path = input_["path"]
        except KeyError as err:
            err = f"Missing key {err} while parsing {input_}"
            raise KeyError(err)


class Molecule(InputFile):
    """Input class for molecule input.

    Objects can be initialized with dictionary/JSON/YAML data with the following
    keys:

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
        """String used to refer to this map elsewhere in the input file."""
        return self._alias

    @alias.setter
    def alias(self, value):
        self._alias = value

    @property
    def format(self) -> str:
        """Format of molecule input.

        One of:

        * ``pdb``:  :ref:`pdb` format
        * ``pqr``:  :ref:`pqr` format
        """
        return self._format

    @format.setter
    def format(self, value) -> str:
        self._format = value.lower()

    @property
    def path(self) -> str:
        """Path of molecule input."""
        return self._path

    @path.setter
    def path(self, value):
        self._path = value

    def validate(self):
        """Validate the object.

        :raises ValueError:  if object is not valid
        """
        errors = []
        if self._path is None:
            errors.append("path is not set.")
        if self._format not in ["pdb", "pqr"]:
            errors.append(f"{self._format} is not a valid format.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)

    def to_dict(self) -> dict:
        """Convert object to dictionary."""
        return {
            "alias": self._alias,
            "format": self._format,
            "path": self._path,
        }

    def from_dict(self, input_):
        """Load object contents from dictionary.

        :param dict input_:  input dictionary
        """
        try:
            self._alias = input_["alias"]
            self._format = input_["format"].lower()
            self._path = input_["path"]
        except KeyError as err:
            err = f"Missing key {err} while parsing {input_}."
            raise KeyError(err)


class Parameter(InputFile):
    """Input class for parameter file input.

    Objects can be initialized with dictionary/JSON/YAML data with the following
    keys:

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
        """String used to refer to this map elsewhere in the input file."""
        return self._alias

    @alias.setter
    def alias(self, value):
        self._alias = value

    @property
    def format(self) -> str:
        """Format of the parameter file.

        One of:

        * ``flat``:  :ref:`apbsflatparm` format
        * ``xml``:  :ref:`apbsxmlparm` format
        """
        return self._format

    @format.setter
    def format(self, value):
        self._format = value.lower()

    @property
    def path(self) -> str:
        """Path to the parameter file."""
        return self._path

    @path.setter
    def path(self, value):
        self._path = value

    def validate(self):
        """Validate this object.

        :raises ValueError:  if object is invalid
        """
        errors = []
        if self._format not in ["flat", "xml"]:
            errors.append(f"{self._format} is not a valid format.")
        if self._path is None:
            errors.append("Path cannot be None.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)

    def to_dict(self) -> dict:
        """Convert object to dictionary."""
        return {
            "alias": self._alias,
            "format": self._format,
            "path": self._path,
        }

    def from_dict(self, input_):
        """Load object contents from dictionary.

        :param dict input_:  input dictionary
        """
        try:
            self._alias = input_["alias"]
            self._format = input_["format"].lower()
            self._path = input_["path"]
        except KeyError as err:
            err = f"Missing key {err} while parsing {input_}."
            raise KeyError(err)


class Read(InputFile):
    """Input class for parameter file input.

    Objects can be initialized with dictionary/JSON/YAML data with the following
    keys:

    * ``molecules``:  a list of molecule input objects (see :class:`Molecule`)
    * ``potential maps``:  a list of electrostatic potential map input objects (see :class:`read.Map`)
    * ``charge density maps``:  a list of charge density map input objects (see :class:`Map`)
    * ``ion accessibility maps``:  a list of ion accessibility map input objects (see :class:`Map`)
    * ``dielectric maps``:  a list of dielectric map input objects (see :class:`DielectricMapGroup`)
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
        """List of :class:`Molecule` objects."""
        return self._molecules

    @molecules.setter
    def molecules(self, value):
        self._molecules = value

    @property
    def potential_maps(self) -> list:
        """List of electrostatic potential :class:`Map` objects.".

        These maps can be used to set the electrostatic potential at the
        boundary to values outside of the traditional mappings.

        Units of electrostatic potential are $k_b T e_c^{-1}$.
        """
        return self._potential_maps

    @potential_maps.setter
    def potential_maps(self, value):
        self._potential_maps = value

    @property
    def charge_density_maps(self) -> list:
        """List of charge density :class:`Map` objects..

        These maps can be used to provide charge density (source term)
        distributions outside of the normal atom-based charge distribution
        obtained from a molecular structure.

        Units of charge density are $e_c Å^{-3}$.
        """
        return self._charge_density_maps

    @charge_density_maps.setter
    def charge_density_maps(self, value):
        self._charge_density_maps = value

    @property
    def ion_accessibility_maps(self) -> list:
        """List of ion accessibility :class:`Map` objects..

        The maps specify ion accessibility values which range between 0
        (inaccessible) and the value of the Debye-Hückel screening parameter;
        these values have units of $Å^{-2}$.
        """
        return self._ion_accessibility_maps

    @ion_accessibility_maps.setter
    def ion_accessibility_maps(self, value):
        self._ion_accessibility_maps = value

    @property
    def dielectric_maps(self) -> list:
        """List of :class:`DielectricMapGroup` objects.

        These 3-tuples represent the dielectric function mapped to 3 meshes,
        shifted by one-half grid spacing in the x, y, and z directions.  The
        inputs are maps of dielectric variables between the solvent and
        biomolecular dielectric constants; these values are unitless.

        :returns: list of 3-tuples of :class:`Map` objects
        """
        return self._dielectric_maps

    @dielectric_maps.setter
    def dielectric_maps(self, value):
        self._dielectric_maps = value

    def validate(self):
        """Validate this input object.

        :raises ValueError:  if invalid
        """
        errors = []
        if not self._molecules and not self._charge_density_maps:
            errors.append(
                "No molecule input provided and no charge density map "
                "specified."
            )
        if not self._molecules and not self._dielectric_maps:
            errors.append(
                "No molecule input provided and no dielectric maps specified."
            )
        for mol in self._molecules:
            if mol.format == "pdb" and not self._parameters:
                errors.append("Have PDB-format molecule but no parameters.")
        for obj in (
            self._molecules
            + self._potential_maps
            + self._charge_density_maps
            + self._ion_accessibility_maps
            + self._dielectric_maps
            + self._parameters
        ):
            try:
                obj.validate()
            except ValueError as err:
                errors.append(f"{err}.")
        if errors:
            err = " ".join(errors)
            raise ValueError(err)

    def to_dict(self) -> dict:
        """Convert object to dictionary."""
        output = dict()
        output["molecules"] = [mol.to_dict() for mol in self._molecules]
        output["potential maps"] = [
            map_.to_dict() for map_ in self._potential_maps
        ]
        output["charge density maps"] = [
            map_.to_dict() for map_ in self._charge_density_maps
        ]
        output["ion accessibility maps"] = [
            map_.to_dict() for map_ in self._ion_accessibility_maps
        ]
        output["dielectric maps"] = [
            map_.to_dict() for map_ in self._dielectric_maps
        ]
        output["parameters"] = [param.to_dict() for param in self._parameters]
        return output

    def from_dict(self, input_):
        """Load object contents from dictionary.

        :param dict input_:  input dictionary
        """
        self._molecules = [
            Molecule(dict_=dict_)
            for dict_ in input_.get("molecules", [])
        ]
        self._potential_maps = [
            Map(dict_=dict_)
            for dict_ in input_.get("potential maps", [])
        ]
        self._charge_density_maps = [
            Map(dict_=dict_)
            for dict_ in input_.get("charge density maps", [])
        ]
        self._ion_accessibility_maps = [
            Map(dict_=dict_)
            for dict_ in input_.get("ion accessibility maps", [])
        ]
        self._dielectric_maps = [
            DielectricMapGroup(dict_=dict_)
            for dict_ in input_.get("dielectric maps", [])
        ]
        self._parameters = [
            Parameter(dict_=dict_)
            for dict_ in input_.get("parameters", [])
        ]
