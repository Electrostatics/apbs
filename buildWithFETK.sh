#!/bin/bash
#run this in apbsroot to build apbs with fetk support

#download, hack (maloc incompatability), and build fetk
wget http://www.fetk.org/codes/download/fetk-1.5.tar.gz
tar xzvf fetk-1.5.tar.gz
cd fetk
echo 'VPUBLIC int Vio_getc(Vio *thee){  ASC *asc; asc = thee->axdr; return asc->buf[asc->pos++];  }' >> maloc/src/vsys/vio.c
echo | ./fetk-build maloc punc gamer mc
cp -r build/x86_64-unknown-linux-gnu/include/ .
cp -r build/x86_64-unknown-linux-gnu/lib/ .

#uncomment for static library support
#cd lib
#rename .la .a *
#cd ..

#build apbs
cd ../build
cmake -DENABLE_FETK=YES -DBUILD_SHARED_LIBS=YES ..
make

#test fem support
cd ../examples/born
../../bin/apbs apbs-mol-fem.in
../../bin/apbs apbs-smol-fem.in
#../../bin/apbs apbs-mol-fem-extmesh.in

