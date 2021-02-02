from typing import Literal
from numpy import array, NAN
from pyparsing import (
    CaselessLiteral,
    CharsNotIn,
    Combine,
    Dict,
    Forward,
    Group,
    OnlyOnce,
    Keyword,
    LineEnd,
    LineStart,
    MatchFirst,
    NotAny,
    OneOrMore,
    Optional,
    ParseException,
    ParseResults,
    QuotedString,
    Regex,
    Suppress,
    White,
    Word,
    ZeroOrMore,
    alphas,
    alphanums,
    downcaseTokens,
    empty,
    lineEnd,
    printables,
    oneOf,
    replaceWith,
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

pyValue_list = [
    number,
    Keyword("True").setParseAction(replaceWith(True)),
    Keyword("False").setParseAction(replaceWith(False)),
    Keyword("NAN", caseless=True).setParseAction(replaceWith(NAN)),
    Keyword("None").setParseAction(replaceWith(None)),
    QuotedString('"""', multiline=True),
    QuotedString("'''", multiline=True),
    QuotedString('"'),
    QuotedString("'"),
]

# Common patterns
pyValue = MatchFirst(e.setWhitespaceChars(" \t\r") for e in pyValue_list)


class ApbsLegacyInput:

    # Questions:
    #   1. Can READ, ELEC, and PRINT have leading/trailing spaces?
    #      A: Yes
    #   2. do keyword and values HAVE to be on the same line?
    #   3. are keyword and non-path values case sensitive?
    #      A: No

    identifier = Word(alphas, alphanums + r"_")
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
    # mol format(pqr|pdb) path
    #     is path considered relative?
    mol_val = CaselessLiteral("mol")
    mol_format_val = CaselessLiteral("pqr") | CaselessLiteral("pdb")
    mol_value = Group(mol_val + mol_format_val + path_val)
    # parm format(flat) path
    #     is path considered relative?
    parm_val = CaselessLiteral("parm")
    parm_format_val = CaselessLiteral("flat")
    parm_value = Group(parm_val + parm_format_val + path_val)
    # diel format(dx) path-x, path-y, path-z
    #     are path-x, path-y, path-z considered relative?
    #     where to find non-zero ionic strength
    diel_val = CaselessLiteral("diel")
    dx_format_val = CaselessLiteral("dx")
    diel_value = Group(
        diel_val + dx_format_val + path_val + path_val + path_val
    )
    # kappa format(dx) path
    #     dependant on a previous diel
    #     is path considered relative?
    kappa_val = CaselessLiteral("kappa")
    kappa_value = Group(kappa_val + dx_format_val + path_val)
    # charge format(dx) path
    #     is path considered relative?
    charge_val = CaselessLiteral("charge")
    charge_value = Group(charge_val + dx_format_val + path_val)

    read_body = Group(
        ZeroOrMore(mol_value)
        + ZeroOrMore(parm_value)
        + ZeroOrMore(diel_value)
        + ZeroOrMore(kappa_value)
        + ZeroOrMore(charge_value)
    )
    read_value = Group(Suppress(read_val) + read_body + Suppress(end_val))

    # ELEC section specific grammar
    elec_name_val = CaselessLiteral("name")
    elec_name_value = Group(elec_name_val + Word(alphanums))

    elec_type_options_val = oneOf(
        "mg-auto mg-para mg-manual fe-manual mg-dummy", caseless=True
    )
    elec_type_value = elec_type_options_val

    # ELEC Keywords
    akeyPRE_val = CaselessLiteral("akeyPRE")
    akeyPRE_options_val = oneOf("unif geom", caseless=True)
    akeyPRE_value = Group(akeyPRE_val + akeyPRE_options_val)

    akeySOLVE_val = CaselessLiteral("akeySOLVE")
    akeySOLVE_options_val = oneOf("resi", caseless=True)
    akeySOLVE_value = Group(akeySOLVE_val + akeySOLVE_options_val)

    async_val = CaselessLiteral("async")
    async_value = Group(async_val + number)

    bcfl_val = CaselessLiteral("bcfl")
    bcfl_options_val = oneOf("zero sdh mdh focus", caseless=True)
    bcfl_value = Group(bcfl_val + bcfl_options_val)

    calcenergy_val = CaselessLiteral("calcenergy")
    calcenergy_options_val = oneOf("no total comps", caseless=True)
    calcenergy_value = Group(calcenergy_val + calcenergy_options_val)

    calcforce_val = CaselessLiteral("calcforce")
    calcforce_options_val = oneOf("no total comps", caseless=True)
    calcforce_value = Group(calcenergy_val + calcenergy_options_val)

    cgcent_val = CaselessLiteral("cgcent")
    cgcent_mol_val = CaselessLiteral("mol") + number
    cgcent_coord_val = Group(number * 3)
    cgcent_value = Group(cgcent_val + (cgcent_mol_val | cgcent_coord_val))

    cglen_val = CaselessLiteral("cglen")
    cglen_coord_val = Group(number * 3)
    cglen_value = Group(cglen_val + cgcent_coord_val)

    chgm_val = CaselessLiteral("chgm")
    chgm_options_val = oneOf("spl0 spl2", caseless=True)
    chgm_value = Group(chgm_val + chgm_options_val)

    dime_val = CaselessLiteral("dime")
    dime_coord_val = Group(number * 3)
    dime_value = Group(dime_val + dime_coord_val)

    elec_body = Group(
        ZeroOrMore(elec_name_value)
        + ZeroOrMore(elec_type_value)
        + ZeroOrMore(akeyPRE_value)
        + ZeroOrMore(akeySOLVE_value)
        + ZeroOrMore(async_value)
        + ZeroOrMore(bcfl_value)
        + ZeroOrMore(calcenergy_value)
        + ZeroOrMore(calcforce_value)
        + ZeroOrMore(cgcent_value)
        + ZeroOrMore(cglen_value)
        + ZeroOrMore(chgm_value)
        + ZeroOrMore(dime_value)
    )
    elec_value = Group(Suppress(elec_val) + elec_body + Suppress(end_val))

    all_values = Group(
        OneOrMore(read_value) + OneOrMore(elec_value) + quit_val
    )

    # PRINT...END
    # QUIT

    def __init__(self):
        # self.read_section = Group(
        #    self.read_value + self.read_body + self.end_value
        # )
        pass

    def paramParser(self):
        """Create a pattern matching any definition of parameters with the form

        variable_name value [value] [value]

        Value can be any standard python value (int, number, None, False, True, NaN
        or quoted strings) or a raw string, which can be multiline if additional
        lines start with a whitespace.

        Return a Dict element to allow accessing data using the varible name as a key.

        This Dict has two special fields :
            names_ : the list of column names found
            units_ : a dict in the form {key : unit}
        """

        def formatBloc(t):
            """Format the result to have a list of (key, values) easily usable with Dict

            Add two fields :
                names_ : the list of column names found
                units_ : a dict in the form {key : unit}
            """
            rows = []

            # store units and names
            units = {}
            names = []

            for row in t:
                print(f"ROW: {row}")
                if row.name == self.end_val:
                    break
                rows.append(ParseResults([row.name, row.value]))
                names.append(row.name)
                if row.unit:
                    units[row.name] = row.unit[0]

            # rows.append(ParseResults(["names_", names]))
            # rows.append(ParseResults(["unit_", units]))

            return rows

        print(self.all_values)
        paramBloc = OneOrMore(self.all_values).setParseAction(formatBloc)

        return Dict(paramBloc)

    def loads(self, input_data: str):
        """Parse an input file as a string"""

        # Group section name and content
        section = Group(self.all_values + self.paramParser())

        # Build the final parser and suppress empty sections
        parser = Dict(OneOrMore(section))
        parser.ignore(self.comment + restOfLine)

        return parser.parseString(input_data, parseAll=True)

    def load(self, filename: str):
        """
        Read Legacy Input file and pass it to loads as a string

        :param str filename: The path/filename to the APBS legacy config file
        :return: the ApbsConfig object
        :rtype: ApbsConfig
        """
        with open(filename, "r") as fp:
            data = fp.read()
        try:
            data = self.loads(data)

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
    test = ApbsLegacyInput()
    print(test.load("../../examples/helix/Apbs_solv-TEMPLATE.in"))
