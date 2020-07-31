from .test_coordinate import TestCoordinate
import sys # noqa
sys.path.insert(0, '..') # noqa
from geometry import SurfacePoint # noqa

class TestSurfacePoint(TestCoordinate):
    def test_is_on_surf(self) -> None:
        sut = SurfacePoint(0, 0, 0)
        self.assertFalse(sut.is_on_surf)

        sut = SurfacePoint(0, 0, 0, is_on_surf=True)
        self.assertTrue(sut.is_on_surf)
