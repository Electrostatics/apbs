from pathlib import Path
import pprint
from pyparsing import CaselessLiteral as CLiteral
from pyparsing import (
    Combine,
    Group,
    LineEnd,
    OneOrMore,
    Optional,
    ParseSyntaxException,
    ParseResults,
    Suppress,
    Word,
    ZeroOrMore,
    alphanums,
    empty,
    nums,
    oneOf,
    printables,
    pyparsing_common,
    restOfLine,
)
from re import VERBOSE, search


# GLOBAL Values
COMMENT = "#"
DEBUG = False
END_VAL = CLiteral("END")
EOL = LineEnd().suppress()
INTEGER_VAL = Combine(Optional("-") + Word(nums))
IDENTIFIER = Word(alphanums, alphanums + r"_" + r"-") | INTEGER_VAL
NUMBER_VAL = pyparsing_common.real | INTEGER_VAL  # .setDebug()
PATH_VAL = Word(printables)  # .setDebug()

FINAL_OUTPUT = {}


def debug(message: str):
    if DEBUG:
        print(message)


def formatReadBlock(t: ParseResults, section: str, groups: list):
    """Convert lists of lists to a dictionary"""

    if section not in FINAL_OUTPUT:
        FINAL_OUTPUT[section] = {}
    idx = len(FINAL_OUTPUT[section])
    debug(f"IDX: {idx}")
    FINAL_OUTPUT[section][idx] = {}

    debug(f"T: {t}")
    for result in t[0]:
        debug(f"TYPE: {type(result)}")
        for field in result:
            debug(f"type item: {type(field)} {field}")
            if isinstance(field, ParseResults):
                key = field[0].lower()
                debug(f"key: {key}")
                if key in groups:
                    if key not in FINAL_OUTPUT[section][idx]:
                        debug(f"ADD KEY: {key}")
                        FINAL_OUTPUT[section][idx][key] = {}
                    subkey = f"{field[1]}".lower()
                    if subkey not in FINAL_OUTPUT[section][idx][key]:
                        debug(f"ADD SUBKEY: {subkey}")
                        FINAL_OUTPUT[section][idx][key][subkey] = []
                    if field[2] is not None:
                        debug(
                            f"VALUES: KEY: {key}, SUBKEY: {subkey}, item2: {field[2]}"
                        )
                        item2 = ", ".join(field[2].split())
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
    diel = Group(CLiteral("diel") - file_fmt - PATH_VAL - PATH_VAL - PATH_VAL)

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
        return formatReadBlock(t, "READ", groups)

    return Group(
        Suppress(CLiteral("READ")) - body - Suppress(END_VAL)
    ).setParseAction(formatRead)


def printParser():
    """Setup the grammar for the PRINT section."""

    # PRINT section specific grammar
    val = CLiteral("PRINT")
    choices = oneOf("elecEnergy elecForce apolEnergy apolForce", caseless=True)
    expr = IDENTIFIER + Optional(
        OneOrMore(oneOf("+ -") + IDENTIFIER)
        + ZeroOrMore(oneOf("+ -") + IDENTIFIER)
    )
    body = Group(choices - expr)

    def formatPrint(t: ParseResults):

        section = "PRINT"
        if section not in FINAL_OUTPUT:
            FINAL_OUTPUT[section] = {}
        idx = len(FINAL_OUTPUT[section])
        FINAL_OUTPUT[section][idx] = {}

        for row in t[0]:
            debug(f"TYPE: {type(row)}")
            for item in row:
                debug(f"type item: {type(item)} {item}")
                if isinstance(item, str):
                    key = item.lower()
                    debug(f"key: {key}")
                    if key not in FINAL_OUTPUT[section][idx].keys():
                        FINAL_OUTPUT[section][idx][key] = row[1:]
                        break
            else:
                break
            break

        return FINAL_OUTPUT

    value = Group(Suppress(val) - body - Suppress(END_VAL)).setParseAction(
        formatPrint
    )

    return value


