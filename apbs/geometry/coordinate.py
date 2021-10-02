from typing import Generic, TypeVar, Callable
import numpy as np

"""
Using Coordinate as a lower-level abstraction over len==3 generic container,
with class Point potentially having more associated data/behavior to come.
"""

T = TypeVar("T")


class Coordinate(Generic[T]):
    """
    Attributes:
        data (Array[T]): data to be stored in the point (max three values)

    NOTE: initializes values to 0 if none are passed in. Will not cast to
            type-hinted type.
    """

    def __init__(self, *vals, array: np.ndarray = None):
        if len(vals) not in (0, 3):
            raise RuntimeError("Can only pass in 3 values for a Point.")

        if len(vals) == 3:
            self._data = np.array(vals, dtype=np.float32)
        elif array is not None:
            self._data = np.array(array, dtype=np.float32)
        else:
            self._data = np.zeros(3, dtype=np.float32)

    def any(self, predicate: Callable[[float], bool]) -> bool:
        return any(predicate(idx) for idx in self._data)

    def all(self, predicate: Callable[[float], bool]) -> bool:
        return all(predicate(idx) for idx in self._data)

    def __getitem__(self, idx: int) -> T:
        if idx > 2 or idx < -2:
            raise IndexError("Point has only 3 dimensions.")
        return self._data[idx]

    def __setitem__(self, idx: int, value: T) -> None:
        if idx > 2 or idx < -2:
            raise IndexError("Point has only 3 dimensions.")
        self._data[idx] = value

    def __lt__(self, other: "Coordinate[T]") -> bool:
        return (self._data < other._data).all()

    def __le__(self, other: "Coordinate[T]") -> bool:
        return (self._data <= other._data).all()

    def __gt__(self, other: "Coordinate[T]") -> bool:
        return (self._data > other._data).all()

    def __ge__(self, other: "Coordinate[T]") -> bool:
        return (self._data >= other._data).all()

    def __eq__(self, other: "Coordinate[T]") -> bool:
        return (self._data == other._data).all()

    def __ne__(self, other: "Coordinate[T]") -> bool:
        return not (self._data == other._data).all()

    def __str__(self):
        return f"Coordinate <{self.x}, {self.y}, {self.z}>"

    def __repr__(self):
        return f"Coordinate <{self.x}, {self.y}, {self.z}>"

    def __add__(self, other: "Coordinate") -> "Coordinate":
        if isinstance(other, Coordinate):
            return Coordinate(array=self._data + other._data)
        elif isinstance(other, (float, int)):
            return Coordinate(array=self._data + other)
        raise RuntimeError("Unexpected data type for this operation.")

    def __sub__(self, other: "Coordinate") -> "Coordinate":
        if isinstance(other, Coordinate):
            return Coordinate(array=self._data - other._data)
        elif isinstance(other, (float, int)):
            return Coordinate(array=self._data - other)
        raise RuntimeError("Unexpected data type for this operation.")

    def __mul__(self, other: "Coordinate") -> "Coordinate":
        if isinstance(other, Coordinate):
            return Coordinate(array=self._data * other._data)
        elif isinstance(other, (float, int)):
            return Coordinate(array=self._data * other)
        raise RuntimeError("Unexpected data type for this operation.")

    def __truediv__(self, other: "Coordinate") -> "Coordinate":
        if isinstance(other, Coordinate):
            return Coordinate(array=self._data / other._data)
        elif isinstance(other, (float, int)):
            return Coordinate(array=self._data / other)
        raise RuntimeError("Unexpected data type for this operation.")

    @property
    def x(self) -> T:
        return self._data[0]

    @property
    def y(self) -> T:
        return self._data[1]

    @property
    def z(self) -> T:
        return self._data[2]

    @x.setter
    def x(self, value: T) -> None:
        self._data[0] = value

    @y.setter
    def y(self, value: T) -> None:
        self._data[1] = value

    @z.setter
    def z(self, value: T) -> None:
        self._data[2] = value
