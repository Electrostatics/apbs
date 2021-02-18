import argparse
from pathlib import Path
from pprint import pprint
from pyparsing import CaselessLiteral as CLiteral
from pyparsing import (
    Combine,
    Group,
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
from re import search

# Purpose:
#   The ApbsLegacyInput class was written to parse input files
#   that were developed over the years and has a "less formal" syntax.
#   The Input file syntax is pretty well documented at
#   https://apbs.readthedocs.io/en/latest/using/index.html#input-file-syntax

# Approach:
#   The ApbsLegacyInput class parses an entire input file by the major
#   sections:
#      - READ = Holds directives for reading input files for molecules
#      - ELEC = Holds polar calculation parameters
#      - APOLAR = Holds apolar calculation parameters
#      - PRINT = Holds specification for summary output
#
#   There are several auxillary classes in this file that are used to
#   define "groupings" or private "namespaces". For example, the
#   genericToken class has tokens for can be used across many other
#   classes. The apolarToken class only has tokens that are in the
#   APOLAR section. These classes help build a heirarchy or tokens
#   and grammars that are specified in the documentation.
#   The list of token classes are:
#       - genericToken
#       - apolarToken
#       - elecToken
#       - pbToken
#   There are also classes for specific tokens and grammars for
#   more targeted parsing. For example, the tabiParser class is only
#   for parsing the ELEC section where the calculation type is tabi-auto.
#   The list of parsing classes are:
#       tabiParser:
#       fe_manualParser:
#       geoflow_autoParser:
#       mg_autoParser:
#       mg_manualParser:
#       mg_paraParser:
#       mg_dummyParser:
#       pbam_autoParser:
#       pbsam_autoParser:
#
#   Each SECTION has a parser and formatter. The parser specifies the
#   grammar used to match the section of the input file and generates
#   a pyparsing.ParseResults value. That value is forwarded to a
#   formatter to produce a dictionary for that SECTION. For example,
#   the readParser parses the input file for the READ section. The
#   readParser forwards the ParseResults to the formatRead method to
#   produce the 'READ' dictionary in the FINAL_OUTPUT dictionary.
#
#   NOTE: Since the formatters have ParseResults that can be any
#         combination of nested string, key/value, key/list,
#         key/dictionary, or ParseResults; the code has to check
#         data types for each item at each level. This code is
#         combersome and hard to write/read. If the input file format
#         followed a more traditional eBNF, the grammars would be much
#         simpler and the formatting code would be much cleaner.
#
#   NOTE: Whenever possible, convert integers in config files to python
#         integers. Likewise, convert floats to python floats.

# Algorithm:
#   The pyparsing module is used to define methods for grammars for each
#   section.
#
#   For example, the readParser is responsible for parsing the
#   READ section of the input file and producing a section of the final
#   dictionary.
#
#   The final dictionary holds the entire parsed file and is meant to be
#   used as a "main" configuration for running the apbs extecutable.

# Caveats:
#   There are cases where keywords can be used multiple times. In other
#   words, the keywords are not always "unique" which is problematic in
#   building a dictionary of key/value pairs. It is also the case that
#   "sections" like ELEC, APOLAR, and PRINT can be repeated. Technically,
#   even the READ section can be repeated like the following:
#       READ mol prq ABC.pqr END
#       READ mol pdb DEF.pdb END
#
#   To deal with this, each section is a "dict" with integer keys that
#   incrementally build up based on the order the section was read from
#   the input file. The example above would result in a dictionary section
#   like the following:
#       'READ': {0: {'mol': {'pqr': ['ABC.pqr']}},
#                1: {'mol': {'pqr': ['DEF.pqr']}}}
#
#   If the input file had combined the values in on READ section, like the
#   following:
#       READ
#         mol prq ABC.pqr
#         mol pdb DEF.pdb
#       END
#   Then the resulting dictionary would be:
#       'READ': {0: {'mol': {'pqr': ['ABC.pqr', 'DEF.pqr']}}}
#
#   The grammars are used to "group" sections, subsection, keys, and values
#   together. One problem that was discovered was example files with PRINT
#   sections that did not have "spaces" between IDENTIFIERS and operators.
#   For example, the following works correctly:
#       PRINT elecEnergy mol-1 + mol-2 - complex_mol END
#   Since IDENTIFIERS can have dashes/hyphens and underscores in them, the
#   grammar can only support them if the operators are separated from the
#   IDENTIFIERS by spaces. The following will not parse correctly:
#       PRINT elecEnergy mol-1+mol-2-complex_mol END
#
#   The grammars and parsers convert all input and produce all output in
#   lowercase alphanumeric representation. This could be a problem for
#   filenames/pathnames.


class ApbsLegacyInput:
    """Class for reading in legacy APBS input files."""

    def __init__(self):
        """Setup parsing tokens and grammar for the APBS input file format."""

        # Tokens used by multiple parsers
        self.COMMENT = "#"
        # TODO: The DEBUG flag should be set/accessed from a higher
        #       level debugging object that is set from a config
        #       file or command line option.
        self.DEBUG = False
        self.END_VAL = CLiteral("END")

        # The FINAL_OUTPUT is a dictionary produced after parsing a file
        # using the grammar and processing rules.
        self.FINAL_OUTPUT = {}

        # The highest level grammar to parse the READ, ELEC, APOLAR, and
        # PRINT sections until the QUIT keyword is found.
        self.grammar = OneOrMore(self.readParser()) + OneOrMore(
            self.apolarParser() | self.elecParser()
        ) + ZeroOrMore(self.printParser()) + Suppress(CLiteral("QUIT")) | (
            # The following is one way to "narrow down" errors in an input
            # file. It was adopted from:
            # https://web.archive.org/web/20160821175151/http://pyparsing.wikispaces.com/share/view/30875955
            # The idea is that the parsers above will succeed or the
            # following will succeed and set the ParseSyntaxException
            # stating that the token in the file is "unknown" and will
            # show the line and column where the parser stopped. It does
            # not always point out subtle errors like when a key is used
            # in a section that does not support it. For example, using
            # a keyword from pbam-auto in an ELEC section that specifies
            # using tabi-auto type.
            empty
            - ~Word(printables).setName("<unknown>")
        )

    @staticmethod
    def get_integer_grammar():
        """Convenience function that is used by other classes."""
        return Combine(Optional("-") + Word(nums))  # .setDebug()

    @staticmethod
    def get_identifier_grammar():
        """Convenience function that is used by other classes."""
        return (
            Word(alphanums, alphanums + r"_" + r"-")
            | ApbsLegacyInput.get_integer_grammar()
        )  # .setDebug()

    @staticmethod
    def get_number_grammar():
        """Convenience function that is used by other classes."""
        return (
            pyparsing_common.real | ApbsLegacyInput.get_integer_grammar()
        )  # .setDebug()

    @staticmethod
    def get_path_grammar():
        """Convenience function that is used by other classes."""
        return Word(printables)  # .setDebug()

    def debug(self, message: str):
        """Convenience function to track down grammar/parsing problems"""
        if self.DEBUG:
            print(message)

    def formatReadSection(self, results: ParseResults, groups: list) -> dict:
        """Format the READ section of the APBS input file.

        :param results ParseResults: the pyparsing representation of the matching grammar
        :param groups list: the section of the FINAL_OUTPUT to generate
        :return: a dictionary containing the READ section of the input file
        :rtype: dict

        Example: Convert the following:
            read
                mol pqr 24dup.pqr
            end
        To:
            'READ': {0: {'mol': {'pqr': ['24dup.pqr']}}}
        """

        # TODO: More documentation!
        """Convert lists of lists to a dictionary"""

        section = "READ"
        if section not in self.FINAL_OUTPUT:
            self.FINAL_OUTPUT[section] = {}
        idx = len(self.FINAL_OUTPUT[section])
        self.debug(f"IDX: {idx}")
        self.FINAL_OUTPUT[section][idx] = {}

        # TODO: More error checking
        #       - What if key not in groups?
        self.debug(f"RESULTS: {results}")
        for result in results[0]:
            self.debug(f"TYPE: {type(result)}")
            for field in result:
                self.debug(f"type item: {type(field)} {field}")
                if isinstance(field, ParseResults):
                    key = field[0].lower()
                    self.debug(f"key: {key}")
                    if key in groups:
                        if key not in self.FINAL_OUTPUT[section][idx]:
                            self.debug(f"ADD KEY: {key}")
                            self.FINAL_OUTPUT[section][idx][key] = {}
                        subkey = f"{field[1]}".lower()
                        if subkey not in self.FINAL_OUTPUT[section][idx][key]:
                            self.debug(f"ADD SUBKEY: {subkey}")
                            self.FINAL_OUTPUT[section][idx][key][subkey] = []
                        if field[2] is not None:
                            self.debug(
                                f"VALUES: KEY: {key}, SUBKEY: {subkey}, item2: {field[2]}"
                            )
                            item2 = ", ".join(field[2].split())
                            self.FINAL_OUTPUT[section][idx][key][
                                subkey
                            ].append(item2)

        return self.FINAL_OUTPUT

    def readParser(self):
        """Setup the tokens and grammar for the READ section."""

        # https://apbs.readthedocs.io/en/latest/using/input/read.html

        PATH_VAL = ApbsLegacyInput.get_path_grammar()

        # tokens/grammars:
        file_fmt = oneOf("dx gz", caseless=True)
        charge = Group(CLiteral("charge") - file_fmt - PATH_VAL)
        diel = Group(
            CLiteral("diel") - file_fmt - PATH_VAL - PATH_VAL - PATH_VAL
        )
        kappa = Group(CLiteral("kappa") - file_fmt - PATH_VAL)
        mol_format = oneOf("pqr pdb", caseless=True)
        mol = Group(CLiteral("mol") - mol_format - PATH_VAL)
        parm_format = oneOf("flat xml", caseless=True)
        parm = Group(CLiteral("parm") - parm_format - PATH_VAL)
        pot = Group(CLiteral("pot") - file_fmt - PATH_VAL)
        groups = ["charge", "diel", "kappa", "mol", "parm", "pot"]

        grammar = Group(
            OneOrMore(mol)
            & ZeroOrMore(charge)
            & ZeroOrMore(diel)
            & ZeroOrMore(kappa)
            & ZeroOrMore(parm)
            & ZeroOrMore(pot)
        )

        def formatRead(results: ParseResults):
            return self.formatReadSection(results, groups)

        return Group(
            Suppress(CLiteral("READ")) - grammar - Suppress(self.END_VAL)
        ).setParseAction(formatRead)

    def printParser(self):
        """Setup the tokens and grammar for the PRINT section."""

        IDENTIFIER = ApbsLegacyInput.get_identifier_grammar()

        # tokens/grammars:
        choices = oneOf(
            "elecEnergy elecForce apolEnergy apolForce", caseless=True
        )
        expr = IDENTIFIER + Optional(
            OneOrMore(oneOf("+ -") + IDENTIFIER)
            + ZeroOrMore(oneOf("+ -") + IDENTIFIER)
        )
        grammar = Group(choices - expr)

        def formatPrint(results: ParseResults):
            """Format the PRINT section of the APBS input file.

            :param results ParseResults: the pyparsing representation of the matching grammar
            :return: a dictionary containing the PRINT section of the input file
            :rtype: dict

            Example: Convert the following:
                print elecEnergy complex - mol2 - mol1 end
            To:
                'PRINT': {0: {'elecenergy': ['complex', '-', 'mol2', '-', 'mol1']}}
            """

            section = "PRINT"
            if section not in self.FINAL_OUTPUT:
                self.FINAL_OUTPUT[section] = {}
            idx = len(self.FINAL_OUTPUT[section])
            self.FINAL_OUTPUT[section][idx] = {}

            for row in results[0]:
                self.debug(f"TYPE: {type(row)}")
                for item in row:
                    self.debug(f"type item: {type(item)} {item}")
                    if isinstance(item, str):
                        key = item.lower()
                        self.debug(f"key: {key}")
                        if key not in self.FINAL_OUTPUT[section][idx].keys():
                            self.FINAL_OUTPUT[section][idx][key] = row[1:]
                            break
                else:
                    # NOTE: UGLY but this is how to break out of the inner and
                    #       outer loop as documented at:
                    #       https://note.nkmk.me/en/python-break-nested-loops
                    break
                break

            return self.FINAL_OUTPUT

        value = Group(
            Suppress(CLiteral("PRINT")) - grammar - Suppress(self.END_VAL)
        ).setParseAction(formatPrint)

        return value

    def formatSection(self, t: ParseResults, section: str):
        """Format the ELEC or APOLAR section of the APBS input file.

        :param results ParseResults: the pyparsing representation of the matching grammar
        :return: a dictionary containing the ELEC or APOLAR section of the input file
        :rtype: dict

        Example: Convert the following:
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
        To:
            'ELEC': {0: {'bcfl': 'sdh',
                  'calcenergy': 'total',
                  'calcforce': 'no',
                  'cgcent': ['mol', '3'],
                  'cglen': ['156', '121', '162'],
                  'chgm': 'spl0',
                  'dime': ['161', '161', '161'],
                  'fgcent': ['mol', '3'],
                  'fglen': ['112', '91', '116'],
                  'ion': {0: {'charge': '1', 'conc': 0.05, 'radius': 2.0},
                          1: {'charge': '-1', 'conc': 0.05, 'radius': 2.0}},
                  'mol': '1',
                  'name': 'mol1',
                  'pbe': 'npbe',
                  'pdie': 2.0,
                  'sdens': 10.0,
                  'sdie': 78.4,
                  'srad': 1.4,
                  'srfm': 'mol',
                  'swin': 0.3,
                  'temp': 298.15,
                  'type': 'mg-auto'}
        """

        if section not in self.FINAL_OUTPUT:
            self.FINAL_OUTPUT[section] = {}
        idx = len(self.FINAL_OUTPUT[section])
        self.FINAL_OUTPUT[section][idx] = {}
        self.debug(f"SECTION OUTPUT: {self.FINAL_OUTPUT[section]}")

        retval = {}

        for result in t[0]:
            self.debug(f"RESULT TYPE: {type(result)}")
            self.debug(f"RESULT VALUE: {result}")
            self.debug(f"RESULT LENGTH: {len(result)}")
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
                self.debug(f"FOUND PBE?: {retval}")
                retval["pbe"] = result[0]
                continue
            # NOTE: Normal Key/Value case
            key = result[0]
            value = result[1]
            if len(result) == 2:
                self.debug(f"KEY: {key} VALUE: {value}")
                if isinstance(value, ParseResults):
                    self.debug(f"ParseResults VALUE: {value}")
                    value = value.asList()
                if key in retval:
                    self.debug(f"Key Already Exits: {retval}")
                    retval[key].append(value)
                    continue
                self.debug(f"NORMAL Key/Value: {retval}")
                # NOTE: It is easier to put the value into an List
                #       and append multiple values to the key. Later
                #       we post-process the result to remove the List
                #       if it only has 1 value.
                retval[key] = [value]
                continue
            # NOTE: More complicated than Key/Value case, probably ion
            self.debug(f"COMPLICATED KEY: {result[0]}")
            if result[0] not in retval:
                retval[result[0]] = {}
            sub_idx = len(retval[result[0]])
            for item in result[1:]:
                self.debug(f"SUB_IDX: {sub_idx}")
                self.debug(f"ITEM: {item}")
                if sub_idx not in retval[result[0]]:
                    retval[result[0]][sub_idx] = {}
                if item[0] in retval[result[0]][sub_idx]:
                    self.debug("WARN: We need to change key/value to key/dict")
                retval[result[0]][sub_idx][item[0]] = item[1]

        # NOTE: Post process retval to replace Key/List with Key/Value
        #       if the List only has 1 element in it
        for item in retval:
            self.debug(f"PRE KEY/VALUE: {item}")
            if isinstance(retval[item], list) and len(retval[item]) == 1:
                retval[item] = retval[item][0]
                self.debug(f"POST KEY/VALUE: {retval[item]}")

        self.FINAL_OUTPUT[section][idx] = retval
        self.debug(f"FINAL_OUTPUT: {self.FINAL_OUTPUT[section]}")
        return self.FINAL_OUTPUT

    def apolarParser(self):
        """Setup the tokens and grammar for the APOLAR section.

        :return: a dictionary containing the APOLAR section of the input file
        :rtype: dict
        """

        grammar = (
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

        def formatApolar(results: ParseResults):
            return self.formatSection(results, "APOLAR")

        return Group(
            Suppress(CLiteral("APOLAR")) - grammar - Suppress(self.END_VAL)
        ).setParseAction(formatApolar)

    def elecParser(self):
        """Setup the tokens and grammar for the ELEC section.

        :return: a dictionary containing the APOLAR section of the input file
        :rtype: dict
        """

        grammar = (
            tabiParser.grammar
            | fe_manualParser.grammar
            | geoflow_autoParser.grammar
            | mg_autoParser.grammar
            | mg_manualParser.grammar
            | mg_paraParser.grammar
            | mg_dummyParser.grammar
            | pbam_autoParser.grammar
            | pbsam_autoParser.grammar
        )

        def formatElec(results: ParseResults):
            return self.formatSection(results, "ELEC")

        return Group(
            Suppress(CLiteral("ELEC")) - grammar - Suppress(self.END_VAL)
        ).setParseAction(formatElec)

    def raise_error(self, source: str, pe: ParseSyntaxException):
        """Parsing failed - try to be produce a helpful error messsage.

        :param source str: the filename or string representing the data
        :param pe ParseSyntaxException: the Exception that was caught
        :return: None
        :rtype: None
        """

        repeat_count = 70
        message = "\n" + "=" * repeat_count + "\n"
        message += f"ERROR: {type(pe)}\n"
        message += f"Parsing {source}\n"
        message += f"Line Number: {pe.lineno}:\n"
        message += f"Column: {pe.col}:\n"
        message += "=" * repeat_count + "\n"
        message += f"Line: \n{pe.line}\n"
        message += " " * (pe.col - 1) + "^\n"
        message += "=" * repeat_count + "\n"
        pe.msg = message
        raise pe

    def __del__(self):
        """Wipe out any previous results."""
        del self.FINAL_OUTPUT

    def loads(self, input_data: str):
        """Parse the input as a string

        :param str input_data: a string containiner an APBS legacy input configuration file
        :return: a dictionary configuration files contents
        :rtype: dict
        """

        parser = self.grammar
        parser.ignore(self.COMMENT + restOfLine)

        value: ParseResults = None
        self.debug(f"DEFAULT value: TYPE {type(value)} {value}")

        try:
            value = self.grammar.searchString(input_data)
        except ParseSyntaxException as pe:
            self.raise_error("STRING", pe)

        # NOTE: the ParseResults has 1 or more "wrappers"
        #       around the dictionary so we just want to
        #       unwrap the value to get to that actual
        #       dictionary or str representing the data.
        self.debug(f"value: TYPE {type(value)}")
        if isinstance(value, ParseResults):
            self.debug(f"value: TYPE[0]{type(value[0])}")
            if isinstance(value[0], ParseResults):
                self.debug(f"value: TYPE[0][0] {type(value[0][0])}")
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
                self.raise_error(filename, pe)


class genericToken:
    """Generic tokens/grammars that can be used by other classes."""

    NUMBER_VAL = ApbsLegacyInput.get_number_grammar()

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
    """APOLAR specific tokens/grammars that can be used by other classes."""

    # https://apbs.readthedocs.io/en/latest/using/input/apolar/index.html

    NUMBER_VAL = ApbsLegacyInput.get_number_grammar()
    dpos = Group(CLiteral("dpos") - NUMBER_VAL)
    press = Group(CLiteral("press") - NUMBER_VAL)
    srfm = Group(CLiteral("srfm") - oneOf("sacc", caseless=True))


class elecToken:
    """ELEC specific tokens/grammars that can be used by other classes."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/index.html

    # The following tokens are used by at least 2 of the parser types with
    # the same format and rules

    IDENTIFIER = ApbsLegacyInput.get_identifier_grammar()
    INTEGER_VAL = ApbsLegacyInput.get_integer_grammar()
    NUMBER_VAL = ApbsLegacyInput.get_number_grammar()
    PATH_VAL = ApbsLegacyInput.get_path_grammar()

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
    """ELEC tabi specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/tabi.html

    INTEGER_VAL = ApbsLegacyInput.get_integer_grammar()
    NUMBER_VAL = ApbsLegacyInput.get_number_grammar()

    # TODO: Should this be replaced which a check to make
    # sure that "NUMBER_VAL is between 0.0 and 1.0"
    mac = Group(CLiteral("mac") - NUMBER_VAL)
    mesh = Group(CLiteral("mesh") - oneOf("0 1 2 ses skin"))
    outdata = Group(CLiteral("outdata") - oneOf("0 1"))
    tree_n0 = Group(CLiteral("tree_n0") - INTEGER_VAL)
    tree_order = Group(CLiteral("tree_order") - INTEGER_VAL)

    grammar = (
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
    """ELEC fe-manual specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/fe-manual.html

    IDENTIFIER = ApbsLegacyInput.get_identifier_grammar()
    INTEGER_VAL = ApbsLegacyInput.get_integer_grammar()
    NUMBER_VAL = ApbsLegacyInput.get_number_grammar()

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

    grammar = (
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
    """ELEC geoflow-auto specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/geoflow-auto.html

    NUMBER_VAL = ApbsLegacyInput.get_number_grammar()

    press = Group(CLiteral("press") - NUMBER_VAL)
    vdwdisp = Group(CLiteral("vdwdisp") - oneOf("0 1"))

    grammar = (
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
    """ELEC mg-auto specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-auto.html

    grammar = (
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
    """ELEC mg-manual specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-manual.html

    INTEGER_VAL = ApbsLegacyInput.get_integer_grammar()

    nlev = Group(CLiteral("nlev") - INTEGER_VAL)

    grammar = (
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
    """ELEC mg-para specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-para.html

    NUMBER_VAL = ApbsLegacyInput.get_number_grammar()

    # TODO: Combine with dime?
    pdime = Group(CLiteral("pdime") - elecToken.grid_floats)
    ofrac = Group(CLiteral("ofrac") - NUMBER_VAL)

    grammar = (
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
    """ELEC mg-dummy specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-dummy.html

    grammar = (
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
    """ELEC pbam-auto and pbsam-auto specific tokens/grammars."""

    IDENTIFIER = ApbsLegacyInput.get_identifier_grammar()
    INTEGER_VAL = ApbsLegacyInput.get_integer_grammar()
    NUMBER_VAL = ApbsLegacyInput.get_number_grammar()
    PATH_VAL = ApbsLegacyInput.get_path_grammar()

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
    """ELEC pbam-auto specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/pbam-auto.html

    grammar = (
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
    """ELEC pbsam-auto specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/pbsam-auto.html

    NUMBER_VAL = ApbsLegacyInput.get_number_grammar()
    PATH_VAL = ApbsLegacyInput.get_path_grammar()

    # pbsam-auto specific Keywords
    exp = Group(CLiteral("exp") - PATH_VAL)
    imat = Group(CLiteral("imat") - PATH_VAL)
    surf = Group(CLiteral("surf") - PATH_VAL)
    tolsp = Group(CLiteral("tolsp") - NUMBER_VAL)

    grammar = (
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


def printBanner(prefix: str, item: str):
    """Helper function to print a banner around a message.

    :param prefix str: the prefix string to put in front of the item
    :param item str: the string to be displayed
    :return: None
    :rtype: None

    Example output of the command:
        printBanner(f"FILE {idx}", file)
    where idx is 0 and file is "/LONG_PATH_TO/apbs-mol-auto.in" would
    produce the output:
        ======================================================================
        ==  FILE 0: LONG_PATH_TO/actin-dimer/apbs-mol-auto.in
        ======================================================================
    """

    lbanner = "=" * 70
    sbanner = "=" * 2
    print(f"\n{lbanner}\n{sbanner}  {prefix}: {item}\n{lbanner}")


def get_example_files(opt_path: str = "", pattern: str = "**/*.in") -> list:
    """Helper function to get one or more example input files.

    :param opt_path str: the relative path under the examples directory
    :param pattern str: the regular expression to match files
    :return: list of absolute filenames that match the pattern
    :rtype: list

    NOTE: This function filters out any filenames that have TEMPLATE or
          start with dxmath which are not valid APBS input files.

    Example output of the command:
        get_example_files("actin-dimer", "apbs-mol-auto.in")
    returns:
        ["/LONG_PATH_TO/actin-dimer/apbs-mol-auto.in"]
    """

    search_path = (
        Path(__file__).absolute().parent.parent.parent / "examples" / opt_path
    )
    matches = Path(search_path).glob(pattern)
    matches = filter(lambda x: not search("TEMPLATE", x.name), matches)
    return filter(lambda x: not x.name.startswith("dxmath"), matches)


def build_parser(default_values: dict):
    """Build argument parser.
    :return:  argument parser
    :rtype:  argparse.ArgumentParser
    """

    desc = "ApbsLegacyInput file parser"

    parser = argparse.ArgumentParser(
        description=desc,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--all",
        action="store_const",
        dest="all",
        default=False,
        const="value-to-store",
        help=("Run on all files found in apbs/examples/* directories"),
    )
    return parser


def main():
    """Main driver for running from command line."""

    default_values = {"SINGLE": True}

    parser = build_parser(default_values)
    args = parser.parse_args()

    # The following are files in the examples directory
    # that are useful for testing purposes but SHOULD NOT
    # replace the use of pytest!
    relfilename = "solv/apbs-smol.in"
    relfilename = "pbsam-gly/gly_dynamics.in"
    relfilename = "helix/apbs_solv.in"
    relfilename = "smpbe/apbs-smpbe-24dup.in"
    relfilename = "actin-dimer/apbs-mol-auto.in"

    example_dir = relfilename.split("/")[0]
    example_pattern = relfilename.split("/")[1]

    if args.all:
        # Parse all the APBS input files found in apbs/examples/*
        files = get_example_files()
    else:
        files = get_example_files(example_dir, example_pattern)

    for idx, file in enumerate(files):
        printBanner(f"FILE {idx}", file)
        apbs_input = ApbsLegacyInput()
        try:
            pprint(apbs_input.load(file))
        except Exception as e:
            apbs_input.raise_error(file, e)


if __name__ == "__main__":
    main()
