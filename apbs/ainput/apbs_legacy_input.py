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


def formatGroupBlock(t: ParseResults, section: str, groups: list):
    """Convert lists of lists to a dictionary"""

    # print(f"IDX: {idx}")
    if section not in FINAL_OUTPUT.keys():
        FINAL_OUTPUT[section] = {}
    idx = len(FINAL_OUTPUT[section])
    FINAL_OUTPUT[section][idx] = {}

    # print(f"T: {t}")
    for row in t[0]:
        # print(f"TYPE: {type(row)}")
        for item in row:
            # print(f"type item: {type(item)} {item}")
            if isinstance(item, ParseResults):
                key = item[0].lower()
                # print(f"key: {key}")
                if key in groups:
                    if key not in FINAL_OUTPUT[section][idx].keys():
                        # print(f"ADD KEY: {key}")
                        FINAL_OUTPUT[section][idx][key] = {}
                    subkey = f"{item[1]}".lower()
                    if subkey not in FINAL_OUTPUT[section][idx][key].keys():
                        # print(f"ADD SUBKEY: {subkey}")
                        FINAL_OUTPUT[section][idx][key][subkey] = []
                    if item[2] is not None:
                        # print(
                        #     f"VALUES: KEY: {key}, SUBKEY: {subkey}, item2: {item[2]}"
                        # )
                        item2 = ", ".join(item[2].split())
                        FINAL_OUTPUT[section][idx][key][subkey].append(item2)

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
        return formatGroupBlock(t, "READ", groups)

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
        idx = len(FINAL_OUTPUT[section])
        FINAL_OUTPUT[section][idx] = {}

        for row in t[0]:
            # print(f"TYPE: {type(row)}")
            for item in row:
                # print(f"type item: {type(item)} {item}")
                if isinstance(item, str):
                    key = item.lower()
                    # print(f"key: {key}")
                    if key not in FINAL_OUTPUT[section][idx].keys():
                        FINAL_OUTPUT[section][idx][key] = row[1:]
                        break
            else:
                break
            break

        return FINAL_OUTPUT

    value = Group(Suppress(val) + body + Suppress(END_VAL)).setParseAction(
        formatPrint
    )

    return value


def formatBlock(t: ParseResults, section: str):

    if section not in FINAL_OUTPUT.keys():
        FINAL_OUTPUT[section] = {}
    idx = len(FINAL_OUTPUT[section])
    FINAL_OUTPUT[section][idx] = {}

    for row in t[0]:
        # print(f"ELEC TYPE: {type(row)} ROW: {row}")
        if isinstance(row, str):
            FINAL_OUTPUT[section][idx]["type"] = row
            continue
        if len(row) == 1:
            FINAL_OUTPUT[section][idx]["pbe"] = row[0]
            continue
        if len(row) == 2:
            # print(f"KEY: {row[0]} TYPE {type(row[1])}")
            value = row[1]
            if isinstance(row[1], ParseResults):
                value = (row[1]).asList()
            FINAL_OUTPUT[section][idx][row[0]] = value
            continue
        # print(f"WHAT KEY: {row[0]}")
        if row[0] not in FINAL_OUTPUT.keys():
            FINAL_OUTPUT[section][idx][row[0]] = []
        for item in row[1:]:
            # print(f"WHAT: {item}")
            FINAL_OUTPUT[section][idx][row[0]].append([item[0], item[1]])

    return FINAL_OUTPUT


def apolarParser():

    body = (
        ZeroOrMore(elecToken.name)
        & ZeroOrMore(genericToken.bconc)
        & ZeroOrMore(genericToken.calcenergy)
        & ZeroOrMore(genericToken.calcforce)
        & ZeroOrMore(apolarToken.dpos)
        & ZeroOrMore(genericToken.gamma)
        & ZeroOrMore(genericToken.grid)
        & ZeroOrMore(genericToken.mol)
        & ZeroOrMore(apolarToken.press)
        & ZeroOrMore(genericToken.sdens)
        & ZeroOrMore(genericToken.srad)
        & ZeroOrMore(apolarToken.srfm)
        & ZeroOrMore(genericToken.swin)
        & ZeroOrMore(genericToken.temp)
    )

    def formatApolar(t: ParseResults):
        return formatBlock(t, "APOLAR")

    return Group(
        Suppress(CLiteral("APOLAR")) - body - Suppress(END_VAL)
    ).setParseAction(formatApolar)


