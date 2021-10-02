import pathlib
import pytest
from pyparsing import ParseException
from apbs.chemistry.atom_list import AtomList
from apbs.pqr import PQRReader


def search_dir(opt_path):
    return (
        pathlib.Path(__file__).parent.parent.parent.absolute()
        / "examples"
        / opt_path
    )


class TestPQRReader:
    def test_basic(self):
        """This test values in the records in the sample data"""
        sut = PQRReader()
        sample = r"""
ATOM   5226  HD1 TYR   337     -24.642  -2.718  30.187  0.115 1.358
                  
HETATM     39 O3PB ADP     1     -16.362  -6.763  26.980 -0.900 1.700
ATOM      7  CD   LYS D   1      44.946 37.289  9.712    -0.0608  1.9080
REMARK This is just a   comment hid ing in the data
ATOM     39 O3PB ADP     1  D   -16.362  -6.763  26.980 -0.900 1.700
"""
        atomlist: AtomList = sut.loads(sample)
        assert len(atomlist) == 4
        assert atomlist[1].field_name in "HETATM"
        for idx in range(1):
            assert abs(atomlist[idx].x - -24.642) < 0.001
            assert int(atomlist[idx].x) == -24

    @pytest.mark.xfail(
        raises=ParseException,
        reason="This should fail because of typo in data",
    )
    def test_bad(self):
        """This test will fail because there is a typo in the sample data"""
        sut = PQRReader()
        sample = r"""
ATOM   5226  HD1 TYR   337     -24.642  -2.718  30.187  0.115 1.358
REMARK The next line is incorrect on purpose (e.g. ATAM instead of ATOM)
ATAM     39 O3PB ADP     1  D   -16.362  -6.763  26.980 -0.900 1.700
"""
        atomlist: AtomList = sut.loads(sample)

    def test_load(self):
        """Test to load all the data from an example file"""
        sut = PQRReader()
        pqr_input = search_dir("actin-dimer/mol1.pqr")
        atomlist: AtomList = sut.load(pqr_input)
        assert len(atomlist) == 5877

    @pytest.mark.slow
    def test_load_all(self):
        """Test to load all the data from all the example files"""
        sut = PQRReader()
        # NOTE: There are 77 files under the examples directory
        matches = pathlib.Path(search_dir("")).glob("**/*.pqr")
        for match in matches:
            print(f"FILE: {match}")
            atomlist: AtomList = sut.load(match)
            assert len(atomlist) > 0
