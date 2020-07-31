import sys # noqa
sys.path.insert(0, '..') # noqa
import unittest
from unittest import TestCase
from geometry import Sphere

class TestSphere(TestCase):
    '''
    Tests for static class Sphere, which handles complex geometrical
    calculations for spheres.
    '''

    @unittest.skip('Needs testing')
    def test_spherical_distribution(self):
        for npoints in [ 1e2, 1e4, 1e6 ]:
            sut: Surface = Sphere.spherical_distribution(npoints)
