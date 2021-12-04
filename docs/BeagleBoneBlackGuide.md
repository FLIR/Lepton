# Driver and Software to connect a BeagleBone Black to a Lepton
The driver and data capture software provides an example implementation to
collect Lepton frames over its VOSPI interface and extract the pixel data into
16-bit grayscale images.  The code has been successfully tested on a BeagleBone
Green, which is substantially similar to the BeagleBone Black, a.k.a. BBB (some
development was done on a BBB provided by FLIR).

## Hardware setup
To support Lepton 3.X, the breakout board really needs to supply the VSYNC GPIO
from the Lepton. This is __not__ easy with the current version of the breakout
board -- it requires a tech to add a wire to the proper pad inside the socket,
shown as pin 2 GPIO3 in the Lepton data sheet.

### Attaching the break-out board to the BBB
Attach (female-to-male) jumper wires between the following breakout board pins
and the BBB P9 connector:

* (breakout board pin) -> (BBB connector pin)

* SCL ->   P9 pin 19

* SDA ->   P9 pin 20

* MISO ->  P9 pin 21

* CLK ->   P9 pin 22

* CS ->    P9 pin 17

* GND ->   P9 pin 1

* VIN ->   P9 pin 3

* VSYNC -> P8 pin 17

Beware that with the BBB it may be required to attach CLK only after the board
has booted up, if booting from the uSD card (while it interfered with the pin
strapping on the BBB, it did not affect the BBG, which doesn't need a switch
held down to boot off the uSD card).

It is also very useful to connect a USB-to-serial adapter to the J1 connector
to debug boot failures.

## Development station setup

### Installing the Toolchain and building the Linux kernel from source
The kernel to be used with the Lepton driver must be compiled, and the
following config options should be set:

* Due to low-latency requirements for collecting VOSPI data from the Lepton
  (missing subframe deadlines results in frame loss), disable the CPU idle
  functionality (unset `CONFIG_CPU_IDLE` in the kernel .config).  This change
  resulted in much lower frame loss during testing, though it will result in
  higher overall power usage, if this is a concern.

* Verify that the DMA engine is enabled (`CONFIG_DMA_ENGINE=y`)

* Enable the videobuf2 code:

    - `CONFIG_VIDEOBUF2_CORE=m`,

    - `CONFIG_VIDEOBUF2_MEMOPS=m`,

    - `CONFIG_VIDEOBUF2_DMA_CONTIG=m`,

    - `CONFIG_VIDEOBUF2_VMALLOC=m`

  (These can also be set to `=y`, to skip the requirement to load these
  modules before loading the Lepton module). 

The instructions for building the kernel found at
<https://eewiki.net/display/linuxonarm/BeagleBone+Black#BeagleBoneBlack-TIBSP>
were successfully used to build the kernel with the above .config changes, as
well as to build the Lepton kernel module code out-of-tree.  Running the
`build_kernel.sh` step will pop up a `make menuconfig` ncurses window for
changing the kernel config settings, and when complete produces a kernel
zImage, a modules tarball, the kernel configuration, firmware, and device-tree
binaries under the `deploy/` subdirectory, checks out the kernel source under
the `KERNEL/` subdirectory, and downloads a toolchain beneath the `dl/`
subdirectory.  Beware that this script takes a very long time to run and
requires network access.  Subsequent builds can be made to run much faster by
editing the script and commenting out the `FULL_REBUILD=1` line and adding
`unset FULL_REBUILD` in its place.  Copying the
`/path/to/ti-linux-kernel-dev/KERNEL/.config` file into
`/path/to/ti-linux-kernel-dev/patches/defconfig` before running the script
again will preserve changes made to the kernel .config, so these options will
already be set for the `make menuconfig` step.

