
include ../../Makefile.in

INCLUDE = $(OPENCVLIB_INC)  $(ROOT_INC) $(MIROOTLIB_INC) $(MIFITSLIB_INC) $(MILIB_INC) $(CFITSIO_INC) $(MT_INC)
LDLIBS  = $(OPENCVLIB_LIB)  $(ROOT_ALLLIBS) $(MIROOTLIB_LIB) $(MIFITSLIB_LIB) $(MILIB_LIB) $(CFITSIO_LIB) $(MT_LIB) $(LIB)

TARGET = optflow

all: $(TARGET) 

optflow: optflow.o  arg_optflow.o
	$(CXXN) $@.o arg_$@.o  ${LDLIBS} -o $@

.SUFFIXES:
.SUFFIXES: .o .c .f .F .cc

.c.o:
	$(CCN) -c $<

.F.o : 
	$(FCN) -c $<

.f.o:
	$(FCN) -c $<

.cc.o:
	${CXXN} -c $<

install: all
	if [ ! -d $(INST_BIN_DIR) ]; then mkdir $(INST_BIN_DIR); fi
	install ${TARGET} ${INST_BIN_DIR}

clean:
	-rm -f *.o

cleaner: clean
	-rm -f ${TARGET} *% *~ core
