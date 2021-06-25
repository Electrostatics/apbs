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
    * ``concentration``:  concentration of ion species; see :func:`concentration`

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
