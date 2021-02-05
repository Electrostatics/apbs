import pathlib
import pytest
from pyparsing import ParseException
from apbs.ainput.apbs_legacy_input import ApbsLegacyInput


def search_dir(opt_path):
    return (
        pathlib.Path(__file__).parent.parent.parent.parent.absolute()
        / "examples"
        / opt_path
    )


def get_sample():
    return r"""
read 
    mol pqr mol1.pqr
    mol pqr mol2.pqr
    mol pqr complex.pqr
end

# CALCULATE POTENTIAL FOR FIRST COMPONENT 
elec name mol1
    mg-auto
    dime  161 161 161
    cglen 156 121 162
    fglen 112  91 116
    cgcent mol 3
    fgcent mol 3
    mol 1
    npbe
    bcfl sdh
    ion charge 1 conc 0.050 radius 2.0
    ion charge -1 conc 0.050 radius 2.0
    pdie 2.0
    sdie 78.4
    srfm mol
    chgm spl0
    srad 1.4
    swin 0.3
    sdens 10.0
    temp 298.15
    calcenergy total
    calcforce no
end

# CALCULATE POTENTIAL FOR SECOND COMPONENT
elec name mol2
    mg-auto
    dime  161 161 161
    cglen 156 121 162
    fglen 112  91 116
    cgcent mol 3
    fgcent mol 3
    mol 2
    npbe
    bcfl sdh
    ion charge 1 conc 0.050 radius 2.0
    ion charge -1 conc 0.050 radius 2.0
    pdie 2.0
    sdie 78.4
    srfm mol
    chgm spl0
    srad 1.4
    swin 0.3
    sdens 10.0
    temp 298.15 
    calcenergy total
    calcforce no
end

# CALCULATE POTENTIAL FOR COMPLEX
elec name complex
    mg-auto
    dime  161 161 161
    cglen 156 121 162
    fglen 112  91 116
    cgcent mol 3
    fgcent mol 3
    mol 3
    npbe
    bcfl sdh
    ion charge 1 conc 0.050 radius 2.0
    ion charge -1 conc 0.050 radius 2.0
    pdie 2.0
    sdie 78.4
    srfm mol
    chgm spl0
    srad 1.4
    swin 0.3
    sdens 10.0
    temp 298.15
    calcenergy total
    calcforce no
end

# COMBINE TO GIVE BINDING ENERGY
print elecEnergy complex - mol2 - mol1 end

quit
"""


class TestApbsLegacyInput:
    def test_basic(self):
        """This test values in the records in the sample data"""
        sut = ApbsLegacyInput()
        config: ApbsLegacyInput = sut.loads(get_sample())
        assert config[0][0] in "READ"

    @pytest.mark.xfail(
        raises=ParseException,
        reason="This should fail because of typo in data",
    )
    def test_bad(self):
        """This test will fail because there is a typo in the sample data"""
        sut = ApbsLegacyInput()
        config: ApbsLegacyInput = sut.loads(get_sample())

    def test_load(self):
        """Test to load all the data from an example file"""
        sut = ApbsLegacyInput()
        pqr_imput = search_dir("actin-dimer/apbs-mol-auto.in")
        config: ApbsLegacyInput = sut.load(pqr_imput)
        assert len(config) == 5

    @pytest.mark.slow
    def test_load_all(self):
        """Test to load all the data from all the example files"""
        sut = ApbsLegacyInput()
        # NOTE: There are 135 files under the examples directory
        matches = pathlib.Path(search_dir("")).glob("**/*.in")
        for match in matches:
            print(f"FILE: {match}")
            config: ApbsLegacyInput = sut.load(match)
            print(config)
            assert len(config) > 0
