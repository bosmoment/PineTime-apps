## Bosmoment-style Pinetime firmware applications

Friendly firmware applications for the Pinetime!

This repository contains a number of applications and helper modules specific
for the PineTime[1] open source smartwatch. 

Highlights:

- Built on top of the RIOT[2] open source embedded operating system
- Apache NimBLE[3] as an open-source bluetooth 5.0 stack.
- LittlevGL[4] for the user interface.

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

First follow the quick start guide from RIOT[5] to get familiar and validate your
toolchain.

When checking out this repository, don't forget to initialize the RIOT submodule
contained within: `git submodule init`.

Applications are contained in the `apps` dir with a single application per
directory. The `pinetime` application should give you a good starting point.

Currently the Segger J-Link tools are used for flashing the application. It is
possible to use a different programmer by overriding the settings in the
`Makefile.include` for the `pinetime` board in the RIOT tree. 

### Tips

- STDIO is implemented via the Segger RTT protocol. 

- The directory where a RIOT tree is expected can be overridden by setting the
  `RIOTBASE` variable in the application makefile or from the command line.

- A fork of the Android GadgetBridge application is available here[6] with basic
  support for this RIOT PineTime application.

### Planned features

- Device side Bluetooth key code confirmation (Only the host verifies the
  pairing code at the moment, the firmware always confirms the code as valid)
- Persistent Bluetooth bonds
- Secure over the air updates

### Development

[1]: https://www.pine64.org/pinetime/
[2]: https://github.com/RIOT-os/RIOT/
[3]: https://github.com/apache/mynewt-nimble
[4]: https://github.com/littlevgl/lvgl
[5]: https://doc.riot-os.org/index.html#the-quickest-start
[6]: https://codeberg.org/bergzand/GadgetBridge
