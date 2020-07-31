from unittest import TestCase
import unittest
from . import TestAtomList
import sys
sys.path.insert(0, '..') # noqa
from generic import CellList # noqa

class TestCellList(TestAtomList):

    def test_max_radius(self):
        sut = CellList(self.__class__.fn)
        self.assertEqual(sut.max_radius, 3.)
