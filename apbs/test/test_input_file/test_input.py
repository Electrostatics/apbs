"""Test APBS input file parsing."""
import logging
from input_file.read import Read


_LOGGER = logging.getLogger(__name__)


def test_input():
    """Test input files."""
    Read()
