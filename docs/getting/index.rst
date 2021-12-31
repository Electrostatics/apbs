.. _registering: http://eepurl.com/by4eQr
.. _GitHub releases: https://github.com/Electrostatics/apbs/releases
.. _Visual C++ Redistributable Package:  https://aka.ms/vs/17/release/vc_redist.x86.exe

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

The best way to install APBS is via download of a pre-compiled binary from `GitHub releases`_ (after `registering`_, of course).

^^^^^^^^^^^^
Requirements
^^^^^^^^^^^^

A few libraries are needed to run APBS from the pre-built binary:

* All platforms
  * Python 3.9

.. caution:: 

  On Linux you may need to set your LD_LIBRARY_PATH and PATH environment variables:
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
include
  header files for building software that calls APBS
lib
  libraries for building software that calls APBS
share/apbs/docs
  the APBS documentation
share/apbs/examples
  APBS examples
share/apbs/tests
  the APBS test suite
share/apbs/tools
  useful programs to help process APBS input and output


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
| Ubuntu     | 3.9     | Yes     | Yes        | Yes  | Yes   | Yes  | Yes     | No          |
+------------+---------+---------+------------+------+-------+------+---------+-------------+
| MacOSX     | 3.9     | Yes     | Yes        | Yes  | Yes   | Yes  | Yes     | No          |
+------------+---------+---------+------------+------+-------+------+---------+-------------+
| Windows 10 | 3.9     | Yes     | Yes        | Yes  | Yes   | Yes  | No      | No          |
+------------+---------+---------+------------+------+-------+------+---------+-------------+
