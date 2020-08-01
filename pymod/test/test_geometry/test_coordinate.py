import sys # noqa
sys.path.insert(0, '..') # noqa

import numpy as np
from geometry import Coordinate
import pytest


class TestCoordinate:

    @pytest.mark.parametrize('args,expect', [
        ((0, 0, 0), np.array([0, 0, 0], dtype=np.float32)),
        ((1, 2, 3), np.array([1, 2, 3], dtype=np.float32)),
        (tuple(), np.array([0, 0, 0], dtype=np.float32)),
    ])
    def test_ctor(self, args, expect):
        sut = Coordinate(*args)
        assert ( sut._data == expect ).all()

    @pytest.mark.parametrize('args',[
        range(1),
        range(2),
        range(4),
        range(5),
    ])
    def test_exceptions(self, args):
        with pytest.raises(RuntimeError):
            sut = Coordinate(*args)

    def test_property_get(self):
        sut = Coordinate(1, 2, 3)
        assert sut.x == 1
        assert sut.y == 2
        assert sut.z == 3

    def test_property_set(self):
        sut = Coordinate(0, 0, 0)
        assert sut.x == 0
        assert sut.y == 0
        assert sut.z == 0
        sut.x = 1
        sut.y = 2
        sut.z = 3
        assert sut.x == 1
        assert sut.y == 2
        assert sut.z == 3

    def test_get_idx(self):
        sut = Coordinate(0, 0, 0)
        sut[0] = 1
        sut[1] = 2
        sut[2] = 3
        assert sut.x == 1
        assert sut.y == 2
        assert sut.z == 3

        with pytest.raises(IndexError):
            sut[3] = 5

    def test_operators(self):
        lo = Coordinate(0, 0, 0)
        hi = Coordinate(1, 1, 1)
        assert hi > lo

        same1 = Coordinate(0, 0, 0)
        same2 = Coordinate(0, 0, 0)
        assert same1 == same2

        lo = Coordinate(0, 0, 0)
        hi = Coordinate(1, 1, 1)
        assert hi != lo

        same1 = Coordinate(0, 0, 0)
        same2 = Coordinate(0, 0, 0)
        assert same1 <= same2
        lo = Coordinate(0, 0, 0)
        hi = Coordinate(1, 1, 1)
        assert lo <= hi

        same1 = Coordinate(0, 0, 0)
        same2 = Coordinate(0, 0, 0)
        assert same1 >= same2
        lo = Coordinate(0, 0, 0)
        hi = Coordinate(1, 1, 1)
        assert lo <= hi

        c = Coordinate(0, 0, 0)
        assert c + 1 == Coordinate(1, 1, 1)

        c = Coordinate(1, 1, 1)
        assert c - 1 == Coordinate(0, 0, 0)

        c = Coordinate(1, 1, 1)
        assert c * 2 == Coordinate(2, 2, 2)

        c = Coordinate(2, 2, 2)
        assert c / 2 == Coordinate(1, 1, 1)

    def test_any(self):
        c = Coordinate(3, 2, 2)
        assert c.any(lambda x: x > 2)

        c = Coordinate(-1, 2, 2)
        assert c.any(lambda x: x < 0)
        assert c.any(lambda x: x == -1)

    def test_all(self):
        c = Coordinate(2, 2, 2)
        assert c.all(lambda x: x == 2)

        c = Coordinate(-1, 2, 2)
        assert c.all(lambda x: x < 3)
