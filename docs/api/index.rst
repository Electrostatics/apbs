.. _api-label:

=============
API reference
=============

.. currentmodule:: apbs
.. module:: apbs

The :program:`apbs` command provides a command-line interface to APBS's functionality.
It is built on classes and functions in the :mod:`apbs` module.
The API (application programming interface) of :mod:`apbs` is documented here for developers who might want to directly use the APBS code.

.. Note::

   The API is still changing and there is currently no guarantee that
   it will remain stable between minor releases.

.. todo:: Add API documentation for all new Python routines

------------------
Input file support
------------------

.. todo:: Eventually move this into the main documentation

APBS accepts either `JSON <json.org>`_- or `YAML <yaml.org>`_-format input files.

These input files consist of the following keywords and objects:

* ``read``:  :ref:`read_input_file`

.. todo:: finish other required and optional sections

