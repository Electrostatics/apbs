from ._version import __version__  # noqa: F401

header = """

----------------------------------------------------------------------

    APBS -- Adaptive Poisson-Boltzmann Solver
    Version " PACKAGE_STRING "

    Nathan A. Baker (nathan.baker@pnnl.gov)
    Pacific Northwest National Laboratory

    Additional contributing authors listed in the code documentation.

    Copyright (c) 2010-2020 Battelle Memorial Institute. Developed at
    the Pacific Northwest National Laboratory, operated by Battelle
    Memorial Institute, Pacific Northwest Division for the U.S. Department
    of Energy.

    Portions Copyright (c) 2002-2010, Washington University in St. Louis.
    Portions Copyright (c) 2002-2020, Nathan A. Baker.
    Portions Copyright (c) 1999-2002, The Regents of the University of
    California.
    Portions Copyright (c) 1995, Michael Holst.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the developer nor the names of its contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------

    APBS uses FETK (the Finite Element ToolKit) to solve the
    Poisson-Boltzmann equation numerically.  FETK is a portable collection
    of finite element modeling class libraries developed by the Michael Holst
    research group and written in an object-oriented form of C.  FEtk is
    designed to solve general coupled systems of nonlinear partial differential
    equations using adaptive finite element methods, inexact Newton methods,
    and algebraic multilevel methods.  More information about FEtk may be found
    at <http://www.FEtk.ORG>.

----------------------------------------------------------------------

    APBS also uses Aqua to solve the Poisson-Boltzmann equation numerically.
    Aqua is a modified form of the Holst group PMG library
    <http://www.FEtk.ORG> which has been modified by Patrice Koehl
    <http://koehllab.genomecenter.ucdavis.edu/> for improved efficiency and
    memory usage when solving the Poisson-Boltzmann equation.

----------------------------------------------------------------------

    Please cite your use of APBS as:
    Baker NA, Sept D, Joseph S, Holst MJ, McCammon JA. Electrostatics of
    nanosystems: application to microtubules and the ribosome. Proc.
    Natl. Acad. Sci. USA 98, 10037-10041 2001.

"""
