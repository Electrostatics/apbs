APBS - Adaptive Poisson-Boltzmann Solver
========================================

[![Documentation Status](https://readthedocs.org/projects/apbs/badge/?version=latest)](https://apbs.readthedocs.io/en/latest/?badge=latest)
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/github/Electrostatics/apbs?branch=master&svg=true)](https://ci.appveyor.com/project/intendo/apbs)
[![Github Action Build Status](https://github.com/Electrostatics/apbs/workflows/Build/badge.svg)](https://github.com/Electrostatics/apbs/actions)

This repository contains the APBS software.

For more information about APBS, please see

* Home page:  http://www.poissonboltzmann.org/
* Documentation: http://apbs.readthedocs.io

### Python Package

*Note: under heavy devlopment*

Development dependencies:

- SWIG >4.0
- GCC capable of C++17 (recommended >=7.5.0)
- pip >=19

To install the python package, use the following:

```shell
$ python setup.py build
$ python setup.py install
```

To use the python module:

```python
>>> import apbs
>>>
>>> # access bits of APBS already ported to python
>>> from apbs import (
>>>     chemistry,
>>>     geometry,
>>>     grid,
>>>     multigrid,
>>>     pqr,
>>>     )
>>>
>>> # To access swig bindings to the C code:
>>> from apbs.bindings.swig import *
>>>
>>> # To access pybind bindings to the C code:
>>> from apbs.bindings.pybind import *
>>>
>>> # To get path to binaries:
>>> apbs.bin.get_path()
>>>
>>> # To get path to libraries:
>>> apbs.lib.get_path()
```

Or, to run apbs binaries directly without having to change your path:

```shell
$ # run apbs on the input file `input.mol`, and 
$ python -m apbs apbs input.mol
$
$ # return the help message for the apbs binary.
$ python -m apbs apbs --help
$
$ # view available binaries
$ python -m apbs --list
````
