// BeagleBone Green Lepton I2C user-space control
// bbg_lepton_I2C.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "LEPTON_Types.h"
#include "LEPTON_ErrorCodes.h"
#include "LEPTON_Macros.h"
#include "LEPTON_I2C_Reg.h"

#include "bbg_lepton_I2C.h"

#define I2C_DEVICE_FILE "/dev/i2c-2"

#define ADDRESS_WIDTH 2  // in bytes

#define DEBUG 5  // get noisier with higher numbers
// #undef DEBUG

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

int lepton_i2c_fd = -1;

LEP_RESULT DEV_I2C_MasterInit(LEP_UINT16 portID,
                              LEP_UINT16 *BaudRate)
{
    LEP_RESULT result = LEP_OK;

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
        return LEP_ERROR;
    }
    else
    {
        if (ioctl(lepton_i2c_fd, I2C_SLAVE, LEP_I2C_DEVICE_ADDRESS) < 0)
        {
            DEBUG_PRINT(1, "Failed I2C ioctl(): %s\n", strerror(errno))
            return LEP_ERROR;
        }
    }
    DEBUG_PRINT(2, "I2C device successfully opened.\n")
    return result;
}

LEP_RESULT DEV_I2C_MasterClose()
{
    DEBUG_PRINT(5, "%s() called.\n", __func__)
    if (lepton_i2c_fd >= 0)
    {
        close(lepton_i2c_fd);
        lepton_i2c_fd = -1;
    }
    DEBUG_PRINT(2, "I2C device closed.\n")
    return LEP_OK;
}

LEP_RESULT DEV_I2C_MasterReset()
{
    DEBUG_PRINT(5, "%s() called.\n", __func__)
    return LEP_OK;
}

LEP_RESULT DEV_I2C_MasterReadData(LEP_UINT16 portID,
                                  LEP_UINT8   deviceAddress,
                                  LEP_UINT16  regAddress,            // Lepton Register Address
                                  LEP_UINT16 *readDataPtr,
                                  LEP_UINT16  wordsToRead,          // Number of 16-bit words to Read
                                  LEP_UINT16 *numWordsRead,         // Number of 16-bit words actually Read
                                  LEP_UINT16 *status
                                  )
{
    LEP_UINT16 be_address = REVERSE_ENDIENESS_UINT16(regAddress);
    int read_size = wordsToRead << 1;
    LEP_UINT8 read_data[read_size];
    int read_count = 0;
    LEP_UINT16 word_data = 0;
    int i = 0;

    DEBUG_PRINT(5, "%s(portID=%d, deviceAddress=%02x, regAddress=%04x, readDataPtr=%p, wordsToRead=%d, numWordsRead=%p, status=%p) called.\n", __func__, portID, deviceAddress, regAddress, readDataPtr, wordsToRead, numWordsRead, status);

    *numWordsRead = 0;

    DEBUG_PRINT(4, "Writing big-endian address %04x\n", be_address)
    int write_data = write(lepton_i2c_fd, &be_address, ADDRESS_WIDTH);
    if (write_data < 0)
    {
        DEBUG_PRINT(1, "Failed I2C write(%d, %04x, %d): %s\n", lepton_i2c_fd, be_address, ADDRESS_WIDTH,
                    strerror(errno))
        return LEP_ERROR_I2C_FAIL;
    }
    read_count = read(lepton_i2c_fd, read_data, read_size);
    if (read_count < 0)
    {
        DEBUG_PRINT(1, "Failed I2C read(%d, %p, %d): %s\n", lepton_i2c_fd, read_data, read_size, strerror(errno))
        return LEP_ERROR_I2C_FAIL;
    }
    *numWordsRead = (read_count >> 1);
    DEBUG_PRINT(4, "Read back %d bytes from I2C read:\n", read_count)
    DEBUG_DUMP(4, read_data, 2, read_count, 32)
    for (i=0; i<*numWordsRead; i++)
    {
        word_data = (read_data[i<<1]) << 8;
        word_data |= (read_data[(i<<1)+1]);
        readDataPtr[i] = word_data;
    }
    DEBUG_PRINT(4, "Copied %d words to readDataPtr:\n", *numWordsRead)
    DEBUG_DUMP(4, readDataPtr, 4, *numWordsRead, 16)
    if (*numWordsRead != wordsToRead)
    {
        DEBUG_PRINT(1, "Read count mismatch: Expected %d words, read %d\n", wordsToRead, *numWordsRead)
        return LEP_ERROR_I2C_FAIL;
    }
    return LEP_OK;
}

