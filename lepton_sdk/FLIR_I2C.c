/*******************************************************************************
**
**    File NAME: FLIR_I2C.c
**
**      AUTHOR:  Hart Thomson
**
**      CREATED: 7/10/2015
**  
**      DESCRIPTION: Lepton Device-Specific Driver for various Master I2C Devices
**
**      HISTORY:  7/19/2015 HT - Initial Draft 
**
** Copyright 2010, 2011, 2012, 2013, 2014, 2015 FLIR Systems - Commercial Vision Systems
**
**  All rights reserved.
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are met:
**
**  Redistributions of source code must retain the above copyright notice, this
**  list of conditions and the following disclaimer.
**
**  Redistributions in binary form must reproduce the above copyright notice,
**  this list of conditions and the following disclaimer in the documentation
**  and/or other materials provided with the distribution.
**
**  Neither the name of the Indigo Systems Corporation nor the names of its
**  contributors may be used to endorse or promote products derived from this
**  software without specific prior written permission.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
**  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
**  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
**  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
**  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
**  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
**  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
**  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
**  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
**  THE POSSIBILITY OF SUCH DAMAGE.
**
*******************************************************************************/
/******************************************************************************/
/** INCLUDE FILES                                                            **/
/******************************************************************************/

#define _WINSOCKAPI_

#include "LEPTON_Types.h"
#include "LEPTON_ErrorCodes.h"
#include "LEPTON_Macros.h"
#include "FLIR_I2C.h"
#include "LEPTON_I2C_Reg.h"
#include <stdio.h>
#include <stdlib.h>

/* Aardvark Includes */
#include "aardvark.h"

/* FTDI Includes */
#include "ftd2xx.h"
#include "libMPSSE_i2c.h"

/* TCP IP Socket Includes */
#if defined(WINDOWSS) || defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

/* Linux i2c device include */
#include "lep_i2cdev.h"

/******************************************************************************/
/** LOCAL DEFINES                                                            **/
/******************************************************************************/

#define FTDI_DEVICE_STRING      "Dual RS232-HS B"
#define DEFAULT_PORT "27316"
#define DEFAULT_ADDR "192.168.7.2"

#define ADDRESS_SIZE_BYTES  2
#define VALUE_SIZE_BYTES    2
#define I2C_BUFFER_SIZE (ADDRESS_SIZE_BYTES + LEP_I2C_DATA_BUFFER_0_LENGTH)
LEP_UINT8 tx[I2C_BUFFER_SIZE];
LEP_UINT8 rx[I2C_BUFFER_SIZE];




/******************************************************************************/
/** LOCAL TYPE DEFINITIONS                                                   **/
/******************************************************************************/

/******************************************************************************/
/** PRIVATE DATA DECLARATIONS                                                **/
/******************************************************************************/
LEP_PROTOCOL_DEVICE_E masterDevice = LINUX_I2CDEV_I2C;
Aardvark handle;

LEP_CMD_PACKET_T cmdPacket;
LEP_RESPONSE_PACKET_T responsePacket;

#if defined(WINDOWSS) || defined(WIN32)
FT_HANDLE ftHandle;
FT_STATUS status;

SOCKET ConnectSocket = INVALID_SOCKET;
WSADATA wsaData;
struct addrinfo *addrresult = NULL,
                 hints;
#endif
/******************************************************************************/
/** PRIVATE FUNCTION DECLARATIONS                                            **/
/******************************************************************************/

/******************************************************************************/
/** EXPORTED PUBLIC DATA                                                     **/
/******************************************************************************/

/******************************************************************************/
/** EXPORTED PUBLIC FUNCTIONS                                                **/
/******************************************************************************/


LEP_RESULT DEV_I2C_MasterSelectDevice(LEP_PROTOCOL_DEVICE_E device)
{
    LEP_RESULT result = LEP_OK;

    masterDevice = device;

    return(result);
}


/******************************************************************************/
/**
 * Performs I2C Master Initialization
 * 
 * @param portID     LEP_UINT16  User specified port ID tag.  Can be used to
 *                   select between multiple cameras
 * 
 * @param BaudRate   Clock speed in kHz. Typically this is 400.
 *                   The Device Specific Driver will try to match the desired
 *                   speed.  This parameter is updated to the actual speed the
 *                   driver can use.
 * 
 * @return LEP_RESULT  0 if all goes well, errno otherwise
 */
