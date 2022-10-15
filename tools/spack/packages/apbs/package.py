# Copyright 2013-2022 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)


from spack import *


class Apbs(CMakePackage):
    """
    APBS (Adaptive Poisson-Boltzmann Solver) solves the equations of continuum electrostatics
    for large biomolecular assemblages. This software was designed "from the ground up"
    using modern design principles to ensure its ability to interface with other computational
    packages and evolve as methods and applications change over time. The APBS code is
    accompanied by extensive documentation for both users and programmers and is supported
    by a variety of utilities for preparing calculations and analyzing results.
    Finally, the free, open-source APBS license ensures its accessibility to the entire
    biomedical community.
    """

    # Homepage and Github URL.
    homepage = "https://www.poissonboltzmann.org/"
    url      = "https://github.com/Electrostatics/apbs/archive/refs/tags/v3.4.0.tar.gz"

    # List of GitHub accounts to notify when the package is updated.
    maintainers = ['thielblz', 'richtesn']

    # SHA256 checksum.
    version('3.4.0', sha256='572ff606974119430020ec948c78e171d8525fb0e67a56dad937a897cac67461')

    # Dependencies.
    depends_on('cmake@3.19', type='build')
    depends_on('python@3.8:3.10', type=('build', 'run'))
    depends_on('blas', type=('build', 'run'))
    depends_on('suite-sparse', type=('build', 'run'))
    depends_on('maloc', type=('build', 'run'))

    def cmake_args(self):
        # Min and max Python versions need to be set as variables to pass tests.
        # See tests/CMakeLists.txt lines 6-14.
        args = [
                '-DPYTHON_MIN_VERSION=3.8',
                '-DPYTHON_MAX_VERSION=3.10',
                ]
        return args
