from math import inf
from typing import List
from . import Atom
from apbs.geometry import Coordinate
from typing import Tuple


class AtomList:
    """
    Thin abstraction over a container of atoms

    Attributes:
        dp (dict): dict for dynamic programming of values that may not need to
                be re-calculated
    """

    def __init__(self, atoms: List = None):
        """
        :param atoms: A list of Atoms
        :type atoms: List
        """
        self._atoms: Tuple[Atom] = atoms if atoms is not None else []
        self.charge: float = None
        self.maxrad: float = None

        self._center = Coordinate()
        self._mincrd = Coordinate()
        self._maxcrd = Coordinate()
        self._dp = dict()

    def center(self) -> Coordinate:
        """Molecule center
        note: not the median molecule, but the average of the max values int
        the x, y, and z coordinates
        """
        if "center" not in self._dp.keys():
            ma = self.maxcrd()
            mi = self.mincrd()
            self._dp["center"] = Coordinate(
                (ma.x + mi.x) * 0.5,
                (ma.y + mi.y) * 0.5,
                (ma.z + mi.z) * 0.5,
            )
        return self._dp["center"]

    def mincrd(self) -> Coordinate:
        """Minimum coordinates"""
        if "min" not in self._dp.keys():
            x, y, z = inf, inf, inf
            for a in self._atoms:
                x = min(x, a.x)
                y = min(y, a.y)
                z = min(z, a.z)
            self._dp["min"] = Coordinate(x, y, z)

        return self._dp["min"]

    def maxcrd(self) -> Coordinate:
        """Maximum coordinates"""
        if "max" not in self._dp.keys():
            x, y, z = 0.0, 0.0, 0.0
            for a in self._atoms:
                x = max(x, a.x)
                y = max(y, a.y)
                z = max(z, a.z)
            self._dp["max"] = Coordinate(x, y, z)

        return self._dp["max"]

    @property
    def max_radius(self) -> float:
        if "max_radius" not in self._dp.keys():
            m = 0.0
            for a in self._atoms:
                m = max(m, a.radius)
            self._dp["max_radius"] = m

        return self._dp["max_radius"]

    @property
    def count(self) -> int:
        return len(self._atoms)

    @property
    def atoms(self) -> Tuple[Atom]:
        return self._atoms
