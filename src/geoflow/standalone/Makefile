CFLAGS=-W -Wall -Wextra -I ../../../include/

# ifdef DEBUG
#	CFLAGS+=-g -O0 -std=c++11
#else PROFILE
#	CFLAGS+=-O3 -g -pg  -march=core2 -fstrict-aliasing -DNDEBUG -DEIGEN_NO_MALLOC -std=c++11
# else
#	CFLAGS+=-O2 -g -DNDEBUG -DEIGEN_NO_MALLOC -std=c++11
# endif
CFLAGS+=-O3 -DNDEBUG -std=c++11

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

