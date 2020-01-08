## Bosmoment-style Pinetime firmware applications

Friendly firmware applications for the Pinetime!

This repository contains a number of applications and helper modules specific
for the [PineTime] open source smartwatch. 

Highlights:

- Built on top of [RIOT], an open source embedded operating system
- Apache [NimBLE] as an open-source bluetooth 5.0 stack.
- [LittlevGL] for the user interface.

### Features

- *Bluetooth LE*: provided by Nimble
- *Bluetooth pairing support*
- *Time synchronisation*: Bluetooth GATT Current Time Service client
- *Graphical user interface*: via LittleVGL.

More to follow

### Structure

The project is separated into a number of directories:

- *RIOT*: The RIOT submodule used for compilation
- *apps*: Contains firmware applications for the PineTime.
- *modules*: Additional modules written to support the firmware applications
- *widgets*: A collection of small user applications for the end user of the
  PineTime, for example: a stopwatch widget; a configuration widget; a
  heartbeat graph widget.

### Getting started

#### Repository setup

When checking out this repository, don't forget to initialize the RIOT submodule
contained within this repository with: `git submodule update`.

#### Building applications

Applications are contained in the `apps` dir with a single application per
directory. The `pinetime` application should give you a good starting point.

Use `make all` in the application directory to build the firmware, `make flash`
to flash it on the target and `make term` to get a serial connection to the
device. 

#### Development

As the project is based on RIOT, it helps to get familiar with RIOT and check
which functionality is provided by the OS.
There is a [quick start] guide available for RIOT to get familiar with RIOT's
build system and to validate your toolchain functionality. Doxygen-based API
documentation is also available there.

Currently the Segger J-Link tools are used for flashing the application. It is
possible to use a different programmer by overriding the settings in the
`Makefile.include` for the `pinetime` board in the RIOT tree. 

### Tips

- STDIO is implemented via the Segger RTT protocol. It is non-blocking by
  default, blocking mode can be enabled by adding
  `STDIO_RTT_ENABLE_BLOCKING_STDOUT` to the CFLAGS. **Note**: the firmware will
  block (hang) on STDIO if no RTT client is attached.

- The directory where a RIOT tree is expected can be overridden by setting the
  `RIOTBASE` variable in the application makefile or from the command line.

- A fork of the Android [GadgetBridge] application is available with basic
  support for this RIOT PineTime application.

### Planned features

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
