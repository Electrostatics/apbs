from apbs.chemistry import Atom
from apbs.geometry import Coordinate
import numpy as np
import pytest


class TestAtom:
    def test_ctor(self):
        sut = Atom(field_name="ATOM", id=1, x=0, y=0, z=0)
        assert (sut.position._data == np.array((0, 0, 0))).all()

    def test_atom_exception(self):
        """test that exception is raised for invalid atoms"""
        with pytest.raises(ValueError):
            assert Atom(0, 0, 0)

    def test_property_get(self):
        sut = Atom(field_name="ATOM", id=1, x=1, y=2, z=3)
        assert sut.x == 1
        assert sut.y == 2
        assert sut.z == 3

    @pytest.mark.parametrize(
        "params1,params2",
        [
            ((1, 1, 1), (2, 2, 2)),
            ((0, 0, 0), (2, 2, 2)),
            ((1, 1, 1), (0, 0, 0)),
        ],
    )
    def test_euclidian_distance_atom(self, params1, params2):
        expect = np.sum((np.array(params1) - np.array(params2)) ** 2)
        a = Atom(field_name="ATOM", id=1, *params1)
        b = Atom(field_name="ATOM", id=2, *params2)
        assert a.euclidian_dist2(b) == expect

    @pytest.mark.parametrize(
        "params1,params2",
        [
            ((1, 1, 1), (2, 2, 2)),
            ((0, 0, 0), (2, 2, 2)),
            ((1, 1, 1), (0, 0, 0)),
        ],
    )
    def test_euclidian_distance_array(self, params1, params2):
        expect = np.sum((np.array(params1) - np.array(params2)) ** 2)
        a = Atom(field_name="ATOM", id=1, *params1)
        assert a.euclidian_dist2(np.array(params2)) == expect

    @pytest.mark.parametrize(
        "params1,params2",
        [
            ((1, 1, 1), (2, 2, 2)),
            ((0, 0, 0), (2, 2, 2)),
            ((1, 1, 1), (0, 0, 0)),
        ],
    )
    def test_euclidian_distance_array2(self, params1, params2):
        expect = np.sum((np.array(params1) - np.array(params2)) ** 2)
        a = Atom(field_name="ATOM", id=1, *params1)
        b = Coordinate(*params2)
        assert a.euclidian_dist2(b) == expect
