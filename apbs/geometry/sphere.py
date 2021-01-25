import numpy as np
from . import Surface, SurfacePoint


class Sphere:
    """
    Static class to hold geometric calculates on spheres.
    """

    def spherical_distribution(self, npoints: int) -> Surface:
        """
        Generates monte-carlo approximation of a sphere using npoints points.

        Shamelessly copied over from the vacc routine.
        """
        surface: Surface

        frac = npoints / 4.0
        ntheta = np.ceil(np.sqrt(np.pi * frac))
        dtheta = np.pi / float(ntheta)
        nphimax = 2 * ntheta

        # Count number of points to be used
        nactual: int = 0
        for idx in range(ntheta):
            theta = dtheta * float(idx)
            sintheta = np.sin(theta)
            costheta = np.cos(theta)
            nphi = np.ceil(sintheta * nphimax)
            nactual += nphi

        surface = Surface(1.0, nactual)
        nactual = 0
        for idx in range(ntheta):
            theta = dtheta * float(idx)
            sintheta = np.sin(theta)
            costheta = np.cos(theta)
            nphi = np.ceil(sintheta * nphimax)
            if nphi != 0:
                dphi = 2 * np.pi / float(nphi)
                for jdx in range(nphi):
                    phi = dphi * float(jdx)
                    sinphi = np.sin(phi)
                    cosphi = np.cos(phi)
                    surface[nactual] = SurfacePoint(
                        cosphi * sintheta,
                        sinphi * sintheta,
                        costheta,
                        is_on_surf=True,
                    )
                    nactual += 1

        surface.npoints = nactual
        return surface
