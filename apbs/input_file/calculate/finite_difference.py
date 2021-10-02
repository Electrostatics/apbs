"""Parameters for a finite-difference polar solvation calculation."""
import logging
from math import log2
from .. import check
from .. import InputFile
from .generic import MobileIons, UseMap, WriteMap


_LOGGER = logging.getLogger(__name__)


MIN_LEVEL = 4
"""Mininum multigrid level in calculations."""


ERROR_TOLERANCE = 1e-6
"""Relative error tolerance for iterative solver."""


class GridDimensions(InputFile):
    """Parameters for controlling the number of grid points and the grid
    spacing.

    For a given direction :math:`i \\in [x, y, z]`, the grid spacing
    :math:`h_i` is related to the number of grid points :math:`n_i` and the
    grid length :math:`l_i` by

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
            "counts": self._counts,
            "lengths": self._lengths,
            "spacings": self._spacings,
        }

    def validate(self):
        errors = []
        if self.counts is None:
            errors.append("counts not set")
        if self.spacings is None:
            errors.append("spacings not set")
        if self.lengths is None:
            errors.append("errors not set")
        if self.levels is None:
            errors.append("levels not set")
        if errors:
            raise ValueError(" ".join(errors))

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
        _LOGGER.debug(
            f"Finding count for target = {target}, min_level = {min_level}, "
            "max_level = {max_level}."
        )
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
                    "Can't return counts because either lengths "
                    f"({self._lengths}) or spacings ({self._spacings}) is "
                    "None."
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
        :raises ValueError:  if not enough information is available to
            calculate spacings
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
        :raises ValueError:  if not enough information is available to
            calculate lengths
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

    def __init__(self, dict_=None, yaml=None, json=None):
        self._molecule = None
        self._position = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def from_dict(self, input_):
        molecule = input_.get("molecule", None)
        if molecule is not None:
            self.molecule = molecule
        position = input_.get("position", None)
        if position is not None:
            self.position = position

    def to_dict(self) -> dict:
        return {"molecule": self.molecule, "position": self.position}

    def validate(self):
        if self.molecule is None and self.position is None:
            raise ValueError("Need to specify either molecule or position.")

    @property
    def molecule(self) -> str:
        """Alias for molecule used to center the finite difference grid.  See
        :ref:`read_new_input` for more information on reading molecules into
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
        self._position = []
        for elem in value:
            if not check.is_number(elem):
                raise TypeError(f"{elem} is not a number.")
            self._position.append(elem)


