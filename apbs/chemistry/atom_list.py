from math import inf
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

    def __init__(self, filename: str = None, atoms=None):
        self._atoms: Tuple[Atom] = atoms if atoms is not None else []
        self.charge: float = None
        self.maxrad: float = None

        self._center = Coordinate()
        self._mincrd = Coordinate()
        self._maxcrd = Coordinate()
        self._dp = dict()
        if filename is not None:
            self.read_pdb(filename)

    def center(self) -> Coordinate:
        """Molecule center
        note: not the median molecule, but the average of the max values int
        the x, y, and z coordinates
        """
        if "center" not in self._dp.keys():
            ma = self.maxcrd()
            mi = self.mincrd()
            self._dp["center"] = Coordinate(
                (ma.x + mi.x) * 0.5, (ma.y + mi.y) * 0.5, (ma.z + mi.z) * 0.5,
            )
        return self._dp["center"]

    def mincrd(self) -> Coordinate:
        """Minimum coordinates
        """
        if "min" not in self._dp.keys():
            x, y, z = inf, inf, inf
            for a in self._atoms:
                x = min(x, a.x)
                y = min(y, a.y)
                z = min(z, a.z)
            self._dp["min"] = Coordinate(x, y, z)

        return self._dp["min"]

    def maxcrd(self) -> Coordinate:
        """Maximum coordinates
        """
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

    def read_pdb(self, fn: str):
        """
        Read serialized atoms in the PBD format from a file
        """
        with open(fn, "r") as f:
            lines = f.readlines()
            idx = 0
            for line in lines:
                fields = [
                    idx.lower() for idx in line.strip().split(" ") if idx != ""
                ]
                if fields[0] in ("atom", "hetatm"):
                    a = Atom()
                    a.name = fields[2].upper()
                    a.res_name = fields[3].upper()
                    a.position = Atom(
                        float(fields[5]), float(fields[6]), float(fields[7]),
                    )
                    a.id = idx

                    if len(fields) == 10:
                        a.charge = float(fields[8])
                        a.radius = float(fields[9])

                    self._atoms.append(a)
                    idx += 1