LEP_RESULT DEV_I2C_MasterInit(LEP_UINT16 portID, 
                              LEP_UINT16 *BaudRate)
{
	LEP_RESULT result = LEP_OK;
	int numAardvarkConnected = 0;
	LEP_UINT16 numFreeDevices;
#if defined(WINDOWSS) || defined(WIN32)
	FT_STATUS status;
	ChannelConfig channelConf;
	uint32 channels;
	int i;
	int res;
	FT_DEVICE_LIST_INFO_NODE devList;

	unsigned long nonBlockMode = 1;
	fd_set Write, fdErr;
	TIMEVAL socketTimeout;
#endif

fprintf(stderr, "You are here.  masterDevice is: %d\n", (int)masterDevice);
    /* Place Device-Specific Interface here
    */
   switch(masterDevice)
   {
#if defined(WINDOWSS) || defined(WIN32)
   case DEV_BOARD_FTDI_V2:
       

       Init_libMPSSE();
       channelConf.ClockRate = I2C_CLOCK_FAST_MODE;
       channelConf.LatencyTimer = 0;
       channelConf.Options = 0;

       result = LEP_ERROR_CREATING_COMM;

       status = I2C_GetNumChannels(&channels);
       if(channels > 0)
	   {
          
          for(i = 0; i < channels; i++)
          {
             status = I2C_GetChannelInfo(i, &devList);
             if(strcmp(FTDI_DEVICE_STRING, devList.Description) == 0)
             {
                status = I2C_OpenChannel(i ,&ftHandle);
                status = I2C_InitChannel(ftHandle,&channelConf);
                result = LEP_OK;

                break;
             }
          }
       }
       
       break;
   case TCP_IP:
		closesocket(ConnectSocket);

		// Initialize Winsock
		res = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (res != 0) {
			printf("WSAStartup failed with error: %d\n", res);
			result = LEP_ERROR;
			return(result);
		}

		ZeroMemory( &hints, sizeof(hints) );
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		res = getaddrinfo(DEFAULT_ADDR, DEFAULT_PORT, &hints, &addrresult);
		if ( res != 0 ) {
			printf("getaddrinfo failed with error: %d\n", res);
			WSACleanup();
			result = LEP_ERROR;
			return(result);
		}

       ConnectSocket = socket(addrresult->ai_family, addrresult->ai_socktype, addrresult->ai_protocol);
       if (ConnectSocket == INVALID_SOCKET) {
           printf("socket failed with error: %ld\n", WSAGetLastError());
           WSACleanup();
           result = LEP_ERROR;
		   return(result);
       }
	   
	   // Set to Non-blocking mode for quick timeout
       ioctlsocket(ConnectSocket, FIONBIO, &nonBlockMode);

       // Connect to server, returns immediately
       res = connect( ConnectSocket, addrresult->ai_addr, (int)addrresult->ai_addrlen);
       if (res == SOCKET_ERROR) 
	   {
           res = WSAGetLastError();
           if (res == WSAEWOULDBLOCK)
		   {
                FD_ZERO(&Write);
                FD_ZERO(&fdErr);
                FD_SET(ConnectSocket, &Write);
                FD_SET(ConnectSocket, &fdErr);

                socketTimeout.tv_sec  = 1; //Sets timeout to 1 second
                socketTimeout.tv_usec = 0; 

                res = select (0, NULL, &Write, &fdErr, &socketTimeout);

                if (res == 0)
                {
                    result = LEP_ERROR;
                }
                else
                {
                    if (FD_ISSET(ConnectSocket, &Write))
                    {
                        result = LEP_OK;
                    }
                    if (FD_ISSET(ConnectSocket, &fdErr))
                    {
                        result = LEP_ERROR;
                    }
                }
            }

	   }
       // Set us back to blocking mode.
       nonBlockMode = 0;
       ioctlsocket(ConnectSocket, FIONBIO, &nonBlockMode);
		
       break;
#endif
   case LINUX_I2CDEV_I2C:
       if (i2cdev_init() != 0)
       {
           return(LEP_ERROR_CREATING_COMM);
       }
       break;
   case AARDVARK_I2C:
   default:
       numAardvarkConnected = aa_find_devices(1, &numFreeDevices);

       if(numAardvarkConnected < 1 || numFreeDevices == AA_PORT_NOT_FREE)
       {
           return(LEP_ERROR_CREATING_COMM);
       }

       handle = aa_open(0);
       aa_i2c_bitrate(handle, *BaudRate);
       aa_target_power(handle, AA_TARGET_POWER_BOTH);
       //aa_i2c_pullup(handle, AA_I2C_PULLUP_NONE);

       break;
   }
   

   return(result);
}

