import sys # noqa
sys.path.insert(0, '..') # noqa
from chemistry import Atom # noqa
import pytest


class TestAtom:
    def test_ctor(self) -> None:
        sut = Atom(0, 0, 0)
        assert sut.x == 0
        assert sut.y == 0
        assert sut.z == 0

        sut = Atom()
        assert sut.x == 0
        assert sut.y == 0
        assert sut.z == 0

    def test_property_get(self):
        sut = Atom(1, 2, 3)
        assert sut.x == 1
        assert sut.y == 2
        assert sut.z == 3

    def test_euclidian_distance(self):
        a = Atom(1, 1, 1)
        b = Atom(2, 2, 2)
        assert a.euclidian_dist(b) == 3

        a = Atom(0, 0, 0)
        b = Atom(2, 2, 2)
        assert a.euclidian_dist(b) == 12
