import os
import re
import sys
import platform
import subprocess

from tools.install_scripts.cmake_setuptools import *

print('-- Ensuring required pip modules are installed')
import pip
pip.main(
        ['install'] + 
        open('requirements.txt', 'r').readlines() + 
        ['--upgrade'])

print('-- Ensuring submodules are up-to-date')
from git import Repo
proj_root = os.path.dirname(os.path.realpath(__file__))
repo = Repo(proj_root)
for submodule in repo.submodules:
    submodule.update(init=True)

from distutils import sysconfig as sc
python_site_pkgs = sc.get_python_lib(prefix='', plat_specific=True)
print('-- Found python site-packages directory {python_site_pkgs}')

setup(
    name='APBS',
    version=str(repo.tags[-1]),
    author='',
    author_email='',
    description='Adaptive Poisson-Boltzmann Solver',
    long_description=open('LICENSE.md', 'r').read(),
    ext_modules=[CMakeExtension(
        proj_root,
        ENABLE_PYTHON='ON',
        CMAKE_BUILD_TYPE='Release',
        PYTHON_SITE_PACKAGES_DIR=python_site_pkgs)],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
    )
