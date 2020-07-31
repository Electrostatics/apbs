import sys # noqa
sys.path.insert(0, '..') # noqa

from geometry import Coordinate
from unittest import TestCase
import unittest


class TestCoordinate(TestCase):

    def test_ctor(self):
        sut = Coordinate(0, 0, 0)
        self.assertTrue(sut.x == 0)
        self.assertTrue(sut.y == 0)
        self.assertTrue(sut.z == 0)

        sut = Coordinate()
        self.assertTrue(sut.x == 0)
        self.assertTrue(sut.y == 0)
        self.assertTrue(sut.z == 0)

        with self.assertRaises(RuntimeError):
            sut = Coordinate(1)

        with self.assertRaises(RuntimeError):
            sut = Coordinate(1, 2)

        with self.assertRaises(RuntimeError):
            sut = Coordinate(1, 2, 3, 4)

    def test_property_get(self):
        sut = Coordinate(1, 2, 3)
        self.assertTrue(sut.x == 1)
        self.assertTrue(sut.y == 2)
        self.assertTrue(sut.z == 3)

    def test_property_set(self):
        sut = Coordinate(0, 0, 0)
        self.assertTrue(sut.x == 0)
        self.assertTrue(sut.y == 0)
        self.assertTrue(sut.z == 0)
        sut.x = 1
        sut.y = 2
        sut.z = 3
        self.assertTrue(sut.x == 1)
        self.assertTrue(sut.y == 2)
        self.assertTrue(sut.z == 3)

    def test_get_idx(self):
        sut = Coordinate(0, 0, 0)
        sut[0] = 1
        sut[1] = 2
        sut[2] = 3
        self.assertTrue(sut.x == 1)
        self.assertTrue(sut.y == 2)
        self.assertTrue(sut.z == 3)

        with self.assertRaises(IndexError):
            sut[3] = 5

    def test_operators(self):
        lo = Coordinate(0, 0, 0)
        hi = Coordinate(1, 1, 1)
        self.assertGreater(hi, lo)

        same1 = Coordinate(0, 0, 0)
        same2 = Coordinate(0, 0, 0)
        self.assertEqual(same1, same2)

        lo = Coordinate(0, 0, 0)
        hi = Coordinate(1, 1, 1)
        self.assertNotEqual(hi, lo)

        same1 = Coordinate(0, 0, 0)
        same2 = Coordinate(0, 0, 0)
        self.assertLessEqual(same1, same2)
        lo = Coordinate(0, 0, 0)
        hi = Coordinate(1, 1, 1)
        self.assertLessEqual(lo, hi)

        same1 = Coordinate(0, 0, 0)
        same2 = Coordinate(0, 0, 0)
        self.assertGreaterEqual(same1, same2)
        lo = Coordinate(0, 0, 0)
        hi = Coordinate(1, 1, 1)
        self.assertLessEqual(lo, hi)

        c = Coordinate(0, 0, 0)
        self.assertEqual(c + 1, Coordinate(1, 1, 1))

        c = Coordinate(1, 1, 1)
        self.assertEqual(c - 1, Coordinate(0, 0, 0))

        c = Coordinate(1, 1, 1)
        self.assertEqual(c * 2, Coordinate(2, 2, 2))

        c = Coordinate(2, 2, 2)
        self.assertEqual(c / 2, Coordinate(1, 1, 1))

    def test_any(self):
        c = Coordinate(3, 2, 2)
        self.assertTrue(c.any(lambda x: x > 2))

        c = Coordinate(-1, 2, 2)
        self.assertTrue(c.any(lambda x: x < 0))
        self.assertTrue(c.any(lambda x: x == -1))

    def test_all(self):
        c = Coordinate(2, 2, 2)
        self.assertTrue(c.all(lambda x: x == 2))

        c = Coordinate(-1, 2, 2)
        self.assertTrue(c.all(lambda x: x < 3))
