import math
from . import (
        Coordinate,
        Atom,
        AtomList,
        Surface,
        Constants,
        CellList,
    )

class AtomComplexCalc:
    '''
    Port of Vacc
    '''

    def __init__(self):
        self.clist = CellList()

    @property
    def stride(self) -> Coordinate:
        return self.clist.stride

    def accessable_outside_inflated_venderwalls_rad(self, center: Coordinate,
            radius: float, atom_id_to_ignore: int) -> bool:
        '''
        Determines if a point is within the union of the spheres centered
        at the atomic centers with radii equal to the sum of their van der
        Waals radii and the probe radius.  Does not include contributions
        from the specified atom.

        :returns: None if not found

        Note: port of Vacc::ivdwAccExclus
        '''
        if radius > self.clist.max_radius:
            raise RuntimeError(f'Got radius %f greater than max radius %f from'
                    ' cell list.' % (radius, self.clist.max_radius))

        c = (pos - self.lower_corner) / self.stride

        # Get cell and ensure exists in the cell list
        for i in range(3):
            if c[i] < 0 or c[i] >= self.npoints[i]:
                # Could not find the cell within the constraints of the cell
                # list
                return False

        for atom in self.clist:
            if atom.id == atom_id_to_ignore:
                continue

            if atom.euclidian_dist(center) > (atom.radius + radius)**2:
                return True

        return False

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
