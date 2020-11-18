#include <pybind11/pybind11.h>

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "bind_nosh.hpp"
#include "bind_vatom.hpp"
#include "bind_valist.hpp"
#include "bind_constants.hpp"

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
  m.doc() = R"pbdoc(APBS Python Bindings

    .. note:: When the C code would return an int to represent an error code, these
      bindings will return a `None` value. For example,

    .. code:: python

      from apbs import NOsh
      nosh = NOsh()

    )pbdoc";

  bind_valist(m);
  bind_nosh(m);
  bind_vatom(m);
  bind_constants(m);
}
