#include <pybind11/pybind11.h>

#include "bind_nosh.hpp"

/**
 * @file tools/python-pybind/module.cpp
 * @author Asher Mancinelli <asher.mancinelli@pnnl.gov>
 *
 * @brief Creates python module and passes to each binding function.
 *
 * @note Keep all binding functions in their own header/impl pair. No raw
 * functions or binding should live in this file; this is for creating the
 * module and passing to binding functions only.
 */

namespace py = pybind11;

PYBIND11_MODULE(apbs, m) {
  m.doc() = R"pbdoc(
    )pbdoc";

  bind_nosh(m);
}