def elecParser():

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
        return formatBlock(t, "ELEC")

    return Group(
        Suppress(CLiteral("ELEC")) - body - Suppress(END_VAL)
    ).setParseAction(formatElec)


class genericToken:

    bconc = Group(CLiteral("bconc") - NUMBER_VAL)
    calc_options = oneOf("no total comps", caseless=True)
    calcenergy = Group(CLiteral("calcenergy") - calc_options)
    calcforce = Group(CLiteral("calcforce") - calc_options)
    gamma = Group(CLiteral("gamma") - NUMBER_VAL)
    grid_coord = Group(NUMBER_VAL * 3)
    grid = Group(CLiteral("grid") - grid_coord)
    mol = Group(CLiteral("mol") - NUMBER_VAL)
    sdens = Group(CLiteral("sdens") - NUMBER_VAL)
    srad = Group(CLiteral("srad") - NUMBER_VAL)
    swin = Group(CLiteral("swin") - NUMBER_VAL)
    temp = Group(CLiteral("temp") - NUMBER_VAL)


class apolarToken:

    # APOLAR section specific grammar
    # https://apbs.readthedocs.io/en/latest/using/input/apolar/index.html
    dpos = Group(CLiteral("dpos") - NUMBER_VAL)
    press = Group(CLiteral("press") - NUMBER_VAL)
    srfm = Group(CLiteral("srfm") - oneOf("sacc", caseless=True))


class elecToken:

    # ELEC section specific grammar
    # https://apbs.readthedocs.io/en/latest/using/input/elec/index.html

    # The following tokens are used by at least 2 of the parser types with
    # the same format and rules

    name = Group(CLiteral("name") - IDENTIFIER)
    grid_floats = Group(NUMBER_VAL * 3)
    grid_ints = Group(INTEGER_VAL * 3)
    mol_id = Group(CLiteral("mol") + INTEGER_VAL)

    async_value = Group(CLiteral("async") - INTEGER_VAL)
    bcfl_options = oneOf("zero sdh mdh focus", caseless=True)
    bcfl = Group(CLiteral("bcfl") - bcfl_options)
    cgcent = Group(CLiteral("cgcent") - (mol_id | grid_floats))
    cglen = Group(CLiteral("cglen") - grid_ints)
    chgm_options = oneOf("spl0 spl2", caseless=True)
    chgm = Group(CLiteral("chgm") - chgm_options)
    dime = Group(CLiteral("dime") - grid_ints)
    etol = Group(CLiteral("etol") - NUMBER_VAL)
    fgcent = Group(CLiteral("fgcent") - (mol_id | grid_floats))
    fglen = Group(CLiteral("fglen") - grid_floats)
    gcent = Group(CLiteral("gcent") - (mol_id | grid_floats))
    glen = Group(CLiteral("glen") - grid_floats)

    # Are charge, conc, and radius ALL required?
    ion_charge = Group(CLiteral("charge") - NUMBER_VAL)
    ion_conc = Group(CLiteral("conc") - NUMBER_VAL)
    ion_radius = Group(CLiteral("radius") - NUMBER_VAL)
    ion = Group(CLiteral("ion") - ion_charge & ion_conc & ion_radius)

    nlev = Group(CLiteral("nlev") - NUMBER_VAL)

    # TODO: I think only 1 of these are allowed (not ZeroOrMore)
    pbe = Group(oneOf("lpbe lrpbe npbe nrpbe", caseless=True))

    # TODO: Number must be >= 1
    pdie = Group(CLiteral("pdie") - NUMBER_VAL)

    # NOTE: Should be a value between 78-80?
    sdie = Group(CLiteral("sdie") - NUMBER_VAL)

    srfm_options = oneOf("mol smol spl2", caseless=True)
    srfm = Group(CLiteral("srfm") - srfm_options)

    usemap_options = oneOf("diel kappa charge", caseless=True)
    usemap = Group(CLiteral("usemap") - usemap_options - INTEGER_VAL)

    write_type_options = oneOf(
        "charge pot smol sspl vdw ivdw lap edens ndens qdens dielx diely dielz kappa",
        caseless=True,
    )
    write_format_options = oneOf("dx avs uhbd", caseless=True)
    write = Group(
        CLiteral("write")
        - write_type_options
        - write_format_options
        - PATH_VAL
    )

    writemat = Group(
        CLiteral("writemat") - oneOf("poisson", caseless=True) - PATH_VAL
    )


