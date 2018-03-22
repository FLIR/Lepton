OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
APPLICATION=vsync_app
CFLAGS=-g -O2 -I../lepton_sdk -I. ${EXTRA_CFLAGS}
LDFLAGS=-L../lepton_sdk -llepton_sdk -ldl

${APPLICATION}: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} ${LDFLAGS}

clean:
	rm -f *.o *.a vsync_app

.PHONY: clean
