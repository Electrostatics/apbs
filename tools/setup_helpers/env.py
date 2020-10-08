import os
import platform
import struct
import sys
from itertools import chain


IS_WINDOWS = (platform.system() == 'Windows')
IS_DARWIN = (platform.system() == 'Darwin')
IS_LINUX = (platform.system() == 'Linux')

IS_CONDA = 'conda' in sys.version or 'Continuum' in sys.version or any([x.startswith('CONDA') for x in os.environ])
CONDA_DIR = os.path.join(os.path.dirname(sys.executable), '..')

IS_64BIT = (struct.calcsize("P") == 8)


def check_env_flag(name, default=''):
    return os.getenv(name, default).upper() in ['ON', '1', 'YES', 'TRUE', 'Y']


def check_negative_env_flag(name, default=''):
    return os.getenv(name, default).upper() in ['OFF', '0', 'NO', 'FALSE', 'N']


def gather_paths(env_vars):
    return list(chain(*(os.getenv(v, '').split(os.pathsep) for v in env_vars)))


def lib_paths_from_base(base_path):
    return [os.path.join(base_path, s) for s in ['lib/x64', 'lib', 'lib64']]

# Set some platform-agnostic environment variables
os.environ['VERBOSE'] = '1'

# We promised that CXXFLAGS should also be affected by CFLAGS
if 'CFLAGS' in os.environ and 'CXXFLAGS' not in os.environ:
    os.environ['CXXFLAGS'] = os.environ['CFLAGS']

_build_dir_name = 'build'
if not os.path.exists(_build_dir_name):
    os.makedirs(_build_dir_name)
build_dir = os.path.realpath(os.path.join('.', _build_dir_name))
os.environ['BUILD_DIR'] = build_dir
