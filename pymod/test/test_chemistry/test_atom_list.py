import os  # noqa
import sys  # noqa
sys.path.insert(0, '..')  # noqa
from geometry import Coordinate  # noqa
from chemistry import AtomList, CellList  # noqa
import pytest
import tempfile

@pytest.yield_fixture
def fn():
    _fn = tempfile.NamedTemporaryFile(delete=False)

    with open(_fn.name, 'w') as f:
        f.write('HETATM    1  C    ALK    1       '
                '1.000   4.000   7.000 0.000 1.000\n')
        f.write('HETATM    1  C    ALK    1       '
                '2.000   5.000   8.000 0.000 2.000\n')
        f.write('HETATM    1  C    ALK    1       '
                '3.000   6.000   9.000 0.000 3.000\n')
    yield _fn.name

    _fn.close()
    os.unlink(_fn.name)


class TestAtomList:
    def test_read_pdb(self, fn: str):

        sut = AtomList(fn)
        assert len(sut._atoms) == 3

        a = sut._atoms[0]
        assert a.name == 'C'
        assert a.res_name == 'ALK'
        assert a.position.x == 1.
        assert a.position.y == 4.
        assert a.position.z == 7.
        assert a.charge == 0.
        assert a.radius == 1.

    def test_min(self, fn: str):

        sut = AtomList(fn)

        lo: Coordinate = sut.mincrd()
        assert lo.x == 1.
        assert lo.y == 4.
        assert lo.z == 7.

    def test_max(self, fn: str):

        sut = AtomList(fn)

        hi: Coordinate = sut.maxcrd()
        assert hi.x == 3.
        assert hi.y == 6.
        assert hi.z == 9.

    def test_center(self, fn: str):

        sut = AtomList(fn)

        mi: Coordinate = sut.center()
        assert mi.x == 2.
        assert mi.y == 5.
        assert mi.z == 8.

    def test_max_radius(self, fn: str):
        sut = CellList(fn)
        assert sut.max_radius == 3.
