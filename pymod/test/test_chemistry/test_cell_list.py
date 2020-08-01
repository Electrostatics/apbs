import sys
sys.path.insert(0, '..') # noqa
from chemistry import CellList # noqa
from .test_atom_list import TestAtomList, fn
import pytest

class TestCellList(TestAtomList):
    @pytest.mark.skip(reason="Needs test")
    def test_cell_list_method(self):
        pass
