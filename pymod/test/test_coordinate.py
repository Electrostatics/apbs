from coordinate import Coordinate
from unittest import TestCase

import sys
sys.path.insert(0, '..')


class TestCoordinate(TestCase):
    def test_property_get(self):

        p = Coordinate(1, 2, 3)
        self.assertTrue(p.x == 1)
        self.assertTrue(p.y == 2)
        self.assertTrue(p.z == 3)

    def test_property_set(self):

        p = Coordinate(0, 0, 0)
        self.assertTrue(p.x == 0)
        self.assertTrue(p.y == 0)
        self.assertTrue(p.z == 0)

        p.x = 1
        p.y = 2
        p.z = 3
        self.assertTrue(p.x == 1)
        self.assertTrue(p.y == 2)
        self.assertTrue(p.z == 3)

    def test_operators(self):
        self.assertTrue(False)


if __name__ == '__main__':
    unittest.main()
