# name of your application
APPLICATION = terminal

BOARD ?= pinetime

# This has to be the absolute path to the RIOT base directory:
PINETIMEBASE ?= $(CURDIR)/../..

CFLAGS += -I$(CURDIR)

DEVELHELP ?= 1

USEWIDGETS += home_terminal
USEWIDGETS += menu_tiles

CFLAGS += -DCONFIG_WIDGET_HOME=WIDGET_HOME_TERMINAL

# basic modules
include $(CURDIR)/modules.mk

include $(PINETIMEBASE)/Makefile.include
