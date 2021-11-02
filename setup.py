"""Set up the APBS package."""
from sys import version_info
from setuptools import find_packages, setup

# NOTE: The following reads the version number and makes
#       if available to the packaging tools before installation.
#       REF: https://stackoverflow.com/questions/458550/standard-way-to-embed-version-into-python-package  # noqa: E501
#       This makes __version__ valid below
with open("apbs/_version.py") as fobj:
    exec(fobj.read())

# NOTE: The reason for version 3.7 vs. 3.8 is that ReadTheDocs
#       is running version 3.7 so setting this to 3.8 causes
#       the documentation build to fail.
if version_info[:2] < (3, 7):
    raise RuntimeError("Python version >= 3.7 is required.")

with open("README.md", "r") as fobj:
    LONG_DESCRIPTION = fobj.read()

setup(
    name="apbs",
    version=__version__,  # noqa: F821
    description="APBS biomolecular solvation software",
    long_description=LONG_DESCRIPTION,
    python_requires=">=3.7",
    author="Nathan Baker",
    author_email="nathanandrewbaker@gmail.com",
    url="https://www.poissonboltzmann.org",
    packages=find_packages(),
    package_data={"": ["*.yaml"]},
    install_requires=["numpy", "pyyaml"],
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
        "Topic :: Scientific/Engineering :: Bio-Informatics",
        "Topic :: Scientific/Engineering :: Chemistry",
        "Topic :: Scientific/Engineering :: Physics",
    ],
)
