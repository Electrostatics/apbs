from apbs.geometry import Coordinate
from apbs.grid import Grid, CurvatureFlag
from typing import List
import pytest


class TestGrid:
    def test_value(self, pt: Coordinate[float]):
        pass

    def test_curvature(self, pt: Coordinate[float], cflag: CurvatureFlag):
        pass

    def test_gradient(self, pt: Coordinate[float], grad: List[float]):
        pass

    def test_integrate(self):
        pass

    def test_norml1(self):
        pass

    def test_norml2(self):
        pass

    def test_norml_inf(self):
        pass

    def test_seminormH1(self):
        pass

    def test_normH1(self):
        pass

    def test_read_dx(self, fn: str):
        pass
