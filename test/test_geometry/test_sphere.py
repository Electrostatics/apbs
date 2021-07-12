from apbs.geometry import Sphere
from apbs.geometry import Surface
import pytest


class TestSphere:
    @pytest.mark.skip(reason="Needs to be implemented.")
    @pytest.mark.parametrize(
        "npoints,expected", [(1e2, 0.0), (1e4, 0.0), (1e6, 0.0)]
    )
    def test_spherical_distribution(self, npoints, expected):
        sut: Surface = Sphere.spherical_distribution(npoints)