LEP_RESULT DEV_I2C_MasterWriteData(LEP_UINT16 portID,
                                   LEP_UINT8   deviceAddress,
                                   LEP_UINT16  regAddress,            // Lepton Register Address
                                   LEP_UINT16 *writeDataPtr,
                                   LEP_UINT16  wordsToWrite,        // Number of 16-bit words to Write
                                   LEP_UINT16 *numWordsWritten,     // Number of 16-bit words actually written
                                   LEP_UINT16 *status
                                   )
{
    int write_size = (wordsToWrite << 1) + ADDRESS_WIDTH;
    LEP_UINT8 write_data[write_size];
    int write_count = 0;
    int i = 0;

    DEBUG_PRINT(5, "%s(portID=%d, deviceAddress=%02x, regAddress=%04x, writeDataPtr=%p, wordsToWrite=%d, numWordsWritten=%p, status=%p) called.\n", __func__, portID, deviceAddress, regAddress, writeDataPtr, wordsToWrite, numWordsWritten, status)

    DEBUG_PRINT(4, "Write %d words in writeDataPtr:\n", wordsToWrite)
    DEBUG_DUMP(4, writeDataPtr, 4, wordsToWrite, 16)
    *numWordsWritten = 0;
    write_data[0] = (regAddress & 0xFF00) >> 8;
    write_data[1] = regAddress & 0xFF;
    for (i=0; i<wordsToWrite; i++)
    {
        write_data[(i<<1)+2] = (writeDataPtr[i] & 0xFF00) >> 8;
        write_data[(i<<1)+3] = writeDataPtr[i] & 0xFF;
    }
    DEBUG_PRINT(4, "Write %d bytes in write_data:\n", write_size)
    DEBUG_DUMP(4, write_data, 2, write_size, 32)
    write_count = write(lepton_i2c_fd, write_data, write_size);
    if (write_count < 0)
    {
        DEBUG_PRINT(1, "Failed I2C write(%d, %p, %d): %s\n", lepton_i2c_fd, write_data, write_size, strerror(errno))
        return LEP_ERROR;
    }
    *numWordsWritten = (write_count >> 1);
    if (write_count != write_size)
    {
        DEBUG_PRINT(1, "Write count mismatch: Expected %d bytes, wrote %d\n", write_size, write_count)
        return LEP_ERROR;
    }
    return LEP_OK;
}

LEP_RESULT DEV_I2C_MasterReadRegister(LEP_UINT16 portID,
                                      LEP_UINT8  deviceAddress, 
                                      LEP_UINT16 regAddress,
                                      LEP_UINT16 *regValue,     // Number of 16-bit words actually written
                                      LEP_UINT16 *status
                                      )
{
    LEP_RESULT result = LEP_OK;
    LEP_UINT16 word_count = 0;

    DEBUG_PRINT(5, "%s(portID=%d, deviceAddress=%02x, regAddress=%04x, regValue=%p, status=%p) called.\n", __func__, portID, deviceAddress, regAddress, regValue, status)
    result = DEV_I2C_MasterReadData(portID, deviceAddress, regAddress, regValue, 1, &word_count, status);

    return result;
}

LEP_RESULT DEV_I2C_MasterWriteRegister(LEP_UINT16 portID,
                                       LEP_UINT8  deviceAddress, 
                                       LEP_UINT16 regAddress,
                                       LEP_UINT16 regValue,     // Number of 16-bit words actually written
                                       LEP_UINT16 *status
                                       )
{
    LEP_RESULT result = LEP_OK;
    LEP_UINT16 word_count = 0;

    DEBUG_PRINT(5, "%s(portID=%d, deviceAddress=%02x, regAddress=%04x, regValue=%04x, status=%p) called.\n", __func__, portID, deviceAddress, regAddress, regValue, status)
    result = DEV_I2C_MasterWriteData(portID, deviceAddress, regAddress, &regValue, 1, &word_count, status);

    return result;
}

// #define UNIT_TEST
#ifdef UNIT_TEST
void usage(char *appname)
{
    fprintf(stderr, "%s usage:\n", appname);
    fprintf(stderr, "\t%s [read_or_write] [reg_address] [reg_value]\n\n", appname);
    fprintf(stderr, "Where: read_or_write is 0 to read, or non-zero to write,\n");
    fprintf(stderr, "       reg_address is the register to read or write (in hexadecimal),\n");
    fprintf(stderr, "       and reg_value is the value to be written if read_or_write != 0 (also hex)\n");
    exit(1);
}

int main(int argc, char **argv)
{
    int reg_address = 2;
    int read_or_write = 0;      // 0: read, 1: write
    LEP_UINT16 reg_value = 0;   // value read, or value to be written
    LEP_UINT16 unused = 0;

    if (argc > 1)
    {
        if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
        {
            DEBUG_DUMP(1, argv[1], 2, strlen(argv[1]), 32)
            usage(argv[0]);
        }
        read_or_write = atoi(argv[1]);
    }
    if (argc > 2)
    {
        reg_address = strtol(argv[2], NULL, 16);
    }
    if ((read_or_write != 0) && (argc > 3))
    {
        reg_value = strtol(argv[3], NULL, 16);
    }
    if (DEV_I2C_MasterInit(0, &unused) != LEP_OK)
    {
        fprintf(stderr, "Cannot open I2C device at address %x\n", LEP_I2C_DEVICE_ADDRESS);
        exit(1);
    }

    if (read_or_write == 0)
    {
        fprintf(stderr, "Reading register address 0x%x:\n", reg_address);
        if (DEV_I2C_MasterReadRegister(0, LEP_I2C_DEVICE_ADDRESS, 2, &reg_value, &unused) == LEP_OK)
        {
            fprintf(stderr, "Read %04x.\n", reg_value);
        }
    }
    else
    {
        fprintf(stderr, "Writing 0x%x to register address 0x%x:\n", reg_value, reg_address);
    }
    DEV_I2C_MasterClose();

    return 0;
}
#endif

