#include "bind_vatom.hpp"
#include <cstdio>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

void bind_vatom(py::module& m)
{
  py::class_<sVatom>(m, "Vatom")
    .def(py::init<>())
#ifdef WITH_TINKER
    .def("setInducedDipole"  , &Vatom_setInducedDipole)
    .def("setNLInducedDipole", &Vatom_setNLInducedDipole)
    .def("setDipole"         , &Vatom_setDipole)
    .def("setQuadrupole"     , &Vatom_setQuadrupole)
    .def("getDipole"         , &Vatom_getDipole)
    .def("getQuadrupole"     , &Vatom_getQuadrupole)
    .def("getInducedDipole"  , &Vatom_getInducedDipole)
    .def("getNLInducedDipole", &Vatom_getNLInducedDipole)
    .def_readwrite("dipole"			     , &sVatom::dipole)  /**< Permanent dipole */
    .def_readwrite("quadrupole"		   , &sVatom::quadrupole)  /**< Permanent quadrupole */
    .def_readwrite("inducedDipole"	 , &sVatom::inducedDipole)   /**< Induced dipole */
    .def_readwrite("nlInducedDipole" , &sVatom::nlInducedDipole)	  /**< Non-local induced dipole */
#endif
    .def("copyTo"            , &Vatom_copyTo)
    .def("copyFrom"          , &Vatom_copyFrom)
    .def_readwrite("radius"  , &sVatom::radius)
    .def_readwrite("charge"  , &sVatom::charge)
    .def_readwrite("partID"  , &sVatom::partID)
    .def_readwrite("epsilon" , &sVatom::epsilon)
    .def_readwrite("id"      , &sVatom::id)
    .def_property("position",
        [] (sVatom& self)
        {
          return std::vector<double>(self.position, self.position+3);
        },
        [] (sVatom& self, py::array_t<double> other)
        {
          py::buffer_info buf = other.request();
          assert(buf.ndim == 1 && "Vatom::position is 1D!");
          assert(other.size() == 3 && "Vatom::position has length 3!");
          auto* ptr = static_cast<double*>(buf.ptr);
          for(int i=0;i<3;i++) self.position[i] = ptr[i];
        })
    .def_property("atomName",
        &Vatom_getAtomName,
        [] (sVatom& self, std::string other)
        {
          for(int i=0;i<VMAX_RECLEN;i++)
          {
            if(other.c_str()[i]=='\0') break;
            self.atomName[i] = other.c_str()[i];
          }
        })
    .def_property("resName",
        &Vatom_getResName,
        [] (sVatom& self, std::string other)
        {
          for(int i=0;i<VMAX_RECLEN;i++)
          {
            if(other.c_str()[i]=='\0') break;
            self.resName[i] = other.c_str()[i];
          }
        });
}
