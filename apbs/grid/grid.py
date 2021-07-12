from typing import List, Optional
from apbs.geometry import Coordinate, Constants
import numpy as np


class CurvatureFlag:
    """Enum class to replace curvature flags in original source"""

    ReducedMaximalCurvature = 0
    MeanCurvature = 1
    GaussCurvature = 2
    TrueMaximalCurvature = 3


class Grid:
    """
    Pulled over from src/mg/vgrid.(h|c)

    Attributes:
        dims       : Number of grid points in a given direction.
                     Previously nx, ny, nz.
        spaces     : Grid spacing in a given direction.
                     Previously hx, hy, hz.
        mins       : Minimums in a given direction.
                     Previously xmin, ymin, zmin.
        maxs       : Maximums in a given direction.
                     Previously xmax, ymax, zmax.
        data       : nx*ny*nz array of data
    """

    def __init__(self, dims, spaces, mins, maxs, data: Optional[List[float]]):
        """Grid constructor

        Parameters
            dims:   Initializes member variable with the same name.
            spaces: Initializes member variable with the same name.
            mins:   Initializes member variable with the same name.
            maxs:   Initializes member variable with the same name.
            data:   Optionally sets the data for the grid
                    (may leave None if will be set later)
        """
        self.dims: Coordinate[int] = dims
        self.spaces: Coordinate[int] = spaces
        self.mins: Coordinate[int] = mins
        self.maxs: Coordinate[int] = maxs
        self.data: Optional[List[float]] = data

    def value(self, pt: Coordinate[float]) -> float:
        """Get potential value (from mesh or approximation) at a point

        :note: Previously returned by pointer, using return code as an error
               code.
                This has been replaced by returning the value and raising an
                exception on error.

        :param   x    : Coordinate at which to evaluate potential
        :returns      : value of grid
        """

        if self.data is None:
            raise RuntimeError("No data available.")

        ret_value = float(0)

        tmp = Coordinate(
            (pt.x - self.mins.x) / self.spaces.x,
            (pt.y - self.mins.y) / self.spaces.y,
            (pt.z - self.mins.z) / self.spaces.z,
        )

        hi = Coordinate(
            int(np.ceil(tmp.x)), int(np.ceil(tmp.y)), int(np.ceil(tmp.z))
        )
        lo = Coordinate(
            int(np.floor(tmp.x)),
            int(np.floor(tmp.y)),
            int(np.floor(tmp.z)),
        )

        hi.x = (
            self.dims.x - 1
            if abs(pt.x - self.maxs.x) < Constants.epsilon
            else hi.x
        )
        hi.y = (
            self.dims.y - 1
            if abs(pt.y - self.maxs.y) < Constants.epsilon
            else hi.y
        )
        hi.z = (
            self.dims.z - 1
            if abs(pt.z - self.maxs.z) < Constants.epsilon
            else hi.z
        )

        lo.x = 0 if abs(pt.x - self.mins.x) < Constants.eps else lo.x
        lo.y = 0 if abs(pt.y - self.mins.y) < Constants.eps else lo.y
        lo.z = 0 if abs(pt.z - self.mins.z) < Constants.eps else lo.z

        if hi < self.dims:
            dx, dy, dz = tmp.x - lo.x, tmp.y - lo.y, tmp.z - lo.z
            ret_value = [
                float(dx * dy * dz * self.data[hi.x, hi.y, hi.z]),
                float(dx * (1.0 - dy) * dz * self.data[hi.x, lo.y, hi.z]),
                float(dx * dy * (1.0 - dz) * self.data[hi.x, hi.y, lo.z]),
                float(
                    dx * (1.0 - dy) * (1.0 - dz) * self.data[hi.x, lo.y, lo.z]
                ),
                float((1.0 - dx) * dy * dz * self.data[lo.x, hi.y, hi.z]),
                float(
                    (1.0 - dx) * (1.0 - dy) * dz * self.data[lo.x, lo.y, hi.z]
                ),
                float(
                    (1.0 - dx) * dy * (1.0 - dz) * self.data[lo.x, hi.y, lo.z]
                ),
                float(
                    (1.0 - dx)
                    * (1.0 - dy)
                    * (1.0 - dz)
                    * self.data[lo.x, lo.y, lo.z]
                ),
            ]

            ret_value = sum(ret_value)

            if ret_value == np.nan:
                # TODO: Add a more descriptive error
                raise RuntimeError(
                    "Value routine failed to converge with the following "
                    "coordinates:\n"
                    f"\tLow: {lo}\n"
                    f"\tHigh: {hi}\n"
                    f"\tCoordinate: {pt}\n"
                )

        return ret_value

    def curvature(self, pt: Coordinate[float], cflag: CurvatureFlag):
        """Get second derivative values at a point

        :param   pt   : Location to evaluate second derivative
        :param   cflag: Curvature method
        :param   curv : Specified curvature value
        :returns      : 1 if successful, 0 if off grid
        """
        ...

    def gradient(self, pt: Coordinate[float], grad: List[float]):
        """Get first derivative values at a point

        :param   pt  : Location to evaluate gradient
        :param   grad: Gradient
        :returns     : 1 if successful, 0 if off grid
        """
        ...

    def integrate(self) -> float:
        """Get the integral of the data"""
        ...

    def norml1(self) -> float:
        r"""Get the \f$L_1\f$ norm of the data.  This returns the integral:
        \f[ \| u \|_{L_1} = \int_\Omega | u(x) | dx  \f]
        """
        ...

    def norml2(self) -> float:
        r"""Computes the \f$L_2\f$ norm of the data."""
        ...

    def norml_inf(self) -> float:
        r"""Computes the \f$L_\infty\f$ norm of the data."""
        ...

    def seminormH1(self) -> float:
        r"""Get the \f$H_1\f$ semi-norm of the data.
        This returns the integral:
          \f[ | u |_{H_1} = \left( \int_\Omega |\nabla u(x)|^2 dx \right)^{1/2} \f]  # noqa: E501
        """
        ...

    def normH1(self) -> float:
        r"""Integral of data
        Get the \f$H_1\f$ norm (or energy norm) of the data.
        This returns the integral:
          \f[ \| u \|_{H_1} = \left( \int_\Omega |\nabla u(x)|^2 dx
                            +        \int_\Omega |u(x)|^2 dx \right)^{1/2} \f]
        """
        ...

    def read_dx(self, fn: str) -> None:
        lines = open(fn, "r").readlines()
        self.data = []
        for line in lines:
            lline = line.lower().strip()

            # skip comments
            if lline[0] == "#":
                continue
            fields = lline.split(" ")

            # TODO: figure out what all these options mean
            if fields[0] in ("object", "attribute", "component"):
                continue

            assert (
                len(fields) == 3
            ), "Found an unknown option or found line with values != 3"

            self.data.append(
                [float(fields[0]), float(fields[1]), float(fields[2])]
            )
