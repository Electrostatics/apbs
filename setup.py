"""Set up the APBS package."""
from runpy import run_path
from sys import version_info
from setuptools import find_packages, setup

# NOTE: Read the version from the VERSION file shared with CMake without
#       importing the apbs package or its dependencies.
__version__ = run_path("apbs/_version.py")["read_version_file"]()

# NOTE: The reason for version 3.7 vs. 3.8 is that ReadTheDocs
#       is running version 3.7 so setting this to 3.8 causes
#       the documentation build to fail.
if version_info[:2] < (3, 7):
    raise RuntimeError("Python version >= 3.7 is required.")

with open("README.md", "r") as fobj:
    LONG_DESCRIPTION = fobj.read()

setup(
    name="apbs",
    version=__version__,
    description="APBS biomolecular solvation software",
    long_description=LONG_DESCRIPTION,
    python_requires=">=3.7",
    author="Nathan Baker",
    author_email="nathanandrewbaker@gmail.com",
    url="https://www.poissonboltzmann.org",
    packages=find_packages(),
    package_data={"": ["*.yaml"]},
    install_requires=["numpy", "pyparsing", "pyyaml"],
    tests_require=["pytest", "requests"],
    keywords="science chemistry biophysics biochemistry",
    classifiers=[
        "Development Status :: 4 - Beta",
        "Environment :: Console",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: Common Public License",
        "Natural Language :: English",
        "Operating System :: MacOS",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Topic :: Scientific/Engineering :: Bio-Informatics",
        "Topic :: Scientific/Engineering :: Chemistry",
        "Topic :: Scientific/Engineering :: Physics",
    ],
)
