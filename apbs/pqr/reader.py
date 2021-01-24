# -*- coding: utf-8 -*-

from apbs.chemistry import Atom, AtomList
from pyparsing import (
    Group,
    LineEnd,
    LineStart,
    Literal,
    Word,
    ZeroOrMore,
    alphas,
    alphanums,
    nums,
    printables,
)

import re


class PQRReader:
    """A grammar/parser for PQR formatted data and files."""

    def __init__(self):
        """Define the grammar for an ATOM/HETATM in PQR format."""
        identifier = Word(alphas, alphanums + r"_")
        alphanumidentifier = Word(alphanums + r"_" + r"'" + r"*")
        integer_val = Word(nums + "-")
        float_val = Word(nums + "-" + ".")
        anything = Word(printables + " " + "\t")
        keyword_val = Literal("ATOM") | Literal("HETATM")
        skip_val = Literal("TER") | Literal("END") | Literal("REMARK")
        skip_value = Group(
            LineStart()
            + skip_val("field_name")
            + anything("anything")
            + LineEnd(),
        )
        atom_value = Group(
            LineStart()
            + keyword_val("field_name")
            + integer_val("atom_number")
            + alphanumidentifier("atom_name")
            + identifier("residue_name")
            + ZeroOrMore(identifier("chain_id"))
            + integer_val("residue_number")
            + ZeroOrMore(identifier("ins_code"))
            + float_val("x")
            + float_val("y")
            + float_val("z")
            + float_val("charge")
            + float_val("radius")
            + LineEnd(),
        )
        # NOTE: Skips blank or lines with only whitespace (tabs, spaces, etc.)
        self.atom = ZeroOrMore(atom_value | skip_value)

    def loads(self, pqr_string: str) -> AtomList:
        """
        Find instances of atoms ignoring other syntax.

        :param str pqr_string: One or more ATOM/HETATM
        :return: the list of Atoms in the pqr_string
        :rtype: AtomList
        """
        atoms = []
        idx: int = 1
        matches = self.atom.parseString(pqr_string, parseAll=True)
        for match in matches:
            if re.search("REMARK|TER|END", match.field_name) is not None:
                continue
            atom = Atom(
                field_name=match.field_name,
                atom_number=int(match.atom_number),
                atom_name=match.atom_name,
                residue_name=match.residue_name,
                chain_id=match.chain_id,
                residue_number=int(match.residue_number),
                ins_code=match.ins_code,
                x=float(match.x),
                y=float(match.y),
                z=float(match.z),
                charge=float(match.charge),
                radius=float(match.radius),
                id=int(idx),
            )
            atoms.append(atom)
            idx += 1
        return AtomList(atoms)

    def load(self, filename: str) -> AtomList:
        """
        Read Atoms from a file in PQR format

        :param str filename: The path/filename to the PQR file
        :return: the list of Atoms in the pqr file
        :rtype: AtomList
        """
        with open(filename, "r") as fp:
            data = fp.read()
        return self.loads(data)


if __name__ == "__main__":
    # execute only if run as a script
    sample = r"""
REMARK This is just test data and values may have been modified for testing
ATOM   5226  HD1 TYR   337     -24.642  -2.718  30.187  0.115 1.358
ATOM      7  CD   LYS D   1      44.946 37.289  9.712    -0.0608  1.9080
ATOM     39 O3PB ADP     1     -16.362  -6.763  26.980 -0.900 1.700
REMARK This is just a comment hiding in the data
HETATM     39 O3PB ADP     1     -16.362  -6.763  26.980 -0.900 1.700
ATOM     39 O3PB ADP     1  D   -16.362  -6.763  26.980 -0.900 1.700
"""

    reader = PQRReader()
    atoms = []
    atoms = reader.loads(sample)
    print(atoms)
