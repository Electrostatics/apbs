"""Standardized functions to check the values of input variables."""
import logging


_LOGGER = logging.getLogger(__name__)


def is_number(value) -> bool:
    """Is this value a float or int?"""
    return isinstance(value, (float, int))


def is_positive_definite(value) -> bool:
    """Is this value a float or int that is strictly greater than zero?"""
    if isinstance(value, (float, int)):
        return value > 0
    else:
        return False


def is_positive_semidefinite(value) -> bool:
    """Is this value a float or int that is greater than or equal to zero?"""
    if isinstance(value, (float, int)):
        return value >= 0
    else:
        return False


def is_string(value) -> bool:
    """Is this value a string?"""
    return isinstance(value, str)


def is_list(value, length=None) -> bool:
    """Is this value a non-string list-like object?  If length is not ``None``,
    does the list have the specified length?"""
    if is_string(value):
        return False
    if length is not None:
        return len(value) == length
    return True


def is_bool(value) -> bool:
    """Is this value a Boolean?"""
    return isinstance(value, bool)
