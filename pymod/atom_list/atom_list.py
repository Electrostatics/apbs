from typing import Tuple
from math import inf

import sys
sys.path.insert(0, '..')
from atom import Atom
from coordinate import Coordinate

class AtomList:
    '''
    Thin abstraction over a container of atoms

    Attributes:
        dp (dict): dict for dynamic programming of values that may not need to
                be re-calculated
    '''
    def __init__(self, filename: str=None, atoms=None):
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
        '''Molecule center'''
        if 'center' not in self._dp.keys():
            ma = self.maxcrd()
            mi = self.mincrd()
            self._dp['center'] = Coordinate(
                    ( ma.x+mi.x ) * .5,
                    ( ma.y+mi.y ) * .5,
                    ( ma.z+mi.z ) * .5,
                    )
        return self._dp['center']

    def mincrd(self) -> Coordinate:
        '''Minimum coordinates'''
        if 'min' not in self._dp.keys():
            x, y, z = inf, inf, inf
            for a in self._atoms:
                x = min(x, a.x)
                y = min(y, a.y)
                z = min(z, a.z)
            self._dp['min'] = Coordinate(x, y, z)

        return self._dp['min']

    def maxcrd(self) -> Coordinate:
        '''Maximum coordinates'''
        if 'max' not in self._dp.keys():
            x, y, z = 0., 0., 0.
            for a in self._atoms:
                x = max(x, a.x)
                y = max(y, a.y)
                z = max(z, a.z)
            self._dp['max'] = Coordinate(x, y, z)
        
        return self._dp['max']

    @property
    def count(self) -> int:
        return len(self._atoms)

    @property
    def atoms(self) -> Tuple[Atom]:
        return self._atoms

    def read_pdb(self, fn: str):
        with open(fn, 'r') as f:
            lines = f.readlines()
            i = 0
            for l in lines:
                l = [ i.lower() for i in l.strip().split(' ') if i != '' ]
                if l[0] in ('atom', 'hetatm'):
                    a = Atom()
                    a.name = l[2].upper()
                    a.res_name = l[3].upper()
                    a.position = Atom(
                        float(l[5]),
                        float(l[6]),
                        float(l[7]),
                        )
                    a.id = i


                    if len(l) == 10:
                        a.charge = float( l[8] )
                        a.radius = float( l[9] )

                    self._atoms.append(a)
                    i += 1