class tabiParser:

    # tabi Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/tabi.html

    # TODO: This should be replaced which a check to make
    # sure that "NUMBER_VAL is between 0.0 and 1.0"
    mac = Group(CLiteral("mac") - NUMBER_VAL)
    mesh = Group(CLiteral("mesh") - oneOf("0 1 2"))
    outdata = Group(CLiteral("outdata") - oneOf("0 1"))
    tree_n0 = Group(CLiteral("tree_n0") - INTEGER_VAL)
    tree_order = Group(CLiteral("tree_order") - INTEGER_VAL)

    body = (
        CLiteral("tabi")
        & ZeroOrMore(elecToken.name)
        & ZeroOrMore(elecToken.ion)
        & ZeroOrMore(mac)
        & ZeroOrMore(mesh)
        & ZeroOrMore(genericToken.mol)
        & ZeroOrMore(outdata)
        & ZeroOrMore(elecToken.pdie)
        & ZeroOrMore(genericToken.sdens)
        & ZeroOrMore(elecToken.sdie)
        & ZeroOrMore(genericToken.srad)
        & ZeroOrMore(genericToken.temp)
        & ZeroOrMore(tree_n0)
        & ZeroOrMore(tree_order)
    )


class fe_manualParser:

    # fe-manual Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/fe-manual.html

    akeyPRE_options = oneOf("unif geom", caseless=True)
    akeyPRE = Group(CLiteral("akeyPRE") - akeyPRE_options)
    akeySOLVE_options = oneOf("resi", caseless=True)
    akeySOLVE = Group(CLiteral("akeySOLVE") - akeySOLVE_options)
    domainLength = Group(CLiteral("domainLength") - elecToken.grid_floats)
    ekey_options = oneOf("simp global frac", caseless=True)
    ekey = Group(CLiteral("ekey") - ekey_options)
    maxsolve = Group(CLiteral("maxsolve") - NUMBER_VAL)
    maxvert = Group(CLiteral("maxvert") - NUMBER_VAL)
    targetNum = Group(CLiteral("targetNum") - INTEGER_VAL)
    targetRes = Group(CLiteral("targetRes") - NUMBER_VAL)

    body = (
        CLiteral("fe-manual")
        & ZeroOrMore(elecToken.name)
        & ZeroOrMore(akeyPRE)
        & ZeroOrMore(akeySOLVE)
        & ZeroOrMore(elecToken.async_value)
        & ZeroOrMore(elecToken.bcfl)
        & ZeroOrMore(genericToken.calcenergy)
        & ZeroOrMore(genericToken.calcforce)
        & ZeroOrMore(elecToken.chgm)
        & ZeroOrMore(domainLength)
        & ZeroOrMore(ekey)
        & ZeroOrMore(elecToken.etol)
        & ZeroOrMore(elecToken.ion)
        & ZeroOrMore(elecToken.pbe)  # lpbe lrpbe npbe nrpbe
        & ZeroOrMore(maxsolve)
        & ZeroOrMore(maxvert)
        & ZeroOrMore(genericToken.mol)
        & ZeroOrMore(elecToken.pdie)
        & ZeroOrMore(genericToken.sdens)
        & ZeroOrMore(elecToken.sdie)
        & ZeroOrMore(genericToken.srad)
        & ZeroOrMore(elecToken.srfm)
        & ZeroOrMore(genericToken.swin)
        & ZeroOrMore(targetNum)
        & ZeroOrMore(targetRes)
        & ZeroOrMore(genericToken.temp)
        & ZeroOrMore(elecToken.usemap)
        & ZeroOrMore(elecToken.write)
    )


