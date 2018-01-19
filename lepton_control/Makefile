OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
APPLICATION=vsync_app

${APPLICATION}: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} ${LDFLAGS}

clean:
	rm -f *.o *.a vsync_app

.PHONY: clean
