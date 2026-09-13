"""Smoke tests for the installed APBS Python extension modules."""

import ast
from importlib import import_module
from pathlib import Path


def load_example_input():
    """Load the input string without executing the legacy driver."""
    source = Path(__file__).with_name("noinput.py").read_text(encoding="utf-8")
    tree = ast.parse(source)
    for node in tree.body:
        if not isinstance(node, ast.Assign):
            continue
        if any(
            isinstance(target, ast.Name) and target.id == "INPUT"
            for target in node.targets
        ):
            return ast.literal_eval(node.value)
    raise RuntimeError("Could not find INPUT in noinput.py")


def main():
    """Parse the same APBS input through both binding implementations."""
    apbs = import_module("apbs")
    apbslib = import_module("apbslib")
    input_text = load_example_input()

    legacy_nosh = apbslib.NOsh_ctor(0, 1)
    try:
        assert apbslib.parseInputFromString(legacy_nosh, input_text) == 1
    finally:
        apbslib.delete_Nosh(legacy_nosh)

    modern_nosh = apbs.NOsh()
    assert modern_nosh.parseInputFromString(input_text) == 1


if __name__ == "__main__":
    main()
