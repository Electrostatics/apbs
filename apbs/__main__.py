import glob
import os
import apbs.bin
import sys
import subprocess

def usage():
    print('''
Usage: python -m apbs [binary] [arguments]

    For example, 

    $ python -m apbs apbs input.mol

    will run apbs on the input file `input.mol`, and 

    $ python -m apbs apbs --help

    will return the help message for the apbs binary.

    See available executables with:

    $ python -m apbs --list

    Or, from a python interpreter:
    
    >>> import apbs, os
    >>> binary_path = apbs.bin.get_path()
    >>> os.listdir(binary_path)
''')
    sys.exit(1)

if len(sys.argv) < 2:
    usage()

if '--list' in sys.argv:
    for i in os.listdir(apbs.bin.get_path()):
        if '__' not in i:
            print(i)

    sys.exit(0)

if '--help' in sys.argv or '-h' in sys.argv:
    usage()

bins = glob.glob(os.path.join(apbs.bin.get_path(), f'{sys.argv[1]}*'))

if len(bins) != 1:
    usage()

bin = bins[0]

if len(sys.argv) > 2:
    subprocess.run([bin, *sys.argv[2:]])
else:
    subprocess.run([bin,])
