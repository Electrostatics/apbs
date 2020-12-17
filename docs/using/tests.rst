==============================
APBS validation and test cases
==============================

This directory serves as the root directory for the APBS test suite.  This
directory contains python source files used for testing an an input file
containing the input files for apbs and the expected results for each test case.

The default input file is called `test_cases.cfg`, and the main testing program is called
`apbs_tester.py`.

-----
Usage
-----

.. note::

  It is important that you run the tests from the command line and that you run them from within the `INSTALL_DIR/share/apbs/tests` directory.


A usage description for `apbs_tester.py` can be obtained by running:

.. code-block:: console

   python3 apbs_tester.py [options]

-------------
Test Sections
-------------

The sections of the test file, :file:`test_cases.cfg`, follow the following format::

  [Some-Target_Test]
  input_dir     : ../path/to/some-example
  some-forces   : forces
  some-input    : * * 1.0E+01 2.0E+02

where:

* The first element in brackets :mailheader:`[Some-Target_Test]` describes the name of the *target_test* section.

* After the first element, the remaining elements are *property*/*value* pairs

* The first property is the :makevar:`input_dir`.
  This is the location of all input files reference in other properties.

* A property has a *name* that is also the basename of the input file concatenated with the file extension, :file:`{input-file}.in`.

* The property name will also be used for the output from :file:`apbs some-input.in` to create the output file, :file:`some-input.out`

* If the *value* of the property is :makevar:`forces` a forces test will be run.

* If the *value* of the property is a list of floats, these are expected outputs.

* If a :makevar:`*` is used in place of a float, the output will be ignored. Some test cases have multiple outputs.
  The test function parses each of these, but if a :makevar:`*` is used, the output will be ignored in testing.
  Most often, the first outputs are intermediate followed by a final output, and the test case is only concerned with the final output.

--------
Examples
--------

The following will run the `apbs_tester.py` specifying the path to the apbs executable and using the geoflow section of the test_cases.cfg file:

.. code-block:: console

   python3 apbs_tester.py -e ${INSTALL_DIR}/bin/apbs -c test_cases.cfg -t geoflow

where :makevar:`INSTALL_DIR` is the path to your installation directory.
