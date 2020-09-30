from typing import List, Union
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
        self.field_name: str = kwargs.get("field_name", None)
        self.atom_number: int = kwargs.get("atom_number", 0)
        self.atom_name: str = kwargs.get("atom_name", None)
        self.residue_name: str = kwargs.get("residue_name", None)
        self.chain_id: str = kwargs.get("chain_id", None)
        self.residue_number: int = kwargs.get("residue_number", 0)
        self.ins_code: str = kwargs.get("ins_code", None)
        self.position = Coordinate()
        if "x" in kwargs and "y" in kwargs and "z" in kwargs:
            self.position = Coordinate(
                kwargs.get("x"), kwargs.get("y"), kwargs.get("z")
            )
        self.charge: float = kwargs.get("charge", 0.0)
        self.radius: float = kwargs.get("radius", 0.0)
        self.epsilon: float = kwargs.get("epsilon", 0.0)
        self.id: int = kwargs.get("id", 0)
        # TODO: The ID must get set to be unique!
        # if ("id" not in kwargs):
        #     raise ValueError("The Atom id must be set to non-zero value")

    def __str__(self):
        return "Atom< name< %s >, %s, radius< %s >, charge< %s >>" % (
            self.name,
            self.position,
            self.radius,
            self.charge,
        )

    def __repr__(self):
        return "Atom< name< %s >, %s  radius< %s >, charge< %s >>" % (
            self.name,
            self.position,
            self.radius,
            self.charge,
        )

    def euclidian_dist2(self, other: Union["Atom", Coordinate, np.ndarray]) -> float:
        """
        Euclidian distance without the square root
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
            raise RuntimeError("Incorrect data type passed into euclidian_dist2")

    @property
    def x(self) -> float:
        return self.position.x

    @property
    def y(self) -> float:
        return self.position.y

    @property
    def z(self) -> float:
        return self.position.z
