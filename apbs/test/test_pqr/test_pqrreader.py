import pathlib
from apbs.chemistry.atom_list import AtomList
from apbs.pqr import PQRReader


class TestPQRReader:
    def test_ctor(self):
        sut = PQRReader()
        sample = r"""
ATOM   5226  HD1 TYR   337     -24.642  -2.718  30.187  0.115 1.358
ATOM      7  CD   LYS D   1      44.946 37.289  9.712    -0.0608  1.9080
REMARK This is just a comment hiding in the data
HETATM     39 O3PB ADP     1     -16.362  -6.763  26.980 -0.900 1.700
ATOM     39 O3PB ADP     1  D   -16.362  -6.763  26.980 -0.900 1.700
REMARK The next line is incorrect on purpose (e.g. ATAM instead of ATOM)
ATAM     39 O3PB ADP     1  D   -16.362  -6.763  26.980 -0.900 1.700
"""
        atomlist: AtomList = sut.loads(sample)
        for idx in range(1):
            assert abs(atomlist.atoms[idx].x - -24.642) < 0.001
            assert int(atomlist.atoms[idx].x) == -24
        print(atomlist.atoms)

    def test_load(self):
        sut = PQRReader()
        pqr_imput = (
            pathlib.Path(__file__).parent.parent.parent.parent.absolute()
            / "examples/actin-dimer/mol1.pqr"
        )
        atomlist: AtomList = sut.load(pqr_imput)
        assert len(atomlist.atoms) == 5861
