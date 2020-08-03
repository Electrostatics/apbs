from typing import List, Dict
from . import SurfacePoint


class Surface:
    """
    Attributes:
        coords: list of SurfacePoints which tracks various coordinates and
            whether they fall on the surface or not.
    """

    def __init__(self, probe_radius: float, npoints: int):
        self.probe_radius = probe_radius
        self.npoints = npoints
        self.coords: List[SurfacePoint] = [
            SurfacePoint() for _ in range(npoints)
        ]
        self._dp: Dict[str, float] = dict()

    def __getitem__(self, idx: int) -> SurfacePoint:
        if idx >= self.npoints or idx < -self.npoints:
            raise IndexError("Requested surface point does not exists.")
        return self.coords[idx]

    def __setitem__(self, idx: int, other: SurfacePoint) -> None:
        if idx >= self.npoints or idx < -self.npoints:
            raise IndexError("Requested surface point does not exists.")
        self.coords[idx] = other

    @property
    def area(self) -> float:
        """Lazily calculate the area of the surface"""

        # TODO: figure out how this is calculated
        # or if it is a straight attribute
        if "area" not in self._dp.keys():
            self._dp["area"] = -1.0

        return self._dp["area"]
