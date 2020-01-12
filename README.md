[![Build Status](https://api.cirrus-ci.com/github/bosmoment/PineTime-apps.svg)](https://cirrus-ci.com/github/bosmoment/PineTime-apps)

# Bosmoment-style Pinetime firmware applications

Friendly firmware applications for the Pinetime!

This repository contains a number of applications and helper modules specific
for the [PineTime] open source smartwatch. 

Highlights:

- Built on top of [RIOT], an open source embedded operating system
- Apache [NimBLE] as an open-source bluetooth 5.0 stack.
- [LittlevGL] for the user interface.

### Pre-build Binaries

Fresh binaries from the CI:

- [PineTime.elf](https://api.cirrus-ci.com/v1/artifact/github/bosmoment/PineTime-apps/build/binaries/PineTime.elf)
- [PineTime.bin](https://api.cirrus-ci.com/v1/artifact/github/bosmoment/PineTime-apps/build/binaries/PineTime.bin)

## Features

- *Bluetooth LE*: provided by Nimble
- *Bluetooth pairing support*
- *Time synchronisation*: Bluetooth GATT Current Time Service client
- *Time keeping*: nRF52832 RTC for ticks per second time/date updates
- *Graphical user interface*: via LittleVGL.

And the default "home" screen includes:
- *Time/date*: The current minutes and hour are shown as numbers, the day,
               the month and year are shown and a second "hand" is shown as a
               circular meter.
- *Battery status*: Battery status is available on the main screen
- *Bluetooth status*: Bluetooth connection and advertisement mode is shown on
                      the screen

More to follow

## Structure

The project is separated into a number of directories:

- *RIOT*: The RIOT submodule used for compilation
- *apps*: Contains firmware applications for the PineTime.
- *modules*: Additional modules written to support the firmware applications
- *widgets*: A collection of small user applications for the end user of the
  PineTime, for example: a stopwatch widget; a configuration widget; a
  heartbeat graph widget.

## Getting started

### Repository setup

When checking out this repository, don't forget to initialize the RIOT submodule
contained within this repository with: `git submodule update`.

### Building applications

Applications are contained in the `apps` dir with a single application per
directory. The `pinetime` application should give you a good starting point.

Use `make all` in the application directory to build the firmware, `make flash`
to flash it on the target and `make term` to get a serial connection to the
device. 

### Development

As the project is based on RIOT, it helps to get familiar with RIOT and check
which functionality is provided by the OS.
There is a [quick start] guide available for RIOT to get familiar with RIOT's
build system and to validate your toolchain functionality. Doxygen-based API
documentation is also available there.

Currently the Segger J-Link tools are used for flashing the application. It is
possible to use a different programmer by overriding the settings in the
`Makefile.include` for the `pinetime` board in the RIOT tree. 

## Bluetooth LE

Currently the firmware is always advertising if no connection is active. As
soon as a host connects to the PineTime, the advertising is stopped and
continues when the host disconnects.

Bonding is available and useable, but the bonds are not persistent between
reboots. (see Planned features).

A custom UUID is included in the advertisement to recognise the device by:
```
9851dc0a-b04a-1399-5646-3b38788cb1c5
```

This UUID can be used for Bluetooth LE filters in Android and such. The device
name itself can be customized to your liking and is not used for identifying the
device.

### Companion phone app

A fork of [GadgetBridge] is available with support for this firmware. The fork
uses the custom UUID specified above to filter and identify the device.

The GadgetBridge app should pick up the device when scanning and allow pairing
with the device. As long as the PineTime remembers the bond, the app and the
Pinetime should be able to restore the secure connection.

The GadgetBridge fork is configured to only allow encrypted reads to the current
time characteristic. The PineTime firmware automatically requests pairing when 
it receives an insufficient encryption error back from the app when requesting
the time.

## Tips

- STDIO is implemented via the Segger RTT protocol. It is non-blocking by
  default, blocking mode can be enabled by adding
  `STDIO_RTT_ENABLE_BLOCKING_STDOUT` to the CFLAGS. **Note**: the firmware will
  block (hang) on STDIO if no RTT client is attached.

- The directory where a RIOT tree is expected can be overridden by setting the
  `RIOTBASE` variable in the application makefile or from the command line.

- A fork of the Android [GadgetBridge] application is available with basic
  support for this RIOT PineTime application.

## Planned features

- Device side Bluetooth key code confirmation (Only the host verifies the
  pairing code at the moment, the firmware always confirms the code as valid)
- Persistent Bluetooth bonds
- Secure over the air updates

[PineTIme]: https://www.pine64.org/pinetime/
[RIOT]: https://github.com/RIOT-os/RIOT/
[NimBLE]: https://github.com/apache/mynewt-nimble
[LittleVGL]: https://github.com/littlevgl/lvgl
[quick start]: https://doc.riot-os.org/index.html#the-quickest-start
[GadgetBridge]: https://codeberg.org/bergzand/Gadgetbridge/src/branch/riotwatch/initial
