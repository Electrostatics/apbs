"""These classes provide information about processing APBS output.

The top-level output summarizes or combines results from multiple calculations.
"""
import logging

# from typing import Type
from . import InputFile
from . import check


_LOGGER = logging.getLogger(__name__)


class Process(InputFile):
    """Arithmetic operations for output from APBS energy/force calculations.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``sums``:  add elements together.  This is a list of :class:`Operation`
      objects.
    * ``products``:  multiply elements together.  This is a list of
      :class:`Operation` objects.
    * ``exps``:  element-wise exponentiation of elements.  This is a list of
      :class:`Operation` objects.
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._sums = []
        self._products = []
        self._exps = []
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def sums(self) -> list:
        """List of sum :class:`Operation` objects.

        :raises TypeError:  if list contains something other than
            class:`Operation` objects
        """
        return self._sums

    @sums.setter
    def sums(self, list_):
        for elem in list_:
            if not isinstance(elem, Operation):
                raise TypeError(f"Found {type(elem)} object in list.")
        self._sums = list_

    @property
    def products(self) -> list:
        """List of product :class:`Operation` objects.

        :raises TypeError:  if list contains something other than
            class:`Operation` objects
        """
        return self._products

    @products.setter
    def products(self, list_):
        for elem in list_:
            if not isinstance(elem, Operation):
                raise TypeError(f"Found {type(elem)} object in list.")
        self._products = list_

    @property
    def exps(self) -> list:
        """List of exponential :class:`Operation` objects.

        :raises TypeError:  if list contains something other than
            class:`Operation` objects
        """
        return self._exps

    @exps.setter
    def exps(self, list_):
        for elem in list_:
            if not isinstance(elem, Operation):
                raise TypeError(f"Found {type(elem)} object in list.")
        self._exps = list_

    def validate(self):
        """Validate contents of object.

        :raises ValueError:  if invalid object encountered
        """
        errors = []
        for obj in self.sums + self.products + self.exps:
            try:
                obj.validate()
            except ValueError as error:
                errors.append(
                    f"Encountered invalid object of type {type(obj)}: {error}."
                )
        if errors:
            errors = " ".join(errors)
            raise ValueError(errors)

    def from_dict(self, input_):
        """Populate object from dictionary.

        :param dict input_:  input information
        :raises KeyError:  if elements are missing from the input dictionary
        """
        _LOGGER.debug(input_["sums"])
        self.sums = [Operation(dict_=dict_) for dict_ in input_["sums"]]
        self.products = [
            Operation(dict_=dict_) for dict_ in input_["products"]
        ]
        self.exps = [Operation(dict_=dict_) for dict_ in input_["exps"]]

    def to_dict(self) -> dict:
        dict_ = {}
        dict_["sums"] = [elem.to_dict() for elem in self.sums]
        dict_["products"] = [elem.to_dict() for elem in self.products]
        dict_["exps"] = [elem.to_dict() for elem in self.exps]
        return dict_


class Operation(InputFile):
    """Generic arithmetic operation.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``alias``:  string for referring to the output of this operation
      elsewhere
    * ``elements``:  a list of elements for the operation; see :class:`Element`
      for details
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._alias = None
        self._elements = []
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def alias(self) -> str:
        """String for referring to the output of this operation elsewhere.

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
    def elements(self) -> list:
        """List of :class:`Element` objects for operation.

        :raises TypeError:  if list contains objects not :class:`Element`
        """
        return self._elements

    @elements.setter
    def elements(self, list_):
        for elem in list_:
            if not isinstance(elem, Element):
                raise TypeError(f"Found {type(elem)} in list.")
        self._elements = list_

    def from_dict(self, dict_):
        """Load object contents from dictionary.

        :param dict dict_:  dictionary with object contents
        :raises KeyError:  if dictionary elements missing
        """
        self.alias = dict_["alias"]
        self.elements = [Element(dict_=edict) for edict in dict_["elements"]]

    def to_dict(self) -> dict:
        dict_ = {"alias": self.alias}
        dict_["elements"] = [element.to_dict() for element in self.elements]
        return dict_

    def validate(self):
        """Validate object.

        :raises ValueError:  if object contents are invalid
        """
        errors = []
        if self._alias is None:
            errors.append("Operation alias cannot be None.")
        for element in self._elements:
            try:
                element.validate()
            except ValueError as error:
                errors.append(f"Unable to validate element:  {error}.")
        if errors:
            errors = " ".join(errors)
            raise ValueError(errors)


class Element(InputFile):
    """Element of an arithmetic operation.

    Objects can be initialized with dictionary/JSON/YAML data with a list of
    the following keys:

    * ``alias``:  alias for quantity on which to operate
    * ``coefficient``:  multiplicative coefficient for quantity (e.g., -1.0 to
      convert a sum to difference)
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._coefficient_ = None
        self._alias = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def coefficient(self) -> float:
        """Return coefficient for this element.

        :raises TypeError:  if not a number
        """
        return self._coefficient

    @coefficient.setter
    def coefficient(self, value):
        if check.is_number(value):
            self._coefficient = value
        else:
            raise TypeError(f"{value} is not a number.")

    @property
    def alias(self) -> str:
        """Return alias for object to which this element refers.

        :raises TypeError:  if alias is not a string.
        """
        return self._alias

    @alias.setter
    def alias(self, value):
        if check.is_string(value):
            self._alias = value
        else:
            raise TypeError(f"{value} is not a string.")

    def validate(self):
        errors = []
        if self.coefficient is None:
            errors.append("Coefficient not set.")
        if self.alias is None:
            errors.append("Alias not set")
        if errors:
            error = " ".join(errors)
            raise ValueError(error)

    def from_dict(self, input_):
        """Load object contents from dictionary.

        :param dict input_:  dictionary with object contents
        :raises KeyError:  if dictionary elements missing
        """
        self.alias = input_["alias"]
        self.coefficient = input_["coefficient"]

    def to_dict(self) -> dict:
        return {"alias": self.alias, "coefficient": self.coefficient}
