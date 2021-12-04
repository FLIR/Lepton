APPLICATION=vsync_app
CFLAGS=-g -O2 -I../lepton_sdk -I. ${EXTRA_CFLAGS}
RPI_CFLAGS=${CFLAGS} -DI2C_DEVICE_FILE=\"/dev/i2c-1\"
BBB_CFLAGS=${CFLAGS} -DI2C_DEVICE_FILE=\"/dev/i2c-2\"
LDFLAGS=-L../lepton_sdk -llepton_sdk -ldl

all: rpi_vsync_app bbb_vsync_app

rpi_vsync_app: vsync_app.c lep_i2cdev.c lep_i2cdev.h
	${CC} ${RPI_CFLAGS} -o $@ vsync_app.c lep_i2cdev.c ${LDFLAGS}

bbb_vsync_app: vsync_app.c lep_i2cdev.c lep_i2cdev.h
	${CC} ${BBB_CFLAGS} -o $@ vsync_app.c lep_i2cdev.c ${LDFLAGS}

clean:
	rm -f *.o *.a *vsync_app

.PHONY: clean
