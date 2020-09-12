#include "bind_valist.hpp"

void Valist_load(Valist *self,
                 int size,
                 std::vector<double> x,
                 std::vector<double> y,
                 std::vector<double> z,
                 std::vector<double> chg,
                 std::vector<double> rad)
{

  int i, j;
  double pos[3];

  Vatom *atom;

  VASSERT(self != VNULL);

  self->atoms = static_cast<Vatom*>(Vmem_malloc(self->vmem, size, sizeof(Vatom)));
  self->number = size;
  for (i = 0; i < size; i++) {
    pos[0] = x[i];
    pos[1] = y[i];
    pos[2] = z[i];
    Vatom_setCharge(&(self->atoms[i]), chg[i]);
    Vatom_setRadius(&(self->atoms[i]), rad[i]);
    Vatom_setPosition(&(self->atoms[i]), pos);
    Vatom_setAtomID(&(self->atoms[i]), i);
  }

  self->center[0] = 0.0;
  self->center[1] = 0.0;
  self->center[2] = 0.0;
  self->maxrad = 0.0;
  self->charge = 0.0;

  /* Reset stat variables */
  atom = &(self->atoms[0]);
  for (i = 0; i < 3; i++) {
    self->maxcrd[i] = self->mincrd[i] = atom->position[i];
  }
  self->maxrad = atom->radius;

  for (i = 0; i < self->number; i++) {
    atom = &(self->atoms[i]);
    for (j = 0; j < 3; j++) {
      if (atom->position[j] < self->mincrd[j])
        self->mincrd[j] = atom->position[j];
      if (atom->position[j] > self->maxcrd[j])
        self->maxcrd[j] = atom->position[j];
    }
    if (atom->radius > self->maxrad) self->maxrad = atom->radius;
    self->charge = self->charge + atom->charge;
  }

  self->center[0] = 0.5 * (self->maxcrd[0] + self->mincrd[0]);
  self->center[1] = 0.5 * (self->maxcrd[1] + self->mincrd[1]);
  self->center[2] = 0.5 * (self->maxcrd[2] + self->mincrd[2]);
}

/**
 * @brief Class to bind sValist to C++ STL types for better interfacing
 * with python.
 */
struct Valist_glue : public Valist
{
  std::array<double, 3> center;   /**< Molecule center (xmin - xmax)/2, etc.*/
  std::array<double, 3> mincrd;   /**< Minimum coordinates */
  std::array<double, 3> maxcrd;   /**< Maximum coordinates */
  std::vector<Vatom> vatoms;       /**< Atom list */
};

void bind_valist(py::module &m)
{
  py::class_<Valist_glue>(m, "Valist")
    .def(py::init<>())
    .def("__del__", [](Valist* self) { Valist_dtor(&self); })
    .def("load", &Valist_load)
    .def("getAtomList"    , &Valist_getAtomList)
    .def("getCenterX"     , &Valist_getCenterX)
    .def("getCenterY"     , &Valist_getCenterY)
    .def("getCenterZ"     , &Valist_getCenterZ)
    .def("getNumberAtoms" , &Valist_getNumberAtoms)
    .def("getAtom"        , &Valist_getAtom)
    .def("memChk"         , &Valist_memChk)
    .def("readPQR"        , &Valist_readPQR)
    .def("readPDB"        , &Valist_readPDB)
    .def("readXML"        , &Valist_readXML)
    .def_readwrite("number"     , &Valist::number)
    .def_readwrite("center"     , &Valist_glue::center)
    .def_readwrite("mincrd"     , &Valist_glue::mincrd)
    .def_readwrite("maxcrd"     , &Valist_glue::maxcrd)
    .def_readwrite("maxrad"     , &Valist::maxrad)
    .def_readwrite("charge"     , &Valist::charge)
    .def_readwrite("vatoms"     , &Valist_glue::vatoms)
    .def_readwrite("vmem"       , &Valist::vmem);
}
