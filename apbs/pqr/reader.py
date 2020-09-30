from pyparsing import Group, Word, ZeroOrMore, alphas, alphanums, nums
from apbs.chemistry import Atom, AtomList


class PQRReader:
    """A grammar/parser for PQR formatted data and files."""

    def __init__(self):
        """Define the grammar for an ATOM in PQR format."""
        # TODO: Limit the field_name grammar to ATOM or HETOM literals
        identifier = Word(alphas, alphanums + "_")
        integer_val = Word(nums + "-")
        float_val = Word(nums + "-" + ".")
        atom_value = Group(
            identifier("field_name")
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

    def loads(self, pqr_string: str) -> AtomList:
        """
        Find instances of atoms ignoring other syntax.

        :param str pqr_string: One or more ATOM/HETOM
        :return: the list of Atoms in the pqr_string
        :rtype: AtomList
        """
        atoms = []
        idx = 1
        for item, _start, _stop in self.atom.scanString(pqr_string):
            for match in item:
                atom = Atom(
                    field_name=match.field_name,
                    atom_number=match.atom_number,
                    atom_name=match.atom_name,
                    residue_name=match.residue_name,
                    chain_id=match.chain_id,
                    residue_number=match.residue_number,
                    ins_code=match.ins_code,
                    x=match.x,
                    y=match.y,
                    z=match.z,
                    charge=match.charge,
                    radius=match.radius,
                    id=idx,
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
            data = fp.read().replace("\n", "")
        return self.loads(data)


if __name__ == "__main__":
    # execute only if run as a script
    sample = r"""
ATOM   5226  HD1 TYR   337     -24.642  -2.718  30.187  0.115 1.358
ATOM      7  CD   LYS D   1      44.946 37.289  9.712    -0.0608  1.9080
ATOM     39 O3PB ADP     1     -16.362  -6.763  26.980 -0.900 1.700
ATOM     39 O3PB ADP     1  DORK   -16.362  -6.763  26.980 -0.900 1.700
"""

    reader = PQRReader()
    atoms = []
    atoms = reader.loads(sample)
