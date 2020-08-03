from apbs.chemistry import CellList
from apbs.test.test_chemistry.test_atom_list import TestAtomList, fn
import pytest


class TestCellList(TestAtomList):
    @pytest.mark.skip(reason="Needs test")
    def test_cell_list_method(self):
        pass
