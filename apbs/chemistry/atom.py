from typing import List, Union
import numpy as np
from apbs.geometry import Coordinate


class Atom:
    '''
    Attributes:
        position (Coordinate): Atomic position
        radius (float): Atomic radius
        charge (float): Atomic charge
        epsilon (float): Epsilon value for WCA calculations
        id (int): Atomic ID; this should be a unique non-negative integer
            assigned based on the index of the atom in a Valist atom array
        res_name (str): Residue name from PDB/PQR file
        name (str): Atom name from PDB/PDR file
    '''

    def __init__(self, *vals: List[float]):
        if len(vals) > 0:
            self.position = Coordinate(*vals)
        else:
            self.position = Coordinate()
        self.radius: float = 0.
        self.charge: float = 0.
        self.partID: float = 0.
        self.epsilon: float = 0.
        self.id: int = 0
        self.res_name: str = ''
        self.name: str = ''

    def __str__(self):
        return 'Atom< name< %s >, %s, radius< %s >, charge< %s >>' \
            % (self.name, self.position, self.radius, self.charge)

    def __repr__(self):
        return 'Atom< name< %s >, %s  radius< %s >, charge< %s >>' \
            % (self.name, self.position, self.radius, self.charge)

    def euclidian_dist2(self, other: Union['Atom', Coordinate, np.ndarray]
                        ) -> float:
        '''
        Euclidian distance without the square root
        '''
        if isinstance(other, Atom):
            return np.sum((self.position._data - other.position._data) ** 2)
        if isinstance(other, Coordinate):
            return np.sum((self.position._data - other._data) ** 2)
        elif isinstance(other, np.ndarray):
            return np.sum((self.position._data - other) ** 2)
        else:
            raise RuntimeError(
                'Incorrect data type passed into euclidian_dist2')

    @property
    def x(self) -> float:
        return self.position.x

    @property
    def y(self) -> float:
        return self.position.y

    @property
    def z(self) -> float:
        return self.position.z
