#pragma once

#include <pybind11/pybind11.h>
namespace py = pybind11;

extern "C"
{
#include "generic/valist.h"
}

/**
 * @file tools/python-pybind/bind_valist.hpp
 * @author Asher Mancinelli <asher.mancinelli@pnnl.gov>
 * @brief Contains bindings for Valist-related functions.
 *
 * @note keep all implementations in the impl unless templated.
 * @note contains bindings for nosh and all classes encapsulated by this struct
 * within the source.
 *
 * @see src/generic/valist.h:195
 */

/**
 * @brief Perform binding of _Vatom_ to module
 */
void bind_vatom(py::module& m);
