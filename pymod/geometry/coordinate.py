from typing import Generic, TypeVar, Callable

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
        if len(vals) not in (0, 3):
            raise RuntimeError("Can only pass in 3 values for a Point.")
        self._data: Array[T]

        if len(vals) == 3:
            self._data = list(vals)
        else:
            self._data = [0, 0, 0]

    def any(self, predicate: Callable[[float], bool]) -> bool:
        for i in self._data:
            if predicate(i):
                return True
        return False

    def all(self, predicate: Callable[[float], bool]) -> bool:
        for i in self._data:
            if not predicate(i):
                return False
        return True

    def __getitem__(self, idx: int) -> T:
        if idx > 2 or idx < -2:
            raise IndexError("Point has only 3 dimensions.")
        return self._data[idx]

    def __setitem__(self, idx: int, value: T) -> None:
        if idx > 2 or idx < -2:
            raise IndexError("Point has only 3 dimensions.")
        self._data[idx] = value

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
        return f'Coordinate <{self.x}, {self.y}, {self.z}>'

    def __add__(self, other: 'Coordinate') -> 'Coordinate':
        if isinstance(other, (float, int)):
            return Coordinate(self.x + other, self.y + other, self.z + other)
        return Coordinate(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self, other: 'Coordinate') -> 'Coordinate':
        if isinstance(other, (float, int)):
            return Coordinate(self.x - other, self.y - other, self.z - other)
        return Coordinate(self.x - other.x, self.y - other.y, self.z - other.z)

    def __mul__(self, other: 'Coordinate') -> 'Coordinate':
        if isinstance(other, (float, int)):
            return Coordinate(self.x * other, self.y * other, self.z * other)
        return Coordinate(self.x * other.x, self.y * other.y, self.z * other.z)

    def __truediv__(self, other: 'Coordinate') -> 'Coordinate':
        if isinstance(other, (float, int)):
            return Coordinate(self.x / other, self.y / other, self.z / other)
        return Coordinate(self.x / other.x, self.y / other.y, self.z / other.z)

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
