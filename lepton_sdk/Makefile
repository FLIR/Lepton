OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS = -g -O2 -I../lepton_control

liblepton_sdk.a: ${OBJECTS}
	${CROSS_COMPILE}ar rcs $@ ${OBJECTS}

clean:
	rm -f *.o *.a

.PHONY: clean
