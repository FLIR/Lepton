CROSS_COMPILE ?= arm-linux-gnueabihf-
CC=${CROSS_COMPILE}gcc
TOPDIR:=$(shell pwd)

export CROSS_COMPILE CC

all: lepton_sdk lepton_control lepton_data_collector

lepton_sdk:
	${MAKE} -C lepton_sdk

lepton_control: lepton_sdk
	${MAKE} -C lepton_control

lepton_data_collector:
	${MAKE} -C lepton_data_collector

clean:
	${MAKE} -C lepton_sdk clean
	${MAKE} -C lepton_control clean
	${MAKE} -C lepton_data_collector clean

.PHONY: all clean lepton_sdk lepton_control lepton_data_collector

