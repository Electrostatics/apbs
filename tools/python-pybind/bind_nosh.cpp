#include "bind_nosh.hpp"

int parseInputFromString(NOsh *nosh, std::string str)
{
  int ret, bufsize;
  Vio *sock;

  startVio();

  VASSERT( bufsize <= VMAX_BUFSIZE );
  sock = Vio_ctor("BUFF","ASC",VNULL,"0","r");

  Vio_bufTake(sock, str.c_str(), str.size());

  ret = NOsh_parseInput(nosh, sock); 
  sock->VIObuffer = VNULL;
  Vio_dtor(&sock);
  return ret;
}

void bind_nosh(py::module& m)
{
  m.def("getPotentials", &getPotentials<double>);

  py::class_<NOsh_calc>(m, "NOsh_calc")
    .def("__init__",
        [] (NOsh_calc* self, NOsh_CalcType calcType)
        {
          self = NOsh_calc_ctor(calcType);
        })
    .def("NOsh_calc_mgparm_set",
        [] (NOsh_calc* nosh, MGparm& mgparm)
        {
          nosh->mgparm = &mgparm;
        })
    .def("__del__",
        [] (NOsh_calc* self)
        {
          NOsh_calc_dtor(&self);
        });

  py::class_<NOsh>(m, "NOsh")
    .def(py::init<>())
    .def("parseInputFromString",
        [] (NOsh* self, std::string str) -> int
        {
          int ret, bufsize;
          Vio *sock;

          startVio();

          VASSERT( bufsize <= VMAX_BUFSIZE );
          sock = Vio_ctor("BUFF","ASC",VNULL,"0","r");

          Vio_bufTake(sock, str.c_str(), str.size());

          ret = NOsh_parseInput(self, sock); 
          sock->VIObuffer = VNULL;
          Vio_dtor(&sock);
          return ret;
        })
    .def("__del__",
        [] (NOsh* self)
        {
          NOsh_dtor(&self);
        });
}
