from typing import Union
import numpy as np
from apbs.geometry import Coordinate


class Atom:
    """
    Attributes:
        position (Coordinate): Atomic position
        radius (float): Atomic radius
        charge (float): Atomic charge
        epsilon (float): Epsilon value for WCA calculations
        id (int): Atomic ID; this should be a unique non-negative integer
            assigned based on the index of the atom in a Valist atom array
        res_name (str): Residue name from PDB/PQR file
        name (str): Atom name from PDB/PDR file
    """

    def __init__(self, *args, **kwargs):
        """
        Arguments:

        :param int id: A unique identifier for this Atom
        :param str field_name: Specifies the type of PQR entry and should be
                         either ATOM or HETATM in order to be parsed by APBS.
        :param int atom_number: The atom index.
        :param str atom_name: The atom name.
        :param str residue_name: The residue name.
        :param str chain_id: An optional value which provides the chain ID of
                         the atom. NOTE: that chain ID support is a new
                         feature of APBS 0.5.0 and later versions.
        :param int residue_number: The residue index.
        :param str ins_code: An optional value which provides the PDB insertion
                         code.
        :param float x: The X atomic coordinate in angstroms
        :param float y: The Y atomic coordinate in angstroms
        :param float z: The Z atomic coordinate in angstroms
        :param float charge: The atomic charge (in electrons).
        :param float radius: The atomic radius (in angstroms).

        :Example:

          atom = Atom(
                    id=42,
                    field_name=ATOM,
                    atom_number=39,
                    atom_name=O3PB,
                    residue_name=ADP,
                    chain_id=None,
                    residue_number=1,
                    ins_code=None,
                    x=-16.362,
                    y=-6.763,
                    z=26.980,
                    charge=-0.900,
                    radius=1.700
                )
        """

        if len(args) > 0:
            self.position = Coordinate(args[0], args[1], args[2])

        self.field_name: str = kwargs.get("field_name", None)
        self.atom_number: int = int(kwargs.get("atom_number", 0))
        self.atom_name: str = kwargs.get("atom_name", None)
        self.residue_name: str = kwargs.get("residue_name", None)
        self.chain_id: str = kwargs.get("chain_id", None)
        self.residue_number: int = int(kwargs.get("residue_number", 0))
        self.ins_code: str = kwargs.get("ins_code", None)
        if "x" in kwargs and "y" in kwargs and "z" in kwargs:
            self.position = Coordinate(
                float(kwargs.get("x")),
                float(kwargs.get("y")),
                float(kwargs.get("z")),
            )
        self.charge: float = float(kwargs.get("charge", 0.0))
        self.radius: float = float(kwargs.get("radius", 0.0))
        self.epsilon: float = float(kwargs.get("epsilon", 0.0))
        self.id: int = int(kwargs.get("id", 0))
        if "id" not in kwargs:
            raise ValueError("The Atom id must be set to non-zero value")
        if self.field_name not in ["ATOM", "HETATM"]:
            raise ValueError(
                f"The Atom fieldname must be ATOM or HETATM, not "
                f"{self.field_name}"
            )

    def __str__(self):
        return (
            f"Atom< name< {self.field_name} >, {self.position}, "
            f"radius< {self.radius} >, charge< {self.charge} > >"
        )

    def __repr__(self):
        return (
            f"Atom< name< {self.field_name} >, {self.position}, "
            f"radius< {self.radius} >, charge< {self.charge} > >"
        )

    def euclidian_dist2(
        self, other: Union["Atom", Coordinate, np.ndarray]
    ) -> float:
        """
        Euclidian distance without the square root

        :param other: Another Atom, Coordinate, or np.ndarray to calculate
                      the euclidian distance from this Atom (without taking
                      the square root)
        :type other: Atom, Coordinate, or np.ndarray
        :return: The euclidian distance between two X,Y,Z coordinates
        :rtype: float
        """
        if isinstance(other, Atom):
            return np.sum((self.position._data - other.position._data) ** 2)
        if isinstance(other, Coordinate):
            return np.sum((self.position._data - other._data) ** 2)
        elif isinstance(other, np.ndarray):
            # TODO: Figure out how to apply
            # https://numpy.org/doc/stable/reference/generated/numpy.dot.html
            return np.sum((self.position._data - other) ** 2)
        else:
            raise TypeError

    @property
    def x(self) -> float:
        return self.position.x

    @property
    def y(self) -> float:
        return self.position.y

    @property
    def z(self) -> float:
        return self.position.z
