/*******************************************************************************
**
**    File NAME: FLIR_I2C.h
**
**      AUTHOR:  Hart Thomson
**
**      CREATED: 7/10/2015
**  
**      DESCRIPTION: 
**
**      HISTORY:  7/10/2015 HT - Initial Draft 
**
**      Copyright 2011,2012,2013,2014,2015 FLIR Systems - Commercial Vision Systems
**      All rights reserved.
**
**      Proprietary - Company Only.
**  
**      This document is controlled to FLIR Technology Level 2.
**      The information contained in this document pertains to a dual use product
**      Controlled for export by the Export Administration Regulations (EAR). 
**      FLIR trade secrets contained herein are subject to disclosure restrictions 
**      as a matter of law. Diversion contrary to US law is prohibited.  
**      US Department of Commerce authorization is required prior to export or 
**      transfer to foreign persons or parties and for uses otherwise prohibited. 
**
*******************************************************************************/
#ifndef _FLIR_I2C_H_ 
    #define _FLIR_I2C_H_

    #ifdef __cplusplus
extern "C"
{
    #endif
/******************************************************************************/
/** INCLUDE FILES                                                            **/
/******************************************************************************/
    #include "LEPTON_Types.h"
    #include "LEPTON_ErrorCodes.h"

/******************************************************************************/
/** EXPORTED DEFINES                                                         **/
/******************************************************************************/
#define IMPLEMENTS_SELECT_DEVICE

/******************************************************************************/
/** EXPORTED TYPE DEFINITIONS                                                **/
/******************************************************************************/
	typedef enum
	{
		REG_READ = 0,
		REG_WRITE,

		END_REG
	} LEP_REG_DIRECTION_E;

	typedef struct
	{
		LEP_UINT8 data[1026];

		LEP_REG_DIRECTION_E readOrWrite;
		LEP_UINT16 bytesToTransfer;
		
		LEP_UINT8 deviceAddress;
		LEP_UINT8 reserved1;

	} LEP_CMD_PACKET_T;

	typedef struct
	{
		LEP_UINT8 data[1024];

		LEP_INT16  status;
		LEP_UINT16 bytesTransferred;

	} LEP_RESPONSE_PACKET_T;

/******************************************************************************/
/** EXPORTED PUBLIC DATA                                                     **/
/******************************************************************************/

/******************************************************************************/
/** EXPORTED PUBLIC FUNCTIONS                                                **/
/******************************************************************************/

    extern LEP_RESULT DEV_I2C_MasterSelectDevice(LEP_PROTOCOL_DEVICE_E device);

    extern LEP_RESULT DEV_I2C_MasterInit(LEP_UINT16 portID,
                                         LEP_UINT16 *BaudRate);

    extern LEP_RESULT DEV_I2C_MasterClose();

    extern LEP_RESULT DEV_I2C_MasterReset(void );

    extern LEP_RESULT DEV_I2C_MasterReadData(LEP_UINT16 portID,
                                             LEP_UINT8   deviceAddress,
                                             LEP_UINT16  regAddress,            // Lepton Register Address
                                             LEP_UINT16 *readDataPtr,
                                             LEP_UINT16  wordsToRead,          // Number of 16-bit words to Read
                                             LEP_UINT16 *numWordsRead,         // Number of 16-bit words actually Read
                                             LEP_UINT16 *status
                                            );

    extern LEP_RESULT DEV_I2C_MasterWriteData(LEP_UINT16 portID,
                                              LEP_UINT8   deviceAddress,
                                              LEP_UINT16  regAddress,            // Lepton Register Address
                                              LEP_UINT16 *writeDataPtr,
                                              LEP_UINT16  wordsToWrite,        // Number of 16-bit words to Write
                                              LEP_UINT16 *numWordsWritten,     // Number of 16-bit words actually written
                                              LEP_UINT16 *status
                                             );

    extern LEP_RESULT DEV_I2C_MasterReadRegister( LEP_UINT16 portID,
                                                  LEP_UINT8  deviceAddress, 
                                                  LEP_UINT16 regAddress,
                                                  LEP_UINT16 *regValue,     // Number of 16-bit words actually written
                                                  LEP_UINT16 *status
                                                );

    extern LEP_RESULT DEV_I2C_MasterWriteRegister( LEP_UINT16 portID,
                                                   LEP_UINT8  deviceAddress, 
                                                   LEP_UINT16 regAddress,
                                                   LEP_UINT16 regValue,     // Number of 16-bit words actually written
                                                   LEP_UINT16 *status
                                                 );

    extern LEP_RESULT DEV_I2C_MasterStatus(void );

/******************************************************************************/
    #ifdef __cplusplus
}
    #endif

#endif  /* _FLIR_I2C_H_ */

