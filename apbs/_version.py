"""APBS Version number.

The canonical version is the ``MAJOR_MINOR_MICRO`` line in the repository's
:file:`VERSION` file, which is also used by CMake.
Keep this module free of package dependencies so :file:`setup.py` and the
documentation can import it.
"""
import re
from pathlib import Path

VERSION_FILE = Path(__file__).resolve().parent.parent / "VERSION"


def read_version_file(path=VERSION_FILE):
    """Return the dotted version from an APBS :file:`VERSION` file.

    :param path: path to the :file:`VERSION` file
    :return: version string such as ``3.4.1``
    :raises ValueError: if no ``MAJOR_MINOR_MICRO`` line is found
    """
    for line in Path(path).read_text().splitlines():
        match = re.fullmatch(r"\s*(\d+)_(\d+)_(\d+)\s*", line)
        if match:
            return ".".join(match.groups())
    raise ValueError(f"No MAJOR_MINOR_MICRO version line in {path}")


def _get_version():
    if VERSION_FILE.is_file():
        return read_version_file()
    try:
        from importlib.metadata import PackageNotFoundError, version
    except ImportError:  # Python < 3.8
        return "unknown"
    try:
        return version("apbs")
    except PackageNotFoundError:
        return "unknown"


__version__ = _get_version()
