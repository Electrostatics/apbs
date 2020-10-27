from pyparsing import (
    CharsNotIn,
    Group,
    Literal,
    Word,
    ZeroOrMore,
    alphas,
    alphanums,
    delimitedList,
    nums,
    pyparsing_common,
    printables,
)
from apbs.chemistry import Atom, AtomList


class MarkdownReader:
    """A grammar/parser for Markdown formatted data and files."""

    def __init__(self):
        """Define the grammar for an ATOM/HETATM in Markdown format."""
        title_identifier = Word(alphas, alphanums + "_")
        break_identifier = CharsNotIn("=")
        desc_identifier = Word(alphas, alphanums + "_")
        table_header_identifier = Word(alphas + "|")
        table_structure_identifier = Literal("---|---|---|---|---")

        # any string of non-whitespace characters, except for ','csv_value = Word(printables, excludeChars=",")
        data_value = Word(printables, excludeChars="|")

        data_value = CharsNotIn("|")
        record = []
        record = print(delimitedList(data_value).parseString(tableline))

        data_line = Group()
        integer_val = Word(nums + "-")
        float_val = Word(nums + "-" + ".")
        keyword_val = Literal("ATOM") | Literal("HETATM")
        atom_value = Group(
            keyword_val("field_name")
            + integer_val("atom_number")
            + identifier("atom_name")
            + identifier("residue_name")
            + ZeroOrMore(identifier("chain_id"))
            + integer_val("residue_number")
            + ZeroOrMore(identifier("ins_code"))
            + float_val("x")
            + float_val("y")
            + float_val("z")
            + float_val("charge")
            + float_val("radius")
        )
        self.atom = atom_value + ZeroOrMore(atom_value)

    def loads(self, Markdown_string: str) -> AtomList:
        """
        Find instances of atoms ignoring other syntax.

        :param str Markdown_string: One or more ATOM/HETATM
        :return: the list of Atoms in the Markdown_string
        :rtype: AtomList
        """
        atoms = []
        idx: int = 1
        for item, _start, _stop in self.atom.scanString(Markdown_string):
            for match in item:
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
        Read Atoms from a file in Markdown format

        :param str filename: The path/filename to the Markdown file
        :return: the list of Atoms in the Markdown file
        :rtype: AtomList
        """
        with open(filename, "r") as fp:
            data = fp.read().replace("\n", "")
        return self.loads(data)


if __name__ == "__main__":
    # execute only if run as a script
    sample = r"""
README for Born APBS examples
=============================

This is the canonical electrostatics test case: Born ion. A non-polarizable ion with a single embedded point charge; has an analytical solution for the potential. We examine the solvation free energy as a function of ionic radius.

Please see apbs.in for details on the particular solvation energy calculations. Analytical results are given in pmf.dat.

This example was contributed by Nathan Baker.

Input File|Description|APBS Version|Results (kJ/mol)|Analytical (kJ/mol)
---|---|---|---|---
[apbs-mol-auto.in](apbs-mol-auto.in)|Sequential, 3 A sphere, 3-level focusing to 0.188 A, srfm mol|**1.5**|**-229.7740**|-230.62
|||1.4.2|-229.774
|||1.4.1|-229.7736|
|||1.4|-229.7736<sup>[3](#3)</sup>
|||1.3|-229.7735
"""

    reader = MarkdownReader()
    atoms = []
    atoms = reader.loads(sample)
    print(atoms)
