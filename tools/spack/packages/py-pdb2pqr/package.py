# Copyright 2013-2022 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)


from spack import *


class PyPdb2pqr(PythonPackage):
    """
    PDB2PQR - determining titration states,
    adding missing atoms, and assigning
    charges/radii to biomolecules.
    """

    # Url for the package's homepage.
    homepage = "http://www.poissonboltzmann.org/"
    pypi     = "pdb2pqr/pdb2pqr-3.5.2.tar.gz"

    # List of GitHub accounts to
    # notify when the package is updated.
    maintainers = ['richtesn', 'thielblz']

    version('3.5.2', sha256='9d145ff3797a563ce818f9d2488413ac339f66c58230670c2455b2572cccd957')
    
    depends_on('python@3.8:', type=('build','run'))
    depends_on('py-docutils@:0.18', type=('build','run'))
    depends_on('py-mmcif-pdbx@1.1.2:', type=('build','run'))
    depends_on('py-numpy', type=('build','run'))
    depends_on('py-propka@3.2:', type=('build','run'))
    depends_on('py-requests', type=('build','run'))
    depends_on('py-setuptools', type=('build','run'))

