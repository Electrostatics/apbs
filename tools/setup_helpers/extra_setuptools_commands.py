from setuptools import dist
from distutils.command.sdist import sdist
from .utils import *


class SDistChecked(sdist):
    """ check submodules on sdist to prevent incomplete tarballs """

    def run(self):
        init_submodules()
        sdist.run(self)


class BinaryDistribution(dist.Distribution):
    def has_ext_modules(foo):
        return True
