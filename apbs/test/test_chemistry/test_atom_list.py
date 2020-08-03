from apbs.geometry import Coordinate
from apbs.chemistry import AtomList, CellList
import os
import pytest
import tempfile

@pytest.fixture()
def pdb_file(tmp_path):

    fp = tmp_path / 'atom.pdb'

    with open(fp, 'w') as f:
        f.write('HETATM    1  C    ALK    1       '
                '1.000   4.000   7.000 0.000 1.000\n')
        f.write('HETATM    1  C    ALK    1       '
                '2.000   5.000   8.000 0.000 2.000\n')
        f.write('HETATM    1  C    ALK    1       '
                '3.000   6.000   9.000 0.000 3.000\n')
    return fp


class TestAtomList:
    def test_read_pdb(self, pdb_file: str):

        sut = AtomList(pdb_file)
        assert len(sut._atoms) == 3

        a = sut._atoms[0]
        assert a.name == 'C'
        assert a.res_name == 'ALK'
        assert a.position.x == 1.
        assert a.position.y == 4.
        assert a.position.z == 7.
        assert a.charge == 0.
        assert a.radius == 1.

    def test_min(self, pdb_file: str):

        sut = AtomList(pdb_file)

        lo: Coordinate = sut.mincrd()
        assert lo.x == 1.
        assert lo.y == 4.
        assert lo.z == 7.

    def test_max(self, pdb_file: str):

        sut = AtomList(pdb_file)

        hi: Coordinate = sut.maxcrd()
        assert hi.x == 3.
        assert hi.y == 6.
        assert hi.z == 9.

    def test_center(self, pdb_file: str):

        sut = AtomList(pdb_file)

        mi: Coordinate = sut.center()
        assert mi.x == 2.
        assert mi.y == 5.
        assert mi.z == 8.

    def test_max_radius(self, pdb_file: str):
        sut = CellList(pdb_file)
        assert sut.max_radius == 3.
