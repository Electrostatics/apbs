#include "bind_valist.hpp"

void bind_valist(py::module& m)
{
  py::class_<Valist>(m, "Valist")
    .def(py::init<>());
}
