#pragma once

#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
extern "C"
{
#include "apbscfg.h" 
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
 * @todo request documentation for this
 */
void Valist_load(Valist *self,
                 int size,
                 std::vector<double> x,
                 std::vector<double> y,
                 std::vector<double> z,
                 std::vector<double> chg,
                 std::vector<double> rad);

/**
 * @brief Perform binding to module
 */
void bind_valist(py::module& m);
