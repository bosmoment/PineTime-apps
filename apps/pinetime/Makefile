# name of your application
APPLICATION = PineTime

BOARD ?= pinetime

# This has to be the absolute path to the RIOT base directory:
PINETIMEBASE ?= $(CURDIR)/../..

CFLAGS += -I$(CURDIR)

DEVELHELP ?= 1

CFLAGS += -DHAL_INPUT_SWITCH_XY=1
CFLAGS += -DHAL_INPUT_REVERSE_X=1
CFLAGS += -DHAL_INPUT_REVERSE_Y=0

# basic modules
include $(CURDIR)/modules.mk

include $(PINETIMEBASE)/Makefile.include