### Update the Lepton SDK (optional)
Download the latest SDK from
<https://www.flir.com/developer/lepton-integration/lepton-integration-windows/>
(see "Lepton SDK and Documentation" link).
Move aside the `lepton_sdk/FLIR_I2C.c` and `lepton_sdk/Makefile` files and
unpack the SDK into the `lepton_sdk/` subdirectory below this working
directory.  Compare the old `FLIR_I2C.c` file with the new and copy in the
portions that add support for Linux i2cdev (look for strings starting
with `i2cdev_` and for the `LINUX_I2CDEV_I2C` define.  The new SDK's Makefile
should be overwritten with the backed-up version that was moved aside.  The SDK
is used for compiling a simple I2C application that commands the Lepton to
begin sending VSYNC pulses from its GPIO3 output.

## BBB setup

### Create the uSD card image
Load the desired run-time image onto a uSD card.  A 4GB IoT Debian Stretch
image downloaded from <https://beagleboard.org/latest-images>
was used successfully during testing.  Also helpful were the instructions for
expanding the partition size, found at
<https://dev.iachieved.it/iachievedit/expanding-your-beaglebone-microsd-filesystem/>.

### Copy the new kernel and supporting files onto the uSD card
Install the following to the uSD card (replace `/path/to/usd/card` with the uSD
card mount point, and `<version string>` with the actual version string):

* `<version string>.zImage` from the `ti-linux-kernel-dev/deploy/` directory into
  `/path/to/usd/card/boot/vmlinuz-<version string>`.

* `config-<version string>` into `/path/to/usd/card/boot/`

* unpack `<version string>-modules.tar.gz` into `/path/to/usd/card`

* unpack `<version string>-firmware.tar.gz` into `/path/to/usd/card/lib/firmware`

* unpack `<version string>-dtbs.tar.gz` into `/path/to/usd/card/boot/dtbs/<version string>`

## Build and install the driver and applications
Now that all the setup is done, the kernel module and applications may be
built.

### Building the driver and applications
First, set up the environment so that the correct toolchain and kernel
source directory can be found.
```
$ export PATH=$PATH:/path/to/ti-linux-kernel-dev/dl/gcc-linaro.../bin
$ export CROSS_COMPILE=arm-linux-gnueabihf-
$ export KDIR=/path/to/ti-linux-kernel-dev/KERNEL
```
(Replace gcc-linaro... with the directory name found under `dl/`).

Next, build the I2C application that commands the Lepton to begin transmitting
VSYNC pulses from its GPIO3 output and the user-space data collector
application by running `make` from the top-level directory (the same directory
containing this README).
```
$ cd /path/to/Lepton/
$ make
```

Next, build the kernel module:
```
$ cd /path/to/Lepton/lepton_module/
$ make
```

### Build and install the device-tree bindings for the Lepton
The Lepton device tree bindings assigns it to the SPI0 controller, enables the
VSYNC input on connector P8 pin 17 as an IRQ source, sets up the SPI clock at
its maximum 24MHz rate (the next step down, 16MHz, results in latency issues),
configures the SPI pins, and sets CPOL=1 and CPHA=1 to match the Lepton.
The device tree source file, `flir-lepton-BBB.dts`, is currently found in the
`lepton_module/` subdirectory of the source code repository.  It's simplest to
build it from inside the `bb.org-overlays` repo, which has the include files
for building device tree overlays for the BBB:
```
$ git clone https://github.com/beagleboard/bb.org-overlays -b legacy-dtc-1.4.4
$ cd bb.org-overlays/src/arm
$ ln -s /path/to/Lepton/lepton_module/flir-lepton-BBB.dts .
$ cd ../..
$ make
```

If the `make` command fails with `dtc: command not found` or if dtc produces a
syntax error, try setting the DTC environment variable to the
path to `dtc` built in the KERNEL directory `/path/to/ti-linux-kernel-dev/KERNEL/scripts/dtc`. 
```
$ make DTC=$KDIR/scripts/dtc/dtc
```
This is only needed if the system's dtc compiler is too old, or isn't installed.

When successful, the `flir-lepton-BBB.dtbo` file (output to
`bb.org-overlays/src/arm`) may then be copied to the uSD card into the
`/lib/firmware/` directory.

### Installing the driver and applications to the BBB
Create a directory from the super-user's home directory for the driver and
applications with `mkdir lepton_vospi`.

Use scp or copy the following files directly to the uSD card in the new
directory:

* `/path/to/Lepton/lepton_control/vsync_app`

* `/path/to/Lepton/lepton_module/lepton.ko`

* `/path/to/Lepton/lepton_data_collector/lepton_data_collector`

In order to be able to use `modprobe` rather than `insmod` to load the module,
install the module alongside the other kernel modules (example commands assume
direct copying to uSD card from development host):
```
$ sudo su
# mkdir /path/to/usd/card/lib/modules/<version string>/extra
# cp lepton.ko /path/to/usd/card/lib/modules/<version string>/extra
```

#### Edit the uEnv.txt file
The kernel compiled on the development system should be specified here.
Find the line starting with `uname_r=`, and set it to
`uname_r=<version string>`.  `version string` comes after the `-` character
in the `vmlinuz-?` copied into the uSD `/boot/` directory in a previous step.

The driver uses contiguous dma-coherent memory, which may need to be increased
using the kernel command line.  Edit the line starting with `cmdline=` and add
the following key=value entries to the string: `coherent_pool=1M cma=40M`.
When editing the command line it is also helpful to remove the `quiet`
parameter for more verbose output during boot.

The device tree bindings should also be added to this file.  Edit a line in the 
`Additional custom capes` section, and change E.G. `<file4>` to
`flir-lepton-BBB`.
For example, change:
```
#uboot_overlay_addr4=/lib/firmware/<file4>.dtbo
```
to
```
uboot_overlay_addr4=/lib/firmware/flir-lepton-BBB.dtbo
```
(remember to remove the comment character).

#### Reboot and check driver latency
Reboot the BeagleBone Black, and verify that it boots up successfully.

Log in as super-user, command the Lepton to begin transmitting VSYNC pulses,
and load the driver (`depmod` is only needed the first time):
```
# /root/lepton_vospi/vsync_app
# depmod -a
# modprobe lepton dyndbg=+p
```
(If the lepton module was loaded at boot-up, the latency debugging should be
set up separately; create a new file `lepton.conf` under `/etc/modprobe.d/`
with the contents: `options lepton dyndbg=+p`, then reboot)

This is the time to determine whether the frame loss rate is acceptable or
not, as the driver will continuously perform SPI DMA transfers in order to
synchronize with the Lepton before frame capture can begin.  Use the `dmesg`
command to look for `VSYNC NNNNNN miss!`, `VSYNC warning`, or `Lost sync`
messages that indicate latency issues reported from the driver.  As noted
above, missed VSYNC interrupts potentially provokes the Lepton to produce
discard packets in subsequent frames until either it recovers or the Lepton
is reset.

Reducing latency is a complicated subject, but sources of latency in the kernel
can be discovered using ftrace (this tool was used to discover 8 to 9
millisecond latency introduced by the CPU idle code).
Other sources of latency include unnecessary processes started up at boot time,
so if using the debian distribution, it may help to disable unused server
applications using the `systemctl` command.

#### Capture frames
Use the `lepton_data_collector` application to capture grayscale images from
the video device.  It consumes raw subframes via the V4L2 `/dev/videoN` device
file (`/dev/video0` by default) and extracts the pixel data, and for Lepton 3.X
(with the command-line argument `-3`) assembles four subframes into a single
larger video frame.  The image files are named after a prefix specified with
the `-o` command-line argument, followed by a 6-digit (prefixed with 0's for
smaller numbers) frame counter and a .gray extension.  Also, to reduce latency,
it is a good idea to have it store frames into memory instead of to a flash
device, so mount a tmpfs directory somewhere and prepend the path to the
prefix.

Example:
```
# cd /root/lepton_vospi
# mkdir /tmp/capture
# mount -t tmpfs tmpfs /tmp/capture
# ./lepton_data_collector -3 -c 50 -o /tmp/capture/frame_
```
When complete, there should be 50 images captured from Lepton 3.X (about 5
seconds worth at ~9 fps) named `/tmp/capture/frame_000000.gray` through
`/tmp/capture/frame_000049.gray`.  These can be viewed on a Linux system using
the ImageJ Java application, available from
<https://imagej.nih.gov/ij/download.html>.  From the File menu, select
`Import->Raw...`, and set image type to 16-bit unsigned along with the width
and height (80x60 for Lepton 2.X, 160x120 for Lepton 3.X) in the dialog box
that appears after selecting the file name.  The entire sequence can be placed
into an image stack if the `Open all files in folder` checkbox is checked.
Loading the files into an image stack makes it possible to produce a .avi movie
from the frames using `File->Save As->AVI...` and setting the frame rate to 9
fps.

