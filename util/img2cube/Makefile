
include ../../Makefile.in

INCLUDE = $(MILIB_INC) $(CFITSIO_INC) $(MT_INC)
LDLIBS  = $(MILIB_LIB) $(CFITSIO_LIB) ${GOTOBLAS2_LIB} $(MT_LIB) $(LIB)

TARGET = img2cube

all: $(TARGET) 

img2cube: img2cube.o  arg_img2cube.o
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
