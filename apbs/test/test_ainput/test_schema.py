import pkg_resources
import logging
import json
import requests
from jsonschema import Draft7Validator


_LOGGER = logging.getLogger(__name__)


def test_validate_schema():
    """Validate the schema."""
    schema = json.loads(pkg_resources.resource_string('apbs', 'data/input-schema.json'))
    Draft7Validator.check_schema(schema)
    raise NotImplementedError()
