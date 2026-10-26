"""Regression tests for the legacy tools/manip/psize.py script."""
import importlib.util
from pathlib import Path

PSIZE_PATH = Path(__file__).resolve().parent.parent / "tools/manip/psize.py"


def load_psize():
    spec = importlib.util.spec_from_file_location("legacy_psize", PSIZE_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def test_set_smallest_reduces_to_integer_grid():
    psize = load_psize().Psize()
    psize.constants["gmemceil"] = 10
    nsmall = psize.setSmallest([161, 161, 161])
    assert all(isinstance(n, int) for n in nsmall)
    assert all((n - 1) % 32 == 0 for n in nsmall)
    assert 200.0 * nsmall[0] * nsmall[1] * nsmall[2] / 1024 / 1024 < 10
