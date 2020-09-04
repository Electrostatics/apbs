#include <pybind11/pybind11.h>

#include "bind_nosh.hpp"

/**
 * @file tools/python-pybind/module.cpp
 * @author Asher Mancinelli <asher.mancinelli@pnnl.gov>
 *
 * @brief Glue code that binds each function/class to python
 *
 * @note Keep all binding functions in their own header/impl pair. No raw
 * functions should live in this file; this is for *binding only*.
 */

namespace py = pybind11;

PYBIND11_MODULE(apbs, m) {
  m.doc() = R"pbdoc(
    )pbdoc";

  /// @see bind_nosh.hpp
  m.def("parseInputFromString", &parseInputFromString);
  m.def("getPotentials", &getPotentials<double>);
}
