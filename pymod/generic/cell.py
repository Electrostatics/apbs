
from . import AtomList

class CellList(AtomList):
    def __init__(self):
        ...

    def better_name(self, center: Coordinate, radius: float, atomID: int
            ) -> bool:
        '''Determines if a point is within the union of the spheres centered
        at the atomic centers with radii equal to the sum of their van der
        Waals radii and the probe radius.  Does not include contributions
        from the specified atom.
        '''
