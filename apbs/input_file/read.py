"""Configuration of input read into APBS."""
import logging
from .input_file import InputFile


_LOGGER = logging.getLogger(__name__)


class Molecule(InputFile):
    """Configuration of molecule input for APBS."""

    def __init__(self):
        self._alias = None
        self._format = None
        self._path = None

    @property
    def alias(self) -> str:
        """Alias for the object read by this class.

        :returns: alias
        """
        return self._alias

    @alias.setter
    def alias(self, value):
        self._alias = value

    @property
    def format(self) -> str:
        """Format of molecule input.

        One of:

          pdb
            PDB format

          pqr
            PQR format

        .. todo::  add links to PDB and PQR format documentation
        .. todo::  add mmCIF support
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


class Map(InputFile):
    """Scalar grid data input."""

    def __init__(self):
        self._alias = None
        self._format = None
        self._path = None

    @property
    def alias(self) -> str:
        """Alias for the object read by this class."""
        return self._alias

    @alias.setter
    def alias(self, value):
        self._alias = value

    @property
    def format(self) -> str:
        """Format for scalar input map.

        One of:

          dx
            OpenDX format

          dx.gz
            GZip-compressed OpenDX format

        .. todo::  add links to OpenDX format documentation
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


class Parameter(InputFile):
    """Parameter file input.

    Specify the charge and radius data to be used with PDB-format molecule
    files.
    """

    def __init__(self):
        self._alias = None
        self._format = None
        self._path = None

    @property
    def alias(self) -> str:
        """Alias for the object read by this class."""
        return self._alias

    @alias.setter
    def alias(self, value):
        self._alias = value

    @property
    def format(self) -> str:
        """Format of the parameter file.

        One of:

        flat
          Flat parameter file format

        xml
          XML parameter file format

        .. todo:: provide links to format documentation
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
            errors.append(f"{self._format} is not a valid format")
        if self._path is None:
            errors.append("Path cannot be None")
        err = "\n".join(errors)
        raise ValueError(err)


class Read(InputFile):
    """Configuration of input read into APBS."""

    def __init__(self):
        self._molecules = []
        self._potential_maps = []
        self._charge_density_maps = []
        self._ion_accessibility_maps = []
        self._dielectric_maps = []
        self._parameters = []

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
                "No molecule input provided and no dieelectric maps specified."
            )
        for mol in self._molecules:
            if mol.format == "pdb" and not self._parameters:
                errors.append("Have PDB-format molecule but no parameters.")
        for obj in (
            self._molecules + self._potential_maps + self._charge_density_maps
            + self._ion_accessibility_maps + self._parameters
        ):
            try:
                obj.validate()
            except ValueError as err:
                errors.append(err)
        for maps in self._dielectric_maps:
            for map in maps:
                try:
                    map.validate()
                except ValueError as err:
                    errors.append(err)
        if errors:
            err = "\n".join(errors)
            raise ValueError(err)

    @property
    def molecules(self) -> list:
        """List of input molecules.

        :returns:  list of :class:`Molecule` objects
        """
        return self._molecules

    @molecules.setter
    def molecules(self, value):
        self._molecules = value

    @property
    def potential_maps(self) -> list:
        """List of electrostatic potential maps.

        These maps can be used to set the electrostatic potential at the
        boundary to values outside of the traditional mappings.

        Units of electrostatic potential are $k_b T e_c^{-1}$.

        .. todo::  add links to appropriate boundary condition options
        .. todo::  make sure Sphinx LaTeX support is turned on

        :returns: list of :class:`Map` objects
        """
        return self._potential_maps

    @potential_maps.setter
    def potential_maps(self, value):
        self._potential_maps = value

    @property
    def charge_density_maps(self) -> list:
        """List of charge density maps.

        These maps can be used to provide charge density (source term)
        distributions outside of the normal atom-based charge distribution
        obtained from a molecular structure.

        Units of charge density are $e_c Å^{-3}$.

        :returns: list of :class:`Map` objects
        """
        return self._charge_density_maps

    @charge_density_maps.setter
    def charge_density_maps(self, value):
        self._charge_density_maps = value

    @property
    def ion_accessibility_maps(self) -> list:
        """List of ion accessibility maps.

        The maps specify ion accessibility values which range between 0
        (inaccessible) and the value of the Debye-Hückel screening parameter;
        these values have units of $Å^{-2}$.

        :returns: list of :class:`Map` objects
        """
        return self._ion_accessibility_maps

    @ion_accessibility_maps.setter
    def ion_accessibility_maps(self, value):
        self._ion_accessibility_maps = value

    @property
    def dielectric_maps(self) -> list:
        """List of tuples of dielectric maps.

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
