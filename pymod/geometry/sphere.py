import math
from . import Surface


class Sphere:
    '''
    Static class to hold geometric calculates on spheres.
    '''

    def spherical_distribution(npoints: int) -> Surface:
        '''
        Generates monte-carlo approximation of a sphere using npoints points.

        Shamelessly copied over from the vacc routine.
        '''
        s: Surface

        frac = npoints / 4.
        ntheta = math.ceil(math.sqrt(math.pi * frac))
        dtheta = math.pi / float(ntheta)
        nphimax = 2 * ntheta

        # Count number of points to be used
        nactual: int = 0
        for i in range(ntheta):
            theta = dtheta * float(i)
            sintheta = math.sin(theta)
            costheta = math.cos(theta)
            nphi = math.ceil(sintheta * nphimax)
            nactual += nphi

        s = Surface(1., nactual)
        nactual = 0
        for i in range(ntheta):
            theta = dtheta * float(i)
            sintheta = math.sin(theta)
            costheta = math.cos(theta)
            nphi = math.ceil(sintheta * nphimax)
            if nphi != 0:
                dphi = 2 * math.pi / float(nphi)
                for j in range(nphi):
                    phi = dphi * float(j)
                    sinphi = math.sin(phi)
                    cosphi = math.cos(phi)
                    s[nactual] = SurfacePoint(
                        cosphi * sintheta,
                        sinphi * sintheta,
                        costheta,
                        is_on_surf=True
                    )
                    nactual += 1

        s.npoints = nactual
        return s
