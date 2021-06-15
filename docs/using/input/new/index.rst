.. _new_input_format:

=================================
YAML- and JSON-format input files
=================================

In its new input format, APBS accepts either `JSON <json.org>`_- or `YAML <yaml.org>`_-format input files.

These input files consist of the following keywords and objects:

.. toctree::
   :maxdepth: 3

   read
   calculate/index
   process

.. todo:: finish other required and optional sections

^^^^^^^^^^^^^^^^^^^^^^^^^^
Input file class structure
^^^^^^^^^^^^^^^^^^^^^^^^^^

The input file object parsing and validation follows the basic pattern implemented in :class:`apbs.input_file.InputFile` (see below).
This class should serve as a template for adding new input file sections.

.. automodule::  apbs.input_file
   :members:
   :undoc-members:
   :show-inheritance: