import json
from pyparsing import (
    CaselessLiteral,
    Group,
    OneOrMore,
    Optional,
    ParseException,
    Regex,
    Suppress,
    Word,
    ZeroOrMore,
    alphas,
    alphanums,
    nums,
    oneOf,
    printables,
    restOfLine,
)

from re import VERBOSE

# from . import ApbsConfig


def convertNumber(t):
    """Convert a string matching a number to a python number"""
    if t.float1 or t.float2 or t.float3:
        return [float(t[0])]
    else:
        return [int(t[0])]


# number : match any number and return asscoiated python value
integer_val = Optional("-") + Word(nums)
number = Regex(
    r"""
       [+-]?                           # optional sign
        (
           (?:\d+(?P<float1>\.\d*)?)   # match 2 or 2.02
         |                             # or
           (?P<float2>\.\d+)           # match .02
        )
        (?P<float3>[Ee][+-]?\d+)?      # optional exponent
       """,
    flags=VERBOSE,
)
number.setParseAction(convertNumber)


class ApbsLegacyInput:

    # Questions:
    #   1. Can READ, ELEC, and PRINT have leading/trailing spaces?
    #      A: Yes
    #   2. do keyword and values HAVE to be on the same line?
    #   3. are keyword and non-path values case sensitive?
    #      A: No

    identifier = Word(alphas, alphanums + r"_") | integer_val
    anything = Word(printables + " " + "\t")
    comment = "#"
    path_val = anything

    # Section Keywords
    read_val = CaselessLiteral("READ")
    elec_val = CaselessLiteral("ELEC")
    print_val = CaselessLiteral("PRINT")
    end_val = CaselessLiteral("END")
    quit_val = CaselessLiteral("QUIT")

    # READ section specific grammar
    # https://apbs.readthedocs.io/en/latest/using/input/read.html
    read_format_val = oneOf("dx gz", caseless=True)
    # charge format path
    #     is path considered relative?
    charge_val = CaselessLiteral("charge")
    charge_value = Group(charge_val + read_format_val + path_val)
    # diel format(dx) path-x, path-y, path-z
    #     are path-x, path-y, path-z considered relative?
    #     where to find non-zero ionic strength
    diel_val = CaselessLiteral("diel")
    diel_value = Group(
        diel_val + read_format_val + path_val + path_val + path_val
    )
    # kappa format path
    #     dependant on a previous diel
    #     is path considered relative?
    kappa_val = CaselessLiteral("kappa")
    kappa_value = Group(kappa_val + read_format_val + path_val)
    # mol format(pqr|pdb) path
    #     is path considered relative?
    mol_val = CaselessLiteral("mol")
    mol_format_val = oneOf("pqr pdb", caseless=True)
    mol_value = Group(mol_val + mol_format_val + path_val)
    # parm format(flat) path
    #     is path considered relative?
    parm_val = CaselessLiteral("parm")
    parm_format_val = oneOf("flat xml", caseless=True)
    parm_value = Group(parm_val + parm_format_val + path_val)
    # pot format(dx|gz) path
    #     is path considered relative?
    pot_val = CaselessLiteral("parm")
    pot_value = Group(pot_val + read_format_val + path_val)

    read_body = Group(
        OneOrMore(mol_value)
        & ZeroOrMore(charge_value)
        & ZeroOrMore(diel_value)
        & ZeroOrMore(kappa_value)
        & ZeroOrMore(parm_value)
        & ZeroOrMore(pot_value)
    )
    read_value = Group(read_val + read_body + Suppress(end_val))

    # ELEC section specific grammar
    # https://apbs.readthedocs.io/en/latest/using/input/elec/index.html
    elec_name_val = CaselessLiteral("name")
    elec_name_value = Group(elec_name_val + identifier)

    # TODO: There must be one (and only one?) of these
    #       plus there are keywords that unique to each option
    elec_type_options_val = oneOf(
        "mg-auto mg-para mg-manual geoflow-auto tabi pbam-auto pbsam-auto fe-manual mg-dummy",
        caseless=True,
    )
    elec_type_value = Group(elec_type_options_val)

    elec_mol_val = CaselessLiteral("mol")
    elec_mol_value = Group(elec_mol_val + number)

    # General Keywords used by multiple ELEC types
    # Are charge, conc, and radius ALL required?
    ion_val = CaselessLiteral("ion")
    ion_charge_val = Group(CaselessLiteral("charge") + number)
    ion_conc_val = Group(CaselessLiteral("conc") + number)
    ion_radius_val = Group(CaselessLiteral("radius") + number)
    ion_value = Group(ion_val + ion_charge_val & ion_conc_val & ion_radius_val)

    pdie_val = CaselessLiteral("pdie")
    # TODO: Number must be >= 1
    pdie_value = Group(pdie_val + number)

    sdens_val = CaselessLiteral("sdens")
    sdens_value = Group(sdens_val + number)

    # NOTE: Should be a value between 78-80?
    sdie_val = CaselessLiteral("sdie")
    sdie_value = Group(sdie_val + number)

    srad_val = CaselessLiteral("srad")
    srad_value = Group(srad_val + number)

    temp_val = CaselessLiteral("temp")
    temp_value = Group(temp_val + number)

    usemap_val = CaselessLiteral("usemap")
    # tabi Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/tabi.html
    tabi_val = CaselessLiteral("tabi")
    # ion - use general ELEC keyword for ion_value
    # mac
    mac_val = CaselessLiteral("mac")
    # TODO: This should be replaced which a check to make
    # sure that "number is between 0.0 and 1.0"
    mac_value = Group(mac_val + number)
    # mesh
    mesh_val = CaselessLiteral("mesh")
    mesh_options_val = oneOf("0 1 2")
    mesh_value = Group(mesh_val + mesh_options_val)
    # mol - use general ELEC keyword for elec_mol_value
    # outdata
    outdata_val = CaselessLiteral("outdata")
    outdata_options_val = oneOf("0 1")
    outdata_value = Group(outdata_val + outdata_options_val)
    # pdie - use general ELEC keyword for pdie_value
    # sdens - use general ELEC keyword for sdens_value
    # sdie - use general ELEC keyword for sdie_value
    # srad - use general ELEC keyword for srad_value
    # temp - use general ELEC keyword for temp_value
    # tree_n0
    tree_n0_val = CaselessLiteral("tree_n0")
    tree_n0_value = Group(tree_n0_val + integer_val)
    # tree_order
    tree_order_val = CaselessLiteral("tree_order")
    tree_order_value = Group(tree_order_val + integer_val)

    tabi_body = (
        tabi_val
        & ZeroOrMore(ion_value)
        & ZeroOrMore(mac_value)
        & ZeroOrMore(mesh_value)
        & ZeroOrMore(mol_value)
        & ZeroOrMore(outdata_value)
        & ZeroOrMore(pdie_value)
        & ZeroOrMore(sdens_value)
        & ZeroOrMore(sdie_value)
        & ZeroOrMore(srad_value)
        & ZeroOrMore(temp_value)
        & ZeroOrMore(tree_n0_value)
        & ZeroOrMore(tree_order_value)
    )

    # ELEC Keywords
    akeyPRE_val = CaselessLiteral("akeyPRE")
    akeyPRE_options_val = oneOf("unif geom", caseless=True)
    akeyPRE_value = Group(akeyPRE_val + akeyPRE_options_val)

    akeySOLVE_val = CaselessLiteral("akeySOLVE")
    akeySOLVE_options_val = oneOf("resi", caseless=True)
    akeySOLVE_value = Group(akeySOLVE_val + akeySOLVE_options_val)

    async_val = CaselessLiteral("async")
    async_value = Group(async_val + integer_val)

    bcfl_val = CaselessLiteral("bcfl")
    bcfl_options_val = oneOf("zero sdh mdh focus", caseless=True)
    bcfl_value = Group(bcfl_val + bcfl_options_val)

    # TODO: Should be able to combine calcenergy and calcforce?
    calcenergy_val = CaselessLiteral("calcenergy")
    calcenergy_options_val = oneOf("no total comps", caseless=True)
    calcenergy_value = Group(calcenergy_val + calcenergy_options_val)

    calcforce_val = CaselessLiteral("calcforce")
    calcforce_options_val = oneOf("no total comps", caseless=True)
    calcforce_value = Group(calcforce_val + calcforce_options_val)

    cgcent_val = CaselessLiteral("cgcent")
    cgcent_mol_val = Group(CaselessLiteral("mol") + integer_val)
    cgcent_coord_val = Group(number * 3)
    cgcent_value = Group(cgcent_val + (cgcent_mol_val | cgcent_coord_val))

    cglen_val = CaselessLiteral("cglen")
    cglen_coord_val = Group(integer_val * 3)
    cglen_value = Group(cglen_val + cgcent_coord_val)

    chgm_val = CaselessLiteral("chgm")
    chgm_options_val = oneOf("spl0 spl2", caseless=True)
    chgm_value = Group(chgm_val + chgm_options_val)

    dime_val = CaselessLiteral("dime")
    dime_coord_val = Group(integer_val * 3)
    dime_value = Group(dime_val + dime_coord_val)

    ekey_val = CaselessLiteral("ekey")
    ekey_options_val = oneOf("simp global frac", caseless=True)
    ekey_value = Group(ekey_val + ekey_options_val)

    etol_val = CaselessLiteral("etol")
    etol_value = Group(etol_val + number)

    fgcent_val = CaselessLiteral("fgcent")
    fgcent_mol_val = Group(CaselessLiteral("mol") + number)
    fgcent_coord_val = Group(number * 3)
    fgcent_value = Group(fgcent_val + (fgcent_mol_val | fgcent_coord_val))

    fglen_val = CaselessLiteral("fglen")
    fglen_coord_val = Group(number * 3)
    fglen_value = Group(fglen_val + fglen_coord_val)

    gamma_val = CaselessLiteral("gamma")
    gamma_value = Group(gamma_val + number)

    gcent_val = CaselessLiteral("gcent")
    gcent_mol_val = CaselessLiteral("mol") + number
    gcent_coord_val = Group(number * 3)
    gcent_value = Group(gcent_val + (gcent_mol_val | gcent_coord_val))

    glen_val = CaselessLiteral("glen")
    glen_coord_val = Group(number * 3)
    glen_value = Group(glen_val + glen_coord_val)

    grid_val = CaselessLiteral("grid")
    grid_coord_val = Group(number * 3)
    grid_value = Group(grid_val + grid_coord_val)

    elec_maxsolve_val = CaselessLiteral("maxsolve")
    elec_maxsolve_value = Group(elec_maxsolve_val + number)

    elec_maxvert_val = CaselessLiteral("maxvert")
    elec_maxvert_value = Group(elec_maxvert_val + number)

    elec_nlev_val = CaselessLiteral("nlev")
    elec_nlev_value = Group(elec_nlev_val + number)

    # TODO: I think only 1 of these are allowed (not ZeroOrMore)
    elec_pbe_options_val = oneOf("lpbe lrpbe npbe nrpbe", caseless=True)
    elec_pbe_value = Group(elec_pbe_options_val)

    # TODO: Combine with dime?
    pdime_val = CaselessLiteral("pdime")
    pdime_coord_val = Group(number * 3)
    pdime_value = Group(pdime_val + pdime_coord_val)

    ofrac_val = CaselessLiteral("ofrac")
    ofrac_value = Group(ofrac_val + number)

    swin_val = CaselessLiteral("swin")
    swin_value = Group(swin_val + number)

    srfm_val = CaselessLiteral("srfm")
    srfm_options_val = oneOf("mol smol spl2", caseless=True)
    srfm_value = Group(srfm_val + srfm_options_val)

    targetRes_val = CaselessLiteral("targetRes")
    targetRes_value = Group(targetRes_val + number)

    usemap_options_val = oneOf("diel kappa charge", caseless=True)
    usemap_value = Group(usemap_val + integer_val)

    write_val = CaselessLiteral("write")
    write_type_options_val = oneOf(
        "charge pot smol sspl vdw ivdw lap edens ndens qdens dielx diely dielz kappa",
        caseless=True,
    )
    write_format_options_val = oneOf("dx avs uhbd", caseless=True)
    write_value = Group(
        usemap_val
        + write_type_options_val
        + write_format_options_val
        + path_val
    )

    elec_body = (
        tabi_body
        #        | fe_manual_body
    )

    elec_body = (
        OneOrMore(elec_type_value)
        & ZeroOrMore(elec_name_value)
        & ZeroOrMore(akeyPRE_value)
        & ZeroOrMore(akeySOLVE_value)
        & ZeroOrMore(async_value)
        & ZeroOrMore(bcfl_value)
        & ZeroOrMore(calcenergy_value)
        & ZeroOrMore(calcforce_value)
        & ZeroOrMore(cgcent_value)
        & ZeroOrMore(cglen_value)
        & ZeroOrMore(chgm_value)
        & ZeroOrMore(dime_value)
        & ZeroOrMore(ekey_value)
        & ZeroOrMore(etol_value)
        & ZeroOrMore(fgcent_value)
        & ZeroOrMore(fglen_value)
        & ZeroOrMore(gamma_value)
        & ZeroOrMore(gcent_value)
        & ZeroOrMore(glen_value)
        & ZeroOrMore(grid_value)
        & ZeroOrMore(ion_value)
        & ZeroOrMore(elec_maxsolve_value)
        & ZeroOrMore(elec_maxvert_value)
        & ZeroOrMore(elec_mol_value)
        & ZeroOrMore(elec_nlev_value)
        & ZeroOrMore(elec_pbe_value)
        & ZeroOrMore(pdie_value)
        & ZeroOrMore(pdime_value)
        & ZeroOrMore(ofrac_value)
        & ZeroOrMore(sdie_value)
        & ZeroOrMore(sdens_value)
        & ZeroOrMore(srad_value)
        & ZeroOrMore(swin_value)
        & ZeroOrMore(srfm_value)
        & ZeroOrMore(targetRes_value)
        & ZeroOrMore(temp_value)
        & ZeroOrMore(usemap_value)
        & ZeroOrMore(write_value)
    )

    elec_value = Group(elec_val + elec_body + Suppress(end_val))

    # PRINT section specific grammar
    print_what_val = oneOf(
        "elecEnergy elecForce apolEnergy apolForce", caseless=True
    )
    print_expr = (
        identifier
        + OneOrMore(oneOf("+ -") + identifier)
        + ZeroOrMore(oneOf("+ -") + identifier)
    )
    print_body = Group(print_what_val + print_expr)

    print_value = Group(print_val + print_body + Suppress(end_val))

    # all_values = OneOrMore(print_value) + Suppress(quit_val)
    all_values = (
        OneOrMore(read_value)
        + OneOrMore(elec_value)
        + OneOrMore(print_value)
        + Suppress(quit_val)
    )

    def __init__(self):
        pass

    def loads(self, input_data: str):
        """Parse the input as a string

        :param str filename: The APBS legacy input configuration file
        :return: the ApbsConfig object
        :rtype: ApbsConfig
        """
        parser = self.all_values
        parser.ignore(self.comment + restOfLine)

        results = self.all_values.searchString(input_data)
        print(results.dump())
        for item in results:
            print(f"ITEM: {item.read}")
            return item

    def load(self, filename: str):
        """
        Read Legacy Input congifuration file and pass it to loads as a string

        :param str filename: The APBS legacy input configuration file
        :return: the ApbsConfig object
        :rtype: ApbsConfig
        """
        with filename.open() as fp:
            data = fp.read()
        try:
            data = self.loads(data)
            return data

        except ParseException as pe:
            # complete the error message
            msg = "ERROR during parsing of %s,  line %d:" % (
                filename,
                pe.lineno,
            )
            msg += "\n" + "-" * 40 + "\n"
            msg += pe.line + "\n"
            msg += " " * (pe.col - 1) + "^\n"
            msg += "-" * 40 + "\n" + pe.msg
            pe.msg = msg
            raise


if __name__ == "__main__":
    # execute only if run as a script
    relfilename = "../../examples/actin-dimer/apbs-mol-auto.in"
    test = ApbsLegacyInput()
    from pathlib import Path

    curr_dir = Path(__file__).parent
    absfilename = curr_dir / relfilename
    print(test.load(absfilename))
