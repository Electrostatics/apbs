from apbs.geometry import SurfacePoint
import pytest


class TestSurface:
    def test_is_on_surf(self):
        sut = SurfacePoint(0, 0, 0)
        assert not sut.is_on_surf

        sut = SurfacePoint(0, 0, 0, is_on_surf=True)
        assert sut.is_on_surf
