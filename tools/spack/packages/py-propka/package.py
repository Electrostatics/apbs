
# Copyright 2013-2022 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)


from spack import *


class PyPropka(PythonPackage):
    """
    PROPKA predicts the pKa values of ionizable
    groups in proteins (version 3.0) and protein-ligand
    complexes (version 3.1 and later) based on the 3D
    structure.
    For proteins without ligands both version should
    produce the same result.
    """

    # Url for the package's homepage.
    homepage = "http://propka.org/"
    pypi     = "propka/propka-3.4.0.tar.gz"

    # List of GitHub accounts to
    # notify when the package is updated.
    maintainers = ['richtesn', 'thielblz']

    version('3.4.0', sha256='f19c2e145faa7eab000ce056a9058b8a30adba6970705047bb4fb7ba7570b020')

    depends_on('py-setuptools', type=('build','run'))

