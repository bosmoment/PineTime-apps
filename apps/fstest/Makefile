# name of your application
APPLICATION = fstest

# If no BOARD is found in the environment, use this default:
BOARD ?= pinetime

# This has to be the absolute path to the RIOT base directory:
PINETIMEBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Use MTD (flash layer)
USEMODULE += mtd

USEMODULE += storage
USEMODULE += core_thread_flags

CFLAGS += -DSTDIO_RTT_ENABLE_BLOCKING_STDOUT

include $(PINETIMEBASE)/Makefile.include
