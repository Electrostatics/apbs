import sys  # noqa
import os  # noqa
sys.path.insert(0, '..')  # noqa
from geometry import Surface
import pytest


class TestSurface:
    @pytest.mark.parametrize('npoints,idx', [
        (1, 1),
        (1, -2),
        (0, 0),
    ])
    def test_getitem_fails(self, npoints, idx):
        sut = Surface(0., npoints)
        with pytest.raises(IndexError):
            tmp = sut[idx]

    @pytest.mark.parametrize('npoints,idx', [
        (1, 1),
        (1, -2),
        (0, 0),
    ])
    def test_setitem_fails(self, npoints, idx):
        sut = Surface(0., npoints)
        tmp = 0
        with pytest.raises(IndexError):
            sut[idx] = tmp
