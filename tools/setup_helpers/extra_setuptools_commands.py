import sys
import platform
import subprocess
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


def repair_wheel(wheel_path: str) -> None:

    repair_cmd = None
    if platform.system() == 'Linux':
        repair_cmd = ['auditwheel', 'repair', wheel_path]
    elif platform.system() == 'Darwin':
        repair_cmd = ['delocate-wheel', '-v', wheel_path]
    else:
        # No wheels need to be repaired on windows
        return

    if not os.path.exists(wheel_path):
        raise RuntimeError(f'Wheel {wheel_path} does not exist.')

    print(f'Repairing wheel {wheel_path}')
    subprocess.check_output(repair_cmd)
