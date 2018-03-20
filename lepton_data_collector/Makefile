OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
APPLICATION=lepton_data_collector
CFLAGS ?= -g -O0

${APPLICATION}: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS} ${LDFLAGS}

clean:
	rm -f *.o *.a ${APPLICATION}

.PHONY: clean
