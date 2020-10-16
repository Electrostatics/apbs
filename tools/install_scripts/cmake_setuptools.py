import os
import sys
import platform
import subprocess
import glob

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from setuptools.command.develop import develop
from distutils.version import LooseVersion

'''

Taken from pybind11 examples of integrating cmake and python compilation

github.com/pybind/cmake_example/blob/master/setup.py

'''


class CMakeExtension(Extension):
    def __init__(self, name, **extra_cmake_args):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath('')
        self.extra_cmake_args = extra_cmake_args or dict()


def replace_run(cls):
    '''Replace run command with custom cmake hook'''

    def find_builddir(self):
        top_level_builddir = os.path.join(os.getcwd(), 'build')
        if not os.path.exists(top_level_builddir):
            raise OSError('Top-level build directory not found.')

        possible_builddirs = glob.glob(
                os.path.join(top_level_builddir, 'temp.*'))

        if len(possible_builddirs) == 0:
            raise OSError('Temp build directory not found.')
        elif len(possible_builddirs) > 1:
            raise RuntimeError('Found multiple temp build directories.'
                    'Please rebuild after removing old build directories.')

        return possible_builddirs[0]

    cls.find_builddir = find_builddir

    old_run = cls.run

    def run(self):

        builddir = self.find_builddir()

        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError('CMake must be installed to build this module.')

        subprocess.check_call(
                ['cmake', '--install', '.'],
                cwd=builddir)

        old_run(self)

    cls.run = run

    return cls


@replace_run
class CMakeInstall(install):
    pass


@replace_run
class CMakeDevelop(develop):
    pass


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        # required for auto-detection of auxiliary "native" libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        # Add extra cmake args to build string
        for k, v in ext.extra_cmake_args.items():
            cmake_args.append('-D' + k + '=' + v)

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            # if sys.maxsize > 2**32:
                # cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        if not os.access(sys.prefix, os.W_OK):
            raise PermissionError('setup.py does not have write access to install prefix')

        cmake_args += ['-DCMAKE_INSTALL_PREFIX=' + 
                os.path.abspath(sys.prefix) ]

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        print(f'CMake arguments: {cmake_args}',
                f'Build directory: {self.build_temp}',
                f'Source directory: {ext.sourcedir}')
        subprocess.check_call(
                ['cmake', ext.sourcedir] + cmake_args,
                cwd=self.build_temp, env=env)
        subprocess.check_call(
                ['cmake', '--build', '.'] + build_args,
                cwd=self.build_temp)