/**
 * Closes the I2C driver connection.
 * 
 * @return LEP_RESULT  0 if all goes well, errno otherwise.
 */
LEP_RESULT DEV_I2C_MasterClose()
{
    LEP_RESULT result = LEP_OK;

    /* Place Device-Specific Interface here
    */ 
   switch(masterDevice)
   {
#if defined(WINDOWSS) || defined(WIN32)
   case DEV_BOARD_FTDI_V2:
       I2C_CloseChannel(ftHandle);
       break;
   case TCP_IP:
		closesocket(ConnectSocket);
   break;
#endif
   case LINUX_I2CDEV_I2C:
       i2cdev_close();
       break;
   case AARDVARK_I2C:
   default:
       aa_close(handle);

       break;
   }
   

    return(result);
}

/**
 * Resets the I2C driver back to the READY state.
 * 
 * @return LEP_RESULT  0 if all goes well, errno otherwise.
 */
LEP_RESULT DEV_I2C_MasterReset(void )
{
   LEP_RESULT result = LEP_OK;

   /* Place Device-Specific Interface here
   */ 
   switch(masterDevice)
   {
   case DEV_BOARD_FTDI_V2:

       break;

   case LINUX_I2CDEV_I2C:
       break;

   case AARDVARK_I2C:
   default:
       aa_close(handle);

       break;
   }


   return(result);
}

