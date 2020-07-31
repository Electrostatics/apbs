from .. import Coordinate
from apbs_types import *
import numpy as np

import sys
import os
sys.path.insert(0, '..')
sys.path.insert(0, os.path.join('..', '..'))


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

    @property
    def x(self) -> float:
        return self.position.x

    @property
    def y(self) -> float:
        return self.position.y

    @property
    def z(self) -> float:
        return self.position.z