def formatBlock(t: ParseResults, section: str):

    if section not in FINAL_OUTPUT:
        FINAL_OUTPUT[section] = {}
    idx = len(FINAL_OUTPUT[section])
    FINAL_OUTPUT[section][idx] = {}
    debug(f"FO: {FINAL_OUTPUT[section]}")

    retval = {}

    for result in t[0]:
        debug(f"RESULT TYPE: {type(result)} RESULT VALUE: {result}")
        debug(f"RESULT LEN: {len(result)}")
        if isinstance(result, str):
            if result in "randorient":
                # NOTE: special case for "randorient" key
                retval[result] = 1
                continue
            # NOTE: We have something like mg-auto so we have to add a "type" key
            retval["type"] = result
            continue
        if len(result) == 1:
            # NOTE: We have something Group (List) with only 1 value
            #       like lrpbe so we have to add a "pbe" key
            debug(f"PBE: {retval}")
            retval["pbe"] = result[0]
            continue
        # NOTE: Normal Key/Value case
        key = result[0]
        value = result[1]
        if len(result) == 2:
            debug(f"KEY: {key} VALUE: {value}")
            if isinstance(value, ParseResults):
                debug(f"ParseResults VALUE: {value}")
                value = value.asList()
            if key in retval:
                debug(f"Already Exits: {retval}")
                retval[key].append(value)
                continue
            debug(f"NORMAL Key/Value: {retval}")
            # NOTE: It is easier to put the value into an List
            #       and append multiple values to the key. Later
            #       we post-process the result to remove the List
            #       if it only has 1 value.
            retval[key] = [value]
            continue
        # NOTE: More complicated than Key/Value case, probably ion
        debug(f"WHAT KEY: {result[0]}")
        if result[0] not in retval:
            retval[result[0]] = {}
        sub_idx = len(retval[result[0]])
        for item in result[1:]:
            debug(f"SV: {sub_idx}")
            debug(f"ITEM: {item}")
            if sub_idx not in retval[result[0]]:
                retval[result[0]][sub_idx] = {}
            if item[0] in retval[result[0]][sub_idx]:
                debug("WARN: We need to change key/value to key/dict")
            retval[result[0]][sub_idx][item[0]] = item[1]

    # NOTE: Post process retval to replace Key/List with Key/Value
    #       if the List only has 1 element in it
    for item in retval:
        debug(f"TRUE1 KEY/VALUE: {item}")
        if isinstance(retval[item], list) and len(retval[item]) == 1:
            retval[item] = retval[item][0]
            debug(f"TRUE2 KEY/VALUE: {retval[item]}")

    FINAL_OUTPUT[section][idx] = retval
    debug(f"FO: {FINAL_OUTPUT[section]}")
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
    mol_id = Group(CLiteral("mol") - INTEGER_VAL)

    async_value = Group(CLiteral("async") - INTEGER_VAL)
    bcfl_options = oneOf("focus map mdh sdh zero", caseless=True)
    bcfl = Group(CLiteral("bcfl") - bcfl_options)
    cgcent = Group(CLiteral("cgcent") - (mol_id | grid_floats))
    cglen = Group(CLiteral("cglen") - grid_floats)
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
    usemap = Group(CLiteral("usemap") - Group(usemap_options - INTEGER_VAL))

    write_type_options = oneOf(
        "charge pot smol sspl vdw ivdw lap edens ndens qdens dielx diely dielz kappa",
        caseless=True,
    )
    write_format_options = oneOf("avs dx flat gz uhbd", caseless=True)
    write = Group(
        CLiteral("write")
        - Group(write_type_options - write_format_options - PATH_VAL)
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
    mesh = Group(CLiteral("mesh") - oneOf("0 1 2 ses skin"))
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
    usemesh = Group(CLiteral("usemesh") - IDENTIFIER)

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
        & ZeroOrMore(elecToken.pbe)
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
        & ZeroOrMore(usemesh)
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
        & ZeroOrMore(elecToken.pbe)
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
        & ZeroOrMore(elecToken.pbe)
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
        & ZeroOrMore(elecToken.pbe)
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
            | Group(CLiteral("move") - NUMBER_VAL - NUMBER_VAL)
            | CLiteral("rot") - NUMBER_VAL
        )
    )
    dx = Group(CLiteral("dx") - PATH_VAL)
    grid2d = Group(
        CLiteral("grid2d")
        - Group(PATH_VAL - oneOf("x y z", caseless=True) - NUMBER_VAL)
    )  # .setDebug()
    gridpts = Group(CLiteral("gridpts") - INTEGER_VAL)
    ntraj = Group(CLiteral("ntraj") - INTEGER_VAL)
    pbc = Group(CLiteral("pbc") - NUMBER_VAL)
    randorient = CLiteral("randorient")
    runname = Group(CLiteral("runname") - Word(alphanums + "_"))
    runtype_options = oneOf(
        "energyforce electrostatics dynamics", caseless=True
    )
    runtype = Group(CLiteral("runtype") - runtype_options)

    # TODO: value of NUMBER_VAL should be 0.00 to 0.15?
    salt = Group(CLiteral("salt") - NUMBER_VAL)

    term_pos_options = oneOf("x<= x>= y<= y>= z<= z>= r<= r>=", caseless=True)
    term_contact = Group(CLiteral("contact") - PATH_VAL)
    term_pos = Group(term_pos_options - NUMBER_VAL - IDENTIFIER)
    term_time = Group(CLiteral("time") - NUMBER_VAL)
    term = Group(
        CLiteral("term") + (term_contact | term_pos | term_time)
    )  # .setDebug()

    termcombine = Group(
        CLiteral("termcombine") - oneOf("and or", caseless=True)
    )
    units = Group(CLiteral("units") - oneOf("kcalmol jmol kT", caseless=True))
    xyz = Group(CLiteral("xyz") - Group(IDENTIFIER - PATH_VAL))


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


