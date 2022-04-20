.. _GitHub repository: https://github.com/Electrostatics/apbs
.. _GitHub releases: https://github.com/Electrostatics/apbs/releases

=============================
How to build APBS from source
=============================

These instructions assume that you have downloaded the source code from `GitHub releases`_.

.. caution:: We do not recommend cloning directly from the head of the master branch because it is typically under development and could be unstable. Unless you really know what you are doing, we advise you to skip the next step.

-------------------------------
Get source directly from Github
-------------------------------

Here are the commands to get the source directly from our `GitHub repository`_, 

.. code:: bash

   git clone https://github.com/Electrostatics/apbs
   cd apbs

-----------------
Shortcut to build
-----------------

There is a script that is used to build APBS in the Github Actions. You may want to use the file, :file:`.build.sh`, as a template for building APBS.

-----------------
Import submodules
-----------------

*As of v3.4.0:* Submodules are only used for Pybind11, so this step is only required if building the Python interface.

We are using Git submodules to manage various pieces of code.  To build the master branch, after cloning it, you will need to do the following from within the top of the source directory:

.. code:: bash

   git submodule init
   git submodule update

------------
Set up CMake
------------

From the top of the source directory, the basic commands for configuring the APBS build for CMake are

.. code:: bash

   mkdir build
   cd build
   # NOTE: This will be you $APBS_BUILD_DIR
   export APBS_BUILD_DIR=`echo $(PWD)`
   cmake ..

To see all the options you can run:

.. code:: bash

   cd $APBS_BUILD_DIR
   ccmake ..

Additional features can be built using the flags described below.

^^^^^^^^^^^^^^
Geometric flow
^^^^^^^^^^^^^^

If you want to use the geometric flow implementation, when invoking CMake, set :makevar:`ENABLE_GEOFLOW` to ``ON``; e.g.,

.. code:: bash

   cd $APBS_BUILD_DIR
   cmake -DENABLE_GEOFLOW=ON ..

^^^^^^^^^^^
Using PB-AM
^^^^^^^^^^^

If you want to use the Poisson-Boltzmann Analytical Method developed by the Teresa Head-Gordon lab, set the CMake variable :makevar:`ENABLE_PBAM` to ``ON``.

.. warning::

   PB-AM currently runs on OS X or Linux only.

.. code:: bash

   cd $APBS_BUILD_DIR
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

   cd $APBS_BUILD_DIR
   cmake -DENABLE_BEM=ON ..

"""""""""""""""""""""""""""""
Getting NanoShaper executable
"""""""""""""""""""""""""""""

Surface meshing software executables are currently pre-built for OS X, Linux, and Windows and can be installed via CMake.
The executables will be placed in the :file:`bin` directory of your build.

NanoShaper is a molecular surface mesh generation software package developed by W. Rocchia and S. Decherchi.

.. code:: bash

   cd $APBS_BUILD_DIR
   cmake -DGET_NanoShaper=ON ..

^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Using finite element support
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*As of v3.4.0:* The Finite Element Toolkit, FETK, is required for building APBS.  
You can set the version of FETK used with the CMake variable :makevar:`FETK_VERSION`.
That variable will be set to a working default if not manually set.

.. code:: bash

   cd $APBS_BUILD_DIR
   cmake -DFETK_VERSION=v1.9.2

For advanced users, you can use a version of FETK other than a released version by setting ``FETK_VERSION``
to the desired git commit hash instead of a version number:

.. code:: bash

   cd $APBS_BUILD_DIR
   cmake -DFETK_VERSION=[git hash]


^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Enabling APBS Python support
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

APBS Python support requires a local installation of `SWIG <http://www.swig.org/>`_.

Assuming SWIG is installed, APBS Python support can be enabled by setting the CMake variable :makevar:`ENABLE_PYTHON` to ``ON``.
If you are on Linux you will also need to set the CMake variable :makevar:`BUILD_SHARED_LIBS` to ``OFF``.

.. code:: bash

   cd $APBS_BUILD_DIR
   cmake -DENABLE_PYTHON=ON ..

---------------------------
Building the code - minimal
---------------------------

Assuming the Cmake command completed successfully, APBS can be built with

.. code:: bash

   cd $APBS_BUILD_DIR
   # Run cmake with the options you prefer:
   make -j

----------------------------
Building the code - advanced
----------------------------

.. code:: bash

   export INSTALL_DIR=$SOME_DIR/apbs
   export PATH=$INSTALL_DIR/bin:$PATH
   # NOTE: In case you need to debug the source code:
   # export RELEASE_TYPE=Debug
   export RELEASE_TYPE=Release
   # NOTE: If cmake or make fail, save yourself and make sure your remove
   #       everything including the build directory. This code base uses
   #       many older autoconf based projects that do not know how to save
   #       state or recover from partial builds. If cmake or make fail, you
   #       should figure out how to fix it and then remove everything and
   #       try again.
   rmdir $APBS_BUILD_DIR
   mkdir -p $APBS_BUILD_DIR
   cd $APBS_BUILD_DIR
   # NOTE: In case you need to debug cmake, use verbose debug/trace mode:
   # cmake -S .. -B $BUILD_DIR --trace-source=../CMakeLists.txt --trace-expand \
   cmake                                        \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR       \
      -DCMAKE_BUILD_TYPE=$RELEASE_TYPE          \
      -DENABLE_GEOFLOW=ON                       \
      -DENABLE_BEM=ON                           \
      -DFETK_VERSION=[version]                  \
      -DENABLE_OPENMP=ON                        \
      -DENABLE_PBAM=ON                          \
      -DENABLE_PBSAM=ON                         \
      -DENABLE_PYTHON=ON                        \
      -DENABLE_TESTS=ON                         \
      -DBUILD_SHARED_LIBS=ON                    \
      ..
   make -j

------------
Testing APBS
------------

.. code:: bash

   cd $APBS_BUILD_DIR
   # NOTE: Assuming you have already built APBS
   # NOTE: So that the apbs and optional NanoShaper binaries are in the path:
   export PATH="$APBS_BUILD_DIR/bin:$PATH"
   ctest -C Release --output-on-failure

---------------
Installing APBS
---------------

.. code:: bash

   export INSTALL_DIR="Some directory - default is /usr/local"
   cd $APBS_BUILD_DIR
   cmake                                  \
      -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
      # NOTE: Add cmake options that you used during the Build APBS section
   ..
   make -j install
