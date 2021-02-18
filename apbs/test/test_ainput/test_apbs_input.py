from pathlib import Path
from re import A
import pytest
from pyparsing import ParseSyntaxException
from apbs.ainput.apbs_legacy_input import (
    ApbsLegacyInput,
    get_example_files,
    printBanner,
)


def get_bad_sample():
    return r"""
qdens-complex-0.250.dx
qdens-pep-0.250.dx -
qdens-rna-0.250.dx -
qdens-diff-0.250.dx 
"""


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
        print(f"CONFIG: {config['READ'][0]}")
        assert "READ" in config
        assert "mol" in config["READ"][0]
        assert "pqr" in config["READ"][0]["mol"]
        assert len(config["READ"][0]["mol"]["pqr"]) == 3

    @pytest.mark.xfail(
        raises=ParseSyntaxException,
        reason="This should fail because of typo in data",
    )
    def test_bad(self):
        """This test will fail because there is a typo in the sample data"""
        sut = ApbsLegacyInput()
        with pytest.raises(ParseSyntaxException):
            config: ApbsLegacyInput = sut.loads(get_bad_sample())

    def test_load(self):
        """Test to load all the data from an example file"""
        sut = ApbsLegacyInput()
        relfilename = "actin-dimer/apbs-mol-auto.in"
        example_dir = relfilename.split("/")[0]
        example_pattern = relfilename.split("/")[1]
        files = []
        files = get_example_files(example_dir, example_pattern)
        for file in files:
            sut = ApbsLegacyInput()
            config = sut.load(file)
            assert len(config["READ"][0]["mol"]["pqr"]) == 3

    @pytest.mark.slow
    def test_load_all(self):
        """Test to load all the data from all the example files"""
        # NOTE: There are 100+ sample input files under the examples directory
        files = get_example_files()
        for idx, file in enumerate(files):
            sut = ApbsLegacyInput()
            printBanner(f"FILE {idx}:", file)
            config: ApbsLegacyInput = sut.load(file)
            print(config)
            assert len(config) > 0
            assert len(config["READ"][0]["mol"]) > 0
            if "ELEC" in config:
                assert len(config["ELEC"][0]) > 0
            if "APOLAR" in config:
                assert len(config["APOLAR"][0]) > 0
