"""Input file parsing classes."""
import logging
import json
from abc import ABC, abstractmethod
import yaml

# from yaml import Dumper
from .apbs_legacy_input import ApbsLegacyInput  # noqa F401


_LOGGER = logging.getLogger(__name__)


class InputFile(ABC):
    """Base class for input file classes."""

    def __init__(self, dict_=None, yaml=None, json=None):
        """Initialize object.

        :param dict dict_:  optional dictionary for initializing object with
            func:`from_dict`
        :param str yaml:  optional YAML string for initializing object with
            func:`from_yaml`
        :param str json:  optional JSON string for initializing object with
            func:`from_json`
        """
        if dict_ is not None:
            self.from_dict(dict_)
        if yaml is not None:
            self.from_yaml(yaml)
        if json is not None:
            self.from_json(json)

    @abstractmethod
    def from_dict(self, input_):
        """Parse dictionary-format input into this object.

        :param dict input_:  input dictionary
        :raises KeyError:  when input is missing
        """
        pass

    @abstractmethod
    def to_dict(self) -> dict:
        """Produce dictionary representation of self."""
        pass

    @abstractmethod
    def validate(self):
        """Validate the object.

        :raises ValueError:  if object is not valid
        """
        pass

    def from_yaml(self, input_):
        """Parse YAML-format input string into this object.

        :param str input_:  YAML-format input string
        """
        dict_ = yaml.safe_load(input_)
        self.from_dict(dict_)

    def from_json(self, input_):
        """Parse JSON-format input string into this object.

        :param str input_:  JSON-format input string
        """
        dict_ = json.loads(input_)
        self.from_dict(dict_)

    def to_json(self) -> str:
        """Produce JSON representation of self."""
        dict_ = self.to_dict()
        return json.dumps(dict_)

    def to_yaml(self) -> str:
        """Produce YAML representation of self."""
        dict_ = self.to_dict()
        return yaml.dump(dict_)
