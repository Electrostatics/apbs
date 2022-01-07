#!/usr/bin/env python

"""
Check intermediate computed energies form an APBS run
"""

import sys
import re
from apbs_logger import Logger

ERROR_TOLERANCE = 1e-6


class ElecEnergy:
    """
    extracts and compares computed intermediate energies
    """

    pattern = r"\s(?P<label>[a-zA-Z]+ [a-zA-Z]+ = )[\-]?(?P<x>[0-9]+(\.[0-9]+)?(E[\+,\-][0-9]+)?)"  # noqa E501

    def __init__(self, label, x):
        """
        creates an elecEnergy from supplies values
        """
        self.label = label
        self.x = x

    def __init__(self, line):  # noqa F811
        """
        extract energy results from a file at a given line
        """
        m = re.search(self.pattern, line)
        self.label = m.group("label")
        self.x = float(m.group("x"))

    def __repr__(self):
        return f"ElecEnergy label:{self.label}, x:{self.x}"

    def diff(self, other):
        """
        compares the value of two energy calculations
        """
        return abs(getattr(self, self.x) - getattr(other, other.x))

    def short(self):
        """
        Return the label
        """
        return self.label


def extract_energy(energy_class, lines, start_pattern):
    """
    extracts intermediate energies
    """
    energy_list = []

    for line in lines:
        if line.lstrip().startswith(start_pattern):
            if re.search(energy_class.pattern, line):
                energy_list.append(parse_energy(energy_class, line))

    return energy_list


def parse_energy(energy_class, lines):
    """
    Parse energy
    """
    energy_item = energy_class(lines)

    return energy_item.x


def check_energies(input_file):
    """
    check energies
    """
    print(f"Checking for intermediate energies in input file {input_file}")

    f = None
    try:
        f = open(input_file, "r")
    except IOError:
        print(f"Couldn't read from energy file {input_file}", file=sys.stderr)
        raise

    input_lines = f.readlines()

    energy_list = extract_energy(
        ElecEnergy, input_lines, "Total electrostatic energy"
    )
    energy_list += extract_energy(
        ElecEnergy, input_lines, "Mobile charge energy"
    )
    energy_list += extract_energy(
        ElecEnergy, input_lines, "Fixed charge energy"
    )
    energy_list += extract_energy(ElecEnergy, input_lines, "Dielectric energy")

    return energy_list


def test():
    """
    Run the test
    """
    lval = open("energy.log", "w")
    logger = Logger(sys.stderr, lval)  # noqa F841
    energy_list = check_energies("actio_stdout.txt")
    print(energy_list)


if __name__ == "__main__":
    print(f"The python source file {sys.argv[0]} is a module and not runnable")
    sys.exit(1)