class geoflow_autoParser:

    # geoflow-auto Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/geoflow-auto.html

    press = Group(CLiteral("press") - NUMBER_VAL)
    vdwdisp = Group(CLiteral("vdwdisp") - oneOf("0 1"))

    body = (
        CLiteral("geoflow-auto")
        & ZeroOrMore(elecToken.name)
        & ZeroOrMore(elecToken.bcfl)
        & ZeroOrMore(genericToken.bconc)
        & ZeroOrMore(elecToken.etol)
        & ZeroOrMore(genericToken.gamma)
        & ZeroOrMore(elecToken.pbe)  # lpbe lrpbe npbe nrpbe
        & ZeroOrMore(genericToken.mol)
        & ZeroOrMore(elecToken.pdie)
        & ZeroOrMore(press)
        & ZeroOrMore(elecToken.sdie)
        & ZeroOrMore(vdwdisp)
    )


class mg_autoParser:

    # mg-auto Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-auto.html

    body = (
        CLiteral("mg-auto")
        & ZeroOrMore(elecToken.name)
        & ZeroOrMore(elecToken.bcfl)
        & ZeroOrMore(genericToken.calcenergy)
        & ZeroOrMore(genericToken.calcforce)
        & ZeroOrMore(elecToken.cgcent)
        & ZeroOrMore(elecToken.cglen)
        & ZeroOrMore(elecToken.chgm)
        & ZeroOrMore(elecToken.dime)
        & ZeroOrMore(elecToken.etol)
        & ZeroOrMore(elecToken.fgcent)
        & ZeroOrMore(elecToken.fglen)
        & ZeroOrMore(elecToken.ion)
        & ZeroOrMore(elecToken.pbe)  # lpbe lrpbe npbe nrpbe
        & ZeroOrMore(genericToken.mol)
        & ZeroOrMore(elecToken.pdie)
        & ZeroOrMore(genericToken.sdens)
        & ZeroOrMore(elecToken.sdie)
        & ZeroOrMore(genericToken.srad)
        & ZeroOrMore(elecToken.srfm)
        & ZeroOrMore(genericToken.swin)
        & ZeroOrMore(genericToken.temp)
        & ZeroOrMore(elecToken.usemap)
        & ZeroOrMore(elecToken.write)
        & ZeroOrMore(elecToken.writemat)
    )


class mg_manualParser:

    # mg-manual Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-manual.html

    nlev = Group(CLiteral("nlev") - INTEGER_VAL)

    body = (
        CLiteral("mg-manual")
        & ZeroOrMore(elecToken.name)
        & ZeroOrMore(elecToken.bcfl)
        & ZeroOrMore(genericToken.calcenergy)
        & ZeroOrMore(genericToken.calcforce)
        & ZeroOrMore(elecToken.chgm)
        & ZeroOrMore(elecToken.dime)
        & ZeroOrMore(elecToken.etol)
        & ZeroOrMore(elecToken.gcent)
        & ZeroOrMore(elecToken.glen)
        & ZeroOrMore(genericToken.grid)
        & ZeroOrMore(elecToken.ion)
        & ZeroOrMore(elecToken.pbe)  # lpbe lrpbe npbe nrpbe
        & ZeroOrMore(genericToken.mol)
        & ZeroOrMore(nlev)
        & ZeroOrMore(elecToken.pdie)
        & ZeroOrMore(genericToken.sdens)
        & ZeroOrMore(elecToken.sdie)
        & ZeroOrMore(genericToken.srad)
        & ZeroOrMore(elecToken.srfm)
        & ZeroOrMore(genericToken.swin)
        & ZeroOrMore(genericToken.temp)
        & ZeroOrMore(elecToken.usemap)
        & ZeroOrMore(elecToken.write)
        & ZeroOrMore(elecToken.writemat)
    )


