"""Define base class for input file classes."""
import logging
import json
from abc import ABC, abstractmethod
import yaml
from yaml import Dumper


_LOGGER = logging.getLogger(__name__)


class InputFile(ABC):
    """Base class for input file classes."""

    def init(self):
        self._alias = None

    @abstractmethod
    def from_dict(self, input_):
        """Parse dictionary-format input into this object.

        :param dict input_:  input dictionary
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

    @abstractmethod
    def to_dict(self) -> dict:
        """Produce dictionary representation of self.

        :returns:  dictionary representation of self
        """
        pass

    def to_json(self) -> str:
        """Produce JSON representation of self.

        :returns:  JSON representation of self
        """
        dict_ = self.to_dict()
        return json.dumps(dict_)

    def to_yaml(self) -> str:
        """Produce YAML representation of self.

        :returns:  YAML representation of self
        """
        dict_ = self.to_dict()
        return yaml.dump(dict_, Dumper)

    @abstractmethod
    def validate(self):
        """Validate the object.

        :raises ValueError:  if object is not valid
        """
        pass
