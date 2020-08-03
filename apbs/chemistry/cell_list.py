from apbs.geometry import Coordinate
from . import AtomList

# Alias for coordinate in cases where 'stride' makes more sense as the type,
# though all the methods may remain the same.
Stride = Coordinate


class CellList(AtomList):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.lower_bound = Coordinate()
        self.upper_bound = Coordinate()
        self.stride = Stride(0.0, 0.0, 0.0)