class mg_paraParser:

    # mg-para Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-para.html

    # TODO: Combine with dime?
    pdime = Group(CLiteral("pdime") - elecToken.grid_floats)
    ofrac = Group(CLiteral("ofrac") - NUMBER_VAL)

    body = (
        CLiteral("mg-para")
        & ZeroOrMore(elecToken.name)
        & ZeroOrMore(elecToken.async_value)
        & ZeroOrMore(elecToken.bcfl)
        & ZeroOrMore(genericToken.calcenergy)
        & ZeroOrMore(genericToken.calcforce)
        & ZeroOrMore(elecToken.cgcent)
        & ZeroOrMore(elecToken.cglen)
        & ZeroOrMore(elecToken.chgm)
        & ZeroOrMore(elecToken.dime)
        & ZeroOrMore(elecToken.etol)
        & ZeroOrMore(elecToken.fgcent)
        & ZeroOrMore(elecToken.fglen)
        & ZeroOrMore(elecToken.ion)
        & ZeroOrMore(elecToken.pbe)
        & ZeroOrMore(genericToken.mol)
        & ZeroOrMore(ofrac)
        & ZeroOrMore(elecToken.pdie)
        & ZeroOrMore(pdime)
        & ZeroOrMore(genericToken.sdens)
        & ZeroOrMore(elecToken.sdie)
        & ZeroOrMore(genericToken.srad)
        & ZeroOrMore(elecToken.srfm)
        & ZeroOrMore(genericToken.swin)
        & ZeroOrMore(genericToken.temp)
        & ZeroOrMore(elecToken.usemap)
        & ZeroOrMore(elecToken.write)
        & ZeroOrMore(elecToken.writemat)
    )


class mg_dummyParser:

    # mg-dummy Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-dummy.html

    body = (
        CLiteral("mg-dummy")
        & ZeroOrMore(elecToken.name)
        & ZeroOrMore(elecToken.bcfl)
        & ZeroOrMore(elecToken.chgm)
        & ZeroOrMore(elecToken.dime)
        & ZeroOrMore(elecToken.gcent)
        & ZeroOrMore(elecToken.glen)
        & ZeroOrMore(genericToken.grid)
        & ZeroOrMore(elecToken.ion)
        & ZeroOrMore(elecToken.pbe)
        & ZeroOrMore(genericToken.mol)
        & ZeroOrMore(elecToken.pdie)
        & ZeroOrMore(genericToken.sdens)
        & ZeroOrMore(elecToken.sdie)
        & ZeroOrMore(genericToken.srad)
        & ZeroOrMore(elecToken.srfm)
        & ZeroOrMore(genericToken.swin)
        & ZeroOrMore(genericToken.temp)
        & ZeroOrMore(elecToken.write)
    )


class pbToken:

    # pbam-auto and pbsam-auto specific Keywords

    thr3dmap = Group(CLiteral("3dmap") - PATH_VAL)
    diff = Group(
        CLiteral("diff")
        - (
            CLiteral("stat")
            | CLiteral("move") - NUMBER_VAL - NUMBER_VAL
            | CLiteral("rot") - NUMBER_VAL
        )
    )
    dx = Group(CLiteral("dx") - PATH_VAL)
    grid2d = Group(
        CLiteral("grid2d")
        - PATH_VAL
        - oneOf("x y z", caseless=True)
        - NUMBER_VAL
    )
    gridpts = Group(CLiteral("gridpts") - INTEGER_VAL)
    ntraj = Group(CLiteral("ntraj") - INTEGER_VAL)
    pbc = Group(CLiteral("pbc") - NUMBER_VAL)
    randorient = Group(CLiteral("randorient"))
    runname = Group(CLiteral("runname") - Word(alphanums + "_"))
    runtype_options = oneOf(
        "energyforce electrostatics dynamics", caseless=True
    )
    runtype = Group(CLiteral("runtype") - runtype_options)

    # TODO: value of NUMBER_VAL should be 0.00 to 0.15?
    salt = Group(CLiteral("salt") - NUMBER_VAL)

    term_pos_options = oneOf("x<= x>= y<= y>= z<= z>= r<= r>=", caseless=True)
    term_contact = CLiteral("contact") + PATH_VAL
    # TODO: is the val an integer or float
    term_pos = term_pos_options + NUMBER_VAL + IDENTIFIER
    term_time = CLiteral("time") - NUMBER_VAL
    term = Group(CLiteral("term") - (term_contact | term_pos | term_time))

    termcombine = Group(
        CLiteral("termcombine") - oneOf("and or", caseless=True)
    )
    units = Group(CLiteral("units") - oneOf("kcalmol jmol kT", caseless=True))
    xyz = Group(CLiteral("xyz") - IDENTIFIER - PATH_VAL)


