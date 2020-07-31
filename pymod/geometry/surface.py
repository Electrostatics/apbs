from typing import TypeVar, List, Dict
from . import SurfacePoint

class Surface:
    '''
    Attributes:
        coords: list of SurfacePoints which tracks various coordinates and
            whether they fall on the surface or not.
    '''

    def __init__(self, probe_radius: float, npoints: int):
        self.probe_radius = probe_radius
        self.npoints = npoints
        self.coords: List[SurfacePoint] = []
        self._dp: Dict[str, float] = dict()

    @property
    def area(self) -> float:
        '''Lazily calculate the area of the surface'''

        #TODO: figure out how this is calculated
        # or if it is a straight attribute
        if 'area' not in self._dp.keys():
            self._dp['area'] = -1.

        return self._dp['area']