LEP_RESULT DEV_I2C_MasterReadData(LEP_UINT16  portID,               // User-defined port ID
                                  LEP_UINT8   deviceAddress,        // Lepton Camera I2C Device Address
                                  LEP_UINT16  regAddress,           // Lepton Register Address
                                  LEP_UINT16 *readDataPtr,          // Read DATA buffer pointer
                                  LEP_UINT16  wordsToRead,          // Number of 16-bit words to Read
                                  LEP_UINT16 *numWordsRead,         // Number of 16-bit words actually Read
                                  LEP_UINT16 *status                // Transaction Status
                                 )
{
   LEP_RESULT result = LEP_OK;
   int ftdiStatus;
   int aardvark_result;

    /* Place Device-Specific Interface here
    */ 
   
   LEP_UINT32 bytesToWrite = ADDRESS_SIZE_BYTES;
   LEP_UINT32 bytesToRead = wordsToRead << 1;
   LEP_UINT32 bytesActuallyWritten = 0;
   LEP_UINT32 bytesActuallyRead = 0;
   LEP_UINT32 wordsActuallyRead = 0;
   LEP_UINT8* txdata = &tx[0];
   LEP_UINT8* rxdata = &rx[0];
   LEP_UINT16 *dataPtr;
   LEP_UINT16 *writePtr;

   *(LEP_UINT16*)txdata = REVERSE_ENDIENESS_UINT16(regAddress);


   switch(masterDevice)
   {   
#if defined(WINDOWSS) || defined(WIN32)
   case DEV_BOARD_FTDI_V2:
       

       /*
         Write the address, which is 2 bytes
       */
       ftdiStatus = I2C_DeviceWrite(ftHandle, (uint32)deviceAddress, ADDRESS_SIZE_BYTES, (uint8*)txdata, (uint32*)&bytesActuallyWritten, 0x1d);
       
       /*
             Read back the data at the address written above
       */
       ftdiStatus = I2C_DeviceRead(ftHandle, (uint32)deviceAddress, (uint32)bytesToRead, (uint8*)rxdata, (uint32*)&bytesActuallyRead, 0x19);
       
       ftdiStatus = 0;
       bytesActuallyRead = bytesToRead;

       if(ftdiStatus != 0 || bytesActuallyRead != bytesToRead)
       {
          result = LEP_ERROR;
       }
       break;
	case TCP_IP:

		memcpy((LEP_UINT8*)cmdPacket.data, (LEP_UINT8*)txdata, ADDRESS_SIZE_BYTES);
		cmdPacket.deviceAddress = (LEP_UINT8)deviceAddress;
		cmdPacket.bytesToTransfer = (LEP_UINT16)bytesToRead;
		cmdPacket.readOrWrite = REG_READ;

		/* Send command to read the data */
		bytesActuallyRead = 0;
		while( bytesActuallyRead < sizeof(LEP_CMD_PACKET_T) )
		{	
			bytesActuallyRead += send(ConnectSocket, ((char*)&cmdPacket) + bytesActuallyRead, sizeof(LEP_CMD_PACKET_T) - bytesActuallyRead, 0);
		}

		/* Receive the response */
		bytesActuallyRead = 0;
		while( bytesActuallyRead < sizeof(LEP_RESPONSE_PACKET_T) )
		{
			bytesActuallyRead += recv(ConnectSocket, ((char*)&responsePacket) + bytesActuallyRead, sizeof(LEP_RESPONSE_PACKET_T) - bytesActuallyRead, 0);
		}
		bytesActuallyRead = responsePacket.bytesTransferred;
		memcpy(rxdata, responsePacket.data, bytesToRead);

		break;
#endif
   case LINUX_I2CDEV_I2C:
        bytesActuallyRead = i2cdev_read_byte_data(txdata, rxdata, bytesToRead);
        if(bytesActuallyRead != bytesToRead)
        {
            result = LEP_ERROR_I2C_FAIL;
        }
        if(bytesActuallyRead < 0)
        {
            // don't shift negative values into wordsActuallyRead
            bytesActuallyRead = 0;
        }
        break;
   case AARDVARK_I2C:
   default:
       
       aardvark_result = aa_i2c_write_read(
               handle, 
               deviceAddress, 
               AA_I2C_NO_FLAGS, 
               bytesToWrite, 
               txdata, 
               (LEP_UINT16*)&bytesActuallyWritten, 
               bytesToRead, 
               rxdata, 
               (LEP_UINT16*)&bytesActuallyRead);

       if(aardvark_result != 0 || bytesActuallyRead != bytesToRead)
       {
          result = LEP_ERROR_I2C_FAIL;
       }
       else
       {
          result = (LEP_RESULT)aardvark_result;
       }
       break;
   }
   

   wordsActuallyRead = (LEP_UINT16)(bytesActuallyRead >> 1);
   *numWordsRead = wordsActuallyRead;

   if(result == LEP_OK)
   {
       dataPtr = (LEP_UINT16*)&rxdata[0];
       writePtr = readDataPtr;
       while(wordsActuallyRead--)
       {
          *writePtr++ = REVERSE_ENDIENESS_UINT16(*dataPtr);
          dataPtr++;
       }
   }


   return(result);
}

