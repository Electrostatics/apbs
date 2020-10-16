import os
import re
import sys
import platform
import subprocess

from tools.install_scripts.cmake_setuptools import *

proj_root = os.path.dirname(os.path.realpath(__file__))

print('-- Ensuring required pip modules are installed')
import pip
pip.main(
        ['install'] + 
        open('requirements.txt', 'r').readlines() + 
        ['--upgrade'])

from distutils import sysconfig as sc
python_site_pkgs = sc.get_python_lib(prefix='', plat_specific=True)
print(f'-- Found python site-packages directory {python_site_pkgs}')

if not os.path.exists('setup.cfg'):
    print('-- Ensuring submodules are up-to-date')
    from git import Repo
    repo = Repo(proj_root)
    for submodule in repo.submodules:
        submodule.update(init=True)

    print('-- Generating MANIFEST.in')
    output = subprocess.run(['git', 'ls-files'], check=True, capture_output=True)
    with open('MANIFEST.in', 'w') as f:
        for line in output.stdout.decode('utf-8').split('\n'):
            if os.path.isfile('./' + line):
                f.write('include "' + line + '"\n')
        f.write('recursive-include externals *\n')

extra_cmake_args = dict(
        ENABLE_PYTHON='ON',
        CMAKE_BUILD_TYPE='Release',
        PYTHON_SITE_PACKAGES_DIR=python_site_pkgs)

import pybind11
extra_cmake_args['pybind11_DIR'] = os.path.join(
        os.path.dirname(pybind11.get_cmake_dir()), 'pybind11')

setup(
    name='APBS',
    version='3.0.0',
    author='',
    author_email='',
    description='Adaptive Poisson-Boltzmann Solver',
    long_description=open('LICENSE.md', 'r').read(),
    ext_modules=[CMakeExtension(
        proj_root,
        **extra_cmake_args)],
    cmdclass=dict(
        build_ext=CMakeBuild,
        install=CMakeInstall,
        develop=CMakeInstall),
    zip_safe=False,
    )
