"""
The ApbsLegacyInput class was written to parse input files
that were developed over the years and has a "less formal" syntax.
The Input file syntax is pretty well documented at
https://apbs.readthedocs.io/en/latest/using/index.html#input-file-syntax
"""

from sys import float_info

import argparse
import logging
from pathlib import Path
from pprint import pprint
from re import search

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

LOGGER = logging.getLogger(__name__)
FILENAME = "STRING"

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
#   GenericToken class has tokens for can be used across many other
#   classes. The ApolarToken class only has tokens that are in the
#   APOLAR section. These classes help build a heirarchy or tokens
#   and grammars that are specified in the documentation.
#   The list of token classes are:
#       - GenericToken
#       - ApolarToken
#       - ElecToken
#       - PbToken
#   There are also classes for specific tokens and grammars for
#   more targeted parsing. For example, the TabiParser class is only
#   for parsing the ELEC section where the calculation type is tabi-auto.
#   The list of parsing classes are:
#       TabiParser:
#       FeManualParser:
#       GeoflowAutoParser:
#       MgAutoParser:
#       MgManualParser:
#       MgParaParser:
#       MgDummyParser:
#       PbamAutoParser:
#       PbsamAutoParser:
#       PygbeParser:
#
#   Each SECTION has a parser and formatter. The parser specifies the
#   grammar used to match the section of the input file and generates
#   a pyparsing.ParseResults value. That value is forwarded to a
#   formatter to produce a dictionary for that SECTION. For example,
#   the read_parser parses the input file for the READ section. The
#   read_parser forwards the ParseResults to the format_read method to
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
#   For example, the read_parser is responsible for parsing the
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
#   sections that did not have "spaces" between identifierS and operators.
#   For example, the following works correctly:
#       PRINT elecEnergy mol-1 + mol-2 - complex_mol END
#   Since identifierS can have dashes/hyphens and underscores in them, the
#   grammar can only support them if the operators are separated from the
#   identifierS by spaces. The following will not parse correctly:
#       PRINT elecEnergy mol-1+mol-2-complex_mol END
#
#   The grammars and parsers convert all input and produce all output in
#   lowercase alphanumeric representation. This could be a problem for
#   filenames/pathnames.


def check_range(minv: float = float_info.min, maxv: float = float_info.max):
    def parseAction(results: ParseResults):
        key = 0
        value = 1
        if isinstance(results[0], ParseResults):
            if isinstance(results[0][key], str):
                if isinstance(results[0][value], float):
                    check_val = results[0][value]
                    if check_val < minv:
                        LOGGER.exception(
                            "ERROR: Value for %s, %s, must be greater than %s",
                            results[0][key],
                            results[0][value],
                            minv,
                        )
                        raise ValueError
                    if check_val > maxv:
                        LOGGER.exception(
                            "ERROR: Value for %s, %s, must be less than %s",
                            results[0][key],
                            results[0][value],
                            maxv,
                        )
                        raise ValueError
        return results

    return parseAction


