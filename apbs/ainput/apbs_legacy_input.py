import pprint
from pyparsing import CaselessLiteral as CLiteral
from pyparsing import (
    Group,
    LineEnd,
    OneOrMore,
    Optional,
    ParseException,
    ParseResults,
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


def convertNumber(t: ParseResults):
    """Convert a string matching a NUMBER_VAL to a python NUMBER_VAL"""
    if t.float1 or t.float2 or t.float3:
        return [float(t[0])]
    else:
        return [int(t[0])]


# GLOBAL Values
EOL = LineEnd().suppress()
INTEGER_VAL = Optional("-") + Word(nums)
NUMBER_VAL = Regex(
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
).setParseAction(convertNumber)
# NUMBER_VAL = Word(nums).setParseAction(lambda s, l, t: [float(t[0])])

IDENTIFIER = Word(alphas, alphanums + r"_") | INTEGER_VAL
PATH_VAL = Word(printables + " " + "\t")
COMMENT = "#"
END_VAL = CLiteral("END")
FINAL_OUTPUT = {}


def formatBlock(t: ParseResults, section: str, groups: list):
    """Convert lists of lists to a dictionary"""

    if section not in FINAL_OUTPUT.keys():
        FINAL_OUTPUT[section] = {}

    # print(f"T: {t}")
    for row in t[0]:
        # print(f"TYPE: {type(row)}")
        for item in row:
            # print(f"type item: {type(item)} {item}")
            if isinstance(item, ParseResults):
                key = item[0].lower()
                # print(f"key: {key}")
                if key in groups:
                    if key not in FINAL_OUTPUT[section].keys():
                        # print(f"ADD KEY: {key}")
                        FINAL_OUTPUT[section][key] = {}
                    subkey = f"{item[1]}".lower()
                    if subkey not in FINAL_OUTPUT[section][key].keys():
                        # print(f"ADD SUBKEY: {subkey}")
                        FINAL_OUTPUT[section][key][subkey] = []
                    if item[2] is not None:
                        # print(
                        #    f"VALUES: KEY: {key}, SUBKEY: {subkey}, item2: {item[2]}"
                        # )
                        item2 = ", ".join(item[2].split())
                        FINAL_OUTPUT[section][key][subkey].append(item2)

    return FINAL_OUTPUT


def readParser():

    # READ section specific grammar
    # https://apbs.readthedocs.io/en/latest/using/input/read.html

    file_fmt = oneOf("dx gz", caseless=True)

    # charge format - is path considered relative?
    charge = Group(CLiteral("charge") - file_fmt - PATH_VAL)

    # diel format(dx) path-x, path-y, path-z
    #     are path-x, path-y, path-z considered relative?
    #     where to find non-zero ionic strength
    diel = Group(CLiteral("diel") + file_fmt - PATH_VAL - PATH_VAL - PATH_VAL)

    # kappa format path - is path considered relative?
    kappa = Group(CLiteral("kappa") - file_fmt - PATH_VAL)

    # mol format(pqr|pdb) path - is path considered relative?
    mol_format = oneOf("pqr pdb", caseless=True)
    mol = Group(CLiteral("mol") - mol_format - PATH_VAL)

    # parm format(flat) path - is path considered relative?
    parm_format = oneOf("flat xml", caseless=True)
    parm = Group(CLiteral("parm") - parm_format - PATH_VAL)

    # pot format(dx|gz) path - is path considered relative?
    pot = Group(CLiteral("pot") - file_fmt - PATH_VAL)

    body = Group(
        OneOrMore(mol)
        & ZeroOrMore(charge)
        & ZeroOrMore(diel)
        & ZeroOrMore(kappa)
        & ZeroOrMore(parm)
        & ZeroOrMore(pot)
    )

    def formatRead(t: ParseResults):
        groups = ["charge", "diel", "kappa", "mol", "param", "pot"]
        return formatBlock(t, "READ", groups)

    return Group(
        Suppress(CLiteral("READ")) - body - Suppress(END_VAL)
    ).setParseAction(formatRead)


def printParser():
    """Setup the grammar for the PRINT section."""

    # PRINT section specific grammar
    val = CLiteral("PRINT")
    choices = oneOf("elecEnergy elecForce apolEnergy apolForce", caseless=True)
    expr = (
        IDENTIFIER
        + OneOrMore(oneOf("+ -") + IDENTIFIER)
        + ZeroOrMore(oneOf("+ -") + IDENTIFIER)
    )
    body = Group(choices - expr)

    def formatPrint(t: ParseResults):
        section = "PRINT"
        if section not in FINAL_OUTPUT.keys():
            FINAL_OUTPUT[section] = {}

        for row in t[0]:
            # print(f"TYPE: {type(row)}")
            for item in row:
                # print(f"type item: {type(item)} {item}")
                if isinstance(item, str):
                    key = item.lower()
                    # print(f"key: {key}")
                    if key not in FINAL_OUTPUT[section].keys():
                        FINAL_OUTPUT[section][key] = row[1:]
                        break
            else:
                break
            break

        return FINAL_OUTPUT

    value = Group(Suppress(val) + body + Suppress(END_VAL)).setParseAction(
        formatPrint
    )

    return value


def apolarParser():

    val = CLiteral("APOLAR")

    body = (
        val
        & ZeroOrMore(genericToken.bconc_value)
        & ZeroOrMore(genericToken.calcenergy_value)
        & ZeroOrMore(genericToken.calcforce_value)
        & ZeroOrMore(apolarToken.dpos_value)
        & ZeroOrMore(genericToken.gamma_value)
        & ZeroOrMore(genericToken.grid_value)
        & ZeroOrMore(genericToken.mol_value)
        & ZeroOrMore(apolarToken.press_value)
        & ZeroOrMore(genericToken.sdens_value)
        & ZeroOrMore(genericToken.srad_value)
        & ZeroOrMore(apolarToken.srfm_value)
        & ZeroOrMore(genericToken.swin_value)
        & ZeroOrMore(genericToken.temp_value)
    )

    def formatApolar(t: ParseResults):

        section = "APOLAR"
        if section not in FINAL_OUTPUT.keys():
            FINAL_OUTPUT[section] = {}

        for row in t[0]:
            print(f"TYPE: {type(row)}")
            for item in row:
                print(f"type item: {type(item)} {item}")
                if isinstance(item, str):
                    key = item.lower()
                    print(f"key: {key}")
                    if key not in FINAL_OUTPUT[section].keys():
                        FINAL_OUTPUT[section][key] = row[1:]

        return FINAL_OUTPUT

    value = Group(Suppress(val) + body + Suppress(END_VAL)).setParseAction(
        formatApolar
    )

    return value


def elecParser():

    val = CLiteral("ELEC")

    body = (
        tabiParser.body
        | fe_manualParser.body
        | geoflow_autoParser.body
        | mg_autoParser.body
        | mg_manualParser.body
        | mg_paraParser.body
        | mg_dummyParser.body
        | pbam_autoParser.body
        | pbsam_autoParser.body
    )

    def formatElec(t: ParseResults):

        section = "ELEC"
        if section not in FINAL_OUTPUT.keys():
            FINAL_OUTPUT[section] = {}

        for row in t[0]:
            # print(f"ELEC TYPE: {type(row)} ROW: {row}")
            if isinstance(row, str):
                FINAL_OUTPUT[section]["type"] = row
                continue
            if len(row) == 1:
                FINAL_OUTPUT[section]["pbe"] = row[0]
                continue
            if len(row) == 2:
                # print(f"KEY: {row[0]} TYPE {type(row[1])}")
                value = row[1]
                if isinstance(row[1], ParseResults):
                    value = (row[1]).asList()
                FINAL_OUTPUT[section][row[0]] = value
                continue
            # print(f"WHAT KEY: {row[0]}")
            if row[0] not in FINAL_OUTPUT.keys():
                FINAL_OUTPUT[section][row[0]] = []
            for item in row[1:]:
                # print(f"WHAT: {item}")
                FINAL_OUTPUT[section][row[0]].append([item[0], item[1]])

        return FINAL_OUTPUT

    value = Group(Suppress(val) + body + Suppress(END_VAL)).setParseAction(
        formatElec
    )

    return value


class genericToken:

    bconc_val = CLiteral("bconc")
    bconc_value = Group(bconc_val + NUMBER_VAL)

    # TODO: Should be able to combine calcenergy and calcforce?
    calcenergy_val = CLiteral("calcenergy")
    calcenergy_options_val = oneOf("no total comps", caseless=True)
    calcenergy_value = Group(calcenergy_val + calcenergy_options_val)

    calcforce_val = CLiteral("calcforce")
    calcforce_options_val = oneOf("no total comps", caseless=True)
    calcforce_value = Group(calcforce_val + calcforce_options_val)

    gamma_val = CLiteral("gamma")
    gamma_value = Group(gamma_val + NUMBER_VAL)

    grid_val = CLiteral("grid")
    grid_coord_val = Group(NUMBER_VAL * 3)
    grid_value = Group(grid_val + grid_coord_val)

    mol_val = CLiteral("mol")
    mol_value = Group(mol_val + NUMBER_VAL)

    sdens_val = CLiteral("sdens")
    sdens_value = Group(sdens_val + NUMBER_VAL)

    srad_val = CLiteral("srad")
    srad_value = Group(srad_val + NUMBER_VAL)

    swin_val = CLiteral("swin")
    swin_value = Group(swin_val + NUMBER_VAL)

    temp_val = CLiteral("temp")
    temp_value = Group(temp_val + NUMBER_VAL)


class apolarToken:

    dpos_value = Group(CLiteral("dpos") - NUMBER_VAL)

    press_val = CLiteral("press")
    press_value = Group(press_val + NUMBER_VAL)

    srfm_val = CLiteral("srfm")
    srfm_options_val = oneOf("sacc", caseless=True)
    srfm_value = Group(srfm_val + srfm_options_val)


class elecToken:

    # ELEC section specific grammar
    # https://apbs.readthedocs.io/en/latest/using/input/elec/index.html

    # TODO: There must be one (and only one?) of these
    #       plus there are keywords that unique to each option
    type_options_val = oneOf(
        "mg-auto mg-para mg-manual geoflow-auto tabi pbam-auto pbsam-auto fe-manual mg-dummy",
        caseless=True,
    )
    type_value = Group(type_options_val)

    # The following tokens are used by at least 2 of the parser types with
    # the same format and rules

    name_val = CLiteral("name")
    name_value = Group(name_val + IDENTIFIER)

    async_val = CLiteral("async")
    async_value = Group(async_val + INTEGER_VAL)

    bcfl_val = CLiteral("bcfl")
    bcfl_options_val = oneOf("zero sdh mdh focus", caseless=True)
    bcfl_value = Group(bcfl_val + bcfl_options_val)

    cgcent_val = CLiteral("cgcent")
    cgcent_mol_val = Group(CLiteral("mol") + INTEGER_VAL)
    cgcent_coord_val = Group(NUMBER_VAL * 3)
    cgcent_value = Group(cgcent_val + (cgcent_mol_val | cgcent_coord_val))

    cglen_val = CLiteral("cglen")
    cglen_coord_val = Group(INTEGER_VAL * 3)
    cglen_value = Group(cglen_val + cglen_coord_val)

    chgm_val = CLiteral("chgm")
    chgm_options_val = oneOf("spl0 spl2", caseless=True)
    chgm_value = Group(chgm_val + chgm_options_val)

    dime_val = CLiteral("dime")
    dime_coord_val = Group(INTEGER_VAL * 3)
    dime_value = Group(dime_val + dime_coord_val)

    etol_val = CLiteral("etol")
    etol_value = Group(etol_val + NUMBER_VAL)

    fgcent_val = CLiteral("fgcent")
    fgcent_mol_val = Group(CLiteral("mol") + NUMBER_VAL)
    fgcent_coord_val = Group(NUMBER_VAL * 3)
    fgcent_value = Group(fgcent_val + (fgcent_mol_val | fgcent_coord_val))

    fglen_val = CLiteral("fglen")
    fglen_coord_val = Group(NUMBER_VAL * 3)
    fglen_value = Group(fglen_val + fglen_coord_val)

    gcent_val = CLiteral("gcent")
    gcent_mol_val = CLiteral("mol") + NUMBER_VAL
    gcent_coord_val = Group(NUMBER_VAL * 3)
    gcent_value = Group(gcent_val + (gcent_mol_val | gcent_coord_val))

    glen_val = CLiteral("glen")
    glen_coord_val = Group(NUMBER_VAL * 3)
    glen_value = Group(glen_val + glen_coord_val)

    # Are charge, conc, and radius ALL required?
    ion_val = CLiteral("ion")
    ion_charge_val = Group(CLiteral("charge") + NUMBER_VAL)
    ion_conc_val = Group(CLiteral("conc") + NUMBER_VAL)
    ion_radius_val = Group(CLiteral("radius") + NUMBER_VAL)
    ion_value = Group(ion_val + ion_charge_val & ion_conc_val & ion_radius_val)

    nlev_val = CLiteral("nlev")
    nlev_value = Group(nlev_val + NUMBER_VAL)

    # TODO: I think only 1 of these are allowed (not ZeroOrMore)
    pbe_options_val = oneOf("lpbe lrpbe npbe nrpbe", caseless=True)
    pbe_value = Group(pbe_options_val)

    pdie_val = CLiteral("pdie")
    # TODO: Number must be >= 1
    pdie_value = Group(pdie_val + NUMBER_VAL)

    # NOTE: Should be a value between 78-80?
    sdie_val = CLiteral("sdie")
    sdie_value = Group(sdie_val + NUMBER_VAL)

    srfm_val = CLiteral("srfm")
    srfm_options_val = oneOf("mol smol spl2", caseless=True)
    srfm_value = Group(srfm_val + srfm_options_val)

    usemap_val = CLiteral("usemap")
    usemap_options_val = oneOf("diel kappa charge", caseless=True)
    usemap_value = Group(usemap_val + INTEGER_VAL)

    write_val = CLiteral("write")
    write_type_options_val = oneOf(
        "charge pot smol sspl vdw ivdw lap edens ndens qdens dielx diely dielz kappa",
        caseless=True,
    )
    write_format_options_val = oneOf("dx avs uhbd", caseless=True)
    write_value = Group(
        usemap_val
        + write_type_options_val
        + write_format_options_val
        + PATH_VAL
    )

    writemat_val = CLiteral("writemat")
    writemat_options_val = oneOf("poisson", caseless=True)
    writemat_value = Group(writemat_val + writemat_options_val + PATH_VAL)


class tabiParser:

    # tabi Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/tabi.html
    tabi_val = CLiteral("tabi")

    mac_val = CLiteral("mac")
    # TODO: This should be replaced which a check to make
    # sure that "NUMBER_VAL is between 0.0 and 1.0"
    mac_value = Group(mac_val + NUMBER_VAL)

    mesh_val = CLiteral("mesh")
    mesh_options_val = oneOf("0 1 2")
    mesh_value = Group(mesh_val + mesh_options_val)

    outdata_val = CLiteral("outdata")
    outdata_options_val = oneOf("0 1")
    outdata_value = Group(outdata_val + outdata_options_val)

    tree_n0_val = CLiteral("tree_n0")
    tree_n0_value = Group(tree_n0_val + INTEGER_VAL)

    tree_order_val = CLiteral("tree_order")
    tree_order_value = Group(tree_order_val + INTEGER_VAL)

    body = (
        tabi_val
        & ZeroOrMore(elecToken.name_value)
        & ZeroOrMore(elecToken.ion_value)
        & ZeroOrMore(mac_value)
        & ZeroOrMore(mesh_value)
        & ZeroOrMore(genericToken.mol_value)
        & ZeroOrMore(outdata_value)
        & ZeroOrMore(elecToken.pdie_value)
        & ZeroOrMore(genericToken.sdens_value)
        & ZeroOrMore(elecToken.sdie_value)
        & ZeroOrMore(genericToken.srad_value)
        & ZeroOrMore(genericToken.temp_value)
        & ZeroOrMore(tree_n0_value)
        & ZeroOrMore(tree_order_value)
    )


class fe_manualParser:

    # fe-manual Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/fe-manual.html
    fe_manual_val = CLiteral("fe-manual")

    akeyPRE_val = CLiteral("akeyPRE")
    akeyPRE_options_val = oneOf("unif geom", caseless=True)
    akeyPRE_value = Group(akeyPRE_val + akeyPRE_options_val)

    akeySOLVE_val = CLiteral("akeySOLVE")
    akeySOLVE_options_val = oneOf("resi", caseless=True)
    akeySOLVE_value = Group(akeySOLVE_val + akeySOLVE_options_val)

    domainLength_val = CLiteral("domainLength")
    domainLength_coord_val = Group(NUMBER_VAL * 3)
    domainLength_value = Group(domainLength_val + domainLength_coord_val)

    ekey_val = CLiteral("ekey")
    ekey_options_val = oneOf("simp global frac", caseless=True)
    ekey_value = Group(ekey_val + ekey_options_val)

    maxsolve_val = CLiteral("maxsolve")
    maxsolve_value = Group(maxsolve_val + NUMBER_VAL)

    maxvert_val = CLiteral("maxvert")
    maxvert_value = Group(maxvert_val + NUMBER_VAL)

    targetNum_val = CLiteral("targetNum")
    targetNum_value = Group(targetNum_val + INTEGER_VAL)

    targetRes_val = CLiteral("targetRes")
    targetRes_value = Group(targetRes_val + NUMBER_VAL)

    body = (
        fe_manual_val
        & ZeroOrMore(elecToken.name_value)
        & ZeroOrMore(akeyPRE_value)
        & ZeroOrMore(akeySOLVE_value)
        & ZeroOrMore(elecToken.async_value)
        & ZeroOrMore(elecToken.bcfl_value)
        & ZeroOrMore(genericToken.calcenergy_value)
        & ZeroOrMore(genericToken.calcforce_value)
        & ZeroOrMore(elecToken.chgm_value)
        & ZeroOrMore(domainLength_value)
        & ZeroOrMore(ekey_value)
        & ZeroOrMore(elecToken.etol_value)
        & ZeroOrMore(elecToken.ion_value)
        & ZeroOrMore(elecToken.pbe_value)  # lpbe lrpbe npbe nrpbe
        & ZeroOrMore(maxsolve_value)
        & ZeroOrMore(maxvert_value)
        & ZeroOrMore(genericToken.mol_value)
        & ZeroOrMore(elecToken.pdie_value)
        & ZeroOrMore(genericToken.sdens_value)
        & ZeroOrMore(elecToken.sdie_value)
        & ZeroOrMore(genericToken.srad_value)
        & ZeroOrMore(elecToken.srfm_value)
        & ZeroOrMore(genericToken.swin_value)
        & ZeroOrMore(targetNum_value)
        & ZeroOrMore(targetRes_value)
        & ZeroOrMore(genericToken.temp_value)
        & ZeroOrMore(elecToken.usemap_value)
        & ZeroOrMore(elecToken.write_value)
    )


class geoflow_autoParser:

    # geoflow-auto Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/geoflow-auto.html
    geoflow_auto_val = CLiteral("geoflow-auto")

    press_val = CLiteral("press")
    press_value = Group(press_val + NUMBER_VAL)

    vdwdisp_val = CLiteral("vdwdisp")
    vdwdisp_options_val = oneOf("0 1")
    vdwdisp_value = Group(vdwdisp_val + vdwdisp_options_val)

    body = (
        geoflow_auto_val
        & ZeroOrMore(elecToken.name_value)
        & ZeroOrMore(elecToken.bcfl_value)
        & ZeroOrMore(genericToken.bconc_value)
        & ZeroOrMore(elecToken.etol_value)
        & ZeroOrMore(genericToken.gamma_value)
        & ZeroOrMore(elecToken.pbe_value)  # lpbe lrpbe npbe nrpbe
        & ZeroOrMore(genericToken.mol_value)
        & ZeroOrMore(elecToken.pdie_value)
        & ZeroOrMore(press_value)
        & ZeroOrMore(elecToken.sdie_value)
        & ZeroOrMore(vdwdisp_value)
    )


class mg_autoParser:

    # mg-auto Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-auto.html
    mg_auto_val = CLiteral("mg-auto")

    body = (
        mg_auto_val
        & ZeroOrMore(elecToken.name_value)
        & ZeroOrMore(elecToken.bcfl_value)
        & ZeroOrMore(genericToken.calcenergy_value)
        & ZeroOrMore(genericToken.calcforce_value)
        & ZeroOrMore(elecToken.cgcent_value)
        & ZeroOrMore(elecToken.cglen_value)
        & ZeroOrMore(elecToken.chgm_value)
        & ZeroOrMore(elecToken.dime_value)
        & ZeroOrMore(elecToken.etol_value)
        & ZeroOrMore(elecToken.fgcent_value)
        & ZeroOrMore(elecToken.fglen_value)
        & ZeroOrMore(elecToken.ion_value)
        & ZeroOrMore(elecToken.pbe_value)  # lpbe lrpbe npbe nrpbe
        & ZeroOrMore(genericToken.mol_value)
        & ZeroOrMore(elecToken.pdie_value)
        & ZeroOrMore(genericToken.sdens_value)
        & ZeroOrMore(elecToken.sdie_value)
        & ZeroOrMore(genericToken.srad_value)
        & ZeroOrMore(elecToken.srfm_value)
        & ZeroOrMore(genericToken.swin_value)
        & ZeroOrMore(genericToken.temp_value)
        & ZeroOrMore(elecToken.usemap_value)
        & ZeroOrMore(elecToken.write_value)
        & ZeroOrMore(elecToken.writemat_value)
    )


class mg_manualParser:

    # mg-manual Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-manual.html
    mg_manual_val = CLiteral("mg-manual")

    nlev_val = CLiteral("nlev")
    nlev_value = Group(nlev_val + INTEGER_VAL)

    body = (
        mg_manual_val
        & ZeroOrMore(elecToken.name_value)
        & ZeroOrMore(elecToken.bcfl_value)
        & ZeroOrMore(genericToken.calcenergy_value)
        & ZeroOrMore(genericToken.calcforce_value)
        & ZeroOrMore(elecToken.chgm_value)
        & ZeroOrMore(elecToken.dime_value)
        & ZeroOrMore(elecToken.etol_value)
        & ZeroOrMore(elecToken.gcent_value)
        & ZeroOrMore(elecToken.glen_value)
        & ZeroOrMore(genericToken.grid_value)
        & ZeroOrMore(elecToken.ion_value)
        & ZeroOrMore(elecToken.pbe_value)  # lpbe lrpbe npbe nrpbe
        & ZeroOrMore(genericToken.mol_value)
        & ZeroOrMore(nlev_value)
        & ZeroOrMore(elecToken.pdie_value)
        & ZeroOrMore(genericToken.sdens_value)
        & ZeroOrMore(elecToken.sdie_value)
        & ZeroOrMore(genericToken.srad_value)
        & ZeroOrMore(elecToken.srfm_value)
        & ZeroOrMore(genericToken.swin_value)
        & ZeroOrMore(genericToken.temp_value)
        & ZeroOrMore(elecToken.usemap_value)
        & ZeroOrMore(elecToken.write_value)
        & ZeroOrMore(elecToken.writemat_value)
    )


class mg_paraParser:

    # mg-para Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-para.html
    mg_para_val = CLiteral("mg-para")

    # TODO: Combine with dime?
    pdime_val = CLiteral("pdime")
    pdime_coord_val = Group(NUMBER_VAL * 3)
    pdime_value = Group(pdime_val + pdime_coord_val)

    ofrac_val = CLiteral("ofrac")
    ofrac_value = Group(ofrac_val + NUMBER_VAL)

    body = (
        mg_para_val
        & ZeroOrMore(elecToken.name_value)
        & ZeroOrMore(elecToken.async_value)
        & ZeroOrMore(elecToken.bcfl_value)
        & ZeroOrMore(genericToken.calcenergy_value)
        & ZeroOrMore(genericToken.calcforce_value)
        & ZeroOrMore(elecToken.cgcent_value)
        & ZeroOrMore(elecToken.cglen_value)
        & ZeroOrMore(elecToken.chgm_value)
        & ZeroOrMore(elecToken.dime_value)
        & ZeroOrMore(elecToken.etol_value)
        & ZeroOrMore(elecToken.fgcent_value)
        & ZeroOrMore(elecToken.fglen_value)
        & ZeroOrMore(elecToken.ion_value)
        & ZeroOrMore(elecToken.pbe_value)
        & ZeroOrMore(genericToken.mol_value)
        & ZeroOrMore(ofrac_value)
        & ZeroOrMore(elecToken.pdie_value)
        & ZeroOrMore(pdime_value)
        & ZeroOrMore(genericToken.sdens_value)
        & ZeroOrMore(elecToken.sdie_value)
        & ZeroOrMore(genericToken.srad_value)
        & ZeroOrMore(elecToken.srfm_value)
        & ZeroOrMore(genericToken.swin_value)
        & ZeroOrMore(genericToken.temp_value)
        & ZeroOrMore(elecToken.usemap_value)
        & ZeroOrMore(elecToken.write_value)
        & ZeroOrMore(elecToken.writemat_value)
    )


class mg_dummyParser:

    # mg-dummy Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-dummy.html
    mg_dummy_val = CLiteral("mg-dummy")

    body = (
        mg_dummy_val
        & ZeroOrMore(elecToken.name_value)
        & ZeroOrMore(elecToken.bcfl_value)
        & ZeroOrMore(elecToken.chgm_value)
        & ZeroOrMore(elecToken.dime_value)
        & ZeroOrMore(elecToken.gcent_value)
        & ZeroOrMore(elecToken.glen_value)
        & ZeroOrMore(genericToken.grid_value)
        & ZeroOrMore(elecToken.ion_value)
        & ZeroOrMore(elecToken.pbe_value)
        & ZeroOrMore(genericToken.mol_value)
        & ZeroOrMore(elecToken.pdie_value)
        & ZeroOrMore(genericToken.sdens_value)
        & ZeroOrMore(elecToken.sdie_value)
        & ZeroOrMore(genericToken.srad_value)
        & ZeroOrMore(elecToken.srfm_value)
        & ZeroOrMore(genericToken.swin_value)
        & ZeroOrMore(genericToken.temp_value)
        & ZeroOrMore(elecToken.write_value)
    )


class pbToken:

    # pbam-auto and pbsam-auto specific Keywords

    thr3dmap_val = CLiteral("3dmap")
    thr3dmap_value = Group(thr3dmap_val + PATH_VAL)

    diff_val = CLiteral("diff")
    diff_move_type_val = CLiteral("move")
    diff_rot_type_val = CLiteral("rot")
    diff_stat_type_val = CLiteral("stat")
    diff_move_val = diff_move_type_val + NUMBER_VAL + NUMBER_VAL
    diff_rot_val = diff_rot_type_val + NUMBER_VAL
    diff_value = Group(
        diff_val + (diff_stat_type_val | diff_move_val | diff_rot_val)
    )

    dx_val = CLiteral("dx")
    dx_value = Group(dx_val + PATH_VAL)

    grid2d_val = CLiteral("grid2d")
    grid2d_options_val = oneOf("x y z", caseless=True)
    grid2d_value = Group(
        grid2d_val + PATH_VAL + grid2d_options_val + NUMBER_VAL
    )

    gridpts_val = CLiteral("gridpts")
    gridpts_value = Group(gridpts_val + INTEGER_VAL)

    ntraj_val = CLiteral("ntraj")
    ntraj_value = Group(ntraj_val + INTEGER_VAL)

    pbc_val = CLiteral("pbc")
    pbc_value = Group(pbc_val + NUMBER_VAL)

    randorient_val = CLiteral("randorient")
    randorient_value = Group(randorient_val)

    runname_val = CLiteral("runname")
    runname_value = Group(runname_val + Word(alphanums))

    runtype_val = CLiteral("runtype")
    runtype_options_val = oneOf(
        "energyforce electrostatics dynamics", caseless=True
    )
    runtype_value = Group(runtype_val + runtype_options_val)

    salt_val = CLiteral("salt")
    # TODO: value of NUMBER_VAL should be 0.00 to 0.15?
    salt_value = Group(salt_val + NUMBER_VAL)

    term_val = CLiteral("term")
    term_contact_type_val = CLiteral("contact")
    term_pos_type_val = oneOf("x<= x>= y<= y>= z<= z>= r<= r>=", caseless=True)
    term_time_type_val = CLiteral("time")
    term_contact_val = CLiteral("contact") + PATH_VAL
    # TODO: is the val an integer or float
    term_pos_val = term_pos_type_val + NUMBER_VAL + IDENTIFIER
    term_time_val = term_time_type_val + NUMBER_VAL
    term_value = Group(
        term_val + (term_contact_val | term_pos_val | term_time_val)
    )

    termcombine_val = CLiteral("termcombine")
    termcombine_option_val = oneOf("and or", caseless=True)
    termcombine_value = Group(termcombine_val + termcombine_option_val)

    units_val = CLiteral("units")
    units_option_val = oneOf("kcalmol jmol kT", caseless=True)
    units_value = Group(units_val + units_option_val)

    xyz_val = CLiteral("xyz")
    xyz_value = Group(xyz_val + IDENTIFIER + PATH_VAL)


class pbam_autoParser:

    # pbam-auto Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/pbam-auto.html
    pbam_auto_val = CLiteral("pbam-auto")

    body = (
        pbam_auto_val
        & ZeroOrMore(elecToken.name_value)
        & ZeroOrMore(pbToken.thr3dmap_value)
        & ZeroOrMore(pbToken.diff_value)
        & ZeroOrMore(pbToken.dx_value)
        & ZeroOrMore(pbToken.grid2d_value)
        & ZeroOrMore(pbToken.gridpts_value)
        & ZeroOrMore(genericToken.mol_value)
        & ZeroOrMore(pbToken.ntraj_value)
        & ZeroOrMore(pbToken.pbc_value)
        & ZeroOrMore(elecToken.pdie_value)
        & ZeroOrMore(pbToken.randorient_value)
        & ZeroOrMore(pbToken.runname_value)
        & ZeroOrMore(pbToken.runtype_value)
        & ZeroOrMore(pbToken.salt_value)
        & ZeroOrMore(elecToken.sdie_value)
        & ZeroOrMore(genericToken.temp_value)
        & ZeroOrMore(pbToken.term_value)
        & ZeroOrMore(pbToken.termcombine_value)
        & ZeroOrMore(pbToken.units_value)
        & ZeroOrMore(pbToken.xyz_value)
    )


class pbsam_autoParser:

    # https://apbs.readthedocs.io/en/latest/using/input/elec/pbsam-auto.html
    pbsam_auto_val = CLiteral("pbsam-auto")

    # pbsam-auto specific Keywords

    exp_val = CLiteral("exp")
    exp_value = Group(exp_val + PATH_VAL)

    imat_val = CLiteral("imat")
    imat_value = Group(imat_val + PATH_VAL)

    surf_val = CLiteral("surf")
    surf_value = Group(surf_val + PATH_VAL)

    tolsp_val = CLiteral("tolsp")
    tolsp_value = Group(tolsp_val + NUMBER_VAL)

    body = (
        pbsam_auto_val
        & ZeroOrMore(elecToken.name_value)
        & ZeroOrMore(pbToken.thr3dmap_value)
        & ZeroOrMore(pbToken.diff_value)
        & ZeroOrMore(pbToken.dx_value)
        & ZeroOrMore(exp_value)
        & ZeroOrMore(pbToken.grid2d_value)
        & ZeroOrMore(imat_value)
        & ZeroOrMore(pbToken.ntraj_value)
        & ZeroOrMore(pbToken.pbc_value)
        & ZeroOrMore(elecToken.pdie_value)
        & ZeroOrMore(pbToken.randorient_value)
        & ZeroOrMore(pbToken.runname_value)
        & ZeroOrMore(pbToken.runtype_value)
        & ZeroOrMore(pbToken.salt_value)
        & ZeroOrMore(elecToken.sdie_value)
        & ZeroOrMore(surf_value)
        & ZeroOrMore(genericToken.temp_value)
        & ZeroOrMore(pbToken.term_value)
        & ZeroOrMore(pbToken.termcombine_value)
        & ZeroOrMore(tolsp_value)
        & ZeroOrMore(pbToken.units_value)
        & ZeroOrMore(pbToken.xyz_value)
    )


class ApbsLegacyInput:

    # Questions:
    #   1. Can READ, ELEC, and PRINT have leading/trailing spaces?
    #      A: Yes
    #   2. do keyword and values HAVE to be on the same line?
    #   3. are keyword and non-path values case sensitive?
    #      A: No

    # ELEC Keywords
    quit_val = CLiteral("QUIT")
    all_values = (
        OneOrMore(readParser())
        + OneOrMore(apolarParser() | elecParser())
        + ZeroOrMore(printParser())
        + Suppress(quit_val)
    )

    def __init__(self):
        self.results = {}

    def loads(self, input_data: str):
        """Parse the input as a string

        :param str input_data: a string containiner an APBS legacy input configuration file
        :return: a dictionary configuration files contents
        :rtype: dict
        """
        parser = self.all_values
        parser.ignore(COMMENT + restOfLine)

        # return self.all_values.searchString(input_data)[0]
        return self.all_values.searchString(input_data)

    def load(self, filename: str):
        """
        Read Legacy Input congifuration file and pass it to loads as a string

        :param str filename: The APBS legacy input configuration file
        :return: a dictionary configuration files contents
        :rtype: dict
        """
        with filename.open() as fp:
            data = fp.read()
        try:
            data = self.loads(data)
            return data

        except ParseException as pe:
            self.display_error(filename, pe)

    def display_error(self, filename, pe):
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
        raise pe


if __name__ == "__main__":
    # execute only if run as a script
    relfilename = "../../examples/pbsam-barn_bars/barn_bars_electro.in"
    relfilename = "../../examples/actin-dimer/apbs-mol-auto.in"
    test = ApbsLegacyInput()
    from pathlib import Path

    curr_dir = Path(__file__).parent
    absfilename = curr_dir / relfilename
    # print(test.load(absfilename))
    test.load(absfilename)
    pprint.pp(FINAL_OUTPUT)
