OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
APPLICATION=test_lepton_driver_lib

${APPLICATION}: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} ${LDFLAGS}

clean:
	rm -f *.o *.a ${APPLICATION}

.PHONY: clean
