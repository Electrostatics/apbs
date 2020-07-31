from apbs_types import *
from typing import Generic, TypeVar
import sys
sys.path.insert(0, '..')

'''
Using Coordinate as a lower-level abstraction over len==3 generic container,
with class Point potentially having more associated data/behavior to come.
'''

T = TypeVar('T')

class Coordinate(Generic[T]):
    '''
    Attributes:
        data (Array[T]): data to be stored in the point (max three values)

    NOTE: initializes values to 0 if none are passed in. Will not cast to
            type-hinted type.
    '''

    def __init__(self, *vals):
        assert len(vals) in (0, 3), "Can only pass in 3 values for a Point."
        self.data: Array[T]

        if len(vals) == 3:
            self.data = list(vals)
        else:
            self.data = [0, 0, 0]

    def __get__(self, idx: int) -> T:
        if idx >= 3:
            raise IndexError("Point has only 3 dimensions.")
        return self._data[idx]

    def __lt__(self, other: 'Coordinate[T]') -> bool:
        return self.x < other.x and self.y < other.y and self.z < other.z

    def __le__(self, other: 'Coordinate[T]') -> bool:
        return self.x <= other.x and self.y <= other.y and self.z <= other.z

    def __gt__(self, other: 'Coordinate[T]') -> bool:
        return self.x > other.x and self.y > other.y and self.z > other.z

    def __ge__(self, other: 'Coordinate[T]') -> bool:
        return self.x >= other.x and self.y >= other.y and self.z >= other.z

    def __eq__(self, other: 'Coordinate[T]') -> bool:
        return self.x == other.x and self.y == other.y and self.z == other.z

    def __ne__(self, other: 'Coordinate[T]') -> bool:
        return not self == other

    def __str__(self):
        return f'Coordinate <{self.x}, {self.y}, {self.z}>'

    def __repr__(self):
        return str(self)

    @property
    def x(self) -> T:
        return self.data[0]

    @property
    def y(self) -> T:
        return self.data[1]

    @property
    def z(self) -> T:
        return self.data[2]

    @x.setter
    def x(self, value: T) -> None:
        self.data[0] = value

    @y.setter
    def y(self, value: T) -> None:
        self.data[1] = value

    @z.setter
    def z(self, value: T) -> None:
        self.data[2] = value
