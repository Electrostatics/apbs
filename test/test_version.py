"""Tests for APBS version metadata."""
import pytest

import apbs
from apbs._version import VERSION_FILE, read_version_file


def test_version_matches_version_file():
    assert apbs.__version__ == read_version_file(VERSION_FILE)


def test_read_version_file_skips_comments(tmp_path):
    path = tmp_path / "VERSION"
    path.write_text("# comment 9_9_9 mentioned here\n12_3_45\n")
    assert read_version_file(path) == "12.3.45"


def test_read_version_file_requires_version_line(tmp_path):
    path = tmp_path / "VERSION"
    path.write_text("# no version here\n")
    with pytest.raises(ValueError):
        read_version_file(path)
