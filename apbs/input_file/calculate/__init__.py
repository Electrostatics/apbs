"""These classes provide input syntax for APBS calculations."""
import logging
from .. import check
from .. import InputFile
from .nonpolar import Nonpolar


_LOGGER = logging.getLogger(__name__)


class Calculate(InputFile):
    """Specify parameters for APBS calculations.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``alias``:  A string that allows the output from the calculation to be
      referenced later.
    * ``type``:  A string that indicates the type of calculation to be
      performed. See :func:`calculation_type` for list.
    * ``parameters``:  Object with parameters for calculation.  See calculation
      types above for details.
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._alias = None
        self._calculation_type = None
        self._parameters = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def validate(self):
        """Check object for validity.

        :raises ValueError:  if invalid content encountered
        """
        errors = []
        if self.alias is None:
            errors.append("Alias not set.")
        if self._calculation_type is None:
            errors.append("Calculation type not set.")
        try:
            self.parameters.validate()
        except ValueError as error:
            errors.append(f"Unable to validate parameters: {error}.")

    @property
    def alias(self) -> str:
        """Alias string to refer to this calculation elsewhere.

        :raises TypeError:  if not string.
        """
        return self._alias

    @alias.setter
    def alias(self, value):
        """Alias string to refer to this calculation elsewhere.

        :raises TypeError:  if not a string
        """
        if not check.is_string:
            raise TypeError(f"{value} is not a string.")
        self._alias = value

    @property
    def calculation_type(self) -> str:
        """Calculation type.

        One of the following:

        * ``nonpolar``:  A nonpolar solvation energy calculation using
          grid-based integrals.  See :class:`nonpolar.Nonpolar`.

        :raises TypeError:  if not a string
        :raises ValueError:  if not a recognized calculation type
        """
        return self._calculation_type

    @calculation_type.setter
    def calculation_type(self, value):
        value = value.lower()
        if check.is_string(value):
            if value in ["nonpolar"]:
                self._calculation_type = value
            else:
                raise ValueError(
                    f"{value} is not a recognized calculation type."
                )
        else:
            raise TypeError(f"{value} is not a string.")

    @property
    def parameters(self) -> InputFile:
        """Parameter object, dependent on calculation type, sub-classed from
        :class:`apbs.input_file.InputFile`.

        The format of this object is based on the calculation type; see
        :func:`calculation_type`.

        :raises TypeError:  if object is not derived from
           :class:`apbs.input_file.InputFile`.
        """
        return self._parameters

    @parameters.setter
    def parameters(self, value):
        if isinstance(value, InputFile):
            self._parameters = value
        else:
            raise TypeError(f"Got parameters of type {type(value)}.")

    def from_dict(self, input_):
        """Load dictionary input into object.

        :param dict input_:  data to load into object
        :raises KeyError:  if input contents are not found
        """
        self.alias = input_["alias"]
        self.calculation_type = input_["type"]
        if self.calculation_type == "nonpolar":
            self.parameters = Nonpolar(dict_=input_["parameters"])

    def to_dict(self) -> dict:
        dict_ = {"alias": self.alias, "type": self.calculation_type}
        dict_["parameters"] = self.parameters.to_dict()
        return dict_
