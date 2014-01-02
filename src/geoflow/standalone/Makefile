CFLAGS=-W -Wall -Wextra -I ../../../include/

ifdef DEBUG
	CFLAGS+=-g -O0
#else PROFILE
#	CFLAGS+=-O3 -g -pg  -march=core2 -fstrict-aliasing -DNDEBUG -DEIGEN_NO_MALLOC
else
	CFLAGS+=-O2 -g -DNDEBUG -DEIGEN_NO_MALLOC 
endif

OBJS=cpbconcz2.o modules.o pbsolvercz.o surfconcz.o solvationsub.o subfunction.o

.PHONY: all
all: osm.exe 

osm.exe: main.cpp $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: ../%.cpp
	$(CXX) $(CFLAGS) -c $<

.PHONY:doc
doc:
	doxygen

.PHONY: clean
clean:
	-rm -rf *.o *.mod *.exe doc