class ApbsLegacyInput:
    """Class for reading in legacy APBS input files."""

    def __init__(self):
        """Setup parsing tokens and grammar for the APBS input file format."""

        # The FINAL_OUTPUT is a dictionary produced after parsing a file
        # using the grammar and processing rules.
        self.final_output = {}

        # The highest level grammar to parse the READ, ELEC, APOLAR, and
        # PRINT sections until the QUIT keyword is found.
        self.grammar = OneOrMore(self.read_parser()) + OneOrMore(
            self.apolar_parser() | self.elec_parser()
        ) + ZeroOrMore(self.print_parser()) + Suppress(CLiteral("QUIT")) | (
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

    def format_read_section(self, results: ParseResults, groups: list) -> dict:
        """Format the READ section of the APBS input file.

        :param results ParseResults: pyparsing results of the matching grammar
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

        section = "READ"
        if section not in self.final_output:
            self.final_output[section] = {}
        idx = len(self.final_output[section])
        LOGGER.debug("IDX: %s", idx)
        self.final_output[section][idx] = {}

        # TODO: More error checking
        #       - What if key not in groups?
        LOGGER.debug("RESULTS: %s", results)
        # Convert and put data types into a dictionary
        for result in results[0]:
            LOGGER.debug("TYPE: %s", type(result))
            for field in result:
                LOGGER.debug("type %s, field: %s: ", type(field), field)
                if isinstance(field, ParseResults):
                    key = field[0].lower()
                    LOGGER.debug("key: %s", key)
                    if key in groups:
                        if key not in self.final_output[section][idx]:
                            LOGGER.debug("ADD KEY: %s", key)
                            self.final_output[section][idx][key] = {}
                        subkey = f"{field[1]}".lower()
                        if subkey not in self.final_output[section][idx][key]:
                            LOGGER.debug("ADD SUBKEY: %s", subkey)
                            self.final_output[section][idx][key][subkey] = []
                        if field[2] is not None:
                            LOGGER.debug(
                                "VALUES: KEY: %s, SUBKEY: %s, field: %s",
                                key,
                                subkey,
                                field[2],
                            )
                            item2 = ", ".join(field[2].split())
                            self.final_output[section][idx][key][
                                subkey
                            ].append(item2)

        return self.final_output

    def read_parser(self):
        """Setup the tokens and grammar for the READ section."""

        # https://apbs.readthedocs.io/en/latest/using/input/read.html

        path_val = ApbsLegacyInput.get_path_grammar()

        # tokens/grammars:
        file_fmt = oneOf("dx gz", caseless=True)
        charge = Group(CLiteral("charge") - file_fmt - path_val)
        diel = Group(
            CLiteral("diel") - file_fmt - path_val - path_val - path_val
        )
        kappa = Group(CLiteral("kappa") - file_fmt - path_val)
        mesh = Group(CLiteral("mesh") - CLiteral("mcsf") - path_val)
        mol_format = oneOf("pqr pdb", caseless=True)
        mol = Group(CLiteral("mol") - mol_format - path_val)
        parm_format = oneOf("flat xml", caseless=True)
        parm = Group(CLiteral("parm") - parm_format - path_val)
        pot = Group(CLiteral("pot") - file_fmt - path_val)
        groups = ["charge", "diel", "kappa", "mol", "parm", "pot"]

        grammar = Group(
            OneOrMore(mol)
            & ZeroOrMore(
                mesh
            )  # .setParseAction(GenericToken.check_depricated)
            & ZeroOrMore(charge)
            & ZeroOrMore(diel)
            & ZeroOrMore(kappa)
            & ZeroOrMore(parm)
            & ZeroOrMore(pot)
        )

        def format_read(results: ParseResults):
            return self.format_read_section(results, groups)

        return Group(
            Suppress(CLiteral("READ")) - grammar - Suppress(CLiteral("END"))
        ).setParseAction(format_read)

    def print_parser(self):
        """Setup the tokens and grammar for the PRINT section."""

        identifier = ApbsLegacyInput.get_identifier_grammar()

        # tokens/grammars:
        choices = oneOf(
            "elecEnergy elecForce apolEnergy apolForce", caseless=True
        )
        expr = identifier + Optional(
            OneOrMore(oneOf("+ -") + identifier)
            + ZeroOrMore(oneOf("+ -") + identifier)
        )
        grammar = Group(choices - expr)

        def format_print(results: ParseResults):
            """Format the PRINT section of the APBS input file.

            :param results ParseResults: pyparsing results of matching grammar
            :return: a dictionary of the PRINT section of the input file
            :rtype: dict

            Example: Convert the following:
                print elecEnergy complex - mol2 - mol1 end
            To:
                'PRINT': {0: {
                                 'elecenergy':
                                 ['complex', '-', 'mol2', '-', 'mol1']
                             }
                         }
            """

            section = "PRINT"
            if section not in self.final_output:
                self.final_output[section] = {}
            idx = len(self.final_output[section])
            self.final_output[section][idx] = {}

            for row in results[0]:
                LOGGER.debug("TYPE: %s", type(row))
                for item in row:
                    LOGGER.debug("type item: %s, item: %s", type(item), item)
                    if isinstance(item, str):
                        key = item.lower()
                        LOGGER.debug("key: %s", key)
                        if key not in self.final_output[section][idx].keys():
                            self.final_output[section][idx][key] = row[1:]
                            break
                else:
                    # NOTE: UGLY but this is how to break out of the inner and
                    #       outer loop as documented at:
                    #       https://note.nkmk.me/en/python-break-nested-loops
                    break
                break

            return self.final_output

        value = Group(
            Suppress(CLiteral("PRINT")) - grammar - Suppress(CLiteral("END"))
        ).setParseAction(format_print)

        return value

    def format_section(self, results: ParseResults, section: str):
        """Format the ELEC or APOLAR section of the APBS input file.

        :param results ParseResults: pyparsing results of the matching grammar
        :return: a dictionary of the ELEC or APOLAR section of the input file
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

        if section not in self.final_output:
            self.final_output[section] = {}
        idx = len(self.final_output[section])
        self.final_output[section][idx] = {}
        LOGGER.debug("SECTION OUTPUT: %s", self.final_output[section])

        retval = {}

        for result in results[0]:
            LOGGER.debug("RESULT TYPE: %s", type(result))
            LOGGER.debug("RESULT VALUE: %s", result)
            LOGGER.debug("RESULT LENGTH: %s", len(result))
            if isinstance(result, str):
                if result in "randorient":
                    # NOTE: special case for "randorient" key
                    retval[result] = 1
                    continue
                # NOTE: We have something like mg-auto so we have to
                #       add a "type" key
                retval["type"] = result
                continue
            if len(result) == 1:
                # NOTE: We have something Group (List) with only 1 value
                #       like lrpbe so we have to add a "pbe" key
                LOGGER.debug("FOUND PBE?: %s", retval)
                retval["pbe"] = result[0]
                continue
            # NOTE: Normal Key/Value case
            key = result[0]
            value = result[1]
            if len(result) == 2:
                LOGGER.debug("KEY: %s, VALUE: %s", key, value)
                if isinstance(value, ParseResults):
                    LOGGER.debug("ParseResults VALUE: %s", value)
                    value = value.asList()
                if key in retval:
                    LOGGER.debug("Key Already Exits: %s", retval)
                    retval[key].append(value)
                    continue
                LOGGER.debug("NORMAL Key/Value: %s", retval)
                # NOTE: It is easier to put the value into an List
                #       and append multiple values to the key. Later
                #       we post-process the result to remove the List
                #       if it only has 1 value.
                retval[key] = [value]
                continue
            # NOTE: More complicated than Key/Value case, probably ion
            LOGGER.debug("COMPLICATED KEY: %s", result[0])
            if result[0] not in retval:
                retval[result[0]] = {}
            sub_idx = len(retval[result[0]])
            for item in result[1:]:
                LOGGER.debug("SUB_IDX: %s", sub_idx)
                LOGGER.debug("ITEM: %s", item)
                if sub_idx not in retval[result[0]]:
                    retval[result[0]][sub_idx] = {}
                if item[0] in retval[result[0]][sub_idx]:
                    LOGGER.debug(
                        "WARNING: We need to change key/value to key/dict"
                    )
                retval[result[0]][sub_idx][item[0]] = item[1]

        # NOTE: Post process retval to replace Key/List with Key/Value
        #       if the List only has 1 element in it
        for item in retval:
            LOGGER.debug("PRE KEY/VALUE: %s", item)
            if isinstance(retval[item], list) and len(retval[item]) == 1:
                retval[item] = retval[item][0]
                LOGGER.debug("POST KEY/VALUE: %s", retval[item])

        self.final_output[section][idx] = retval
        LOGGER.debug("FINAL_OUTPUT: %s", self.final_output[section])
        return self.final_output

    def apolar_parser(self):
        """Setup the tokens and grammar for the APOLAR section.

        :return: a dictionary containing the APOLAR section of the input file
        :rtype: dict
        """

        grammar = (
            ZeroOrMore(ElecToken.name)
            & ZeroOrMore(GenericToken.bconc)
            & ZeroOrMore(GenericToken.calcenergy)
            & ZeroOrMore(GenericToken.calcforce)
            & ZeroOrMore(ApolarToken.dpos)
            & ZeroOrMore(GenericToken.gamma)
            & ZeroOrMore(GenericToken.grid)
            & ZeroOrMore(GenericToken.mol)
            & ZeroOrMore(ApolarToken.press)
            & ZeroOrMore(GenericToken.sdens)
            & ZeroOrMore(GenericToken.srad)
            & ZeroOrMore(ApolarToken.srfm)
            & ZeroOrMore(GenericToken.swin)
            & ZeroOrMore(GenericToken.temp)
        )

        def format_apolar(results: ParseResults):
            return self.format_section(results, "APOLAR")

        return Group(
            Suppress(CLiteral("APOLAR")) - grammar - Suppress(CLiteral("END"))
        ).setParseAction(format_apolar)

    def elec_parser(self):
        """Setup the tokens and grammar for the ELEC section.

        :return: a dictionary containing the APOLAR section of the input file
        :rtype: dict
        """

        grammar = (
            TabiParser.grammar
            | FeManualParser.grammar
            | GeoflowAutoParser.grammar
            | MgAutoParser.grammar
            | MgManualParser.grammar
            | MgParaParser.grammar
            | MgDummyParser.grammar
            | PbamAutoParser.grammar
            | PbsamAutoParser.grammar
            | PygbeParser.grammar
        )

        def format_elec(results: ParseResults):
            return self.format_section(results, "ELEC")

        return Group(
            Suppress(CLiteral("ELEC")) - grammar - Suppress(CLiteral("END"))
        ).setParseAction(format_elec)

    @staticmethod
    def banner_message(lineno: int, column: int, line: str, msg: str) -> str:
        """Parsing failed - try to be produce a helpful error messsage.

        :param filename str: the file being parsed
        :param lineno int: the line number of a file
        :param column int: the column number of a lineno
        :param line str: the line of a file
        :param msg str: the msg to put in the message
        :return: formatted message
        :rtype: None
        """

        repeat_count = 70
        message = "=" * repeat_count + "\n"
        message += f"{msg}\n"
        message += f"Filename: {FILENAME}\n"
        message += f"Line Number: {lineno}:\n"
        message += f"Column: {column}:\n"
        message += "-" * repeat_count + "\n"
        message += f"Line: \n{line}\n"
        message += " " * (column - 1) + "^\n"
        message += "-" * repeat_count + "\n"

        return message

    def raise_error(self, perr: ParseSyntaxException):
        """Parsing failed - try to be produce a helpful error messsage.

        :param pe ParseSyntaxException: the Exception that was caught
        :return: None
        :rtype: None
        """

        msg = f"ERROR: {type(perr)}\n"
        message = self.banner_message(perr.lineno, perr.col, perr.line, msg)
        perr.msg = message
        raise perr

    def __del__(self):
        """Wipe out any previous results."""
        del self.final_output

    def loads(self, input_data: str):
        """Parse the input as a string

        :param str input_data: a string of an APBS legacy input file
        :return: a dictionary configuration files contents
        :rtype: dict
        """

        parser = self.grammar
        parser.ignore("#" + restOfLine)

        value: ParseResults = None

        try:
            value = self.grammar.searchString(input_data)
        except ParseSyntaxException as perr:
            self.raise_error(perr)

        # NOTE: the ParseResults has 1 or more "wrappers"
        #       around the dictionary so we just want to
        #       unwrap the value to get to that actual
        #       dictionary or str representing the data.
        LOGGER.debug("value: TYPE {type(value)}")
        if isinstance(value, ParseResults):
            LOGGER.debug("Type of value[0]: %s", type(value[0]))
            if isinstance(value[0], ParseResults):
                LOGGER.debug("Type of value[0][0] %s", type(value[0][0]))
                if isinstance(value[0][0], (dict, str)):
                    return value[0][0]
            if isinstance(value[0], (dict, str)):
                return value[0]
        if isinstance(value, (dict, str)):
            return value

        raise Exception(
            f"Could not parse data into dictionary from "
            f"TYPE{value}:\n{input_data}"
        )

    def load(self, filename: str):
        """
        Read Legacy Input congifuration file and pass it to loads as a string

        :param str filename: The APBS legacy input configuration file
        :return: a dictionary configuration files contents
        :rtype: dict
        """

        global FILENAME
        FILENAME = filename

        with filename.open() as fptr:
            try:
                return self.loads(fptr.read())
            except ParseSyntaxException as perr:
                self.raise_error(perr)


class GenericToken:
    """Generic tokens/grammars that can be used by other classes."""

    @staticmethod
    def check_depricated(
        instring: str, loc: int, tokenlist: ParseResults
    ) -> ParseResults:
        """Possibly using a depricated keyword or value.

        Try to be produce a helpful error messsage.

        :param instring str: the string that may be using a depricated value
        :param loc int: the character offset into string
        :return: the original or updated ParseResults
        :rtype: ParseResults
        """

        depricated_values = {"ekey": "glob", "bcfl": "mem"}
        substitute_values = {"glob": "global", "mem": "mdh"}
        if isinstance(tokenlist, ParseResults):
            if (
                isinstance(tokenlist[0], ParseResults)
                and len(tokenlist[0]) == 2
            ):
                # We have a key and value
                key = tokenlist[0][0]
                value = tokenlist[0][1]
                if key in depricated_values:
                    LOGGER.warning(
                        "WARNING: Depricated value (%s) for key "
                        "(%s) at offset (%s)",
                        value,
                        key,
                        loc,
                    )
                    if value in substitute_values:
                        new_value = substitute_values[value]
                        # NOTE: Unfortunatly, the location is the
                        #       offset from the begining of instring
                        #       so we don't have a line count.
                        #       Find the line number, column, and
                        #       offending line with this hack :-(
                        count = 0
                        idx = 1
                        lineno = 0
                        column = 0
                        line = ""
                        for data in instring.splitlines():
                            if count > loc:
                                break
                            if key in data and value in data:
                                line = data
                                lineno = idx
                                column = data.find(value) + 1
                            count += len(data) + 1
                            idx += 1
                        LOGGER.warning(
                            ApbsLegacyInput.banner_message(
                                lineno,
                                column,
                                line,
                                f"WARNING: Substituting new value, {new_value}"
                                f", for {value}",
                            )
                        )
                        # Substitute the depricated value with correct value
                        tokenlist[0][1] = new_value
        return tokenlist

    number_val = ApbsLegacyInput.get_number_grammar()

    bconc = Group(CLiteral("bconc") - number_val)
    calc_options = oneOf("no total comps", caseless=True)
    calcenergy = Group(CLiteral("calcenergy") - calc_options)
    calcforce = Group(CLiteral("calcforce") - calc_options)
    gamma = Group(CLiteral("gamma") - number_val)
    grid_coord = Group(number_val * 3)
    grid = Group(CLiteral("grid") - grid_coord)
    mol = Group(CLiteral("mol") - number_val)
    sdens = Group(CLiteral("sdens") - number_val)
    srad = Group(CLiteral("srad") - number_val)
    swin = Group(CLiteral("swin") - number_val)
    temp = Group(CLiteral("temp") - number_val)


class ApolarToken:
    """APOLAR specific tokens/grammars that can be used by other classes."""

    # https://apbs.readthedocs.io/en/latest/using/input/apolar/index.html

    number_val = ApbsLegacyInput.get_number_grammar()
    dpos = Group(CLiteral("dpos") - number_val)
    press = Group(CLiteral("press") - number_val)
    srfm = Group(CLiteral("srfm") - oneOf("sacc", caseless=True))


class ElecToken:
    """ELEC specific tokens/grammars that can be used by other classes."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/index.html

    # The following tokens are used by at least 2 of the parser types with
    # the same format and rules

    identifier = ApbsLegacyInput.get_identifier_grammar()
    integer_val = ApbsLegacyInput.get_integer_grammar()
    number_val = ApbsLegacyInput.get_number_grammar()
    path_val = ApbsLegacyInput.get_path_grammar()

    name = Group(CLiteral("name") - identifier)
    grid_floats = Group(number_val * 3)
    grid_ints = Group(integer_val * 3)
    mol_id = Group(CLiteral("mol") - integer_val)

    async_value = Group(CLiteral("async") - integer_val)
    bcfl_options = oneOf("focus map mdh mem sdh zero", caseless=True)
    bcfl = Group(CLiteral("bcfl") - bcfl_options)
    cgcent = Group(CLiteral("cgcent") - (mol_id | grid_floats))
    cglen = Group(CLiteral("cglen") - grid_floats)
    chgm_options = oneOf("spl0 spl2", caseless=True)
    chgm = Group(CLiteral("chgm") - chgm_options)
    dime = Group(CLiteral("dime") - grid_ints)
    etol = Group(CLiteral("etol") - number_val)
    fgcent = Group(CLiteral("fgcent") - (mol_id | grid_floats))
    fglen = Group(CLiteral("fglen") - grid_floats)
    gcent = Group(CLiteral("gcent") - (mol_id | grid_floats))
    glen = Group(CLiteral("glen") - grid_floats)

    # Are charge, conc, and radius ALL required?
    ion_charge = Group(CLiteral("charge") - number_val)
    ion_conc = Group(CLiteral("conc") - number_val)
    ion_radius = Group(CLiteral("radius") - number_val)
    ion = Group(CLiteral("ion") - ion_charge & ion_conc & ion_radius)

    nlev = Group(CLiteral("nlev") - number_val)

    # TODO: I think only 1 of these are allowed (not ZeroOrMore)
    pbe = Group(oneOf("lpbe lrpbe npbe nrpbe", caseless=True))

    # TODO: Number must be >= 1
    pdie = Group(CLiteral("pdie") - number_val).setParseAction(
        check_range(1.0)
    )

    # NOTE: Should be a value between 78-80?
    sdie = Group(CLiteral("sdie") - number_val)

    srfm_options = oneOf("mol smol spl2", caseless=True)
    srfm = Group(CLiteral("srfm") - srfm_options)

    usemap_options = oneOf("diel kappa charge", caseless=True)
    usemap = Group(CLiteral("usemap") - Group(usemap_options - integer_val))

    write_type_options = oneOf(
        "charge "
        "pot "
        "smol "
        "sspl "
        "vdw "
        "ivdw "
        "lap "
        "edens "
        "ndens "
        "qdens "
        "dielx "
        "diely "
        "dielz "
        "kappa",
        caseless=True,
    )
    write_format_options = oneOf("avs dx flat gz uhbd", caseless=True)
    write = Group(
        CLiteral("write")
        - Group(write_type_options - write_format_options - path_val)
    )

    writemat = Group(
        CLiteral("writemat") - oneOf("poisson", caseless=True) - path_val
    )


class TabiParser:
    """ELEC tabi specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/tabi.html

    integer_val = ApbsLegacyInput.get_integer_grammar()
    number_val = ApbsLegacyInput.get_number_grammar()

    # TODO: Should this be replaced which a check to make
    # sure that "number_val is between 0.0 and 1.0"
    mac = Group(CLiteral("mac") - number_val)
    mesh = Group(CLiteral("mesh") - oneOf("0 1 2 ses skin"))
    outdata = Group(CLiteral("outdata") - oneOf("0 1"))
    tree_n0 = Group(CLiteral("tree_n0") - integer_val)
    tree_order = Group(CLiteral("tree_order") - integer_val)

    grammar = (
        CLiteral("tabi")
        & ZeroOrMore(ElecToken.name)
        & ZeroOrMore(ElecToken.ion)
        & ZeroOrMore(mac)
        & ZeroOrMore(mesh)
        & ZeroOrMore(GenericToken.mol)
        & ZeroOrMore(outdata)
        & ZeroOrMore(ElecToken.pdie)
        & ZeroOrMore(GenericToken.sdens)
        & ZeroOrMore(ElecToken.sdie)
        & ZeroOrMore(GenericToken.srad)
        & ZeroOrMore(GenericToken.temp)
        & ZeroOrMore(tree_n0)
        & ZeroOrMore(tree_order)
    )


class FeManualParser:
    """ELEC fe-manual specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/fe-manual.html

    identifier = ApbsLegacyInput.get_identifier_grammar()
    integer_val = ApbsLegacyInput.get_integer_grammar()
    number_val = ApbsLegacyInput.get_number_grammar()

    akeyPRE_options = oneOf("unif geom", caseless=True)
    akeyPRE = Group(CLiteral("akeyPRE") - akeyPRE_options)
    akeySOLVE_options = oneOf("resi", caseless=True)
    akeySOLVE = Group(CLiteral("akeySOLVE") - akeySOLVE_options)
    domainLength = Group(CLiteral("domainLength") - ElecToken.grid_floats)
    ekey_options = oneOf("simp glob global frac", caseless=True)
    ekey = Group(CLiteral("ekey") - ekey_options).setParseAction(
        GenericToken.check_depricated
    )
    maxsolve = Group(CLiteral("maxsolve") - number_val)
    maxvert = Group(CLiteral("maxvert") - number_val)
    targetNum = Group(CLiteral("targetNum") - integer_val)
    targetRes = Group(CLiteral("targetRes") - number_val)
    usemesh = Group(CLiteral("usemesh") - identifier)

    grammar = (
        CLiteral("fe-manual")
        & ZeroOrMore(ElecToken.name)
        & ZeroOrMore(akeyPRE)
        & ZeroOrMore(akeySOLVE)
        & ZeroOrMore(ElecToken.async_value)
        & ZeroOrMore(ElecToken.bcfl)
        & ZeroOrMore(GenericToken.calcenergy)
        & ZeroOrMore(GenericToken.calcforce)
        & ZeroOrMore(ElecToken.chgm)
        & ZeroOrMore(domainLength)
        & ZeroOrMore(ekey)
        & ZeroOrMore(ElecToken.etol)
        & ZeroOrMore(ElecToken.ion)
        & ZeroOrMore(ElecToken.pbe)
        & ZeroOrMore(maxsolve)
        & ZeroOrMore(maxvert)
        & ZeroOrMore(GenericToken.mol)
        & ZeroOrMore(ElecToken.pdie)
        & ZeroOrMore(GenericToken.sdens)
        & ZeroOrMore(ElecToken.sdie)
        & ZeroOrMore(GenericToken.srad)
        & ZeroOrMore(ElecToken.srfm)
        & ZeroOrMore(GenericToken.swin)
        & ZeroOrMore(targetNum)
        & ZeroOrMore(targetRes)
        & ZeroOrMore(GenericToken.temp)
        & ZeroOrMore(ElecToken.usemap)
        & ZeroOrMore(usemesh)
        & ZeroOrMore(ElecToken.write)
    )


class GeoflowAutoParser:
    """ELEC geoflow-auto specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/geoflow-auto.html

    number_val = ApbsLegacyInput.get_number_grammar()

    press = Group(CLiteral("press") - number_val)
    vdwdisp = Group(CLiteral("vdwdisp") - oneOf("0 1"))
    keyword = CLiteral("geoflow-auto") | CLiteral("geoflow")
    # .setParseAction(GenericToken.check_depricated)

    grammar = (
        keyword
        & ZeroOrMore(ElecToken.name)
        & ZeroOrMore(ElecToken.bcfl)
        & ZeroOrMore(GenericToken.bconc)
        & ZeroOrMore(ElecToken.etol)
        & ZeroOrMore(GenericToken.gamma)
        & ZeroOrMore(
            GenericToken.grid
        )  # . setParseAction(GenericToken.check_depricated)
        & ZeroOrMore(ElecToken.pbe)
        & ZeroOrMore(GenericToken.mol)
        & ZeroOrMore(ElecToken.pdie)
        & ZeroOrMore(press)
        & ZeroOrMore(ElecToken.sdie)
        & ZeroOrMore(vdwdisp)
    )


class MgAutoParser:
    """ELEC mg-auto specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-auto.html

    grammar = (
        CLiteral("mg-auto")
        & ZeroOrMore(ElecToken.name)
        & ZeroOrMore(ElecToken.bcfl)
        & ZeroOrMore(GenericToken.calcenergy)
        & ZeroOrMore(GenericToken.calcforce)
        & ZeroOrMore(ElecToken.cgcent)
        & ZeroOrMore(ElecToken.cglen)
        & ZeroOrMore(ElecToken.chgm)
        & ZeroOrMore(ElecToken.dime)
        & ZeroOrMore(ElecToken.etol)
        & ZeroOrMore(ElecToken.fgcent)
        & ZeroOrMore(ElecToken.fglen)
        & ZeroOrMore(ElecToken.ion)
        & ZeroOrMore(ElecToken.pbe)
        & ZeroOrMore(GenericToken.mol)
        & ZeroOrMore(ElecToken.pdie)
        & ZeroOrMore(GenericToken.sdens)
        & ZeroOrMore(ElecToken.sdie)
        & ZeroOrMore(GenericToken.srad)
        & ZeroOrMore(ElecToken.srfm)
        & ZeroOrMore(GenericToken.swin)
        & ZeroOrMore(GenericToken.temp)
        & ZeroOrMore(ElecToken.usemap)
        & ZeroOrMore(ElecToken.write)
        & ZeroOrMore(ElecToken.writemat)
    )


class MgManualParser:
    """ELEC mg-manual specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-manual.html

    integer_val = ApbsLegacyInput.get_integer_grammar()

    nlev = Group(CLiteral("nlev") - integer_val)

    grammar = (
        CLiteral("mg-manual")
        & ZeroOrMore(ElecToken.name)
        & ZeroOrMore(ElecToken.bcfl)
        & ZeroOrMore(GenericToken.calcenergy)
        & ZeroOrMore(GenericToken.calcforce)
        & ZeroOrMore(ElecToken.chgm)
        & ZeroOrMore(ElecToken.dime)
        & ZeroOrMore(ElecToken.etol)
        & ZeroOrMore(ElecToken.gcent)
        & ZeroOrMore(ElecToken.glen)
        & ZeroOrMore(GenericToken.grid)
        & ZeroOrMore(ElecToken.ion)
        & ZeroOrMore(ElecToken.pbe)
        & ZeroOrMore(GenericToken.mol)
        & ZeroOrMore(nlev)
        & ZeroOrMore(ElecToken.pdie)
        & ZeroOrMore(GenericToken.sdens)
        & ZeroOrMore(ElecToken.sdie)
        & ZeroOrMore(GenericToken.srad)
        & ZeroOrMore(ElecToken.srfm)
        & ZeroOrMore(GenericToken.swin)
        & ZeroOrMore(GenericToken.temp)
        & ZeroOrMore(ElecToken.usemap)
        & ZeroOrMore(ElecToken.write)
        & ZeroOrMore(ElecToken.writemat)
    )


class MgParaParser:
    """ELEC mg-para specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-para.html

    number_val = ApbsLegacyInput.get_number_grammar()

    pdime = Group(CLiteral("pdime") - ElecToken.grid_floats)
    ofrac = Group(CLiteral("ofrac") - number_val)

    grammar = (
        CLiteral("mg-para")
        & ZeroOrMore(ElecToken.name)
        & ZeroOrMore(ElecToken.async_value)
        & ZeroOrMore(ElecToken.bcfl)
        & ZeroOrMore(GenericToken.calcenergy)
        & ZeroOrMore(GenericToken.calcforce)
        & ZeroOrMore(ElecToken.cgcent)
        & ZeroOrMore(ElecToken.cglen)
        & ZeroOrMore(ElecToken.chgm)
        & ZeroOrMore(ElecToken.dime)
        & ZeroOrMore(ElecToken.etol)
        & ZeroOrMore(ElecToken.fgcent)
        & ZeroOrMore(ElecToken.fglen)
        & ZeroOrMore(ElecToken.ion)
        & ZeroOrMore(ElecToken.pbe)
        & ZeroOrMore(GenericToken.mol)
        & ZeroOrMore(ofrac)
        & ZeroOrMore(ElecToken.pdie)
        & ZeroOrMore(pdime)
        & ZeroOrMore(GenericToken.sdens)
        & ZeroOrMore(ElecToken.sdie)
        & ZeroOrMore(GenericToken.srad)
        & ZeroOrMore(ElecToken.srfm)
        & ZeroOrMore(GenericToken.swin)
        & ZeroOrMore(GenericToken.temp)
        & ZeroOrMore(ElecToken.usemap)
        & ZeroOrMore(ElecToken.write)
        & ZeroOrMore(ElecToken.writemat)
    )


class MgDummyParser:
    """ELEC mg-dummy specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/mg-dummy.html

    grammar = (
        CLiteral("mg-dummy")
        & ZeroOrMore(ElecToken.name)
        & ZeroOrMore(ElecToken.bcfl)
        & ZeroOrMore(
            GenericToken.calcenergy
        )  # .setParseAction(GenericToken.check_depricated)
        & ZeroOrMore(
            GenericToken.calcforce
        )  # .setParseAction(GenericToken.check_depricated)
        & ZeroOrMore(ElecToken.chgm)
        & ZeroOrMore(ElecToken.dime)
        & ZeroOrMore(ElecToken.gcent)
        & ZeroOrMore(ElecToken.glen)
        & ZeroOrMore(GenericToken.grid)
        & ZeroOrMore(ElecToken.ion)
        & ZeroOrMore(
            ElecToken.nlev
        )  # .setParseAction(GenericToken.check_depricated)
        & ZeroOrMore(ElecToken.pbe)
        & ZeroOrMore(GenericToken.mol)
        & ZeroOrMore(ElecToken.pdie)
        & ZeroOrMore(GenericToken.sdens)
        & ZeroOrMore(ElecToken.sdie)
        & ZeroOrMore(GenericToken.srad)
        & ZeroOrMore(ElecToken.srfm)
        & ZeroOrMore(GenericToken.swin)
        & ZeroOrMore(GenericToken.temp)
        & ZeroOrMore(ElecToken.write)
    )


class PygbeParser:
    """ELEC pygbe specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/pygbe.html

    grammar = CLiteral("pygbe") & ZeroOrMore(ElecToken.name)


class PbToken:
    """ELEC pbam-auto and pbsam-auto specific tokens/grammars."""

    identifier = ApbsLegacyInput.get_identifier_grammar()
    integer_val = ApbsLegacyInput.get_integer_grammar()
    number_val = ApbsLegacyInput.get_number_grammar()
    path_val = ApbsLegacyInput.get_path_grammar()

    thr3dmap = Group(CLiteral("3dmap") - path_val)
    diff = Group(
        CLiteral("diff")
        - (
            CLiteral("stat")
            | Group(CLiteral("move") - number_val - number_val)
            | CLiteral("rot") - number_val
        )
    )
    dx = Group(CLiteral("dx") - path_val)
    grid2d = Group(
        CLiteral("grid2d")
        - Group(path_val - oneOf("x y z", caseless=True) - number_val)
    )  # .setDebug()
    gridpts = Group(CLiteral("gridpts") - integer_val)
    ntraj = Group(CLiteral("ntraj") - integer_val)
    pbc = Group(CLiteral("pbc") - number_val)
    randorient = CLiteral("randorient")
    runname = Group(CLiteral("runname") - Word(alphanums + "_"))
    runtype_options = oneOf(
        "energyforce electrostatics dynamics", caseless=True
    )
    runtype = Group(CLiteral("runtype") - runtype_options)

    # TODO: value of number_val should be 0.00 to 0.15?
    salt = Group(CLiteral("salt") - number_val)

    term_pos_options = oneOf("x<= x>= y<= y>= z<= z>= r<= r>=", caseless=True)
    term_contact = Group(CLiteral("contact") - path_val)
    term_pos = Group(term_pos_options - number_val - identifier)
    term_time = Group(CLiteral("time") - number_val)
    term = Group(
        CLiteral("term") + (term_contact | term_pos | term_time)
    )  # .setDebug()

    termcombine = Group(
        CLiteral("termcombine") - oneOf("and or", caseless=True)
    )
    units = Group(CLiteral("units") - oneOf("kcalmol jmol kT", caseless=True))
    xyz = Group(CLiteral("xyz") - Group(identifier - path_val))


class PbamAutoParser:
    """ELEC pbam-auto specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/pbam-auto.html

    keyword = CLiteral("pbam-auto") | CLiteral("pbam")
    # .setParseAction(GenericToken.check_depricated)

    grammar = (
        keyword
        & ZeroOrMore(ElecToken.name)
        & ZeroOrMore(PbToken.thr3dmap)
        & ZeroOrMore(PbToken.diff)
        & ZeroOrMore(PbToken.dx)
        & ZeroOrMore(PbToken.grid2d)
        & ZeroOrMore(PbToken.gridpts)
        & ZeroOrMore(GenericToken.mol)
        & ZeroOrMore(PbToken.ntraj)
        & ZeroOrMore(PbToken.pbc)
        & ZeroOrMore(ElecToken.pdie)
        & ZeroOrMore(PbToken.randorient)
        & ZeroOrMore(PbToken.runname)
        & ZeroOrMore(PbToken.runtype)
        & ZeroOrMore(PbToken.salt)
        & ZeroOrMore(ElecToken.sdie)
        & ZeroOrMore(GenericToken.temp)
        & ZeroOrMore(PbToken.term)
        & ZeroOrMore(PbToken.termcombine)
        & ZeroOrMore(PbToken.units)
        & ZeroOrMore(PbToken.xyz)
    )


class PbsamAutoParser:
    """ELEC pbsam-auto specific tokens/grammars."""

    # https://apbs.readthedocs.io/en/latest/using/input/elec/pbsam-auto.html

    number_val = ApbsLegacyInput.get_number_grammar()
    path_val = ApbsLegacyInput.get_path_grammar()

    # pbsam-auto specific Keywords
    exp = Group(CLiteral("exp") - path_val)
    imat = Group(CLiteral("imat") - path_val)
    surf = Group(CLiteral("surf") - path_val)
    tolsp = Group(CLiteral("tolsp") - number_val)

    keyword = CLiteral("pbsam-auto") | CLiteral("pbsam")
    # .setParseAction(GenericToken.check_depricated)

    grammar = (
        keyword
        & ZeroOrMore(ElecToken.name)
        & ZeroOrMore(PbToken.thr3dmap)
        & ZeroOrMore(PbToken.diff)
        & ZeroOrMore(PbToken.dx)
        & ZeroOrMore(exp)
        & ZeroOrMore(PbToken.grid2d)
        & ZeroOrMore(imat)
        & ZeroOrMore(PbToken.ntraj)
        & ZeroOrMore(PbToken.pbc)
        & ZeroOrMore(ElecToken.pdie)
        & ZeroOrMore(PbToken.randorient)
        & ZeroOrMore(PbToken.runname)
        & ZeroOrMore(PbToken.runtype)
        & ZeroOrMore(PbToken.salt)
        & ZeroOrMore(ElecToken.sdie)
        & ZeroOrMore(surf)
        & ZeroOrMore(GenericToken.temp)
        & ZeroOrMore(PbToken.term)
        & ZeroOrMore(PbToken.termcombine)
        & ZeroOrMore(tolsp)
        & ZeroOrMore(PbToken.units)
        & ZeroOrMore(PbToken.xyz)
    )


def print_banner(prefix: str, item: str):
    """Helper function to print a banner around a message.

    :param prefix str: the prefix string to put in front of the item
    :param item str: the string to be displayed
    :return: None
    :rtype: None

    Example output of the command:
        print_banner(f"FILE {idx}", file)
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


def build_parser():
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
        "--verbose",
        action="store_const",
        dest="verbose",
        default=False,
        const="value-to-store",
        help=("Print out the resulting dictionary"),
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

    parser = build_parser()
    args = parser.parse_args()

    # The following are files in the examples directory
    # that are useful for testing purposes but SHOULD NOT
    # replace the use of pytest!
    relfilename = "solv/apbs-smol.in"
    relfilename = "pbsam-gly/gly_dynamics.in"
    relfilename = "helix/apbs_solv.in"
    relfilename = "smpbe/apbs-smpbe-24dup.in"
    relfilename = "actin-dimer/apbs-mol-auto.in"
    relfilename = "pbam/toy_dynamics.in"
    relfilename = "born/apbs-mol-fem.in"
    relfilename = "geoflow/1a63.in"

    example_dir = relfilename.split("/")[0]
    example_pattern = relfilename.split("/")[1]

    if args.all:
        # Parse all the APBS input files found in apbs/examples/*
        files = get_example_files()
    else:
        files = get_example_files(example_dir, example_pattern)

    for idx, file in enumerate(files):
        if args.verbose:
            print_banner(f"FILE {idx}", file)
        apbs_input = ApbsLegacyInput()
        try:
            results = apbs_input.load(file)
            if args.verbose:
                pprint(results)
        except ParseSyntaxException as perr:
            ApbsLegacyInput.raise_error(perr)

    print("FINISH")


if __name__ == "__main__":
    main()
