CROSS_COMPILE ?= arm-linux-gnueabihf-
CC=${CROSS_COMPILE}gcc
TOPDIR:=$(shell pwd)
LEPTON_CONTROL_DIR:=${TOPDIR}/lepton_control
LEPTON_SDK_DIR:=${TOPDIR}/lepton_sdk
CFLAGS=-g -O2 -I${LEPTON_SDK_DIR} -I${LEPTON_CONTROL_DIR} ${EXTRA_CFLAGS}
LDFLAGS=-L${LEPTON_SDK_DIR} -llepton_sdk -ldl

export CROSS_COMPILE CC CFLAGS LDFLAGS LEPTON_SDK_DIR LEPTON_CONTROL_DIR

all: lepton_sdk lepton_control

lepton_sdk:
	${MAKE} -C lepton_sdk

lepton_control: lepton_sdk
	${MAKE} -C lepton_control

clean:
	${MAKE} -C lepton_sdk clean
	${MAKE} -C lepton_control clean

.PHONY: all clean lepton_sdk lepton_control

