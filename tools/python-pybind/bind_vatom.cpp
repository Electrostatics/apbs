#include "bind_vatom.hpp"

/**
 * @brief Class allows for the c-style arrays to be replaced by
 * C++ STL types which more easily interface with python.
 */
class Vatom_glue : public Vatom
{
public:
  std::array<double, 3> position; /**< Atomic position */
  std::string resName; /**< Residue name from PDB/PQR file */
  std::string atomName; /**< Atom name from PDB/PDR file */
};

void bind_vatom(py::module& m)
{
  py::class_<Vatom_glue>(m, "Vatom", py::dynamic_attr())
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
    .def_readwrite("dipole"			     , &Vatom::dipole)  /**< Permanent dipole */
    .def_readwrite("quadrupole"		   , &Vatom::quadrupole)  /**< Permanent quadrupole */
    .def_readwrite("inducedDipole"	 , &Vatom::inducedDipole)   /**< Induced dipole */
    .def_readwrite("nlInducedDipole" , &Vatom::nlInducedDipole)	  /**< Non-local induced dipole */
#endif
    .def("setResName"        , &Vatom_setResName)
    .def("setAtomName"       , &Vatom_setAtomName)
    .def("getResName"        , &Vatom_getResName)
    .def("getAtomName"       , &Vatom_getAtomName)
    .def("setPosition"       , &Vatom_setPosition)
    .def("copyTo"            , &Vatom_copyTo)
    .def("copyFrom"          , &Vatom_copyFrom)
    .def_readwrite("position", &Vatom_glue::position)
    .def_readwrite("radius"  , &Vatom::radius)
    .def_readwrite("charge"  , &Vatom::charge)
    .def_readwrite("partID"  , &Vatom::partID)
    .def_readwrite("epsilon" , &Vatom::epsilon)
    .def_readwrite("id"      , &Vatom::id)
    .def_readwrite("resName" , &Vatom_glue::resName)
    .def_readwrite("atomName", &Vatom_glue::atomName);
}
