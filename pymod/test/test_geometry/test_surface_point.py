from .test_coordinate import *
import sys  # noqa
sys.path.insert(0, '..')  # noqa
from geometry import SurfacePoint  # noqa
import pytest


class TestSurface:
    def test_is_on_surf(self):
        sut = SurfacePoint(0, 0, 0)
        assert not sut.is_on_surf

        sut = SurfacePoint(0, 0, 0, is_on_surf=True)
        assert sut.is_on_surf
