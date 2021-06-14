"""Parameters for a finite-difference polar solvation calculation."""
import logging
from math import log2
from .. import check
from .. import InputFile

_LOGGER = logging.getLogger(__name__)
MIN_LEVEL = 4
"""Mininum multigrid level in calculations."""


class GridDimensions(InputFile):
    """Parameters for controlling the number of grid points and the grid
    spacing.

    For a given direction :math:`i \\in [x, y, z]`, the grid spacing :math:`h_i`
    is related to the number of grid points :math:`n_i` and the grid length
    :math:`l_i` by

    .. math::

       l_i = h_i (n_i - 1)

    Therefore, only two of the following three properties are needed.  When all
    three are provided, the :func:`spacings` will be inferred from the 
    :func:`counts` and :func:`lengths`.

    * ``counts``:  the number of grid points in each direction; see
      :func:`counts` for more information.
    * ``spacings``:  the spacing of grid points in each direction; see
      :func:`spacings` for more information.
    * ``lengths``:  the span of the grid in each dimension; see :func:`lengths`
      for more information.

    The following properties are read-only and cannot be set:

    * ``levels``:  the number of multigrid levels in the calculation; see
      :func:`levels` for more information.

    The number of levels :math:`p` is a constant for the entire domain (i.e.,
    it doesn't vary by grid direction.  The number of levels is related to the
    number of grid points :math:`n_i` in the direction :math:`i` by the formula

    .. math::

       n_i = c \\, 2^{p} + 1

    where :math:`c` is a non-zero integer.  The value of :math:`p` is chosen as
    the smallest that satisfies the formula above for all :math:`n_i`.
    Calculations become more efficient for larger :math:`p` so APBS will adjust
    the numbers of grid points :math:`n_i` *downwards* to ensure that :math:`p`
    is equal to or greater than :const:`MIN_LEVEL`, likely resulting in lower
    grid resolution (and accuracy).

    The most common values for :math:`n_i` are 65, 97, 129, and 161 (they can
    be different in each direction); these are all compatible with
    :math:`p = 4`.
    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._counts = None
        self._spacings = None
        self._lengths = None
        self._levels = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def from_dict(self, input_):
        """Initialize object from dictionary."""
        counts = input_.get("counts", None)
        if counts is not None:
            self.counts = counts
        lengths = input_.get("lengths", None)
        if lengths is not None:
            self.lengths = lengths
        spacings = input_.get("spacings", None)
        if spacings is not None:
            self.spacings = spacings

    def to_dict(self):
        return {
            "counts":  self._counts,
            "lengths":  self._lengths,
            "spacings":  self._spacings
        }

    def validate(self):
        _ = self.counts
        _ = self.lengths
        _ = self.spacings
        _ = self.levels

    @property
    def levels(self) -> int:
        """Number of multigrid levels, an integer greater than 2.

        :raises TypeError:  if not an integer
        :raises ValueError:  if not greater than 2
        """
        return self._levels

    @staticmethod
    def find_count(target, min_level=MIN_LEVEL, max_level=None) -> tuple:
        """Find the number of grid points closest to the given target that
        satisfies

        .. math::

           n_i = \\arg \\max_p c \\, 2^p + 1

        where :math:`n_i` is the number of grid points (always less than or
        equal to target), :math:`c` is a positive integer constant, and
        :math:`p` is the number of levels, greater than or equal to
        :const:`MIN_LEVEL`.

        .. note::  assumes ``target`` is greater than or equal to 
            2 ** :const:`MIN_LEVEL`

        :param int target:  target number of grid points
        :param int max_level:  maximum level for multigrid
        :param int min_level:  minimum level for multigrid
        :returns:  (:math:`n_i`, :math:`c`, :math:`p`)
        """
        if max_level is None:
            max_level = int(log2(target))
        _LOGGER.debug(f"Finding count for target = {target}, min_level = {min_level}, max_level = {max_level}.")
        best_n = 2 ** min_level + 1
        best_c = 1
        best_p = min_level
        for level in range(min_level, max_level + 1):
            for const in range(1, target, 2):
                n = const * 2 ** level + 1
                if n > target:
                    break
                if (target - n) < (target - best_n):
                    best_n = n
                    best_c = const
                    best_p = level
        _LOGGER.debug((best_n, best_c, best_p))
        return (best_n, best_c, best_p)

    def adjust_counts(self) -> bool:
        """Adjust numbers of grid points to acheive multigrid level of at least
        :const:`MIN_LEVEL`.

        This routine alters self._counts and (re)sets self._level.  The new
        counts will always be less than or equal to the old counts.

        :param list targets:  list of target numbers of grid points.
        :returns:  True if counts were changes from targets
        """
        min_level = None
        max_level = None
        for target in self._counts:
            _LOGGER.debug(f"Adjusting target {target}.")
            _, _, level = self.find_count(target)
            _LOGGER.debug(f"Got level {level}.")
            if min_level is None:
                min_level = level
            min_level = min(level, min_level)
            if max_level is None:
                max_level = level
            max_level = max(level, max_level)
        self._levels = min_level
        for i, target in enumerate(self._counts):
            self._counts[i], _, _ = self.find_count(
                target, min_level=min_level, max_level=max_level
            )

    @property
    def counts(self) -> list:
        """Target number of grid points in a finite-difference
        Poisson-Boltzmann calculation.

        .. note::  the actual number of grid points might be adjusted to
           achieve a better multigrid level.  The most common values for grid
           dimensions are 65, 97, 129, and 161 (they can be different in each
           direction); these are all compatible with a :func:`levels` value of
           4.

        :returns: 3-vector of integers greater than 32 indicating number of
          grid points in the x-, y-, and z-directions. The numbers may be
          different in each direction.
        :raises IndexError:  if the length of the value is not 3.
        :raises TypeError:  if the counts are not integers greater than
            2**``MIN_LEVEL``.
        :raises ValueError:  if not enough information is present to calculate
            counts
        """
        if self._counts is None:
            if (self._lengths is not None) and (self._spacings is not None):
                counts = []
                for i in range(3):
                    count = int(self._lengths[i] / self._spacings[i] + 0.5) + 1
                    counts.append(count)
                self.counts = counts
            else:
                raise ValueError(
                    f"Can't return counts because either lengths "
                    f"({self._lengths}) or spacings ({self._spacings}) is None."
                )
        return self._counts

    @counts.setter
    def counts(self, value):
        if len(value) != 3:
            raise IndexError(f"Counts are {len(value)} rather than 3.")
        self._counts = []
        for i, elem in enumerate(value):
            if (elem < 2 ** MIN_LEVEL) or not isinstance(elem, int):
                self._counts = None
                raise TypeError(
                    f"Vector {value} does not contain positive integers."
                )
            self._counts.append(elem)
        _LOGGER.debug(f"Target grid nunmbers are {self._counts}.")
        self.adjust_counts()
        _LOGGER.debug(
            f"Actual grid number are {self._counts} with level {self.levels}."
        )

    @property
    def spacings(self) -> list:
        """Spacings of the grid in a finite-difference Poisson-Boltzmann
        calculation.

        :returns: 3-vector of positive numbers indicating grid spacings in the
          x-, y-, and z-directions in Å. The spacings may be different in each
          direction.
        :raises IndexError:  if the length of the value is not 3.
        :raises TypeError:  if the spacings are not positive numbers.
        :raises ValueError:  if not enough information is available to calculate
            spacings
        """
        if self._spacings is None:
            if (self._lengths is not None) and (self._counts is not None):
                spacings = []
                for i in range(3):
                    space = self._lengths[i] / (self._counts[i] - 1)
                    spacings.append(space)
                return spacings
            else:
                raise ValueError(
                    f"Cannot calculate spacings because either lengths "
                    f"{self._lengths} or counts {self._counts} is None."
                )
        return self._spacings

    @spacings.setter
    def spacings(self, value):
        if len(value) != 3:
            raise IndexError(f"Spacings are {len(value)} rather than 3.")
        self._spacings = []
        for i, elem in enumerate(value):
            if not check.is_positive_definite(elem):
                raise TypeError(
                    f"Vector {value} does not contain positive numbers"
                )
            self._spacings.append(elem)

    @property
    def lengths(self) -> list:
        """Length of the grid in a finite-difference Poisson-Boltzmann
        calculation.

        :returns: 3-vector of positive numbers indicating grid lengths in the
          x-, y-, and z-directions in Å. The lengths may be different in each
          direction.
        :raises IndexError:  if the length of the value is not 3.
        :raises TypeError:  if the lengths are not positive numbers.
        :raises ValueError:  if not enough information is available to calculate
            lengths
        """
        if self._lengths is None:
            if (self._spacings is not None) and (self._counts is not None):
                lengths = []
                for i in range(3):
                    length = self._spacings[i] * (self._counts[i] - 1)
                    lengths.append(length)
                return lengths
            else:
                raise ValueError(
                    f"Cannot calculate lengths because spacings "
                    f"{self._spacings} or counts {self._counts} is None."
                )
        return self._lengths

    @lengths.setter
    def lengths(self, value):
        if len(value) != 3:
            raise IndexError(f"Length is {len(value)} rather than 3.")
        self._lengths = []
        for elem in value:
            if not check.is_positive_definite(elem):
                raise TypeError(
                    f"Vector {value} does not contain positive numbers"
                )
            self._lengths.append(elem)


class GridCenter(InputFile):
    """Parameters for specifying the center of a finite difference grid.

    Objects can be initialized with dictionary/JSON/YAML data with one of the
    following keys:

    * ``molecule``:  use the center of the specified molecule.  See
      :func:`molecule`.
    
    * ``position``:  use a specific position (coordinates).  See
      :func:`position`.

    """

    def __init__(self, dict_, yaml, json):
        self._molecule = None
        self._position = [None, None, None]
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def molecule(self) -> str:
        """Alias for molecule used to center the finite difference grid.  See
        :ref:`read_input_file` for more information on reading molecules into
        APBS.

        :returns:  alias for molecule at center
        :raises TypeError:  if alias is not string
        """
        return self._molecule

    @molecule.setter
    def molecule(self, value):
        if check.is_string(value):
            self._molecule = value
        else:
            raise TypeError(f"{value} (type {type(value)}) is not a string.")

    @property
    def position(self) -> list:
        """Coordinates for the grid center.

        :returns: 3-element list of :class:`float` positions in Å units
        :raises TypeError: if input is not a list of floats
        :raises IndexError:  if input is not length 3
        """
        return self._position

    @position.setter
    def position(self, value):
        if len(value) != 3:
            raise IndexError(f"Length of list {value} is not 3.")
        for i, elem in enumerate(value):
            if not check.is_number(elem):
                raise TypeError(f"{elem} is not a number.")
            self._position[i] = elem


class Manual(InputFile):
    """Parameters specific to a manual finite-difference polar solvation
    Poisson-Boltzmann calculation.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    .. todo:: finish this
    """

    def __init__(self, dict_, yaml, json):
        super().__init__(dict_=dict_, yaml=yaml, json=json)


class ParallelFocus(InputFile):
    """Parameters specific to a parallel focusing finite-difference polar
    solvation Poisson-Boltzmann calculation.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``overlap fraction``:  overlap between parallel focusing subdomains; see
      :func:`overlap_fraction`.

    * ``processor array``:  array of processors; see :func:`processor_array`

    * ``asynchronous rank`` (optional):  rank of processor to behave as; see
      :func:`asynchronous_rank`

    .. todo:: finish this
    """

    def __init__(self, dict_, yaml, json):
        self._overlap_fraction = None
        self._processor_array = [None, None, None]
        self._asynchronous_rank = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def processor_array(self) -> list:
        """Distribution of processors over problem domain.

        :returns: a 3-vector of positive integers: ``[npx npy npz]`` the
          integer number of processors to be used in the x-, y- and
          z-directions of the system. The product ``npx × npy × npz`` should be
          less than or equal to the total number of processors with which APBS
          was invoked (usually via mpirun). If more processors are provided at
          invocation than actually used during the run, the extra processors
          are not used in the calculation. The processors are tiled across the
          domain in a Cartesian fashion with a specified amount of overlap (see
          :func:`overlap_fraction`) between each processor to ensure continuity
          of the solution. Each processor's subdomain will contain the number
          of grid points specified by the dime keyword. For broad spatial
          support of the splines, every charge included in partition needs to
          be at least 1 grid space (first-order spline), 2 grid spaces
          (third-order spline), or 3 grid spaces (fifth-order spline) away from
          the partition boundary.

        :raise TypeError:  if not a list or list of positive integers.
        :raise IndexError:  if vector doesn't have length 3.
        """
        return self._processor_array

    @property
    def asynchronous_rank(self) -> int:
        """Integer rank of processor in processor array.

        This should be a positive integer.

        :raises TypeError:  if not a positive integer
        """
        return self._asynchronous_rank

    @asynchronous_rank.setter
    def asynchronous_rank(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        if not isinstance(value, int):
            raise TypeError(f"Value {value} is not a positive integer.")
        self._asynchronous_rank = value

    @processor_array.setter
    def processor_array(self, value):
        if len(value) != 3:
            raise IndexError(
                f"Processor array has length {len(value)} instead of length 3."
            )
        for i, elem in enumerate(value):
            if not check.is_positive_definite(elem):
                raise TypeError(
                    f"Processor array {value} does not contain positive numbers."
                )
            if not isinstance(elem, int):
                raise TypeError(
                    f"Processor array {value} does not contain integers."
                )
            self._processor_array[i] = elem

    @property
    def overlap_fraction(self) -> float:
        """Fraction of size of parallel focusing domains that overlap.

        This is a positive floating point number less than 1.

        :raises TypeError:  if the value is not a positive number
        :raises ValueError:  if the value is greater than 1
        """
        return self._overlap_fraction

    @overlap_fraction.setter
    def overlap_fraction(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(
                f"Overlap fraction (type {type(value)}) is not a positive number."
            )
        if value > 1.0:
            raise ValueError(f"Overlap fraction {value} is greater than 1.")
        self._overlap_fraction = value


class Focus(InputFile):
    """Parameters specific to a focused-grid finite-difference polar solvation
    Poisson-Boltzmann calculation.

    Focusing provides automatically configured finite difference
    Poisson-Boltzmann calculations.

    This multigrid calculation automatically sets up and performs a string of
    single-point PBE calculations to "focus" on a region of interest
    (binding site, etc.) in a system. It is basically an automated way to set
    parameters in :class:`Manual`, allowing for (hopefully) easier use. Most
    users should use this :class:`Focus` rather than :class:`Manual`.

    Focusing is a method for solving the Poisson-Boltzmann equation in a finite
    difference setting. Some of the earliest references to this method are from
    Gilson and Honig [#Gilson]_. The method starts by solving the equation on a
    coarse grid (i.e., few grid points) with large dimensions (i.e., grid
    lengths). The solution on this coarse grid is then used to set the
    Dirichlet boundary condition values for a smaller problem domain -- and
    therefore a finer grid -- surrounding the region of interest. The finer
    grid spacing in the smaller problem domain often provides greater accuracy
    in the solution.

    .. note::

       During focusing calculations, you may encounter the message ``WARNING!
       Unusually large potential values detected on the focusing boundary!``
       for some highly charged systems based on location of the focusing
       boundary. First, you should determine if you received any other warning
       or error messages as part of this calculation, particularly those
       referring to exceeded number of iterations or error tolerance. Next, you
       should check if the calculation converged to a reasonable answer. In
       particular, you should check sensitivity to the grid spacing by making
       small changes to the grid lengths and see if the changes in energies are
       correspondingly small. If so, then this warning can be safely ignored.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``coarse grid center``:  center of the coarse grid, see
      :func:`coarse_grid_center`

    * ``coarse grid dimensions``:  dimensions of the coarse grid, see
      :func:`coarse_grid_dimensions`

    * ``fine grid center``:  center of the fine grid, see
      :func:`fine_grid_center`

    * ``fine grid dimensions``:  dimensions of the fine grid, see
      :func:`fine_grid_dimensions`

    * ``parallel``:  a Boolean, indicating whether a parallel calculation 
      should be performed; see :func:`parallel`.  If this value is true, the
      ``parallel parameters`` object should be included in the input file; see
      :func:`parallel_parameters`.

    * ``parallel parameters``:  *optional* information for configuring a
      parallel focusing run; *required* if :func:`parallel` is True. See
      :func:`parallel_parameters` and :class:`ParallelFocus` for more 
      information.

    .. todo:: finish this

    .. [#Gilson] Gilson MK and Honig BH, Calculation of electrostatic
       potentials in an enzyme active site. Nature, 1987. 330(6143): p. 84-6.
       DOI:`10.1038/330084a0 <http://dx.doi.org/10.1038/330084a0>`_
    """

    def __init__(self, dict_, yaml, json):
        self._fine_grid_center = None
        self._fine_grid_dimensions = None
        self._coarse_grid_center = None
        self._coarse_grid_dimesons = None
        self._parallel = None
        self._parallel_parameters = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def parallel(self) -> bool:
        """Indicate whether a parallel calculation should be performed.

        If set, the :func:`parallel_parameters` property must also be set.
        Set the :func:`parallel_parameters` property before setting this value
        to True (sorry).

        :raises TypeError:  if set to something other than :class:`bool`.
        :raises ValueError:  if set to True and :func:`parallel_parameters`
          hasn't been set (sorry).
        """
        return self._parallel

    @parallel.setter
    def parallel(self, value):
        if not check.is_bool(value):
            raise TypeError(f"Value {value} is not a Boolean.")
        if value and self._parallel_parameters is None:
            raise ValueError(
                "The 'parallel parameters' object has not been provided."
            )
        self._parallel = value

    @property
    def parallel_parameters(self) -> ParallelFocus:
        """Provide parameters for a parallel focusing calculation.

        This property is optional and only used if :func:`parallel` is True.

        :raises TypeError:  if not :class:`ParallelFocus` class
        """
        return self._parallel_parameters

    @parallel_parameters.setter
    def parallel_parameters(self, value):
        if not isinstance(value, ParallelFocus):
            raise TypeError(
                f"Value is type {type(value)} rather than ParallelFocus."
            )
        self._parallel_parameters = value

    @property
    def coarse_grid_dimensions(self) -> GridDimensions:
        """Dimensions of the coarse grid in a focusing finite-difference
        Poisson-Boltzmann calculation.

        This is the starting mesh, so it should be large enough to completely
        enclose the biomolecule and ensure that the chosen boundary condition
        is appropriate.

        :raises TypeError:  if the value is not :class:`GridDimensions`.
        """
        return self._coarse_grid_dimensions

    @coarse_grid_dimensions.setter
    def coarse_grid_length(self, value):
        if not isinstance(value, GridDimensions):
            raise TypeError(
                f"Value {value} is type {type(value)} rather than GridDimensions."
            )
        self._coarse_grid_dimensions = value

    @property
    def coarse_grid_center(self) -> GridCenter:
        """The center of the coarse grid in a focusing calculation.

        :raises TypeError:  if not :class:`GridCenter` object
        """
        return self._coarse_grid_center

    @coarse_grid_center.setter
    def coarse_grid_center(self, value):
        if not isinstance(value, GridCenter):
            raise TypeError(
                f"{value} (type {type(value)}) is not GridCenter type."
            )
        self._coarse_grid_center = value

    @property
    def fine_grid_dimensions(self) -> GridDimensions:
        """Dimensions of the coarse grid in a focusing finite-difference
        Poisson-Boltzmann calculation.

        This should enclose the region of interest in the biomolecule.

        :raises TypeError:  if the value is not :class:`GridDimensions`.
        """
        return self._fine_grid_dimensions

    @fine_grid_dimensions.setter
    def fine_grid_dimensions(self, value):
        if not isinstance(value, GridDimensions):
            raise TypeError(
                f"Value {value} is type {type(value)} "
                f"rather than GridDimensions."
            )

    @property
    def fine_grid_center(self) -> GridCenter:
        """The center of the fine grid in a focusing calculation.

        :raises TypeError:  if not :class:`GridCenter` object
        """
        return self._fine_grid_center

    @fine_grid_center.setter
    def fine_grid_center(self, value):
        if not isinstance(value, GridCenter):
            raise TypeError(
                f"{value} (type {type(value)}) is not GridCenter type."
            )
        self._fine_grid_center = value


class FiniteDifference(InputFile):
    """Parameters for a finite-difference polar solvation Poisson-Boltzmann
    calculation.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``boundary condition``:  :func:`boundary_condition`
    * ``calculate energy``:  see :func:`calculate_energy`
    * ``calculate forces``:  see :func:`calculate_forces`
    * ``calculation type``:  see :func:`calculation_type`
    * ``calculation parameters``:  see :func:`calculation_parameters`
    * ``charge discretization``:  method used to map charges onto the grid; see
      :func:`charge_discretization`

    .. todo:: finish this
    """

    def __init__(self, dict_, yaml, json):
        self._boundary_condition = None
        self._calculate_energy = None
        self._calculate_forces = None
        self._calculation_type = None
        self._calculation_parameters = None
        self._charge_discretization = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    @property
    def charge_discretization(self) -> str:
        """The method by which the biomolecular point charges (i.e., Dirac
        delta functions) by which charges are mapped to the grid used for the
        finite difference calculation.

        As we are attempting to model delta functions, the support (domain) of
        these discretized charge distributions is always strongly dependent on
        the grid spacing.

        The following types of discretization are supported:

        * ``linear``: Traditional trilinear interpolation (linear splines). The
          charge is mapped onto the nearest-neighbor grid points. Resulting
          potentials are very sensitive to grid spacing, length, and position.

        * ``cubic``: Cubic B-spline discretization. The charge is mapped onto
          the nearest- and next-nearest-neighbor grid points. Resulting
          potentials are somewhat less sensitive (than ``linear``) to grid
          spacing, length, and position.

        * ``quintic``: Quintic B-spline discretization. Similar to ``cubic``,
          except the charge/multipole is additionally mapped to include
          next-next-nearest neighbors (125 grid points receive charge density).

        :raises TypeError:  if not set to string
        :raises ValueError:  if not one of the allowed values above
        """
        return self._charge_discretization

    @charge_discretization.setter
    def charge_discretization(self, value):
        if not check.is_string(value):
            raise TypeError(f"{value} (type {type(value)}) is not a string.")
        value = value.lower()
        if value not in ["linear", "cubic", "quintic"]:
            raise ValueError(f"{value} is not an allowed value.")
        self._charge_discretization = value

    @property
    def calculation_type(self) -> str:
        """Specify the type of finite difference calculation to perform:

        * ``focus``:  this uses multiple grids to generate high-resolution
          solutions at a region of interest.  See :class:`Focus` for more
          information.

        * ``manual``:  perform a traditional non-focused calculation.  See
          :class:`Manual` for more information.

        :raises ValueError:  if invalid calculation type specified
        """
        return self._calculation_type

    @calculation_type.setter
    def calculation_type(self, value):
        if value not in ["focus", "manual"]:
            raise ValueError(f"Unknown calculation type:  {value}.")
        self._calculation_type = value

    @property
    def calculation_parameters(self) -> InputFile:
        """Specify parameters specific to the calculation type.

        The specific class is specific to the calculation type (see
        :func:`calculation_type`):

        * ``focus``:  :class:`Focus`
        * ``manual``:  :class:`Manual`

        .. note::  The :func:`calculation_type` property must be set before
           setting this property (sorry...)

        :raises ValueError:  if calculation parameter class doesn't match
          calculation type or if 
        """
        return self._calculation_parameters

    @calculation_parameters.setter
    def calculation_parameters(self, value):
        if (
            (
                self._calculation_type == "focus"
                and not isinstance(value, Focus)
            )
            or (
                self._calculation_type == "manual"
                and not isinstance(value, Manual)
            )
            or (self._calculation_type is None)
        ):
            raise ValueError(
                f"Calculation type is {type(self._calculation_type)} but value type is {type(value)}."
            )
        self._calculation_parameters = value

    @property
    def boundary_condition(self) -> str:
        """Boundary condition for Poisson-Boltzmann equation.

        This property can have one of the following values:

        * ``zero``:  Dirichlet condition where the potential at the boundary is
          set to zero. This condition is not commonly used and can result in
          large errors if used inappropriately.

        * ``single sphere``:  Dirichlet condition where the potential at the
          boundary is set to the values prescribed by a Debye-Hückel model for a
          single sphere with a point charge, dipole, and quadrupole. The sphere
          radius in this model is set to the radius of the biomolecule and the
          sphere charge, dipole, and quadrupole are set to the total moments of
          the protein. This condition works best when the boundary is
          sufficiently far (multiple Debye lengths) from the biomolecule.

        * ``multiple sphere``:  Dirichlet condition where the potential at the
          boundary is set to the values prescribed by a Debye-Hückel model for
          multiple, non-interacting spheres with a point charges. The radii of
          the non-interacting spheres are set to the atomic radii of and the
          sphere charges are set to the atomic charges. This condition works
          better than ``single sphere`` for closer boundaries but can be very
          slow for large biomolecules.

        * ``focus`` :c:var:`alias`:  Dirichlet condition where the potential at
          the boundary is set to the values computed by a previous (usually
          lower-resolution) PB calculation with alias :c:var:`alias`. All of the
          boundary points should lie within the domain of the previous
          calculation for best accuracy; if any boundary points lie outside,
          their values are computed using the ``single sphere`` Debye-Hückel
          boundary condition (see above).

        * ``map`` :c:var:`alias`:  Dirichlet condition where the potential
          values are read from an external map with alias :c:var:`alias` read
          into APBS as described in :ref:`read_input_file`.

        :raises ValueError:  if set to an invalid boundary type
        :raise IndexError:  if an insufficient number of words are present
        """
        return self._boundary_condition

    @boundary_condition.setter
    def boundary_condition(self, value):
        words = value.split()
        if words[0] == "zero":
            self._boundary_condition = "zero"
        elif words[0] == "single" and words[1] == "sphere":
            self._boundary_condition = "single sphere"
        elif words[0] == "multiple" and words[1] == "sphere":
            self._boundary_condition = "multiple sphere"
        elif words[1] == "focus":
            self._boundary_condition = " ".join(words)
        elif words[1] == "map":
            self._boundary_condition = " ".join(words)
        else:
            raise ValueError(f"Unknown boundary condition: {value}.")

    @property
    def calculate_energy(self) -> bool:
        """Indicate whether energy should be calculated.

        :raises TypeError:  if not Boolean
        """
        return self._calculate_energy

    @calculate_energy.setter
    def calculate_energy(self, value):
        if check.is_bool(value):
            self._calculate_energy = value
        else:
            raise ValueError(f"{value} is not Boolean.")

    @property
    def calculate_forces(self) -> bool:
        """Indicate whether forces should be calculated.

        :raises TypeError:  if not Boolean
        """
        return self._calculate_forces

    @calculate_forces.setter
    def calculate_forces(self, value):
        if check.is_bool(value):
            self._calculate_forces = value
        else:
            raise ValueError(f"{value} is not Boolean.")
