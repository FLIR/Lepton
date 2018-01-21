/*!
 * \file libMPSSE_i2c.h
 *
 * \author FTDI
 * \date 20110505
 *
 * Copyright © 2011 Future Technology Devices International Limited
 * Company Confidential
 *
 * Project: libMPSSE
 * Module: I2C
 *
 * Rivision History:
 * 0.1 - initial version
 * 0.2 - 20110708 - added FT_ReadGPIO, FT_WriteGPIO & 3-phase clocking
 */

#ifndef LIBMPSSE_I2C_H
#define LIBMPSSE_I2C_H

#if defined(WINDOWSS) || defined(WIN32)
#include "ftd2xx.h"


/******************************************************************************/
/*								Macro defines								  */
/******************************************************************************/

#ifdef __cplusplus
#define FTDI_API extern "C"
#else
#define FTDI_API
#endif

/* Options to I2C_DeviceWrite & I2C_DeviceRead */
#define	I2C_TRANSFER_OPTIONS_START_BIT		0x00000001	/*Generate start condition before transmitting */
#define I2C_TRANSFER_OPTIONS_STOP_BIT		0x00000002	/*Generate stop condition before transmitting */

/*Continue transmitting data in bulk without caring about Ack or nAck from device if this bit is 
not set. If this bit is set then stop transitting the data in the buffer when the device nAcks*/
#define I2C_TRANSFER_OPTIONS_BREAK_ON_NACK	0x00000004

#define I2C_CMD_GETDEVICEID_RD	0xF9
#define I2C_CMD_GETDEVICEID_WR	0xF8

#define I2C_GIVE_ACK	1
#define I2C_GIVE_NACK	0

#define I2C_DISABLE_3PHASE_CLOCKING	0x0001




/******************************************************************************/
/*								Type defines								  */
/******************************************************************************/

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;
typedef unsigned long long uint64;

typedef signed char   int8;
typedef signed short  int16;
typedef signed long   int32;
typedef signed long long int64;

#ifndef _MSC_VER
//typedef unsigned char	bool;
#endif

typedef enum I2C_ClockRate_t{
	I2C_CLOCK_STANDARD_MODE = 100000,							// 100kb/sec
	I2C_CLOCK_STANDARD_MODE_3P = 133333,						// 100kb/sec with 3-phase clocks
	I2C_CLOCK_FAST_MODE = 400000,								// 400kb/sec
	I2C_CLOCK_FAST_MODE_PLUS = 1000000, 						// 1000kb/sec
	I2C_CLOCK_HIGH_SPEED_MODE = 3400000 					    // 3.4Mb/sec
}I2C_CLOCKRATE;


/**/
typedef struct ChannelConfig_t
{
	I2C_CLOCKRATE	ClockRate; 

	uint8			LatencyTimer;

	uint32			Options;

	
}ChannelConfig;


/******************************************************************************/
/*								External variables							  */
/******************************************************************************/





/******************************************************************************/
/*								Function declarations						  */
/******************************************************************************/
FTDI_API FT_STATUS I2C_GetNumChannels(uint32 *numChannels);
FTDI_API FT_STATUS I2C_GetChannelInfo(uint32 index, 
	FT_DEVICE_LIST_INFO_NODE *chanInfo);
FTDI_API FT_STATUS I2C_OpenChannel(uint32 index, FT_HANDLE *handle);
FTDI_API FT_STATUS I2C_InitChannel(FT_HANDLE handle, ChannelConfig *config);
FTDI_API FT_STATUS I2C_CloseChannel(FT_HANDLE handle);
FTDI_API FT_STATUS I2C_DeviceRead(FT_HANDLE handle, uint32 deviceAddress, 
uint32 bytesToTransfer, uint8 *buffer, uint32 *bytesTransfered, uint32 options);
FTDI_API FT_STATUS I2C_DeviceWrite(FT_HANDLE handle, uint32 deviceAddress, 
uint32 bytesToTransfer, uint8 *buffer, uint32 *bytesTransfered, uint32 options);
FTDI_API void Init_libMPSSE(void);
FTDI_API void Cleanup_libMPSSE(void);
FTDI_API FT_STATUS FT_WriteGPIO(FT_HANDLE handle, uint8 dir, uint8 value);
FTDI_API FT_STATUS FT_ReadGPIO(FT_HANDLE handle,uint8 *value);



/******************************************************************************/

#endif  // WINDOWSS || WIN32

#endif	/*LIBMPSSE_I2C_H*/

