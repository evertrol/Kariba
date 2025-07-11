CONFIG = ../make.config
include $(CONFIG)

# Path to Kariba root
KARIBA = ../src


ifndef GSL
	GSL_CPPFLAGS=
	GSL_LDFLAGS=
else
	GSL_CPPFLAGS=-I$(GSL)/include
	GSL_LDFLAGS=-L$(GSL)/lib
endif


CXXFLAGS += $(OPENMP)
CPPFLAGS = -I${GSL}/include -I${KARIBA} -I.
LD = $(CXX)
LDFLAGS = -L${GSL}/lib $(OPENMP) $(EXTRA_LDFLAGS)
LDLIBS = -lgsl -lm
RM = rm -f

# Kariba library
LIBKARIBA = ${KARIBA}/libkariba.a

TEST_MAIN = test_main

# Test source files and objects
MAIN_OBJ = test_main.o
PARTICLE_OBJ = test_particles.o
RADIATION_OBJ = test_radiation.o
DISTRIBUTIONS_OBJ = test_distributions.o
POWERLAW_OBJ = test_powerlaw.o
BKNPOWER_OBJ = test_bknpower.o
COMPTON_OBJ = test_compton.o

# All test executables
ALL_TESTS = $(PARTICLE_TESTS) $(DISTRIBUTIONS_TESTS) $(RADIATION_TESTS) $(BKNPOWER_TESTS) $(COMPTON_TESTS) $(POWERLAW_TESTS)
OBJECTS = $(MAIN_OBJ) $(PARTICLE_OBJ) $(RADIATION_OBJ) $(DISTRIBUTIONS_OBJ) $(POWERLAW_OBJ) $(BKNPOWER_OBJ) $(COMPTON_OBJ)

.PHONY: all tests main distclean

all: main ebl

main: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(TEST_MAIN) $(OBJECTS) $(LIBKARIBA) $(LDLIBS)
ebl: test_ebl.cpp doctest.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LIBKARIBA) $(LDLIBS) -o test_ebl $<

%.o: %.cpp doctest.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<


clean:
	$(RM) $(OBJECTS)

distclean: clean
	$(RM) $(ALL_TESTS)
