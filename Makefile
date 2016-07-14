LIBS = 
BINS = util

SUBDIRS = $(LIBS) $(BINS) script scriptR setup

all: libs bins

libs : 
	for dir in ${LIBS}; do \
	(cd $$dir; ${MAKE} all); \
	done

bins: libs
	for dir in ${BINS}; do \
	(cd $$dir; ${MAKE} all); \
	done

install: all
	for dir in ${BINS}; do \
	(cd $$dir; ${MAKE} install); \
	done

clean:
	for dir in ${SUBDIRS}; do \
	(cd $$dir; ${MAKE} clean); \
	done

cleaner:
	-rm -f *~
	for dir in ${SUBDIRS}; do \
	(cd $$dir; ${MAKE} cleaner); \
	done

allclean: cleaner

uninstall: cleaner
	-rm -fr lib
