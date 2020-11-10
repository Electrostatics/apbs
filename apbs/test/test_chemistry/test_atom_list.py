from apbs.geometry import Coordinate
from apbs.chemistry import Atom, AtomList
from apbs.pqr import PQRReader
from pytest import approx, fixture


@fixture
def get_atom_list(tmp_path) -> AtomList:
    """Return the AtomList from the file generated."""

    fqpn = tmp_path / "atom.pdb"

    with open(fqpn, "w") as f:
        f.write(
            "HETATM    1  C    ALK    1       "
            "1.000   4.000   7.000 0.000 1.000\n"
        )
        f.write(
            "HETATM    1  C    ALK    1       "
            "2.000   5.000   8.000 0.000 2.000\n"
        )
        f.write(
            "HETATM    1  C    ALK    1       "
            "3.000   6.000   9.000 0.000 3.000\n"
        )

    reader = PQRReader()
    return reader.load(fqpn)


def test_atom_list(get_atom_list: AtomList):

    sut = get_atom_list
    assert sut.count == 3

    a: Atom = sut._atoms[0]
    assert a.atom_name == "C"
    assert a.residue_name == "ALK"
    assert a.x == 1.0
    assert a.y == 4.0
    assert a.z == 7.0
    assert 0.0 == approx(a.charge)
    assert 1.0 == approx(a.radius)


def test_min(get_atom_list: AtomList):

    sut = get_atom_list

    lo: Coordinate = sut.min_coord()
    assert lo.x == approx(1.0)
    assert lo.y == approx(4.0)
    assert lo.z == approx(7.0)


def test_max(get_atom_list: AtomList):

    sut = get_atom_list

    hi: Coordinate = sut.max_coord()
    assert hi.x == 3.0
    assert hi.y == 6.0
    assert hi.z == 9.0


def test_center(get_atom_list: AtomList):

    sut = get_atom_list

    mi: Coordinate = sut.center()
    assert mi.x == 2.0
    assert mi.y == 5.0
    assert mi.z == 8.0
