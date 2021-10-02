import numpy as np
import sys  # noqa

from apbs.geometry import (
    Coordinate,
    Surface,
    Constants,
)

from . import (
    Atom,
    AtomList,
    CellList,
)


class AtomComplexCalc:
    """Port of Vacc."""

    def __init__(
        self, alist: AtomList, clist: CellList, surface_density: float
    ):
        """
        .. note:: This is not just a port of the ctor for Vacc, but also
        replaces Vacc_storeParms. Vacc_storeParms seems to be simply an
        extension to the constructor, so it's job is replicated here.

        If it is found that Vacc_storeParms is not just an extension for the
        constructor, it should be moved to another method.
        """
        self.clist = clist
        self.alist = alist
        self.surface_density = surface_density
        max_radius = alist.max_radius + clist.max_radius
        max_area = 4.0 * (max_radius ** 2) * np.pi
        nsphere = np.ceil(max_area * surface_density)  # noqa: F841

        # TODO: calculate reference shpere (see VaccSurf_refSphere)

    @property
    def stride(self) -> Coordinate:
        return self.clist.stride

    def accessible_outside_inflated_vdw_radius(
        self, center: Coordinate, radius: float, atom_id_to_ignore: int
    ) -> bool:
        """
        Determines if a point is within the union of the spheres centered
        at the atomic centers with radii equal to the sum of their van der
        Waals radii and the probe radius.  Does not include contributions
        from the specified atom.

        :returns: None if not found

        .. note:: port of Vacc::ivdwAccExclus
        """
        if radius > self.clist.max_radius:
            raise RuntimeError(
                f"Got radius {radius} greater than max radius "
                f"{self.clist.max_radius} from cell list."
            )

        c = (center - self.lower_corner) / self.stride

        # Get cell and ensure exists in the cell list
        for i in range(3):
            if c[i] < 0 or c[i] >= self.npoints[i]:
                # Could not find the cell within the constraints of the cell
                # list
                return False

        for atom in self.clist:
            if atom.id == atom_id_to_ignore:
                continue

            if atom.euclidian_dist(center) > (atom.radius + radius) ** 2:
                return True

        return False

    def atom_surface(self, atom: Atom, ref: Surface, prad: float) -> Surface:
        """Create a new surface from the points that do fall on the reference
        surface.

        :param Atom atom: Atom from which surface will be constructed.
        :param Surface ref: The reference surface.
        :param float prad: The previous radius
        :return: Returns surface generated from the atom.
        :rtype: Surface

        .. note:: Although this seems like a candidate for a static method, the
        `accessable_outside_inflated_venderwalls_rad` method of this class
        *is* called from this function, and therefore must be a regular method.

        """

        arad = atom.radius
        apos = atom.position
        atom_id = atom.id
        surf: Surface

        if arad < Constants.very_small_eps:
            return Surface(prad, 0)

        rad = arad + prad

        # Possibly merge these two loops?
        npoints = 0
        pos = Coordinate()
        for idx in range(ref.npoints):
            pos.x = rad(ref.xs[idx]) + apos.x
            pos.y = rad(ref.ys[idx]) + apos.y
            pos.z = rad(ref.zs[idx]) + apos.z

            # need to implement
            if self.accessible_outside_inflated_vdw_radius(pos, prad, atom_id):
                npoints += 1
                ref.is_on_surf[idx] = True
            else:
                ref.is_on_surf[idx] = False

        surf = Surface(prad, npoints)
        for idx in range(ref.npoints):
            if ref.coords[idx].is_on_surf:
                surf.coords.append((rad * ref.coords[idx] + apos))
                surf.coords[-1].is_on_surf = True

        surf.area = (
            4.0 * np.pi * rad * rad * float(surf.npoints) / float(ref.npoints)
        )

        return surf
