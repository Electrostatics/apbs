import os
import re
import sys
import platform
import subprocess

from tools.setup_helpers.cmake_setuptools import *
from tools.setup_helpers.extra_setuptools_commands import *
from setuptools import setup, find_packages

proj_name = 'APBS'

if not os.path.exists(os.path.join('.', __file__)):
    print('-- Must run setup.py in project root directory!')
    sys.exit()

if os.path.exists('MANIFEST'):
    os.remove('MANIFEST')

proj_root = os.path.dirname(os.path.abspath(__file__))
old_path = os.getcwd()
os.chdir(proj_root)
sys.path.insert(0, proj_root)

from distutils import sysconfig as sc
python_site_pkgs = sc.get_python_lib(prefix='', plat_specific=True)
print(f'-- Found python site-packages directory {python_site_pkgs}')

extra_cmake_args = dict(
    ENABLE_PYTHON='ON',
    CMAKE_BUILD_TYPE='Release',
    PYTHON_SITE_PACKAGES_DIR=python_site_pkgs,
    )

setup(
    name=proj_name,
    version='3.0.0',
    author='',
    author_email='',
    description='Adaptive Poisson-Boltzmann Solver',
    long_description=open('LICENSE.md', 'r').read(),
    ext_modules=[CMakeExtension(
        f'{proj_root}.apbs.bindings.pybind',
        **extra_cmake_args)],
    cmdclass={
        'build_ext': CMakeBuild,
        'install': CMakeInstall,
        'develop': CMakeInstall,
        'clean': CleanBuild,
        'sdist': SDistChecked,
        },
    zip_safe=False,
    packages=find_packages(include=['apbs', 'apbs.*']),
    package_dir={
        'apbs': str(os.path.join('.', 'apbs')),
        },
    package_data={
        proj_name: [
            'build/bin/*'
            'build/lib/*'
            ]
        },
    classifiers=[
        'Programming Language :: C++',
        'Programming Language :: Python :: 3',
        ] + ['Programming Language :: Python :: 3.{}' for i in range(5, 8)],
    )
