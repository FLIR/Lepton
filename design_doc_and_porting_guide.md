# Design Documentation and Porting Guide

## Lepton driver design

The Lepton driver has been streamlined as much as possible to collect subframes
(full frames for Lepton 2.X) as quickly as possible with minimal processing.
The V4L2 and videobuf2 kernel frameworks were selected due to their suitability
for video data capture, for convenience in allocation and handling of data
buffers, and for supplying a sample user-space application that communicates
with compliant video device drivers to determine video parameters and performs
most of the needed operations already (see
<https://www.kernel.org/doc/html/v4.10/media/uapi/v4l/capture-example.html>).
The V4L2 documentation's recommended strategy of streaming raw data from a
video capture device and performing any needed processing in user-space is the
approach used in the overall Lepton kernel module design.  See
<https://www.kernel.org/doc/html/v4.10/media/kapi/v4l2-core.html> for online
documentation and API references for both V4L2 and videobuf2.  Another resource
that was helpful in designing the driver was a series of well-written LWN
articles found at <https://lwn.net/Articles/203924/>, although they have become
somewhat out-of-date since 2006.

As noted in the README, low latency is crucial for keeping the Lepton from
entering an error state from which it may not recover.  Any gaps in frame
collection result in discard packets appearing in subsequent frames.
Therefore, every VSYNC IRQ must trigger a SPI DMA that completes before the
next interrupt arrives, and the driver also needs to recognize when
synchronization has been lost due to missed timing and attempt recovery.

Another consideration is that the first packets read from the Lepton may begin
with a non-zero linecount, so an extra line needs to be read out in each DMA
transfer to synchronize later transfers so they begin with a zero linecount
(reading an extra line once synchronized is harmless, as the extra line will
contain a discard packet).  For this reason, the driver will always perform
SPI DMA transfers even when the device file hasn't been opened, so that the
synchronization can complete before an application opens the device and begins
streaming video data.  This also avoids provoking the Lepton into entering
its error state when an application stops streaming.

Some care has been taken to make it possible to unload and reload the module,
though this hasn't been extensively tested and it is not recommended to do so
in a production environment.  This should make code changes somewhat easier to
debug without requiring a reboot in between (beware that it is all to easy to
provoke the Lepton into its error state; adding GPIOs to control the Lepton
power-down and reset lines via software is also recommended).

### Device tree fragment for Lepton
A device tree fragment in `flir-lepton-00A0.dts` is used for configuring which
SPI controller and SPI settings (CPHA, CPOL, I/O pins, bus speed, etc) are to
be used by the Lepton, as well as which GPIO to register for the VSYNC IRQ.
The driver parses the device tree entry to register the IRQ when the module
is loaded.  When porting to another Linux platform, replace this device tree
fragment with one that matches the GPIOs and SPI controller on the target
system.  This should be the bulk of the porting work required for Linux.

### High-level driver code walkthrough
The driver code can be found in the file
`lepton_vospi_driver/lepton_module/flir_lepton.c`, which has a corresponding
header file `lepton_vospi_driver/lepton_module/flir_lepton.h`.  Code, defines,
and structures specific to the Lepton video data format have been placed in a
separate set of files found in `lepton_vospi_driver/lepton_vospi_lib/` so they
can be shared with user-space code; the files have been soft-linked into the
`lepton_module/` directory to simplify the build.

#### Module private data
The private data used by the driver is contained in `struct lepton`.  It
contains the mutex and lock needed for protecting the other struct fields while
being updated.  The following briefly describes the other struct members:

* started - A user-space application has sent the START_STREAMING ioctl to the
  driver; cleared when the STOP_STREAMING ioctl is received

* synced - The previous frame had a valid line counter in its last video data
  packet, so valid data should now be received in subsequent packets; reset
  when an invalid data packet is encountered

* telemetry_enabled - This is a placeholder for adding telemetry support

* unfilled_bufs - Holds a linked list of available buffers queued by user space

* spare_buf - A struct containing spare buffer parameters; the spare buffer is
  used when no buffers have been queued from user space (usually because no
  application is running), and is sized to hold one extra line for
  synchronization

* v4l2_dev, vid_dev - Structs used by the V4L2 subsystem

* q - A struct used by videobuf2

* spi_dev - A struct used by the SPI subsystem

* vsync_count - A simple interrupt counter

* lep_vospi_info - A struct used by the lepton_vospi_lib functions, used for
  properly sizing the SPI DMA buffer (the size will be variable once telemetry
  is supported)

* current_lep_buf - Holds the address of last queued buffer removed from the
  unfilled_bufs list, used by the DMA completion function to de-queue buffers
  back to user space

* spi_xfer, spi_msg - Used in interrupt context for setting up async SPI DMA
  transfers

* last_spi_done_ts - Timestamp set during DMA completion to check for
  timing problems

#### V4L2 and videobuf2 ioctl callbacks
Following the private struct declarations are the callbacks used for V4L2
ioctl()s, along with the `lepton_set_fmt_fields()` function that fills in the
raw video format parameters returned to user space by a few of the callbacks.

Next, the lepton_fops and lepton_ioctl_ops structs are filled in; the
lepton_fops callbacks are delegated to V4L2 and videobuf2 functions, and the
lepton_ioctl_ops callbacks point to the V4L2 ioctl functions declared above.
The lepton_videodev_template struct is then defined using these structs for
its fops and ioctl_ops members.

The next section defines the videobuf2 ioctl() callbacks, which are then
referenced by the lepton_video_qops struct.  These callbacks are used for
buffer management and also start and stop streaming.

#### Device tree info
The following structs, lepton_id_table and lepton_of_match, are used during the
driver probe to locate the correct device tree node.  These are referenced
later in the lepton_spi_driver definition.

#### Interrupt handling
By this point, the setup code is mostly complete, and the meat of the driver
code begins.  The following covers the IRQ handling and SPI DMA transfer
functions:

* `lepton_vsync_handler()` - This is the IRQ handler for the VSYNC input; it
  double-checks the timing to make sure the previous DMA has finished before
  attempting to start another, and will print a warning if the previous DMA
  completed less than one millisecond before interrupt arrived.  Depending on
  buffer availability, it will either fill in queued buffer with VoSPI data, or
  will fill in a spare buffer if streaming hasn't started yet, or if
  synchronization was lost.

* `lepton_start_transfer()` - This is called in interrupt context by the
  `lepton_vsync_handler()` function.  It sets up the `spi_msg` and `spi_xfer`
  structs , and clears the buffer to 0's before passing them all on to the
  `spi_async()` function.

* `lepton_spi_done_callback()` - This is called when `spi_async()` completes,
  and SPI DMA data has been written into the buffer.  It will modify the
  following private struct members:

    - `last_spi_done_ts` - This lets the interrupt handler know that the DMA has
      completed, so it can kick off a new one; it will also check the timestamp
      to display latency warnings to dmesg

    - `current_lep_buf` - This is copied then cleared; the copy will be used
      outside the spin-lock to decide whether the buffer needs to be de-queued
      back to user space

    - `synced` and `discard_count` - These are set based on whether the
      recieved frame had a valid line count in its last video data packet

If the current_lep_buf copy was non-NULL and the recieved frame looked ok, the buffer
will then be de-queued with the VB2_BUF_STATE_DONE flag set; if the frame was
bad, the VB2_BUF_STATE_ERROR flag will be set to warn the user-space
application that data corruption has occurred (this is expected to be
recoverable), and a `Lost frame sync!` debug message will be sent to dmesg.

#### Driver probe
The next function `lepton_probe()` is called when the module is loaded and a
matching node in the device tree is found.  It performs allocation and
initialization of private data, V4L2 and videobuf2 data and SPI structs;
registers with the V4L2 subsystem; registers the device file; and finally sets
up the interrupt handler once the driver is ready to begin handling the next
VSYNC rising edge from the Lepton.

#### Driver removal
The `lepton_remove()` function unregisters the video device file, and also
unregisters the driver from the V4L2 subsystem.  The memory allocated for
needed structs was allocated using `devm_kzalloc()` and is therefore expected
to be automatically freed when the module is unloaded.

#### SPI subsystem initialization
The lepton_spi_driver definition completes the driver code, referencing the
`lepton_of_match` and `lepton_id_table` structs and `lepton_probe()` and
`lepton_remove()` functions declared above, before being passed on to
`module_spi_driver()`.

## Lepton VoSPI library code
The `lepton_vospi_lib/` subdirectory contains code that can be shared between
the driver and user-space.  The `lepton_vospi_info` struct collects parameters
needed by both code bases, and is initialized using the `init_lepton_info()`
function, which takes the Lepton version (as an enum for 2.X or 3.X) and
telemetry location (an enum with off, start, or end values) as arguments.
After initialization, the driver may then make use of the struct for sizing the
SPI DMA buffer as well as for parameters returned via V4L2 ioctls, and as the
first parameter to `is_subframe_line_counter_valid()` used for checking data
synchronization.  User-space applications may use the struct entries for
determining the pixel width and height of image data, and pass the struct
itself to utility functions that search for valid subframe indices, check for
line counter errors, and extract pixel data (even from multiple Lepton 3.X
subframes).

### Subframe index extraction
The `is_subframe_index_valid()` function is useful both for synchronization
with subframe 0 of Lepton 3.X frames, and for verifying received subframe
indices; it will always return 1 for Lepton 2.X frames.  It automatically
updates the expected next subframe index when called; the last subframe or any
missed subframe resets the expected next subframe index (the index is of course
not used for Lepton 2.X).

### Pixel data extraction
The `extract_pixel_data()` function places image data into its `pixel_data`
parameter.  Its `done` output parameter will be set to 1 for all Lepton 2.X
frames, or when the last subframe from a Lepton 3.X has been received, and
signals that the full frame of pixel data is ready for storage or transmission
(the same unchanged `pixel_data` buffer should be passed in until `done` is set
to 1).

## User-space application design
The user-space application `lepton_data_collector` began as a copy of the
sample `capture.c` source code (see above for the URL).  The `-o` option
was modified to produce separate files for each frame for easy storage of frame
sequences.  The `-c` frame count option was altered to count full video frames
received from the driver instead of subframes (needed for Lepton 3.X).  The
`-2` and `-3` options were added for specifying the Lepton version (auto-
detection has not been implemented, and would require multiple frames to be
parsed while searching for the presence or absence of subframe indices.
Alternatively, the I2C application could be merged with this application, and
I2C commands could be used to collect the Lepton version).  The `-t` option
was added for future telemetry support.  Note that although other buffer
location options are made available with the `-r` and `-u` options, only the
`mmap` storage type (the default) has been tested.

Frames received from the driver will be passed into the library function
`is_subframe_index_valid()` to make sure the subframe index is correct for
Lepton 3.X subframes and also to synchronize on subframe 0.  The
`extract_pixel_data()` library routine is then called to produce full image
frames to be stored as soon as the `done` output parameter is set to 1.