def display_error(source: str, pe: ParseSyntaxException):

    copy = 60
    msg = "\n" + "=" * copy + "\n"
    msg += f"ERROR: {type(pe)}\n"
    msg += f"Parsing {source}\n"
    msg += f"Line Number: {pe.lineno}:\n"
    msg += f"Column: {pe.col}:\n"
    msg += "=" * copy + "\n"
    msg += f"Line: \n{pe.line}\n"
    msg += " " * (pe.col - 1) + "^\n"
    msg += "=" * copy + "\n"
    pe.msg = msg
    raise pe


class ApbsLegacyInput:
    def clean(self):
        FINAL_OUTPUT = {}
        del FINAL_OUTPUT
        FINAL_OUTPUT = {}

    def __init__(self):
        self.all_values = OneOrMore(readParser()) + OneOrMore(
            apolarParser() | elecParser()
        ) + ZeroOrMore(printParser()) + Suppress(CLiteral("QUIT")) | (
            empty - ~Word(printables).setName("<unknown>")
        )

    def loads(self, input_data: str):
        """Parse the input as a string

        :param str input_data: a string containiner an APBS legacy input configuration file
        :return: a dictionary configuration files contents
        :rtype: dict
        """
        parser = self.all_values
        parser.ignore(COMMENT + restOfLine)

        value: ParseResults = None
        debug(f"DEFAULT value: TYPE {type(value)} {value}")

        try:
            value = self.all_values.searchString(input_data)
        except ParseSyntaxException as pe:
            display_error("STRING", pe)

        # NOTE: the ParseResults has 1 or more "wrappers"
        #       around the dictionary so we just want to
        #       unwrap the value to get to that actual
        #       dictionary or str representing the data.
        debug(f"value: TYPE {type(value)}")
        if isinstance(value, ParseResults):
            debug(f"value: TYPE[0]{type(value[0])}")
            if isinstance(value[0], ParseResults):
                debug(f"value: TYPE[0][0] {type(value[0][0])}")
                if isinstance(value[0][0], (dict, str)):
                    return value[0][0]
            if isinstance(value[0], (dict, str)):
                return value[0]
        if isinstance(value, (dict, str)):
            return value

        raise Exception(
            f"Could not parse data into dictionary from TYPE{value}:\n{input_data}"
        )

    def load(self, filename: str):
        """
        Read Legacy Input congifuration file and pass it to loads as a string

        :param str filename: The APBS legacy input configuration file
        :return: a dictionary configuration files contents
        :rtype: dict
        """
        with filename.open() as fp:
            try:
                return self.loads(fp.read())
            except ParseSyntaxException as pe:
                display_error(filename, pe)


def printBlock(prefix: str, item: str):

    print(
        "\n" * 1
        + "=" * 70
        + "\n"
        + "=" * 2
        + " " * 2
        + f"{prefix}: {item}"
        + "\n"
        + "=" * 70
    )


def get_legacy_input_files(
    opt_path: str = "", pattern: str = "**/*.in"
) -> list:
    search_path = (
        Path(__file__).absolute().parent.parent.parent / "examples" / opt_path
    )
    matches = Path(search_path).glob(pattern)
    matches = filter(lambda x: not search("TEMPLATE", x.name), matches)
    return filter(lambda x: not x.name.startswith("dxmath"), matches)


if __name__ == "__main__":
    # execute only if run as a script

    relfilename = "solv/apbs-smol.in"
    relfilename = "pbsam-gly/gly_dynamics.in"
    relfilename = "helix/apbs_solv.in"
    relfilename = "smpbe/apbs-smpbe-24dup.in"
    test = ApbsLegacyInput()

    single = False

    example_dir = relfilename.split("/")[0]
    example_pattern = relfilename.split("/")[1]

    files = []

    if single:
        files = get_legacy_input_files(example_dir, example_pattern)
    else:
        files = get_legacy_input_files()

    for idx, file in enumerate(files):
        printBlock(f"FILE {idx}:", file)
        test = ApbsLegacyInput()
        try:
            pprint.pp(test.load(file))
            del FINAL_OUTPUT
            FINAL_OUTPUT = {}
        except Exception as e:
            display_error(file, e)
