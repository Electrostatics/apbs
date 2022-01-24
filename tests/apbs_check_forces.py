#! /usr/bin/env python

"""
Checks computed forces from an apbs run
"""

import sys
import re
from apbs_logger import Logger

ERROR_TOLERANCE = 1e-6


class PolarForce:
    """
    Exctracts and compares computations of polar forces
    """

    # A crazy regex pattern used to match label/value sets
    patterns = [
        r"\s+",
        r"(?P<label>[a-zA-Z]+)\s+",
        r"(?P<x>[+-]?\d\.\d+E[+-]\d+)\s+",
        r"(?P<y>[+-]?\d\.\d+E[+-]\d+)\s+",
        r"(?P<z>[+-]?\d\.\d+E[+-]\d+)",
    ]
    pattern = "".join(patterns)

    def __init__(self, label, x, y, z):
        """
        Constructs a polar force result from supplied values
        """
        self.label = label
        self.x = x
        self.y = y
        self.z = z

    def __init__(self, line):  # noqa F811
        """
        Extracts ploar force results from a file at a given line
        """
        try:
            m = re.search(self.pattern, line)
            self.label = m.group("label")
            self.x = float(m.group("x"))
            self.y = float(m.group("y"))
            self.z = float(m.group("z"))
        except Exception as msg:
            sys.stderr.write(f"\nLINE:{line}\nException:{msg}\n")
            raise msg

    def diff(self, other):
        """
        Compares the value of two polar force field results
        """

        diff_dict = {}
        for d in ("x", "y", "z"):
            diff_dict[d] = abs(getattr(self, d) - getattr(other, d))
        return diff_dict

    def __repr__(self):
        """
        Get the PolarForce
        """
        return "PolarForce{ label:%s, x:%g, y:%g, z:%g}\n" % (
            self.label,
            self.x,
            self.y,
            self.z,
        )

    def short(self):
        """
        Get the label
        """
        return self.label


class ApolarForce(PolarForce):
    """
    Exctracts and compares computations of apolar forces
    """

    # A crazy regex pattern used to match label/value sets
    patterns = [
        r"\s+",
        r"(?P<label>[a-zA-Z]+)\s+",
        r"(?P<atom>\w+)\s+",
        r"(?P<x>[+-]?\d\.\d+E[+-]\d+)\s+",
        r"(?P<y>[+-]?\d\.\d+E[+-]\d+)\s+",
        r"(?P<z>[+-]?\d\.\d+E[+-]\d+)",
    ]
    pattern = "".join(patterns)

    def __init__(self, label, atom, x, y, z):
        """
        Constructs an apolar force result from supplied values
        """
        super(ApolarForce, self).__init__(self, x, y, z)
        self.label = label

    def __init__(self, line):  # noqa F811
        """
        Extracts aploar force results from a file at a given line
        """
        try:
            m = re.search(self.pattern, line)
            self.label = m.group("label")
            self.atom = m.group("atom")
            self.x = float(m.group("x"))
            self.y = float(m.group("y"))
            self.z = float(m.group("z"))
        except Exception as msg:
            sys.stderr.write(f"\nLINE:{line}\nException:{msg}\n")
            raise msg

    def __rpr__(self):
        return "ApolarForce{ label:%s, atom:%s, x:%g, y:%g, z:%g}\n" % (
            self.label,
            self.atom,
            self.x,
            self.y,
            self.z,
        )

    def short(self):
        return f"{self.label} for self.atom"


def extract_forces(force_class, lines, start_pattern):
    """
    Extracts force results
    """
    # force_dict = {}
    in_section = False
    in_forces = False
    start_line = -1
    end_line = -1
    for line_number, line_text in enumerate(lines):
        if not in_section:
            if line_text.startswith(start_pattern):
                in_section = True
        if in_section and not in_forces:
            if re.search(force_class.pattern, line_text):
                in_forces = True
                start_line = line_number
        if in_section and in_forces:
            if not re.search(force_class.pattern, line_text):
                end_line = line_number
                break
    return parse_forces(force_class, lines[start_line:end_line])


def parse_forces(force_class, lines):
    """
    parse forces into a dictionary
    """
    force_dict = {}
    for line in lines:
        force_item = force_class(line)
        force_dict[force_item.label] = force_item
    return force_dict


def compare_force_dicts(test_force_dict, true_force_dict, logger):
    """
    Compares force dictionaries
    """

    for force_key in test_force_dict.keys():
        test_force = test_force_dict[force_key]
        true_force = true_force_dict[force_key]
        diff_dict = test_force.diff(true_force)

        for (diff_key, diff_value) in diff_dict.items():
            test_value = getattr(test_force, diff_key)
            true_value = getattr(true_force, diff_key)

            if diff_value == 0.0:
                logger.message(
                    f"*** Comparison {test_force.short()} in {diff_key} "
                    + "PASSED ***"
                )
                logger.log(
                    f"Comparison {test_force.short()} in {diff_key} PASSED "
                    + f"({test_value})"
                )
            elif diff_value < ERROR_TOLERANCE:
                logger.message(
                    f"*** Comparison {test_force.short()} in {diff_key} "
                    + "PASSED (with rounding error - see log)***"
                )
                logger.log(
                    f"Comparison {test_force.short()} in {diff_key} "
                    + f"PASSED within error ({test_value}; "
                    + f"expected {true_value})"
                )
            else:
                logger.message(
                    f"*** Comparison {test_force.short()} in {diff_key} "
                    + "FAILED ***"
                )
                logger.message(f"   APBS returned {test_value}")
                logger.message(
                    f"   Expected result is {true_value} (difference of: "
                    + f"{diff_value})"
                )
                logger.log(
                    f"Comparison {test_force.short()} in {diff_key} "
                    + f"FAILED ({test_value}; expected {true_value})"
                )


def check_forces(input_file, polar_file, apolar_file, logger):
    """
    Check the forces
    """
    logger.both(f"Checking forces for input file {input_file}")

    f = None
    try:
        f = open(input_file, "r")
    except IOError:
        sys.stderr.write(f"Couldn't read from forces file {input_file}")
        raise
    input_lines = f.readlines()

    test_polar_force_dict = extract_forces(
        PolarForce, input_lines, "print force"
    )
    test_apolar_force_dict = extract_forces(
        ApolarForce, input_lines, "print APOL force"
    )

    try:
        f = open(polar_file, "r")
    except IOError:
        sys.stderr.write(f"Couldn't read from forces file {input_file}")
        raise
    input_lines = f.readlines()
    true_polar_force_dict = parse_forces(PolarForce, input_lines)

    try:
        f = open(apolar_file, "r")
    except IOError:
        sys.stderr.write(f"Couldn't read from forces file {input_file}")
        raise
    input_lines = f.readlines()
    true_apolar_force_dict = parse_forces(ApolarForce, input_lines)

    logger.both("Checking Polar Forces")
    compare_force_dicts(test_polar_force_dict, true_polar_force_dict, logger)

    logger.both("Checking Apolar Forces")
    compare_force_dicts(test_apolar_force_dict, true_apolar_force_dict, logger)


def test():
    """
    test
    """
    lval = open("forces.log", "w")
    logger = Logger(sys.stderr, lval)
    check_forces("apbs-forces.out", "polarforces", "apolarforces", logger)


if __name__ == "__main__":
    sys.stderr.write(
        f"The python source file {sys.argv[0]} is a module and not runnable"
    )
    sys.exit(1)
