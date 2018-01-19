OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))

liblepton_sdk.a: ${OBJECTS}
	${CROSS_COMPILE}ar rcs $@ ${OBJECTS}

clean:
	rm -f *.o *.a

.PHONY: clean
