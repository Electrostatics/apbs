from collections import namedtuple
from . import Coordinate # noqa
from . import AtomList # noqa

# Alias for coordinate in cases where 'stride' makes more sense as the type,
# though all the methods may remain the same.
Stride = Coordinate

class CellList(AtomList):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.lower_bound = Coordinate()
        self.upper_bound = Coordinate()
        self.stride = Stride(0., 0., 0.)

    @property
    def max_radius(self) -> float:
        if 'max_radius' not in self._dp.keys():
            m = 0.
            for a in self._atoms:
                m = max(m, a.radius)
            self._dp['max_radius'] = m

        return self._dp['max_radius']
