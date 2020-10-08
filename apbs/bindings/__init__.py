import os
import sys

# Shared libraries will be in current directory, so we have to add to path
sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
from . import pybind, swig