class pbam_autoParser:

    # pbam-auto Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/pbam-auto.html

    body = (
        CLiteral("pbam-auto")
        & ZeroOrMore(elecToken.name)
        & ZeroOrMore(pbToken.thr3dmap)
        & ZeroOrMore(pbToken.diff)
        & ZeroOrMore(pbToken.dx)
        & ZeroOrMore(pbToken.grid2d)
        & ZeroOrMore(pbToken.gridpts)
        & ZeroOrMore(genericToken.mol)
        & ZeroOrMore(pbToken.ntraj)
        & ZeroOrMore(pbToken.pbc)
        & ZeroOrMore(elecToken.pdie)
        & ZeroOrMore(pbToken.randorient)
        & ZeroOrMore(pbToken.runname)
        & ZeroOrMore(pbToken.runtype)
        & ZeroOrMore(pbToken.salt)
        & ZeroOrMore(elecToken.sdie)
        & ZeroOrMore(genericToken.temp)
        & ZeroOrMore(pbToken.term)
        & ZeroOrMore(pbToken.termcombine)
        & ZeroOrMore(pbToken.units)
        & ZeroOrMore(pbToken.xyz)
    )


class pbsam_autoParser:

    # pbsam-auto Keywords
    # https://apbs.readthedocs.io/en/latest/using/input/elec/pbsam-auto.html

    # pbsam-auto specific Keywords
    exp = Group(CLiteral("exp") - PATH_VAL)
    imat = Group(CLiteral("imat") - PATH_VAL)
    surf = Group(CLiteral("surf") - PATH_VAL)
    tolsp = Group(CLiteral("tolsp") - NUMBER_VAL)

    body = (
        CLiteral("pbsam-auto")
        & ZeroOrMore(elecToken.name)
        & ZeroOrMore(pbToken.thr3dmap)
        & ZeroOrMore(pbToken.diff)
        & ZeroOrMore(pbToken.dx)
        & ZeroOrMore(exp)
        & ZeroOrMore(pbToken.grid2d)
        & ZeroOrMore(imat)
        & ZeroOrMore(pbToken.ntraj)
        & ZeroOrMore(pbToken.pbc)
        & ZeroOrMore(elecToken.pdie)
        & ZeroOrMore(pbToken.randorient)
        & ZeroOrMore(pbToken.runname)
        & ZeroOrMore(pbToken.runtype)
        & ZeroOrMore(pbToken.salt)
        & ZeroOrMore(elecToken.sdie)
        & ZeroOrMore(surf)
        & ZeroOrMore(genericToken.temp)
        & ZeroOrMore(pbToken.term)
        & ZeroOrMore(pbToken.termcombine)
        & ZeroOrMore(tolsp)
        & ZeroOrMore(pbToken.units)
        & ZeroOrMore(pbToken.xyz)
    )


class ApbsLegacyInput:

    # Questions:
    #   1. Can READ, ELEC, and PRINT have leading/trailing spaces?
    #      A: Yes
    #   2. do keyword and values HAVE to be on the same line?
    #   3. are keyword and non-path values case sensitive?
    #      A: No

    # ELEC Keywords
    all_values = (
        OneOrMore(readParser())
        + OneOrMore(apolarParser() | elecParser())
        + ZeroOrMore(printParser())
        + Suppress(CLiteral("QUIT"))
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
