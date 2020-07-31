from generic import Atom
import unittest
import sys
sys.path.insert(0, '..')


class TestAtom(unittest.TestCase):

    def test_ctor(self) -> None:
        sut = Atom(0, 0, 0)
        self.assertTrue(sut.x == 0)
        self.assertTrue(sut.y == 0)
        self.assertTrue(sut.z == 0)

        sut = Atom()
        self.assertTrue(sut.x == 0)
        self.assertTrue(sut.y == 0)
        self.assertTrue(sut.z == 0)

    def test_property_get(self):
        sut = Atom(1, 2, 3)
        self.assertTrue(sut.x == 1)
        self.assertTrue(sut.y == 2)
        self.assertTrue(sut.z == 3)

    def test_euclidian_distance(self):
        a = Atom(1, 1, 1)
        b = Atom(2, 2, 2)
        self.assertEqual(a.euclidian_dist(b), 3)

        a = Atom(0, 0, 0)
        b = Atom(2, 2, 2)
        self.assertEqual(a.euclidian_dist(b), 12)
