import os
import sys
import platform
import subprocess
import glob
from typing import Optional
from shutil import rmtree
from setuptools import setup, Extension, Command
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from setuptools.command.develop import develop
import setuptools
from distutils.version import LooseVersion

from .env import *


def get_build_dir() -> Optional[str]:
    build_dir = os.environ.get('BUILD_DIR')
    if not build_dir:
        print('-- BUILD_DIR not set in environment. This is likely a'
                'configuration error. Please contact the maintainers.')
        sys.exit(1)

    return build_dir


def replace_run(cls):
    '''Replace run command with custom cmake hook'''

    old_run = cls.run

    def run(self):

        build_dir = os.environ.get('BUILD_DIR')
        if not build_dir:
            print('-- BUILD_DIR not set in environment. This is likely a'
                'configuration error. Please contact the maintainers.')
            sys.exit(1)

        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError('CMake must be installed to build this module.')

        if not os.access(sys.prefix, os.W_OK):
            raise PermissionError('setup.py does not have write access to install prefix')

        subprocess.check_call(
                ['cmake', '--install', '.'],
                cwd=build_dir)

        old_run(self)

    cls.run = run

    return cls

def pip_install():
    print('-- Ensuring required pip modules are installed')
    subprocess.check_call([
        sys.executable,
        '-m',
        'pip',
        'install',
        '--trusted-host', 'pypi.org',
        '--trusted-host', 'files.pythonhosted.org',
        '--upgrade',
        'pip',
        'setuptools'
        ])
    subprocess.check_call([
        sys.executable,
        '-m',
        'pip',
        'install',
        '-r',
        'requirements.txt'
        ])

def init_submodules():
    """ verify that the submodules are checked out and clean
        use `git submodule update --init`; on failure
    """
    if not os.path.exists('.git'):
        return
    with open('.gitmodules') as f:
        for line in f:
            if 'path' in line:
                p = line.split('=')[-1].strip()
                if not os.path.exists(p):
                    raise ValueError('Submodule {} missing'.format(p))

    proc = subprocess.Popen(['git', 'submodule', 'status'],
                            stdout=subprocess.PIPE)
    status, _ = proc.communicate()
    status = status.decode("ascii", "replace")
    for line in status.splitlines():
        if line.startswith('-') or line.startswith('+'):
            raise ValueError('Submodule not clean: {}'.format(line))

def generate_manifest():
    print('-- Generating MANIFEST.in')
    output = subprocess.run(['git', 'ls-files'], check=True, capture_output=True)
    with open('MANIFEST.in', 'w') as f:
        for line in output.stdout.decode('utf-8').split('\n'):
            if os.path.isfile('./' + line):
                f.write('include ' + line + '\n')
        f.write('recursive-include externals *\n')

