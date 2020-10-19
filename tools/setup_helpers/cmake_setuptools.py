import os
import sys
import platform
import subprocess
import glob
from typing import Optional
from shutil import rmtree
from setuptools import setup, Extension, Command
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from setuptools.command.develop import develop
import setuptools
from distutils.version import LooseVersion

from .env import *
from .utils import *


class CleanBuild(Command):
    '''Clean build directory'''

    user_options = []

    def initialize_options(self):
        ...

    def finalize_options(self):
        ...

    def run(self):
        build_dir = get_build_dir()
        rmtree(build_dir)


class CMakeExtension(Extension):
    '''Base for CMake setuptools extension'''
    def __init__(self, name, **extra_cmake_args):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath('')
        self.extra_cmake_args = extra_cmake_args or dict()


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

        pip_install()
        init_submodules()
        generate_manifest()

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        build_dir = get_build_dir()

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

        if IS_WINDOWS:
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        cmake_args += ['-DCMAKE_INSTALL_PREFIX=' + 
                os.path.abspath(sys.prefix) ]

        os.environ['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(os.environ.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())

        print(f'-- CMake arguments: {cmake_args}')
        print(f'-- Build directory: {build_dir}')
        print(f'-- Source directory: {ext.sourcedir}')

        # We currently aren't checking the output of these since they sometimes
        # give false negatives that halt the build process.
        # TODO: investigate false negatives
        print('-- Configuring')
        subprocess.run(
                ['cmake', ext.sourcedir] + cmake_args,
                cwd=build_dir)

        print('-- Building')
        subprocess.run(
                ['cmake', '--build', '.'] + build_args,
                cwd=build_dir)