class Manual(InputFile):
    """Parameters specific to a manual finite-difference polar solvation
    Poisson-Boltzmann calculation.

    Manually-configured finite difference multigrid Poisson-Boltzmann
    calculations.

    This is a standard single-point multigrid PBE calculation without focusing
    or additional refinement. This :func:`FiniteDifference.calculation_type`
    offers the most control of parameters to the user. Several of these
    calculations can be strung together to perform focusing calculations by
    judicious choice of the :func:`FiniteDifference.boundary_condition`
    property; however, the setup of the focusing is not automated as it is in
    the :class:`Focus` :func:`FiniteDifference.calculation_type`. Therefore,
    this command should primarily be used by more experienced users.

    Objects can be initialized with dictionary/JSON/YAML data with the
    following keys:

    * ``grid center``:  center of the grid, see :func:`grid_center`

    * ``grid dimensions``:  dimensions of the grid, see :func:`grid_dimensions`

    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._grid_center = None
        self._grid_dimensions = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def from_dict(self, input_):
        """Load object from dictionary.

        :raises KeyError:  if missing entries
        """
        self.grid_center = GridCenter(dict_=input_["grid center"])
        self.grid_dimensions = GridDimensions(dict_=input_["grid dimensions"])
        return super().from_dict(input_)

    def to_dict(self) -> dict:
        return {
            "grid center": self.grid_center.to_dict(),
            "grid dimensions": self.grid_dimensions.to_dict(),
        }

    def validate(self):
        errors = []
        if self.grid_dimensions is None:
            errors.append("grid dimensions not set.")
        else:
            self.grid_dimensions.validate()
        if self.grid_center is None:
            errors.append("grid center not set")
        else:
            self.grid_center.validate()
        if errors:
            raise ValueError(" ".join(errors))

    @property
    def grid_dimensions(self) -> GridDimensions:
        """Dimensions of the grid in a focusing finite-difference
        Poisson-Boltzmann calculation.

        :raises TypeError:  if the value is not :class:`GridDimensions`.
        """
        return self._grid_dimensions

    @grid_dimensions.setter
    def grid_dimensions(self, value):
        if not isinstance(value, GridDimensions):
            raise TypeError(
                f"Value {value} is type {type(value)} rather than "
                f"GridDimensions."
            )
        self._grid_dimensions = value

    @property
    def grid_center(self) -> GridCenter:
        """The center of the grid in a finite difference Poisson-Boltzmann
        calculation.

        :raises TypeError:  if not :class:`GridCenter` object
        """
        return self._grid_center

    @grid_center.setter
    def grid_center(self, value):
        if not isinstance(value, GridCenter):
            raise TypeError(
                f"{value} (type {type(value)}) is not GridCenter type."
            )
        self._grid_center = value


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

    def __init__(self, dict_=None, yaml=None, json=None):
        self._overlap_fraction = None
        self._processor_array = None
        self._asynchronous_rank = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def from_dict(self, input_):
        """Load object from dictionary.

        :raises KeyError: for missing items
        """
        self.overlap_fraction = input_["overlap fraction"]
        self.processor_array = input_["processor array"]
        self.asynchronous_rank = input_.get("asynchronous rank", None)

    def to_dict(self) -> dict:
        return {
            "overlap fraction": self.overlap_fraction,
            "processor array": self.processor_array,
            "asynchronous rank": self.asynchronous_rank,
        }

    def validate(self):
        if self.asynchronous_rank is not None:
            num_proc = 1
            for i in range(3):
                num_proc *= self.processor_array[i]
            if self.asynchronous_rank > num_proc:
                raise ValueError(
                    f"Processor rank {self.asynchronous_rank} is greater than "
                    "the number of processors {num_proc}."
                )

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
        self._processor_array = []
        for elem in value:
            if not check.is_positive_definite(elem):
                raise TypeError(
                    f"Processor array {value} does not contain positive "
                    f"numbers."
                )
            if not isinstance(elem, int):
                raise TypeError(
                    f"Processor array {value} does not contain integers."
                )
            self._processor_array.append(elem)

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
                f"Overlap fraction (type {type(value)}) is not a positive "
                f"number."
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
    Gilson and Honig
    (DOI:`10.1038/330084a0 <http://dx.doi.org/10.1038/330084a0>`_). The method
    starts by solving the equation on a coarse grid (i.e., few grid points)
    with large dimensions (i.e., grid lengths). The solution on this coarse
    grid is then used to set the Dirichlet boundary condition values for a
    smaller problem domain -- and therefore a finer grid -- surrounding the
    region of interest. The finer grid spacing in the smaller problem domain
    often provides greater accuracy in the solution.

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

    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._fine_grid_center = None
        self._fine_grid_dimensions = None
        self._coarse_grid_center = None
        self._coarse_grid_dimensions = None
        self._parallel = None
        self._parallel_parameters = None
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def from_dict(self, input_):
        """Load object from dictionary.

        :raises KeyError:  if missing items.
        """
        self.fine_grid_center = GridCenter(dict_=input_["fine grid center"])
        self.fine_grid_dimensions = GridDimensions(
            dict_=input_["fine grid dimensions"]
        )
        self.coarse_grid_center = GridCenter(
            dict_=input_["coarse grid center"]
        )
        self.coarse_grid_dimensions = GridDimensions(
            dict_=input_["coarse grid dimensions"]
        )
        self.parallel = input_["parallel"]
        if self.parallel:
            self.parallel_parameters = ParallelFocus(
                dict_=input_["parallel parameters"]
            )

    def to_dict(self) -> dict:
        dict_ = {
            "fine grid center": self.fine_grid_center.to_dict(),
            "fine grid dimensions": self.fine_grid_dimensions.to_dict(),
            "coarse grid center": self.coarse_grid_center.to_dict(),
            "coarse grid dimensions": self.coarse_grid_dimensions.to_dict(),
            "parallel": self.parallel,
        }
        if dict_["parallel"]:
            dict_["parallel parameters"] = self.parallel_parameters.to_dict()
        return dict_

    def validate(self):
        errors = []
        if self.fine_grid_center is None:
            errors.append("fine grid center not set.")
        else:
            self.fine_grid_center.validate()
        if self.coarse_grid_center is None:
            errors.append("coarse grid center not set.")
        else:
            self.coarse_grid_center.validate()
        if (self.fine_grid_dimensions is None) or (
            self.coarse_grid_dimensions is None
        ):
            errors.append("Either fine or coarse grid dimensions not set.")
        else:
            self.fine_grid_dimensions.validate()
            self.coarse_grid_dimensions.validate()
            for i in range(3):
                if (
                    self.coarse_grid_dimensions.lengths[i]
                    < self.fine_grid_dimensions.lengths[i]
                ):
                    raise ValueError(
                        f"Coarse grid length "
                        f"{self.coarse_grid_dimensions.lengths[i]} is less "
                        f"than fine grid length "
                        f"{self.fine_grid_dimensions.lengths[i]}"
                    )
        if self.parallel:
            if self.parallel_parameters is None:
                raise ValueError("Missing parallel parameters.")
            self.parallel_parameters.validate()
        if errors:
            raise ValueError(" ".join(errors))

    @property
    def parallel(self) -> bool:
        """Indicate whether a parallel calculation should be performed.

        If set, the :func:`parallel_parameters` property must also be set.
        Set the :func:`parallel_parameters` property before setting this value
        to True (sorry).

        :raises TypeError:  if set to something other than :class:`bool`.
        """
        return self._parallel

    @parallel.setter
    def parallel(self, value):
        if not check.is_bool(value):
            raise TypeError(f"Value {value} is not a Boolean.")
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
    def coarse_grid_dimensions(self, value):
        if not isinstance(value, GridDimensions):
            raise TypeError(
                f"Value {value} is type {type(value)} rather than "
                f"GridDimensions."
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
        self._fine_grid_dimensions = value

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
    * ``calculation type``:  see :func:`calculation_type` as well as
      :class:`Focus` and :class:`Manual`
    * ``calculation parameters``:  see :func:`calculation_parameters`
    * ``charge discretization``:  method used to map charges onto the grid; see
      :func:`charge_discretization`
    * ``error tolerance``:  solver error tolerance; see :func:`error_tolerance`
    * ``equation``:  what version of the Poisson-Boltzmann equation to solve;
      see :func:`equation`
    * ``ions``:  information about mobile ion species; see :func:`ions`
    * ``molecule``:  alias to molecule for calculation; see :func:`molecule`
    * ``no-op``:  determine whether the solver should be run; see :func:`noop`
    * ``solute dielectric``:  see :func:`solute_dielectric`
    * ``solvent dielectric``:  see :func:`solvent_dielectric`
    * ``solvent radius``:  see :func:`solvent_radius`
    * ``surface method``:  see :func:`surface_method`
    * ``surface spline window``:  see :func:`surface_spline_window`
    * ``temperature``:  see :func:`temperature`
    * ``use maps``:  use input map for one or more properties of the system;
      see :func:`use_maps`
    * ``write atom potentials``:  write out atom potentials; see
      :func:`write_atom_potentials`
    * ``write maps``:  write out one or more properties of the system to a map;
      see :func:`write_maps`

    """

    def __init__(self, dict_=None, yaml=None, json=None):
        self._boundary_condition = None
        self._calculate_energy = None
        self._calculate_forces = None
        self._calculation_type = None
        self._calculation_parameters = None
        self._charge_discretization = None
        self._error_tolerance = None
        self._equation = None
        self._ions = None
        self._molecule = None
        self._noop = False
        self._solute_dielectric = None
        self._solvent_dielectric = None
        self._solvent_radius = None
        self._surface_method = None
        self._surface_spline_window = None
        self._temperature = None
        self._use_maps = []
        self._write_atom_potentials = None
        self._write_maps = []
        super().__init__(dict_=dict_, yaml=yaml, json=json)

    def from_dict(self, input_):
        """Load object from dictionary.

        :raises KeyError:  if some entries not found.
        :raises ValueError:  for invalid entries.
        """
        self.boundary_condition = input_["boundary condition"]
        self.calculate_energy = input_["calculate energy"]
        self.calculate_forces = input_["calculate forces"]
        self.calculation_type = input_["calculation type"]
        if self.calculation_type == "focus":
            self.calculation_parameters = Focus(
                dict_=input_["calculation parameters"]
            )
        elif self.calculation_type == "manual":
            self.calculation_parameters = Manual(
                dict_=input_["calculation parameters"]
            )
        else:
            raise ValueError(
                f"Invalid calculation type {input_['calculation type']}."
            )
        self.charge_discretization = input_["charge discretization"]
        self.error_tolerance = input_["error tolerance"]
        self.equation = input_["equation"]
        self.ions = MobileIons(dict_=input_["ions"])
        self.molecule = input_["molecule"]
        self.noop = input_["no-op"]
        self.solute_dielectric = input_["solute dielectric"]
        self.solvent_dielectric = input_["solvent dielectric"]
        self.solvent_radius = input_["solvent radius"]
        self.surface_method = input_["surface method"]
        self.surface_spline_window = input_["surface spline window"]
        self.temperature = input_["temperature"]
        for map_dict in input_["use maps"]:
            self.use_maps.append(UseMap(dict_=map_dict))
        self.write_atom_potentials = input_["write atom potentials"]
        for map_dict in input_["write maps"]:
            self.write_maps.append(WriteMap(dict_=map_dict))

    def to_dict(self) -> dict:
        return {
            "boundary condition": self.boundary_condition,
            "calculate energy": self.calculate_energy,
            "calculate forces": self.calculate_forces,
            "calculation type": self.calculation_type,
            "calculation parameters": self.calculation_parameters.to_dict(),
            "charge discretization": self.charge_discretization,
            "error tolerance": self.error_tolerance,
            "equation": self.equation,
            "ions": self.ions.to_dict(),
            "molecule": self.molecule,
            "no-op": self.noop,
            "solute dielectric": self.solute_dielectric,
            "solvent dielectric": self.solvent_dielectric,
            "solvent radius": self.solvent_radius,
            "surface method": self.surface_method,
            "surface spline window": self.surface_spline_window,
            "temperature": self.temperature,
            "use maps": [map_.to_dict() for map_ in self.use_maps],
            "write atom potentials": self.write_atom_potentials,
            "write maps": [map_.to_dict() for map_ in self.write_maps],
        }

    def validate(self):
        errors = []
        if self.boundary_condition is None:
            errors.append("boundary condition not set.")
        if self.calculate_energy is None:
            errors.append("calculate energy not set.")
        if self.calculate_forces is None:
            errors.append("calculate forces not set.")
        if self.calculation_type is None:
            errors.append("calculation type not set.")
        if self.calculation_parameters is None:
            errors.append("calculation parameters not set.")
        else:
            try:
                self.calculation_parameters.validate()
            except ValueError as error:
                errors.append(str(error))
        if self.charge_discretization is None:
            errors.append("charge discretization not set.")
        if self.error_tolerance is None:
            errors.append("error tolerance not set.")
        if self.equation is None:
            errors.append("equation not set.")
        if self.ions is None:
            _LOGGER.debug(
                "The ions feel bad that you left them out but it's OK, "
                "they'll get over it."
            )
        else:
            try:
                self.ions.validate()
            except ValueError as error:
                errors.append(str(error))
        if self.molecule is None:
            errors.append("molecule not set.")
        if self.noop is None:
            errors.append("no-op not set.")
        if self.solute_dielectric is None:
            errors.append("solute dielectric not set.")
        if self.solvent_dielectric is None:
            errors.append("solvent dielectric not set.")
        if self.solvent_radius is None:
            errors.append("solvent radius not set.")
        if self.surface_method is None:
            errors.append("surface_method not set.")
        elif (
            "spline" in self.surface_method
            and self.surface_spline_window is None
        ):
            errors.append("surface spline window not set.")
        if self.temperature is None:
            errors.append("temperature not set.")
        for map_ in self._use_maps:
            try:
                map_.validate()
            except ValueError as error:
                errors.append(str(error))
        if self.write_atom_potentials:
            _LOGGER.debug(
                "It's OK if you don't want to write out atom potentials."
            )
        for map_ in self.write_maps:
            try:
                map_.validate()
            except ValueError as error:
                errors.append(str(error))
        if errors:
            raise ValueError(" ".join(errors))

    @property
    def noop(self) -> bool:
        """Determine whether solver is run.

        If set to ``True``, then skip running the solver but still calculate
        coefficient maps, etc.

        The default value of this property is ``False``.

        :raises TypeError:  if not set to :class:`bool`
        """
        return self._noop

    @noop.setter
    def noop(self, value):
        if not isinstance(value, bool):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a Boolean."
            )

    @property
    def write_maps(self) -> list:
        """Write out maps related to computed properties.

        See :class:`WriteMap` for more information.

        :raises TypeError:  if set to wrong type
        """
        return self._write_maps

    @write_maps.setter
    def write_maps(self, value):
        if not check.is_list(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a list."
            )
        for elem in value:
            if not isinstance(elem, WriteMap):
                raise TypeError(
                    f"Value {elem} (type {type(elem)}) is not a WriteMap "
                    f"object."
                )
        self._write_maps = value

    @property
    def write_atom_potentials(self) -> str:
        """Write out the electrostatic potential at each atom location.

        Write out text file with potential at center of atom in units of
        :math:`k_b \\, T \\, e_c^{-1}`.

        .. note::

           These numbers are meaningless by themselves due to the presence of
           "self-energy" terms that are sensitive to grid spacing and position.
           These numbers should be evaluated with respect to a reference
           calculation:  the potentials from that reference calculation should
           be subtracted from the target system.  For example, one calculation
           might include a molecule with a heterogeneous dielectric coefficient
           and the reference system might be exactly the same system setup but
           with a homeogeneous dielectric coefficient.  If the results from the
           reference calculation are substracted from the first calculation,
           then the result will be a physically meaningful reaction field
           potential. However, the results from the first and reference
           calculations are meaningless by themselves.

        :returns:  path to text file for writing atom potential values.
        :raises TypeError:  if not set to string
        """
        return self._write_atom_potentials

    @write_atom_potentials.setter
    def write_atom_potentials(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        self._write_atom_potentials = value

    @property
    def use_maps(self) -> list:
        """Information for (optionally) using maps read into APBS.

        :returns:  list of :class:`UseMap` objects
        :raises TypeError:  if not a list of :class:`UseMap` objects
        """
        return self._use_maps

    @use_maps.setter
    def use_maps(self, value):
        if not check.is_list(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a list."
            )
        for elem in value:
            if not isinstance(elem, UseMap):
                raise TypeError(
                    f"List contains element {elem} of type {type(elem)}."
                )
            self._use_maps.append(elem)

    @property
    def temperature(self) -> float:
        """Temperature for the calculation in Kelvin.

        :raises ValueError:  if not a positive number (no violations of the
            3rd Law!)
        """
        return self._temperature

    @temperature.setter
    def temperature(self, value):
        if check.is_positive_definite(value):
            self._temperature = value
        else:
            raise ValueError(f"{value} is not a positive number.")

    @property
    def surface_spline_window(self) -> float:
        """Window for spline-based surface definitions (not needed otherwise).

        This is the distance (in Å) over which the spline transitions from the
        solvent dielectric value to the solute dielectric value.  A typical
        value is 0.3 Å.

        :returns:  positive number
        :raises TypeError:  if not a positive number
        """
        return self._surface_spline_window

    @surface_spline_window.setter
    def surface_spline_window(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        self._surface_spline_window = value

    @property
    def surface_method(self) -> str:
        """Method used to defined solute-solvent interface.

        One of the following values:

        * ``molecular surface``:  The dielectric coefficient is defined based
          on a molecular surface definition. The problem domain is
          divided into two spaces. The "free volume" space is defined by the
          union of solvent-sized spheres (see :func:`solvent_radius`) which do
          not overlap with the solute atoms. This free volume is assigned bulk
          solvent dielectric values. The complement of this space is assigned
          solute dielectric values. When the solvent radius is set to zero,
          this method corresponds to a van der Waals surface definition. The
          ion-accessibility coefficient is defined by an "inflated" van der
          Waals model. Specifically, the radius of each biomolecular atom is
          increased by the radius of the ion species (as specified with the
          :func:`ion` property). The problem domain is then divided into two
          spaces. The space inside the union of these inflated atomic spheres
          is assigned an ion-accessibility value of 0; the complement space is
          assigned the bulk ion accessibility value. See Connolly ML, J Appl
          Crystallography 16 548-558, 1983 (`10.1107/S0021889883010985
          <https://doi.org/10.1107/S0021889883010985>`_).

        * ``smoothed molecular surface``:  The dielectric and ion-accessibility
          coefficients are defined as for the ``molecular surface`` (see
          above). However, they are then "smoothed" by a 9-point harmonic
          averaging to somewhat reduce sensitivity to the grid setup. See
          Bruccoleri et al. J Comput Chem 18 268-276, 1997
          (`10.1007/s00214-007-0397-0
          <http://dx.doi.org/10.1007/s00214-007-0397-0>`_).

        * ``cubic spline``:  The dielectric and ion-accessibility coefficients
          are defined by a cubic-spline surface as described by Im et al,
          Comp Phys Commun 111 (1-3) 59-75, 1998
          (`10.1016/S0010-4655(98)00016-2
          <https://doi.org/10.1016/S0010-4655(98)00016-2>`_). The width of the
          dielectric interface is controlled by the :func:`spline_window`
          property. These spline-based surface definitions are very stable
          with respect to grid parameters and therefore ideal for calculating
          forces. However, they require substantial reparameterization of the
          force field; interested users should consult Nina et al, Biophys
          Chem 78 (1-2) 89-96, 1999 (`10.1016/S0301-4622(98)00236-1
          <http://dx.doi.org/10.1016/S0301-4622(98)00236-1>`_). Additionally,
          these surfaces can generate unphysical results with non-zero ionic
          strengths.

        * ``septic spline``: The dielectric and ion-accessibility coefficients
          are defined by a 7th order polynomial. This surface definition has
          characteristics similar to the cubic spline, but provides higher
          order continuity necessary for stable force calculations with atomic
          multipole force fields (up to quadrupole).

        :raises TypeError:  if not set to a string
        :raises ValueError:  if set to invalid value
        """
        return self._surface_method

    @surface_method.setter
    def surface_method(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)} is not a string."
            )
        value = value.lower()
        if value not in [
            "molecular surface",
            "smoothed molecular surface",
            "cubic spline",
            "septic spline",
        ]:
            raise ValueError(f"Value {value} is an invalid surface method.")
        self._surface_method = value

    @property
    def solvent_radius(self) -> float:
        """Radius of the solvent molecules.

        This parameter is used to define various solvent-related surfaces and
        volumes (see :func:`surface_method`). This value is usually set to 1.4
        Å for a water-like molecular surface and set to 0 Å for a van der Waals
        surface.

        :raises ValueError:  if value is not a non-negative number
        """
        return self._solvent_radius

    @solvent_radius.setter
    def solvent_radius(self, value):
        if check.is_positive_semidefinite(value):
            self._solvent_radius = value
        else:
            raise ValueError(f"{value} is not a non-negative number.")

    @property
    def solvent_dielectric(self) -> float:
        """Solvent dielectric.

        78.5 is a good choice for water at 298 K.

        :returns:  a floating point number greater than or equal to one
        :raises TypeError:  if not a number
        :raises ValueError:  if not greater than or equal to 1
        """
        return self._solvent_dielectric

    @solvent_dielectric.setter
    def solvent_dielectric(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        if value < 1:
            raise ValueError(f"Value {value} is not >= 1.")
        self._solvent_dielectric = value

    @property
    def solute_dielectric(self) -> float:
        """Solute dielectric.

        The dielectric value of a solute is often chosen using the following
        rules of thumb:

        * 1:  only used to compare calculation results with non-polarizable
          molecular simulation

        * 2-4:  "molecular" dielectric value; used when conformational degrees
          of freedom are modeled explicitly

        * 4-8:  used to mimic sidechain libration and other small-scale
          conformational degrees of freedom

        * 8-12:  used to model larger-scale sidechain rearrangement

        * 20-40:  used to model larger-scale macromolecular conformational
          changes and/or water penetration into interior of molecule

        .. note::

           What does the continuum dielectric value of a non-continuum molecule
           mean?  Hard to say -- this approximation can be very difficult to
           interpret and can significant affect your results.

        :returns:  a floating point number greater than or equal to one
        :raises TypeError:  if not a number
        :raises ValueError:  if not greater than or equal to 1
        """
        return self._solute_dielectric

    @solute_dielectric.setter
    def solute_dielectric(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        if value < 1:
            raise ValueError(f"Value {value} is not >= 1.")
        self._solute_dielectric = value

    @property
    def molecule(self) -> str:
        """Specify which molecule to use for calculation.

        :returns:  alias for molecule read (see :ref:`read_new_input`)
        :raises TypeError:  if not set to a string
        """
        return self._molecule

    @molecule.setter
    def molecule(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        self._molecule = value

    @property
    def equation(self) -> str:
        """Specifies which version of the Poisson-Boltzmann equation (PBE) to
        solve:

        * Most users should use one of these:

          * ``linearized pbe``
          * ``nonlinear pbe``

        * These versions are experimental and unstable:

          * ``linearized regularized pbe``
          * ``nonlinear regularized pbe``

        :raises TypeError:  if not set to a string.
        :raises ValueError:  if set to an invalid value
        """
        return self._equation

    @equation.setter
    def equation(self, value):
        if not check.is_string(value):
            raise TypeError(
                f"Value {value} (type {type(value)}) is not a string."
            )
        value = value.lower()
        if value not in [
            "linearized pbe",
            "nonlinear pbe",
            "linearized regularized pbe",
            "nonlinear regularized pbe",
        ]:
            raise ValueError(f"Value {value} is invalid.")
        self._equation = value

    @property
    def ions(self) -> MobileIons:
        """Descriptions of mobile ion species.

        :raises TypeError:  if not set to a :class:`Ions` object
        """
        return self._ions

    @ions.setter
    def ions(self, value):
        if not isinstance(value, MobileIons):
            raise TypeError(
                f"Value {value} (type {type(value)} is not an Ions object."
            )
        self._ions = value

    @property
    def error_tolerance(self) -> float:
        """Relative error tolerance for iterative solver.

        If not specified, the default value is :const:`ERROR_TOLERANCE`.

        :raises TypeError:  if not set to positive number
        :raises ValueError:  if not set to number less than 1
        """
        if self._error_tolerance is None:
            self._error_tolerance = ERROR_TOLERANCE
        return self._error_tolerance

    @error_tolerance.setter
    def error_tolerance(self, value):
        if not check.is_positive_definite(value):
            raise TypeError(f"Value {value} is not a positive number.")
        if value >= 1.0:
            raise ValueError(f"Value {value} is not less than 1.0.")
        self._error_tolerance = value

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
                f"Calculation type is {type(self._calculation_type)} but "
                f"value type is {type(value)}."
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
          boundary is set to the values prescribed by a Debye-Hückel model for
          a single sphere with a point charge, dipole, and quadrupole. The
          sphere radius in this model is set to the radius of the biomolecule
          and the sphere charge, dipole, and quadrupole are set to the total
          moments of the protein. This condition works best when the boundary
          is sufficiently far (multiple Debye lengths) from the biomolecule.

        * ``multiple sphere``:  Dirichlet condition where the potential at the
          boundary is set to the values prescribed by a Debye-Hückel model for
          multiple, non-interacting spheres with a point charges. The radii of
          the non-interacting spheres are set to the atomic radii of and the
          sphere charges are set to the atomic charges. This condition works
          better than ``single sphere`` for closer boundaries but can be very
          slow for large biomolecules.

        * ``focus`` :c:var:`alias`:  Dirichlet condition where the potential at
          the boundary is set to the values computed by a previous (usually
          lower-resolution) PB calculation with alias :c:var:`alias`. All of
          the boundary points should lie within the domain of the previous
          calculation for best accuracy; if any boundary points lie outside,
          their values are computed using the ``single sphere`` Debye-Hückel
          boundary condition (see above).

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
        elif words[0] == "focus":
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
