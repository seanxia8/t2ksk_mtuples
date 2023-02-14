#SKOFL_ROOT = /usr/local/skofl_11c
#ATMPD_ROOT = /home/atmpd/skrep/11c


include $(SKOFL_ROOT)/config.gmk
#include /home/skofl/sklib_gcc4.8.5/skofl_16c/config.gmk ## gcc34 in 14c config.gmk seems no longer supported

LOCAL_INC	= 

LOCAL_LIBS	= -L$(ATMPD_ROOT)/lib -lThreeProb

#
#  Objects
#

OBJLIB   = MiniTupleInterface.o 
OBJ2 = fQMiniTupleInterface.o
OBJ3 = fQMiniTupleInterface.o MiniTupleInterface.o

PROGRAMS = build_minituple build_fQminituple

all: $(PROGRAMS)

clean: 
	$(RM) *.o *~ core fort.*  $(PROGRAMS)

build_minituple : build_minituple.o $(OBJLIB)
	LD_RUN_PATH=$(LIBDIR):$(SKOFL_LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

build_fQminituple : build_fQminituple.o $(OBJ2)
	LD_RUN_PATH=$(LIBDIR):$(SKOFL_LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

