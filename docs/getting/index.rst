.. _registering: http://eepurl.com/by4eQr
.. _SourceForge: http://sourceforge.net/projects/apbs
.. _GitHub releases: https://github.com/Electrostatics/apbs/releases
.. _Visual C++ Redistributable Package:  https://aka.ms/vs/16/release/vc_redist.x64.exe

============
Getting APBS
============

.. note::

   *Before you begin!* APBS funding is dependent on your help for continued development and support. Please `register <http://eepurl.com/by4eQr>`_ before using the software so we can accurately report the number of users to our funding agencies.

-----------
Web servers
-----------

Most functionality is available through our online web servers.

The web server offers a simple way to use both APBS and PDB2PQR without the need to download and install additional programs.

After `registering`_, please visit http://server.poissonboltzmann.org/ to access the web server.

-------------------------------------
Installing from pre-compiled binaries
-------------------------------------

The best way to install APBS is via download of a pre-compiled binary from `SourceForge`_ or `GitHub releases`_ (after `registering`_, of course).

.. caution::

  On Windows 10, if you get a popup error when running the APBS binaries, you will need to install the `Visual C++ Redistributable Package`_

.. caution:: 

  On Linux and MacOS, you may need to set your LD_LIBRARY_PATH and PATH environment variables:
  For example, in bash with APBS installed in $HOME/apbs, you would set the environment variables in your .bashrc like:
  .. code-block:: bash

    export LD_LIBRARY_PATH=$HOME/apbs/lib:${LD_LIBRARY_PATH}
    export PATH=$HOME/apbs/bin:${PATH}

^^^^^^^^^^^^^^^^^^
What's in the box?
^^^^^^^^^^^^^^^^^^

The binary distributions typically provide the following contents:

bin
  contains the main APBS executable
share/apbs
  contains additional APBS-related files
doc
  the APBS programmer guide
examples
  APBS examples
tests
  the APBS test suite
tools
  useful programs to help process APBS input and output
include
  header files for building software that calls APBS
lib
  libraries for building software that calls APBS


---------------------------
Installing from source code
---------------------------

Those who enjoy an adventure can download the source code from `GitHub releases`_ and install from source code following the directions at the link below:

.. toctree::
   :maxdepth: 2

   source

------------------------
Current platform support
------------------------

+------------+---------+---------+------------+------+-------+------+---------+-------------+
| OS         | PYTHON  | GEOFLOW | BEM,       | FETK | PBSAM | PBAM | PYTHON  | SHARED_LIBS |
|            | VERSION |         | NanoShaper |      |       |      | SUPPORT |             |
+============+=========+=========+============+======+=======+======+=========+=============+
| Ubuntu     | 3.7+    | Yes     | Yes        | Yes  | Yes   | Yes  | Yes     | Yes         |
+------------+---------+---------+------------+------+-------+------+---------+-------------+
| MacOSX     | 3.7+    | Yes     | Yes        | Yes  | Yes   | Yes  | Yes     | Yes         |
+------------+---------+---------+------------+------+-------+------+---------+-------------+
| Windows 10 | 3.7+    | Yes     | Yes        | Yes  | No    | Yes  | No      | No          |
+------------+---------+---------+------------+------+-------+------+---------+-------------+
