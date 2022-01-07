#! /usr/bin/env python

"""
Provides functions for verifying results from a test run
"""

import sys
from math import log10, floor
from apbs_logger import Logger  # noqa F401

ERROR_TOLERANCE = 1e-4


def round_sigfigs(x, sigfigs):
    """
    Rounds a number to a specified number of significant figures
    """
    value = 0.0
    try:
        value = round(x, sigfigs - int(floor(log10(abs(x)))) - 1)
    except ValueError as msg:
        sys.stderr.write(f"The value ({x}) caused an error: {msg}")
        raise
    return value


def check_results(computed_result, expected_result, input_file, logger, ocd):
    """
    Compares computed results to an expected results within some margin of
    error
    """

    # OCD mode requires a match up to 12 significant figures
    if ocd:
        computed_result = round_sigfigs(computed_result, 12)
        expected_result = round_sigfigs(expected_result, 12)

    # Non-OCD mode only requires a match out to six significan figures
    else:
        computed_result = round_sigfigs(computed_result, 6)
        expected_result = round_sigfigs(expected_result, 6)

    # Compute the error in the calculation
    error = abs((computed_result - expected_result) / expected_result * 100.0)

    # An exact match after rounding to specifiec precision means the test
    # passed
    if computed_result == expected_result:
        logger.message("*** PASSED ***\n")
        logger.log(f"PASSED {computed_result:.12e}\n")

    # Otherwise, test that the error is below error tolerance
    elif error < ERROR_TOLERANCE * 100:
        logger.message("*** PASSED (with rounding error - see log) ***\n")
        logger.log(
            f"PASSED within error ({computed_result:.12e}; "
            + f"expected {expected_result:.12e}; {error}% error)\n"
        )

    # If neither is true, the test failed
    else:
        logger.message("*** FAILED ***\n")
        logger.message(f"   APBS returned      {computed_result:.12e}\n")
        logger.message(
            f"   Expected result is {expected_result:.12e} ({error}% error)\n"
        )
        logger.log(
            f"FAILED ({computed_result:.12e}; "
            + f"expected {expected_result:.12e}; {error}% error)\n"
        )
        raise RuntimeError


if __name__ == "__main__":
    sys.stderr.write(
        f"The python source file {sys.argv[0]} is a module and not runnable"
    )
    sys.exit(1)
