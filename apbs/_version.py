"""APBS Version number.
Store the version here so:

* we don't load dependencies by storing it in :file:`__init__.py`
* we can import it in setup.py for the same reason
* we can import it into your module
"""

file = open('../VERSION', 'r')
lines = file.readlines()
verLine = [x for x in lines if x and not x.startswith('#')]
__version__ = verLine[0].strip().replace('_', '.')
