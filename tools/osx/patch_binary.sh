#!/bin/bash

declare -a EXECS=( analysis born del2dx dx2uhbd mergedx mgmesh similarity tensor2dx value apbs
		   benchmark coulomb dx2mol dxmath mergedx2 multivalue smooth uhbd_asc2bin )
declare -a LIBS=( libmaloc.1.dylib libamd.1.dylib libpunc.1.dylib libmc.1.dylib libgamer.1.dylib
		  libsuperlu.1.dylib libumfpack.1.dylib libblas.1.dylib libvf2c.1.dylib
		  libtetgen.1.dylib libtriangle.1.dylib libiomp5.dylib )

NEWLIBPATH="@executable_path/../Frameworks"

for TARGET in ${LIBS[@]}
do
	LIBFILE=Frameworks/$TARGET
	TARGETID=$(otool -DX $LIBFILE)
	NEWTARGETID=$NEWLIBPATH/$TARGET
	install_name_tool -id $NEWTARGETID $LIBFILE

	for EXEC in ${EXECS[@]}
	do
		install_name_tool -change $TARGETID $NEWTARGETID MacOS/$EXEC
	done
done