LEP_RESULT DEV_I2C_MasterWriteData(LEP_UINT16  portID,              // User-defined port ID
                                   LEP_UINT8   deviceAddress,       // Lepton Camera I2C Device Address
                                   LEP_UINT16  regAddress,          // Lepton Register Address
                                   LEP_UINT16 *writeDataPtr,        // Write DATA buffer pointer
                                   LEP_UINT16  wordsToWrite,        // Number of 16-bit words to Write
                                   LEP_UINT16 *numWordsWritten,     // Number of 16-bit words actually written
                                   LEP_UINT16 *status)              // Transaction Status
{
   LEP_RESULT result = LEP_OK;
   int ftdiStatus;
   int aardvark_result;
   
   LEP_INT32 bytesOfDataToWrite = (wordsToWrite << 1);
   LEP_INT32 bytesToWrite = bytesOfDataToWrite + ADDRESS_SIZE_BYTES;
   LEP_INT32 bytesActuallyWritten = 0;
   LEP_UINT8* txdata = &tx[0];
   LEP_UINT16 *dataPtr;
   LEP_UINT16 *txPtr;

   *(LEP_UINT16*)txdata = REVERSE_ENDIENESS_UINT16(regAddress);
   dataPtr = (LEP_UINT16*)&writeDataPtr[0];
   txPtr = (LEP_UINT16*)&txdata[ADDRESS_SIZE_BYTES]; 
   while(wordsToWrite--){
      *txPtr++ = (LEP_UINT16)REVERSE_ENDIENESS_UINT16(*dataPtr);
      dataPtr++;
   }

   switch(masterDevice)
   {
#if defined(WINDOWSS) || defined(WIN32)
    case DEV_BOARD_FTDI_V2:
       
       ftdiStatus = I2C_DeviceWrite(ftHandle, (uint32)deviceAddress, bytesToWrite, (uint8*)txdata, (uint32*)&bytesActuallyWritten, 0x13);
       
       if(ftdiStatus != 0 || bytesActuallyWritten != bytesToWrite)
       {
          result = LEP_ERROR;
       }
       break;
	case TCP_IP:
		memcpy(cmdPacket.data, txdata, bytesToWrite);
		cmdPacket.deviceAddress = (LEP_UINT8)deviceAddress;
		cmdPacket.bytesToTransfer = (LEP_UINT16)bytesToWrite;
		cmdPacket.readOrWrite = REG_WRITE;

		/* Send command to write the data */
		bytesActuallyWritten = 0;
		while( bytesActuallyWritten < sizeof(LEP_CMD_PACKET_T) )
		{
			bytesActuallyWritten += send(ConnectSocket, ((char*)&cmdPacket) + bytesActuallyWritten, sizeof(LEP_CMD_PACKET_T) - bytesActuallyWritten, 0);
		}

		/* Receive the response */
		bytesActuallyWritten = 0;
		while( bytesActuallyWritten < sizeof(LEP_RESPONSE_PACKET_T) )
		{
			bytesActuallyWritten += recv(ConnectSocket, ((char*)&responsePacket) + bytesActuallyWritten, sizeof(LEP_RESPONSE_PACKET_T) - bytesActuallyWritten, 0);
		}
		bytesActuallyWritten = responsePacket.bytesTransferred;

		break;
#endif
   case LINUX_I2CDEV_I2C:
        bytesActuallyWritten = i2cdev_write_byte_data(txdata, bytesToWrite);
        if(bytesActuallyWritten != bytesToWrite)
        {
            result = LEP_ERROR;
        }
        if(bytesActuallyWritten < 0)
        {
            // don't shift negative values into numWordsWritten
            bytesActuallyWritten = 0;
        }
        break;
   case AARDVARK_I2C:
   default:
       
       aardvark_result = aa_i2c_write_ext(handle, deviceAddress, AA_I2C_NO_FLAGS, bytesToWrite, (LEP_UINT8*)txdata, (u16*)&bytesActuallyWritten);
       break;
       
       if(aardvark_result != 0 || bytesActuallyWritten != bytesToWrite)
       {
         result = LEP_ERROR;
       }
   }
   
   *numWordsWritten = (bytesActuallyWritten >> 1);
   
   return(result);
}

LEP_RESULT DEV_I2C_MasterReadRegister( LEP_UINT16 portID,
                                       LEP_UINT8  deviceAddress, 
                                       LEP_UINT16 regAddress,
                                       LEP_UINT16 *regValue,     // Number of 16-bit words actually written
                                       LEP_UINT16 *status
                                     )
{
    LEP_RESULT result = LEP_OK;

   LEP_UINT16 wordsActuallyRead;
    /* Place Device-Specific Interface here
    */ 
   result = DEV_I2C_MasterReadData(portID, deviceAddress, regAddress, regValue, 1 /*1 word*/, &wordsActuallyRead, status);

   return(result);
}

LEP_RESULT DEV_I2C_MasterWriteRegister( LEP_UINT16 portID,
                                        LEP_UINT8  deviceAddress, 
                                        LEP_UINT16 regAddress,
                                        LEP_UINT16 regValue,     // Number of 16-bit words actually written
                                        LEP_UINT16 *status
                                      )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 wordsActuallyWritten;
    /* Place Device-Specific Interface here
    */ 
   result = DEV_I2C_MasterWriteData(portID, deviceAddress, regAddress, &regValue, 1, &wordsActuallyWritten, status);

   return(result);
}

LEP_RESULT DEV_I2C_MasterStatus(void )
{
    LEP_RESULT result = LEP_OK;

    /* Place Device-Specific Interface here
    */ 


    return(result);
}


/******************************************************************************/
/** PRIVATE MODULE FUNCTIONS                                                 **/
/******************************************************************************/


