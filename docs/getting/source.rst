.. _GitHub repository: https://github.com/Electrostatics/apbs
.. _GitHub releases: https://github.com/Electrostatics/apbs/releases

=============================
How to build APBS from source
=============================

These instructions assume that you have downloaded the source code from `GitHub releases`_.
Although it is possible to clone the code directly from our `GitHub repository`_, we do not recommend this approach as the head of the master branch is typically under development and unstable.

.. code:: bash

   git clone `GitHub repository`_

-----------------
Shortcut to build
-----------------

There is a script that is used to build APBS in the Github Actions. You may want to use the :file:`.build.sh`. file as a template for building APBS.

.. caution:: When using make, there can be race conditions with CMake, autoconf, downloading dependencies, and make. It is best to run 

.. code:: bash

   VERBOSE=1 make -j 1

-----------------
Import submodules
-----------------

We are using Git submodules to manage various pieces of code.  To build the master branch, after cloning it, you will need to do the following from within the top of the source directory:

.. code:: bash

   git submodule init
   git submodule update

------------
Set up CMake
------------

The basic command for configuring the APBS build is

.. code:: bash

   mkdir build
   cd build
   cmake ..

from the top of the source directory. 
This compiles a basic version of APBS.

To see all the options you can run:

.. code:: bash

   ccmake ..

Additional features can be built using the flags described below.

^^^^^^^^^^^^^^
Geometric flow
^^^^^^^^^^^^^^

If you want to use the geometric flow implementation, when invoking CMake, set :makevar:`ENABLE_GEOFLOW` to ``ON``; e.g.,

.. code:: bash

   cmake -DENABLE_GEOFLOW=ON ..

^^^^^^^^^^^
Using PB-AM
^^^^^^^^^^^

If you want to use the Poisson-Boltzmann Analytical Method developed by the Teresa Head-Gordon lab, set the CMake variable :makevar:`ENABLE_PBAM` to ``ON``.

.. warning::

   PB-AM currently runs on OS X or Linux only.

.. code:: bash

   cmake -DENABLE_PBAM=ON ..

^^^^^^^^^^^^^
Using TABI-PB
^^^^^^^^^^^^^

If you want to use the Treecode-Accelerated Boundary Integral method (TABI-PB) developed by Robert Krasny and Weihua Geng, set the CMake variable :makevar:`ENABLE_BEM` to ``ON``.

TABI-PB requires the use of a molecular surface mesh generation software to create a surface representation of the molecule.
By default, TABI-PB uses NanoShaper to generate an SES or Skin surface.
See `TABI-PB documentation <https://github.com/Treecodes/TABI-PB>`_ for details on choosing NanoShaper.
When TABI-PB runs, it will attempt to generate a surface mesh by looking in your path for the mesh generation executable.
A user can obtain the appropriate executable using the steps described below. The user then must place these executables in their path.

.. code:: bash

   cmake -DENABLE_BEM=ON ..

"""""""""""""""""""""""""""""
Getting NanoShaper executable
"""""""""""""""""""""""""""""

Surface meshing software executables are currently pre-built for OS X, Linux, and Windows and can be installed via CMake.
The executables will be placed in the :file:`bin` directory of your build.

NanoShaper is a molecular surface mesh generation software package developed by W. Rocchia and S. Decherchi.

.. code:: bash

   cmake -DGET_NanoShaper=ON ..

^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Using finite element support
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. warning::

   Finite element methods are currently only supported on POSIX-like operating systems such as OS X or Linux.

To enable finite element support, set the CMake :makevar:`ENABLE_FETK` variable to ``ON``.

On Linux, the FETK shared libraries need to be locatable by the shared library loader.
One way to do this is to update :makevar:`LD_LIBRARY_PATH` to point at :file:`<build-dir>/fetk/lib`, where ``<build-dir>`` is the location where APBS was built.
In base, this can be accomplished with the command:

.. code:: bash

   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<build-dir>/fetk/lib:<install-dir>/fetk/lib
   cmake -DENABLE_FETK=ON ..

^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Enabling APBS Python support
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

APBS Python support requires a local installation of `SWIG <http://www.swig.org/>`_.

Assuming SWIG is installed, APBS Python support can be enabled by setting the CMake variable :makevar:`ENABLE_PYTHON` to ``ON``.
If you are on Linux you will also need to set the CMake variable :makevar:`BUILD_SHARED_LIBS` to ``OFF``.

.. code:: bash

   cmake -DENABLE_PYTHON=ON ..

-----------------
Building the code
-----------------

Assuming the Cmake command completed successfully, APBS can be built with

.. code:: bash

   VERBOSE=1 make -j 1
