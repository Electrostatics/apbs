#include "bind_nosh.hpp"
#include "bind_valist.hpp"

int parseInputFromString(NOsh *nosh, std::string str)
{
  int ret, bufsize;
  Vio *sock;

  startVio();

  VASSERT( bufsize <= VMAX_BUFSIZE );
  sock = Vio_ctor("BUFF","ASC",VNULL,"0","r");

  Vio_bufTake(sock, const_cast<char*>(str.c_str()), str.size());

  ret = NOsh_parseInput(nosh, sock); 
  sock->VIObuffer = VNULL;
  Vio_dtor(&sock);
  return ret;
}

void bind_nosh(py::module& m)
{
  m.def("getPotentials", &getPotentials<double>);

  py::enum_<NOsh_MolFormat>(m, "NOsh_MolFormat").export_values();
  py::enum_<NOsh_CalcType>(m, "NOsh_CalcType").export_values();
  py::enum_<NOsh_ParmFormat>(m, "NOsh_ParmFormat").export_values();
  py::enum_<NOsh_PrintType>(m, "NOsh_PrintType").export_values();

  py::class_<NOsh_calc>(m, "NOsh_calc")
    .def(py::init(
        [] (NOsh_CalcType calcType)
        {
          return std::unique_ptr<NOsh_calc>(NOsh_calc_ctor(calcType));
        }))
    .def("NOsh_calc_mgparm_set",
        [] (NOsh_calc& nosh, MGparm& mgparm)
        {
          nosh.mgparm = &mgparm;
        })
    .def("__del__",
        [] (NOsh_calc* self)
        {
          NOsh_calc_dtor(&self);
        })
    .def_readwrite("mgparm", &NOsh_calc::mgparm)
    .def_readwrite("femparm", &NOsh_calc::femparm)
    .def_readwrite("bemparm", &NOsh_calc::bemparm)
    .def_readwrite("geoflowparm", &NOsh_calc::geoflowparm)
    .def_readwrite("pbamparm", &NOsh_calc::pbamparm)
    .def_readwrite("pbsamparm", &NOsh_calc::pbsamparm)
    .def_readwrite("pbeparm", &NOsh_calc::pbeparm)
    .def_readwrite("apolparm", &sNOsh_calc::apolparm)
    .def_readwrite("calctype", &sNOsh_calc::calctype);

  py::class_<NOsh>(m, "NOsh")
    .def(py::init<>())
    .def("parseInputFromString",
        [] (NOsh& self, std::string str) -> int
        {
          int ret, bufsize;
          Vio *sock;

          startVio();

          VASSERT( bufsize <= VMAX_BUFSIZE );
          sock = Vio_ctor("BUFF","ASC",VNULL,"0","r");

          Vio_bufTake(sock, const_cast<char*>(str.c_str()), str.size());

          ret = NOsh_parseInput(&self, sock); 
          sock->VIObuffer = VNULL;
          Vio_dtor(&sock);
          return ret;
        })
    .def("__del__",
        [] (NOsh* self)
        {
          NOsh_dtor(&self);
        })
    .def("getMolpath"    , [] (NOsh& thee, int imol)  
        { return std::string(NOsh_getMolpath(&thee, imol));    })
    .def("getDielXpath"  , [] (NOsh& thee, int imap)  
        { return std::string(NOsh_getDielXpath(&thee, imap));  })
    .def("getDielYpath"  , [] (NOsh& thee, int imap)  
        { return std::string(NOsh_getDielYpath(&thee, imap));  })
    .def("getDielZpath"  , [] (NOsh& thee, int imap)  
        { return std::string(NOsh_getDielZpath(&thee, imap));  })
    .def("getKappapath"  , [] (NOsh& thee, int imap)  
        { return std::string(NOsh_getKappapath(&thee, imap));  })
    .def("getPotpath"    , [] (NOsh& thee, int imap)  
        { return std::string(NOsh_getPotpath(&thee, imap));    })
    .def("getChargepath" , [] (NOsh& thee, int imap)  
        { return std::string(NOsh_getChargepath(&thee, imap)); })
    .def("elecname"      , [] (NOsh& thee, int ielec) 
        { return std::string(NOsh_elecname(&thee, ielec));     })
    .def("getDielfmt"		 , &NOsh_getDielfmt)
    .def("getKappafmt"	 , &NOsh_getKappafmt)
    .def("getPotfmt"		 , &NOsh_getPotfmt)
    .def("getChargefmt"	 , &NOsh_getChargefmt)
    .def("elec2calc"		 , &NOsh_elec2calc)
    .def("apol2calc"		 , &NOsh_apol2calc)
    .def("printNarg"		 , &NOsh_printNarg)
    .def("printOp"			 , &NOsh_printOp)
    .def("printCalc"		 , &NOsh_printCalc)
    .def("getCalc"       , &NOsh_getCalc)
    .def("printWhat"     , &NOsh_printWhat)
    .def("parseInput"    , &NOsh_parseInput)
    .def("parseInputFile", &NOsh_parseInputFile)
    // These two are wrappers to use std::vector for easier conversion 
    // between python lists and C arrays
    .def("setupElecCalc" , [] (NOsh& thee, std::vector<Valist*> alist)
        {
          NOsh_setupElecCalc(&thee, alist.data());
        })
    .def("setupApolCalc" , [] (NOsh& thee, std::vector<Valist*> alist)
        {
          NOsh_setupApolCalc(&thee, alist.data());
        });
}
