ALL = img2cube  coxreg  expand  ofmov  ofimg2  ofimg2sp optflow

all:
	for dir in $(ALL); do \
	(cd $$dir; ${MAKE} all); \
	done

install: all
	for dir in $(ALL); do \
	(cd $$dir; ${MAKE} install); \
	done

clean:
	for dir in $(ALL); do \
	(cd $$dir; ${MAKE} clean); \
	done

cleaner:
	-rm -f *~
	for dir in $(ALL); do \
	(cd $$dir; ${MAKE} cleaner); \
	done
