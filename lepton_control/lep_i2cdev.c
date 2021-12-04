// Lepton I2C user-space control
// lep_i2cdev.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "LEPTON_Types.h"
#include "LEPTON_ErrorCodes.h"
#include "LEPTON_Macros.h"
#include "LEPTON_I2C_Reg.h"

#include "lep_i2cdev.h"

#define DEBUG 5

#ifdef DEBUG
int debug_level = DEBUG;

#define DEBUG_PRINT(level, args...) { \
    if (level <= debug_level) { \
        fprintf(stderr, args); } }

#define DEBUG_DUMP(level, dptr, dformatwidth, dcount, per_line) { \
    if (level <= debug_level) { \
        int __debug_dump_i = 0, __debug_dump_j = 0; \
        while(__debug_dump_i<dcount) { \
            for (__debug_dump_j=0; __debug_dump_j<per_line; __debug_dump_j++) { \
                if ((__debug_dump_i + __debug_dump_j) >= dcount) \
                    break; \
                fprintf(stderr, "%0*x ", dformatwidth, dptr[__debug_dump_i+__debug_dump_j]); } \
            fprintf(stderr, "\n"); \
            __debug_dump_i+=per_line; } \
        fprintf(stderr, "\n"); } }

#else
#define DEBUG_PRINT(level, args...) {}
#define DEBUG_DUMP(level, dptr, dformatwidth, dcount, per_line) {}
#endif

#ifndef I2C_DEVICE_FILE
#error I2C_DEVICE_FILE should be defined in CFLAGS
#endif

#define ADDRESS_WIDTH 2  // in bytes

int lepton_i2c_fd = -1;

// Fixed to I2C2
int i2cdev_init()
{
    DEBUG_PRINT(5, "%s() called.\n", __func__)
    if (lepton_i2c_fd >= 0)
    {
        close(lepton_i2c_fd);
        lepton_i2c_fd = -1;
    }
    lepton_i2c_fd = open(I2C_DEVICE_FILE, O_RDWR);
    if (lepton_i2c_fd < 0)
    {
        DEBUG_PRINT(1, "Failed I2C open(): %s\n", strerror(errno))
        return -1;
    }
    else
    {
        if (ioctl(lepton_i2c_fd, I2C_SLAVE, LEP_I2C_DEVICE_ADDRESS) < 0)
        {
            DEBUG_PRINT(1, "Failed I2C ioctl(): %s\n", strerror(errno))
            return -1;
        }
    }
    DEBUG_PRINT(2, "I2C device successfully opened.\n")
    return 0;
}

void i2cdev_close()
{
    DEBUG_PRINT(5, "%s() called.\n", __func__)
    if (lepton_i2c_fd >= 0)
    {
        close(lepton_i2c_fd);
        lepton_i2c_fd = -1;
    }
    DEBUG_PRINT(2, "I2C device closed.\n")
}

int i2cdev_read_byte_data(LEP_UINT8 *rx_adr, LEP_UINT8 *rx_data, LEP_UINT32 rx_size)
{
    int read_count = 0;

    DEBUG_PRINT(5, "%s(rx_adr=%p, rx_data=%p, rx_size=%ld) called.\n", __func__, rx_adr, rx_data, rx_size);
    DEBUG_PRINT(4, "Writing big-endian address 0x%02x%02x\n", rx_adr[0], rx_adr[1])
    int write_data = write(lepton_i2c_fd, rx_adr, ADDRESS_WIDTH);
    if (write_data < 0)
    {
        DEBUG_PRINT(1, "Failed I2C write(%d, 0x%02x%02x, %d): %s\n", lepton_i2c_fd, rx_adr[0], rx_adr[1], ADDRESS_WIDTH,
                    strerror(errno))
        return -1;
    }
    read_count = read(lepton_i2c_fd, rx_data, rx_size);
    if (read_count < 0)
    {
        DEBUG_PRINT(1, "Failed I2C read(%d, %p, %d): %s\n", lepton_i2c_fd, rx_data, rx_size, strerror(errno))
        return -1;
    }
    DEBUG_PRINT(4, "Read back %d bytes from I2C read:\n", read_count)
    DEBUG_DUMP(4, rx_data, 2, read_count, 32)
    return read_count;
}

int i2cdev_write_byte_data(LEP_UINT8 *tx_data, LEP_UINT32 tx_size)
{
    int write_count = 0;

    DEBUG_PRINT(5, "%s(tx_data=%p, tx_size=%d) called.\n", __func__, tx_data, tx_size)

    DEBUG_PRINT(4, "Write %d bytes in tx_data:\n", tx_size)
    DEBUG_DUMP(4, tx_data, 4, tx_size, 16)
    write_count = write(lepton_i2c_fd, tx_data, tx_size);
    if (write_count < 0)
    {
        DEBUG_PRINT(1, "Failed I2C write(%d, %p, %d): %s\n", lepton_i2c_fd, tx_data, tx_size, strerror(errno))
        return -1;
    }
    return write_count;
}

