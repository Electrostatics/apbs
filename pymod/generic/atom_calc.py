import math
from . import Atom
from . import AtomList
from . import Surface
from . import Constants

class AtomCalc:

    def __init__(self):
        ...

    @staticmethod
    def atom_surface(atom: Atom, ref: Surface, prad: float) -> Surface:
        '''Create a new surface from the points that do fall on the reference
        surface.

        :param: prad Probe radius
        '''

        arad = atom.radius
        apos = atom.position
        atomID = atom.id
        surf: Surface

        if arad < Constants.very_small_eps:
            return Surface(prad, 0)

        rad = arad + prad

        # Possibly merge these two loops?
        npoints = 0
        pos = Coordinate()
        for i in range(ref.npoints):
            pos.x = rad(ref.xs[i]) + apos.x
            pos.y = rad(ref.ys[i]) + apos.y
            pos.z = rad(ref.zs[i]) + apos.z

            # need to implement
            if ivdwAccExclus(pos, prad, atomID):
                npoints += 1
                ref.is_on_surf[i] = True
            else:
                ref.is_on_surf[i] = False

        surf = Surface(prad, npoints)
        for i in range(ref.npoints):
            if ref.coords[i].is_on_surf:
                surf.coords.append((rad * ref.coords[i] + apos))
                surf.coords[-1].is_on_surf = True

        surf.area = 4. * math.pi * rad * rad * \
                float(surf.npoints) / float(ref.npoints)

        return surf
