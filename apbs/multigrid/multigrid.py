from apbs.geometry import Coordinate

"""
/** @brief   Set partition information which restricts the calculation of
 *           observables to a (rectangular) subset of the problem domain
 *  @ingroup Vpmg
 *  @author  Nathan Baker
 */
VEXTERNC void Vpmg_setPart(
        Vpmg *thee,  /**< Vpmg object */
        double lowerCorner[3],  /**< Partition lower corner */
        double upperCorner[3],  /**< Partition upper corner */
        int bflags[6]  /**< Booleans indicating whether a particular processor
                         is on the boundary with another partition.  0 if the
                         face is not bounded (next to) another partition, and
                         1 otherwise. */
        );
"""


class MultiGrid:
    def set_part(
        self, lower_corner: Coordinate, upper_corner: Coordinate
    ) -> None:
        """
        :param lower_corner: The X,Y,Z coordinates in angstroms
        :type lower_corner: Coordinate
        :param upper_corner: The X,Y,Z coordinates in angstroms
        :type upper_corner: Coordinate
        """
        # TODO: This needs to be implemented
        pass
