# Getting started with the RaspberryPi and Breakout board

## Disclaimer

This guide is for the newer breakout board (seen here). Earlier versions of the breakout board do not include ```VSYNC```.

![Correct breakout board](https://lepton.flir.com/wp-content/uploads/2015/06/1.jpg)

## Requirements

- A computer with the ability to read/write to MicroSD cards
  - A laptop using Linux was used during this guide.
- A RaspberryPi 2/3
  - You can find them [here](https://www.digikey.com/products/en/development-boards-kits-programmers/evaluation-boards-embedded-mcu-dsp/786?k=Raspberry+Pi&k=&pkeyword=Raspberry+Pi&pv1742=726&sf=0&FV=ffe00312&quantity=&ColumnSort=0&page=1&pageSize=25).
  - A MicroUSB to USB-A cable.
  - A MicroSD card with at least 8GB of capacity. MicroSD cards preinstalled with NOOBs can be found [here](https://thepihut.com/collections/raspberry-pi-sd-cards-and-adapters/products/noobs-preinstalled-sd-card).
  - Labelled as __1__.
- A Lepton camera (2.X/3.X)
  - They can be found [here](https://www.digikey.com/products/en/sensors-transducers/image-sensors-camera/532?k=&pkeyword=&s=48165&s=64527&sf=0&FV=ffe00214&quantity=&ColumnSort=0&page=1&pageSize=25). For a higher resolution and telemetry data, it is recommended that you use a [Lepton 3.5](https://www.digikey.com/product-detail/en/flir/500-0771-01/500-0771-01-ND/7606616).
  - Labelled as __2__.
- A breakout board V2.0
  - Details on the board can be found [here](https://lepton.flir.com/hardware/#lepton-breakout-board-v20).
  - Labelled as __3__.
- Female-to-Female Jumper cables
  - Labelled as __4__.
- Some additional software that can be found [here](FIXME put link here).

![Labelled hardware requirements](https://lepton.flir.com/wp-content/uploads/2019/01/2-1.jpg)

## Hardware

### Attaching the breakout board to the RPi

You can find more information on the GPIO and the pinout of the RaspberryPi [here](https://www.raspberrypi.org/documentation/usage/gpio/). Attach (female-to-male) jumper wires between the following breakout board pins and the RaspberryPi board:

![Labelled breakout board](https://lepton.flir.com/wp-content/uploads/2019/01/breakout_labelled.jpg)

- (J2 Pin) -> (Proper name) -> (RPi connector pin)
- P8 -> SCL -> GPIO 3
- P5 -> SDA -> GPIO 2
- P12 -> MISO -> GPIO 9
- P7 -> CLK -> GPIO 11
- P10 -> CS -> GPIO 8
- P15 -> VSYNC -> GPIO 17

We will also need to connect power and ground to the board:

![Labelled breakout board](https://lepton.flir.com/wp-content/uploads/2019/01/vin_gnd.jpg)

- (J3 Pin) -> (Proper name)
- P1 (__Square pin__) -> GROUND
- P2 -> VIN

### Setting up the MicroSD

You have two options for setting up the Micro SD:

<!-- - Using a prebuilt image
  - For this guide we will assume you have access to a linux terminal and a Micro SD card reader.
  - It will come with a kernel, kernel modules and device tree already setup. -->

- Using NOOBs to get an image
- Building the image manually

<!-- ##### Prebuilt image

Insert the MicroSD into your computer. You will need to find out the location of the SD card. The following command will give you that information:

```bash
lsblk
```

![Labelled breakout board](https://lepton.flir.com/wp-content/uploads/2018/12/lsblk-1.png)

A prebuilt image for the `PI` can be found in the `RaspberryPi` folder, with the name `raspberry_pi.img`. This contains the kernel setup with the kernel module for the `lepton` camera preinstalled.

```bash
sudo dd bs=4M if=raspberry_pi.img of=/dev/<SDCARDNAME> status=progress
```

Once this is complete, you can insert it into your Raspberry Pi. -->

#### NOOBs

You will need to install the latest Raspbian image to the MicroSD, which can be found [here](https://www.raspberrypi.org/downloads/). The simplest way of installing the new image would be through the Raspberry Pi Foundation's [NOOBs New Out Of the Box software](https://www.raspberrypi.org/downloads/noobs/).

A full guide to installing with NOOBs can be found [here](https://www.raspberrypi.org/learning/software-guide/quickstart/).

When installing through NOOBs, you will be walked through the installation and updating the raspberrypi to the latest version. 

#### Manually

If you want to install it manually, some instructions can be found [here](https://www.raspberrypi.org/documentation/installation/installing-images/README.md).

## Software

This guide takes the approach of cross-compiling the lepton driver from a
separate Linux development workstation (or a Linux VM running on a development workstation).
Doing a kernel compile on a workstation is normally *far* faster than doing
it natively on the Raspberry Pi itself.

### Collecting source code and build information

This whole section will be performed on the Raspberry Pi itself, so boot up the
Pi. We will need access to a terminal and an internet connection. There are
some dependencies, install them using the following command:

```bash
pi@raspberrypi:~ $ sudo apt install git bc flex
```

Now acquire the kernel source code that matches the running kernel:

```bash
pi@raspberrypi:~ $ git clone https://github.com/RPi-Distro/rpi-source
pi@raspberrypi:~ $ # this will download the matching kernel source
pi@raspberrypi:~ $ ./rpi-source/rpi-source --nomake
```

and a copy of the current kernel config:

```bash
pi@raspberrypi:~ $ sudo modprobe configs 
pi@raspberrypi:~ $ cp /proc/config.gz ~/
pi@raspberrypi:~ $ gunzip ~/config.gz
pi@raspberrypi:~ $ chmod 644 ~/config
```

Determine the current versions of OS version, kernel compiler, source code:

```bash
pi@raspberrypi:~ $ # make note of OS version for later step of selecting which toolchain to install
pi@raspberrypi:~ $ cat /etc/debian_version
pi@raspberrypi:~ $ # make note of compiler version for later step of selecting which toolchain to install
pi@raspberrypi:~ $ grep -o "gcc version \S*" /proc/version
pi@raspberrypi:~ $ # make note of kernel version for later step of selecting the lepton driver code to build
pi@raspberrypi:~ $ grep -o "Linux version \S*" /proc/version
```

### Installing build tools for the Pi onto a Linux build host

Perform this task on your Linux workstation (or Linux VM, if that's all you have available).
Pretty much any Linux distribution will work, but if you're using something other than Debian or Ubuntu
(or one of their derivatives, such as Mint) then you'll need to figure out local equivalents for
this suggested ```apt``` command to install build dependencies.

```bash
user@desktop:~ $ sudo apt install git bc flex bison libncurses5-dev libssl1.0-dev rsync
```

The cross-compiler toolchain that you install needs to be compatible with
the kernel and modules running on your Raspberry Pi. The Raspberry Pi project 
stuck with one toolchain version across many releases of the OS, but recently
switched to a new toolchain.

* If your compiler version on the Pi was reported as ```gcc 4.9.3```, install "old toolchain"
* Otherwise, install "new toolchain"


#### Install old cross-compiler toolchain


[This is the toolchain](https://github.com/raspberrypi/tools) that was used to create many releases of Raspberry Pi OS

```bash
user@desktop:~ $ git clone git://github.com/raspberrypi/tools.git
user@desktop:~ $ export PATH=$PWD/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin:$PATH
```

#### Install new cross-compiler toolchain


[This toolchain project](https://github.com/abhiTronix/raspberry-pi-cross-compilers) provides a variety of toolchains, so you will need to select the one that matches the OS version and gcc version reported on your Pi. 

[https://github.com/abhiTronix/raspberry-pi-cross-compilers/wiki/Cross-Compiler:-Installation-Instructions#b-download-binary](https://github.com/abhiTronix/raspberry-pi-cross-compilers/wiki/Cross-Compiler:-Installation-Instructions#b-download-binary)

Look here to translate the OS version number from your Pi to one of the listed codenames 
(e.g. "10.4" is called "buster"):
[https://en.wikipedia.org/wiki/Debian_version_history](https://en.wikipedia.org/wiki/Debian_version_history)

After downloading the matching toolchain, follow the "Temporary Installation" instructions,
which includes setting both ```PATH``` and ```LD_LIBRARY_PATH``` environment variables

[https://github.com/abhiTronix/raspberry-pi-cross-compilers/wiki/Cross-Compiler:-Installation-Instructions#c1-temporary-installation-use-these-binaries-directly-recommended](https://github.com/abhiTronix/raspberry-pi-cross-compilers/wiki/Cross-Compiler:-Installation-Instructions#c1-temporary-installation-use-these-binaries-directly-recommended)

### Prepare source code on build host

The kernel code and kernel config were acquired earlier using the Raspberry Pi, 
to make sure they are consistent with what is currently installed on the Pi.
Copy them onto your build host now. Note the destination path for ```config```, 
including the added dot.

```bash
user@desktop:~ $ rsync -avz pi@raspberrypi.local:linux/ ~/linux/
user@desktop:~ $ rsync -avz pi@raspberrypi.local:config ~/linux/.config
```

If you have trouble reaching your pi on the network as "raspberry-pi.local",
you can try [these tips](https://www.raspberrypi.org/documentation/remote-access/ip-address.md)
to find its IP address and substitute that address instead.

Next, unpack the lepton source code. The link to lepton source was given in
"Requirements" section, so it can be directly downloaded to your build host. 
Unpack that code (adust the path if the file got downloaded somewhere other than
"Downloads" folder):

```bash
user@desktop:~ $ tar -xvf ~/Downloads/rpi-lepton.tar.gz
```

The lepton source code needs to be configured to match the kernel version
on your Pi. Use the pattern 'kernel-X.Y' below, where X.Y is a placeholder
for the first two parts of the actual version number. For instance, if the
kernel version was reported earlier as "Linux version 5.4.51-v7+"

```bash
user@desktop:~ $ cd ~/rpi-lepton/lepton_module
user@desktop:~/rpi-lepton/lepton_module$ git checkout kernel-5.4
```

Be sure to use the version that matches your Pi version, e.g. replace
"kernel-5.4" with "kernel-4.19" if your Pi is still running 4.19.

### Cross-compile software on build host

This whole section will be performed on your Linux build host, rather than on the Pi itself.
First set up the environment in the command terminal where you are working: 

```bash
user@desktop:~ $ export ARCH=arm
user@desktop:~ $ export CROSS_COMPILE=arm-linux-gnueabihf-
user@desktop:~ $ export export LOCALVERSION="+"
```

Verify that ```PATH``` (and maybe ```LD_LIBRARY_PATH```, depending on which toolchain is installed)
are still set per the toolchain instructions followed earlier, and set them again if 
needed.

```bash
user@desktop:~ $ echo $PATH
user@desktop:~ $ echo $LD_LIBRARY_PATH
```

Compile the Lepton driver and its corresponding device tree overlay:

```bash
user@desktop:~ $ cd ~/linux
user@desktop:~/linux $ make -j $(nproc) oldconfig zImage modules 
user@desktop:~/linux $ cd ~/rpi-lepton/lepton_module
user@desktop:~/rpi-lepton/lepton_module $ make -C ~/linux M=$PWD modules 
user@desktop:~/rpi-lepton/lepton_module $ ~/linux/scripts/dtc/dtc flir-lepton-00A0.dts -o flir-lepton-00A0.dtbo
```

Ignore any warnings that come from compiling. 

Now compile the example lepton apps:
```bash
user@desktop:~/rpi-lepton/lepton_module $ cd ~/rpi-lepton
user@desktop:~/rpi-lepton $ make
```

### Copy built software onto Raspberry Pi

Now that the software has been built, copy it from the build host to the Pi.

```bash
user@desktop:~ $ rsync -avz ~/rpi-lepton pi@raspberrypi.local:rpi-lepton
```

As before, you might need to replace "raspberry-pi.local" with the Pi's IP address.

### Install lepton software on Raspberry Pi

This whole section will be performed on your Pi.

Install the user apps.

```bash
pi@raspberrypi:~ $ cd ~/rpi-lepton
pi@raspberrypi:~/rpi-lepton $ sudo cp lepton_control/vsync_app /usr/bin/
pi@raspberrypi:~/rpi-lepton $ sudo cp lepton_data_collector/lepton_data_collector /usr/bin/
```

Install the kernel module.

```bash
pi@raspberrypi:~ $ cd ~/rpi-lepton/lepton_module
pi@raspberrypi:~/rpi-lepton/lepton_module $ sudo mkdir -p /lib/modules/$(uname -r)/extra
pi@raspberrypi:~/rpi-lepton/lepton_module $ sudo cp lepton.ko /lib/modules/$(uname -r)/extra
pi@raspberrypi:~/rpi-lepton/lepton_module $ sudo depmod -a
```

You will need to install the ```.dtbo``` file into the overlays folder.

```bash
pi@raspberrypi:~/rpi-lepton/lepton_module $ sudo cp flir-lepton-00A0.dtbo /boot/overlays/
```

You will then need to uncomment following values for flags in ```/boot/config.txt```:

```bash
dtparam=i2c_arm=on
dtparam=i2s=on
dtparam=spi=on
```

This can be done by opening the file and ensuring there is no `#` in front of them.

```bash
pi@raspberrypi:~ $ sudo nano /boot/config.txt
```

This will enable i2c and spi after the Raspberry Pi is rebooted. Alongside uncommenting these, you will also need to add the following line:

```
dtoverlay=flir-lepton-00A0
```

This will enable the overlay you just built. You will need to reboot the RaspberryPi for the changes to ```config.txt``` to take effect.

```bash
pi@raspberrypi:~ $ sudo reboot
```

We should be set! Lets test if this worked.

## Testing the lepton camera on the Pi

Verify that the lepton is powered up and responding to i2c commands

```bash
pi@raspberrypi:~ $ sudo apt install i2c-tools
pi@raspberrypi:~ $ sudo modprobe i2c-dev
pi@raspberrypi:~ $ i2cdetect -r -y 1 0x2a 0x2a
```

If the lepton is responding, you will see

```
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                                                 
10:                                                 
20:                               2a                
30:                                                 
40:                                                 
50:                                                 
60:                                                 
70:                                                 
```

If you see dashes `--` in place of `2a`, there is a wiring or hardware problem
that needs to be resolved before going any further. Check the wiring and also 
that the lepton is firmly seated in its socket.

Once everything is connected, run `vsync_app` 

```bash
pi@raspberrypi:~ $ vsync_app
```

The program should terminate quickly after performing various I2C commands. If it runs indefinitely, quit the program, disconnect the VIN and GND, and try again.

You can use the `lepton_data_collector` application to capture grayscale images from the video device. 

- It consumes raw subframes via the V4L2 `/dev/videoN` device file (`/dev/video0` by default) and extracts the pixel data.
- For Lepton 3.X (with the command-line argument `-3`) it assembles four subframes into a single larger video frame.
  - If you are using a Lepton 2.X, use the command line argument `-2` instead.
- The image files are named after a prefix specified with the `-o` command-line argument, followed by a 6-digit (prefixed with 0's for smaller numbers) frame counter and a .gray extension.
- To reduce latency, it is a good idea to have it store frames into memory instead of to a flash device, so mount a tmpfs directory somewhere and prepend the path to the prefix.

Here is an example:

```bash
pi@raspberrypi:~ $ sudo su
root@raspberrypi:/home/pi# mkdir /tmp/capture
root@raspberrypi:/home/pi# mount -t tmpfs tmpfs /tmp/capture
root@raspberrypi:/home/pi# lepton_data_collector -3 -c 50 -o /tmp/capture/frame_
```

When complete, there should be 50 images captured from Lepton 3.X (about 5 seconds worth at ~9 fps) named `/tmp/capture/frame_000000.gray` through `/tmp/capture/frame_000049.gray`.

These images can be viewed on a Linux system using the ImageJ Java application, available from [here](<https://imagej.nih.gov/ij/download.html>). From the File menu, select `Import->Raw...`, and set image type to 16-bit unsigned along with the width and height (80x60 for Lepton 2.X, 160x120 for Lepton 3.X) in the dialog box that appears after selecting the file name.  The entire sequence can be placed into an image stack if the `Open all files in folder` checkbox is checked. Loading the files into an image stack makes it possible to produce a .avi movie from the frames using `File->Save As->AVI...` and setting the frame rate to 9 fps.

## Troubleshooting

### Before troubleshooting

Additional debug information can be retrieved by running:

```bash
user@desktop:~ $ dmesg | less
```

### "I just can't seem to get the module to load!"

There might have been either an issue when compiling or the dependencies weren't correctly found. Try installing the lepton module manually again:

```bash
user@desktop:~ $ sudo modprobe lepton
```

If the lepton module can't be found, try running the following command to build the dependencies:

```bash
user@desktop:~ $ sudo depmod -a
```

Now, run ```modprobe``` again. 
