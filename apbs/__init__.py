from . import (
        bindings,
        chemistry,
        geometry,
        grid,
        multigrid,
        pqr,
        test,
        bin,
        lib,
        )
import platform

def check_vcredist():
    '''Checks availability of Visual C++ libraries

    .. note::
        A lockfile is created when the visual c++ redistributable is found on the system.

    .. warning::
        If the visual c++ redistributable is deleted from the system, the lockfile will still
        exist even though the binaries will not have the needed libraries.
    '''

    import os
    touch = lambda filename: os.close(os.open(filename, os.O_CREAT))

    # When installed, this path should be in the bowels of python site-packages dir, hopefully
    # safe from users
    lockfile = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'has_vcredist.lock')

    # If we've already gone through the install process, we don't need to again.
    if os.path.exists(lockfile):
        return

    proc = subprocess.run('where powershell', capture_output=True)
    if len(proc.stdout) == 0:
        raise RuntimeError('Could not find powershell executable on path.')
    pwsh: str = pwsh.stdout.decode().strip()

    # Checks if any appropriate visual C++ redistributables are already installed on the system
    proc = subprocess.run([pwsh, 'Get-WmiObject -Class Win32_Product -Filter "Name LIKE \'%Visual C++ 2019%\'"'.split(' ')])
    if len(proc.stdout) == 0:
        print('''APBS Error:

        The Microsoft Visual C++ redistributable was not found on your system and the APBS python module will likely
        not function as intended. Would you like the Visual C++ redistributable to be downloaded and ran for you?

        Note: This will open a pop-up window which you will have to navigate yourself.''')
        ans = input('y to accept, anything else to decline. >>>')
        if ans.lower() == 'y':
            import urllib.request
            import platform
            redist = None
            if 'arm' in platform.machine().lower():
                arch = 'arm64'
            elif '32' in platform.architecture()[0]:
                arch = 'x64'
            else:
                arch = 'x86'
            vc_redist_link = f'https://aka.ms/vs/16/release/vc_redist.{arch}.exe'
            vc_redist_fn = vc_redist_link.split('/')[-1]

            print('Fetching redistributable from link: ', vc_redist_link)
            with urllib.request.urlopen(vc_redist_link) as f:
                redist = f.read()

            print('Writing to file: ', vc_redist_fn)
            with open(vc_redist_fn, 'wb') as f:
                f.write(redist)

            print('Running redistributable')
            subprocess.run([vc_redist_fn,])
            
        else:
            print('Continuing to use APBS will likely result in errors. If you would like to download and run'
                    ' the redistributable yourself, please visit the link below and find the correct binary for your system:'
                    '\nhttps://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads')
            return

    # The redistributable was either already found or we've gone through the install process,
    # which means we can create the lockfile.
    touch(lockfile)


if platform.system() == 'Windows':
    check_vcredist()

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
