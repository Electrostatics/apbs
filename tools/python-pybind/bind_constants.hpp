#pragma once

/**
 * @file tools/python-pybind/bind_constants.hpp
 * @author Asher Mancinelli <asher.mancinelli@pnnl.gov>
 * @brief Contains bindings for exported constants.
 *
 * @note The constants exported here were simply the same ones exported by the
 * original SWIG python interface.
 *
 * @see tools/python/apbslib.c
 */

inline void bind_constants(py::module& m)
{
  m.attr("NPT_ENERGY") 		    = py::int_(static_cast<int>(NPT_ENERGY));
  m.attr("NPT_FORCE") 		    = py::int_(static_cast<int>(NPT_FORCE));
  m.attr("NPT_ELECENERGY") 		= py::int_(static_cast<int>(NPT_ELECENERGY));
  m.attr("NPT_ELECFORCE") 		= py::int_(static_cast<int>(NPT_ELECFORCE));
  m.attr("NPT_APOLENERGY") 		= py::int_(static_cast<int>(NPT_APOLENERGY));
  m.attr("NPT_APOLFORCE") 		= py::int_(static_cast<int>(NPT_APOLFORCE));
}
