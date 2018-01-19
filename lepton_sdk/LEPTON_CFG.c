/*******************************************************************************
**
**    File NAME: LEPTON_CFG.c
**
**      AUTHOR:  David Dart
**
**      CREATED: 8/9/2012
**  
**      DESCRIPTION: Contains interfaces to configure the Lepton
**                   Camera
**
**      HISTORY:  8/9/2012 DWD - Initial Draft 
**
**      Copyright 2011,2012,2013,2014 FLIR Systems - Commercial
**      Vision Systems.  All rights reserved.
**
**      Proprietary - PROPRIETARY - FLIR Systems Inc..
**  
**      This document is controlled to FLIR Technology Level 2.
**      The information contained in this document pertains to a
**      dual use product Controlled for export by the Export
**      Administration Regulations (EAR). Diversion contrary to
**      US law is prohibited.  US Department of Commerce
**      authorization is not required prior to export or
**      transfer to foreign persons or parties unless otherwise
**      prohibited.
**
**      Redistribution and use in source and binary forms, with
**      or without modification, are permitted provided that the
**      following conditions are met:
**
**      Redistributions of source code must retain the above
**      copyright notice, this list of conditions and the
**      following disclaimer.
**
**      Redistributions in binary form must reproduce the above
**      copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other
**      materials provided with the distribution.
**
**      Neither the name of the FLIR Systems Corporation nor the
**      names of its contributors may be used to endorse or
**      promote products derived from this software without
**      specific prior written permission.
**
**      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
**      CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
**      WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**      WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
**      PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
**      COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
**      DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**      CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
**      PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
**      USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
**      CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
**      CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
**      NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
**      USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
**      OF SUCH DAMAGE.
**
*******************************************************************************/
/******************************************************************************/
/** INCLUDE FILES                                                            **/
/******************************************************************************/
#include "LEPTON_SDK.h"
#include "LEPTON_CFG.h"
#include <string.h>

/******************************************************************************/
/** LOCAL DEFINES                                                            **/
/******************************************************************************/

/******************************************************************************/
/** LOCAL TYPE DEFINITIONS                                                   **/
/******************************************************************************/

/******************************************************************************/
/** PRIVATE DATA DECLARATIONS                                                **/
/******************************************************************************/

/******************************************************************************/
/** PRIVATE FUNCTION DECLARATIONS                                            **/
/******************************************************************************/

/******************************************************************************/
/** EXPORTED PUBLIC DATA                                                     **/
/******************************************************************************/

/******************************************************************************/
/** EXPORTED PUBLIC FUNCTIONS                                                **/
/******************************************************************************/

LEP_RESULT LEP_SetCfgFlirSerialNumber( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                       LEP_CFG_FLIR_SERIAL_NUMBER_T serialNumber )
{
   LEP_RESULT  result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4; /* 8 bytes max */

   /* Perform Command
   ** Writing the Camera's RM value
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_FLIR_SERIAL_NUMBER,
                              ( LEP_ATTRIBUTE_T_PTR ) & serialNumber,
                              attributeWordLength );
   return( result );
}
#if USE_DEPRECATED_SERIAL_NUMBER_INTERFACE
LEP_RESULT LEP_SetCfgCustSerialNumber( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                       LEP_CFG_CUST_SERIAL_NUMBER_T serialNumber )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 16;   /* 32 bytes max */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_CUST_SERIAL_NUMBER,
                              ( LEP_ATTRIBUTE_T_PTR )serialNumber,
                              attributeWordLength );

   return( result );
}

#else
LEP_RESULT LEP_SetCfgCustSerialNumber(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                      LEP_CFG_CUST_SERIAL_NUMBER_T_PTR serialNumberPtr)
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 16;   /* 32 bytes max */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_CUST_SERIAL_NUMBER,
                              ( LEP_ATTRIBUTE_T_PTR )serialNumberPtr,
                              attributeWordLength );

   return( result );
}
#endif

#if USE_DEPRECATED_PART_NUMBER_INTERFACE
LEP_RESULT LEP_SetCfgFlirPartNumber( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                     LEP_CFG_PART_NUMBER_T partNumber )
{
   LEP_RESULT  result = LEP_OK;
   LEP_UINT16 attributeWordLength = 16; /* 32 bytes  */

   /* Perform Command
   ** Writing the Camera's RM value
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_FLIR_PART_NUMBER,
                              ( LEP_ATTRIBUTE_T_PTR )partNumber,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_SetCfgCustPartNumber( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                     LEP_CFG_PART_NUMBER_T partNumber )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 16;   /* 32 bytes max */

   if( strlen( partNumber ) > LEP_CFG_MAX_PART_NUMBER_CHAR_SIZE )
   {
      return( LEP_DATA_SIZE_ERROR );
   }

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_CUST_PART_NUMBER,
                              ( LEP_ATTRIBUTE_T_PTR )partNumber,
                              attributeWordLength );
   return( result );
}
#else


LEP_RESULT LEP_SetCfgFlirPartNumber(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                    LEP_CFG_PART_NUMBER_T_PTR partNumberPtr)
{
   LEP_RESULT  result = LEP_OK;
   LEP_UINT16 attributeWordLength = 16; /* 32 bytes  */

   /* Perform Command
   ** Writing the Camera's RM value
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_FLIR_PART_NUMBER,
                              ( LEP_ATTRIBUTE_T_PTR )partNumberPtr,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_SetCfgCustPartNumber(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                    LEP_CFG_PART_NUMBER_T_PTR partNumberPtr)
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 16;   /* 32 bytes max */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_CUST_PART_NUMBER,
                              ( LEP_ATTRIBUTE_T_PTR )partNumberPtr,
                              attributeWordLength );
   return( result );
}
#endif

LEP_RESULT LEP_GetCfgVideoSourceSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                        LEP_CFG_VIDEO_INPUT_SOURCE_SELECT_E_PTR vidInputSourcePtr )
{
   LEP_RESULT  result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* two 16-bit values for 32-bit enum */

   /* Validate Parameter(s)
   */
   if( vidInputSourcePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   /* Perform Command
   ** Reading the Camera's current video freeze enable state
   */
   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_VIDEO_INPUT_SELECT,
                              ( LEP_ATTRIBUTE_T_PTR )vidInputSourcePtr,
                              attributeWordLength );
   return( result );
}


LEP_RESULT LEP_SetCfgVideoSourceSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                        LEP_CFG_VIDEO_INPUT_SOURCE_SELECT_E vidInputSource )
{
   LEP_RESULT  result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* two 16-bit values for 32-bit enum */

   /* Validate Parameter(s)
   */
   if( vidInputSource >= LEP_END_VIDEO_INPUT_SOURCE_SELECT )
   {
      return( LEP_RANGE_ERROR );
   }

   /* Perform Command
   ** Writing the Camera's current video freeze enable state
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_VIDEO_INPUT_SELECT,
                              ( LEP_ATTRIBUTE_T_PTR ) & vidInputSource,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_GetCfgVideoModeSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                      LEP_CFG_VIDEO_MODE_SELECT_E_PTR videoModeSelectPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* two 16-bit values for 32-bit enum */

   /* Validate Parameter(s)
   */
   if( videoModeSelectPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   /* Perform Command
   ** Reading the Camera's current video freeze enable state
   */
   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_VIDEO_MODE_SELECT,
                              ( LEP_ATTRIBUTE_T_PTR )videoModeSelectPtr,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_SetCfgVideoModeSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                      LEP_CFG_VIDEO_MODE_SELECT_E videoModeSelect )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* two-word enums */

   if( videoModeSelect >= LEP_END_VIDEO_MODE_SELECT )
   {
      return( LEP_RANGE_ERROR );
   }

   /* Perform Command
   ** Writing the Camera's current video freeze enable state
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_VIDEO_MODE_SELECT,
                              ( LEP_ATTRIBUTE_T_PTR ) & videoModeSelect,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_SetCfgBadPixelReplaceControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                             LEP_CFG_BAD_PIXEL_REPLACE_CONTROL_T BadPixelReplaceControl )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* two word enums */

   /* Perform Command
   ** Writing the Camera's current video freeze enable state
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_BAD_PIXEL_REPLACEMENT,
                              ( LEP_ATTRIBUTE_T_PTR ) & BadPixelReplaceControl,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_GetCfgBadPixelReplaceControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                             LEP_CFG_BAD_PIXEL_REPLACE_CONTROL_T_PTR BadPixelReplaceControlPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum */
   if( BadPixelReplaceControlPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }
   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_BAD_PIXEL_REPLACEMENT,
                              ( LEP_ATTRIBUTE_T_PTR )BadPixelReplaceControlPtr,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_SetCfgTemporalFilterControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_TEMPORAL_FILTER_CONTROL_T TemporalFilterControl )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */

   /* Perform Command
   ** Writing the Camera's current video freeze enable state
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_TEMPORAL_FILTER_CONTROL,
                              ( LEP_ATTRIBUTE_T_PTR ) & TemporalFilterControl,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_GetCfgTemporalFilterControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_TEMPORAL_FILTER_CONTROL_T_PTR TemporalFilterControlPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */
   if( TemporalFilterControlPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }
   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_TEMPORAL_FILTER_CONTROL,
                              ( LEP_ATTRIBUTE_T_PTR )TemporalFilterControlPtr,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_SetCfgColumnNoiseEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                 LEP_CFG_COLUMN_NOISE_ESTIMATE_CONTROL_T ColumnNoiseEstimateControl )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_COLUMN_NOISE_ESTIMATE,
                              ( LEP_ATTRIBUTE_T_PTR ) & ColumnNoiseEstimateControl,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_GetCfgColumnNoiseEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                 LEP_CFG_COLUMN_NOISE_ESTIMATE_CONTROL_T_PTR ColumnNoiseEstimateControlPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */
   if( ColumnNoiseEstimateControlPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }
   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_COLUMN_NOISE_ESTIMATE,
                              ( LEP_ATTRIBUTE_T_PTR )ColumnNoiseEstimateControlPtr,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_SetCfgRowNoiseEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                              LEP_CFG_ROW_NOISE_ESTIMATE_CONTROL_T RowNoiseEstimateControl )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */

   /* Perform Command
   ** Writing the Camera's current video freeze enable state
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_ROW_NOISE_ESTIMATE,
                              ( LEP_ATTRIBUTE_T_PTR ) & RowNoiseEstimateControl,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_GetCfgRowNoiseEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                              LEP_CFG_ROW_NOISE_ESTIMATE_CONTROL_T_PTR RowNoiseEstimateControlPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */
   if( RowNoiseEstimateControlPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }
   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_ROW_NOISE_ESTIMATE,
                              ( LEP_ATTRIBUTE_T_PTR )RowNoiseEstimateControlPtr,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_SetCfgSbnucFfcEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                              LEP_CFG_SBNUC_FFC_ESTIMATE_CONTROL_T SbnucFfcEstimateControl )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */

   /* Perform Command
   ** Writing the Camera's current video freeze enable state
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_SBNUC_FFC_ESTIMATE,
                              ( LEP_ATTRIBUTE_T_PTR ) & SbnucFfcEstimateControl,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_GetCfgSbnucFfcEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                              LEP_CFG_SBNUC_FFC_ESTIMATE_CONTROL_T_PTR SbnucFfcEstimateControlPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */

   if( SbnucFfcEstimateControlPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_SBNUC_FFC_ESTIMATE,
                              ( LEP_ATTRIBUTE_T_PTR )SbnucFfcEstimateControlPtr,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_GetCfgLagrangeFilterControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_LAGRANGE_FILTER_CONTROL_T_PTR lagrangeFilterControlPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 32-bit */

   if( lagrangeFilterControlPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_LAGRANGE_FILTER_CONTROL,
                              ( LEP_ATTRIBUTE_T_PTR )lagrangeFilterControlPtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgLagrangeFilterControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_LAGRANGE_FILTER_CONTROL_T lagrangeFilterControl )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 32-bit */

   if( lagrangeFilterControl.cfgLaGrangeFilterEnable >= LEP_CFG_END_STATE )
   {
      return( LEP_RANGE_ERROR );
   }

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_LAGRANGE_FILTER_CONTROL,
                              ( LEP_ATTRIBUTE_T_PTR ) & lagrangeFilterControl,
                              attributeWordLength );

   return( result );
}


LEP_RESULT LEP_GetCfgNUCPathEnableStates( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                          LEP_CFG_NUC_PATH_CONTROL_T_PTR NUCPathEnableStatesPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 12; /* six enums (6 x 2) */

   if( NUCPathEnableStatesPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_NUC_ENABLE_STATES,
                              ( LEP_ATTRIBUTE_T_PTR )NUCPathEnableStatesPtr,
                              attributeWordLength );
   return( result );
}
LEP_RESULT LEP_SetCfgNUCPathEnableStates( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                          LEP_CFG_NUC_PATH_CONTROL_T NUCPathEnablesStates )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 12; /* six enums (6 x 2 ) */


   /* Perform Command
   ** Writing the Camera's current video freeze enable state
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_NUC_ENABLE_STATES,
                              ( LEP_ATTRIBUTE_T_PTR ) & NUCPathEnablesStates,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_SetCfgNucPathFfcEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_STATE_E enableState )
{
   LEP_RESULT result = LEP_OK;
   LEP_CFG_NUC_PATH_CONTROL_T nucPath;

   result = LEP_GetCfgNUCPathEnableStates( portDescPtr, &nucPath );
   if( result == LEP_OK )
   {
      nucPath.cfgNucFfcOffsetEnable = enableState;
      result = LEP_SetCfgNUCPathEnableStates( portDescPtr, nucPath );
   }
   return( result );
}

LEP_RESULT LEP_GetCfgTemporalFilterSettings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                             LEP_CFG_TEMPORAL_FILTER_SETTINGS_T_PTR temporalFilterSettingsPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 1;    /* struct contains 2 1-byte values */

   if( temporalFilterSettingsPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_TEMPORAL_FILTER_SETTINGS,
                              ( LEP_ATTRIBUTE_T_PTR )temporalFilterSettingsPtr,
                              attributeWordLength );


   return( result );
}
LEP_RESULT LEP_SetCfgTemporalFilterSettings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                             LEP_CFG_TEMPORAL_FILTER_SETTINGS_T temporalFilterSettings )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 1;    /* struct contains 2 1-byte values */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_TEMPORAL_FILTER_SETTINGS,
                              ( LEP_ATTRIBUTE_T_PTR ) & temporalFilterSettings,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_GetCfgScnrV2Settings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                     LEP_CFG_SCNRv2_SETTINGS_T_PTR scnrV2SettingsPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* struct size 4 bytes */

   if( scnrV2SettingsPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_SCNR_V2_SETTINGS,
                              ( LEP_ATTRIBUTE_T_PTR )scnrV2SettingsPtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgScnrV2Settings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                     LEP_CFG_SCNRv2_SETTINGS_T scnrV2Settings )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* struct size 4 bytes */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_SCNR_V2_SETTINGS,
                              ( LEP_ATTRIBUTE_T_PTR ) & scnrV2Settings,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_GetCfgFfcV2Settings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                    LEP_CFG_FFCv2_SETTINGS_T_PTR ffcV2SettingsPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* struct size 4 bytes */

   if( ffcV2SettingsPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_FFC_V2_SETTINGS,
                              ( LEP_ATTRIBUTE_T_PTR )ffcV2SettingsPtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgFfcV2Settings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                    LEP_CFG_FFCv2_SETTINGS_T ffcV2Settings )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* struct size 4 bytes */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_FFC_V2_SETTINGS,
                              ( LEP_ATTRIBUTE_T_PTR ) & ffcV2Settings,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_GetCfgGainMapSourceSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                          LEP_CFG_GAIN_MAP_SOURCE_SELECT_E_PTR gainSourceSelectPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 32-bit */

   if( gainSourceSelectPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_GAIN_MAP_SOURCE,
                              ( LEP_ATTRIBUTE_T_PTR )gainSourceSelectPtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgGainMapSourceSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                          LEP_CFG_GAIN_MAP_SOURCE_SELECT_E gainSourceSelect )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 32-bit */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_GAIN_MAP_SOURCE,
                              ( LEP_ATTRIBUTE_T_PTR ) & gainSourceSelect,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_GetCfgCustomRampSettings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                         LEP_CFG_CUSTOM_RAMP_SETTINGS_T_PTR customSettingsPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4;    /* 4 16-bit values*/

   if( customSettingsPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_CUSTOM_RAMP_SETTINGS,
                              ( LEP_ATTRIBUTE_T_PTR )customSettingsPtr,
                              attributeWordLength );


   return( result );
}

LEP_RESULT LEP_SetCfgCustomRampSettings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                         LEP_CFG_CUSTOM_RAMP_SETTINGS_T customSettings )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4;    /* 4 16-bit values*/

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_CUSTOM_RAMP_SETTINGS,
                              ( LEP_ATTRIBUTE_T_PTR ) & customSettings,
                              attributeWordLength );


   return( result );
}


LEP_RESULT LEP_SetCfgVideoInputSourceConstant( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                               LEP_UINT16 cfgVideoInputSourceConstant )
{
   LEP_RESULT  result = LEP_OK;
   LEP_UINT16 attributeWordLength = 1; /* 1 16-bit values for constant value */


   /* Perform Command
   ** Reading the Camera's current video output source selection
   */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_VIDEO_INPUT_CONSTANT,
                              ( LEP_ATTRIBUTE_T_PTR ) & cfgVideoInputSourceConstant,
                              attributeWordLength );
   return( result );
}


LEP_RESULT LEP_GetCfgVideoInputSourceConstant( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                               LEP_UINT16 *cfgVideoInputSourceConstantPtr )
{
   LEP_RESULT  result = LEP_OK;
   LEP_UINT16 attributeWordLength = 1; /* 1 16-bit values for constant value */

   if( cfgVideoInputSourceConstantPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   /* Perform Command
   ** Writing the Camera's current video output source selection
   */
   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_VIDEO_INPUT_CONSTANT,
                              ( LEP_ATTRIBUTE_T_PTR )cfgVideoInputSourceConstantPtr,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_GetCfgBlindRowProcessingEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                    LEP_CFG_STATE_E_PTR enableStatePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 2 words */

   if( enableStatePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_BLIND_ROW_PROCESSING_ENABLE_STATE,
                              ( LEP_ATTRIBUTE_T_PTR )enableStatePtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgBlindRowProcessingEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                    LEP_CFG_STATE_E enableState )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 2 words */

   if( enableState >= LEP_CFG_END_STATE )
   {
      return( LEP_RANGE_ERROR );
   }

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_BLIND_ROW_PROCESSING_ENABLE_STATE,
                              ( LEP_ATTRIBUTE_T_PTR ) & enableState,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_GetCfgFrameBounceProcessingEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                       LEP_CFG_STATE_E_PTR enableStatePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 2 words */

   if( enableStatePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_FRAME_BOUNCE_PROCESSING_ENABLE_STATE,
                              ( LEP_ATTRIBUTE_T_PTR )enableStatePtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgFrameBounceProcessingEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                       LEP_CFG_STATE_E enableState )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 2 words */

   if( enableState >= LEP_CFG_END_STATE )
   {
      return( LEP_RANGE_ERROR );
   }

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_FRAME_BOUNCE_PROCESSING_ENABLE_STATE,
                              ( LEP_ATTRIBUTE_T_PTR ) & enableState,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_GetCfgFrameBounceProcessingThreshold( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                     LEP_CFG_FBS_THRESHOLD_T_PTR FBSThresholdPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* 32-bit value */

   if( FBSThresholdPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_FRAME_BOUNCE_PROCESSING_THRESHOLD,
                              ( LEP_ATTRIBUTE_T_PTR )FBSThresholdPtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgFrameBounceProcessingThreshold( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                     LEP_CFG_FBS_THRESHOLD_T FBSThreshold )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* 32-bit value */

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_FRAME_BOUNCE_PROCESSING_THRESHOLD,
                              ( LEP_ATTRIBUTE_T_PTR ) & FBSThreshold,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_GetCfgGpioMode( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                               LEP_CFG_GPIO_MODE_E_PTR gpioModePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 32-bit */

   if( gpioModePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_GPIO_MODE_SELECT,
                              ( LEP_ATTRIBUTE_T_PTR )gpioModePtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgGpioMode( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                               LEP_CFG_GPIO_MODE_E gpioMode )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 32-bit */

   if( gpioMode >= LEP_CFG_END_GPIO_MODE )
   {
      return( LEP_RANGE_ERROR );
   }

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_GPIO_MODE_SELECT,
                              ( LEP_ATTRIBUTE_T_PTR ) & gpioMode,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_GetCfgGpioVsyncPhaseDelay( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                          LEP_CFG_VSYNC_DELAY_E_PTR numHsyncLinesPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 32-bit */

   if( numHsyncLinesPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_GPIO_VSYNC_PHASE_DELAY,
                              ( LEP_ATTRIBUTE_T_PTR )numHsyncLinesPtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgGpioVsyncPhaseDelay( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                          LEP_CFG_VSYNC_DELAY_E numHsyncLines )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2;    /* enums are 32-bit */

   if( numHsyncLines >= LEP_END_CFG_VSYNC_DELAY )
   {
      return( LEP_RANGE_ERROR );
   }

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_GPIO_VSYNC_PHASE_DELAY,
                              ( LEP_ATTRIBUTE_T_PTR ) & numHsyncLines,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_GetCfgColumnNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                           LEP_CFG_LUT8_T_PTR columnNoisePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4;    /* 8 byte LUT */

   if( columnNoisePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_COLUMN_NOISE_FACTOR_LUT,
                              ( LEP_ATTRIBUTE_T_PTR )columnNoisePtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgColumnNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                           LEP_CFG_LUT8_T_PTR columnNoisePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4;    /* 8 byte LUT */

   if( columnNoisePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_COLUMN_NOISE_FACTOR_LUT,
                              ( LEP_ATTRIBUTE_T_PTR )columnNoisePtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_GetCfgTemporalNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                             LEP_CFG_LUT8_T_PTR temporalNoisePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4;    /* 8 byte LUT */

   if( temporalNoisePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_TEMPORAL_NOISE_FACTOR_LUT,
                              ( LEP_ATTRIBUTE_T_PTR )temporalNoisePtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgTemporalNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                             LEP_CFG_LUT8_T_PTR temporalNoisePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4;    /* 8 byte LUT */

   if( temporalNoisePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_TEMPORAL_NOISE_FACTOR_LUT,
                              ( LEP_ATTRIBUTE_T_PTR )temporalNoisePtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_GetCfgSpatialNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_LUT8_T_PTR spatialNoisePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4;    /* 8 byte LUT */

   if( spatialNoisePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_SPATIAL_NOISE_FACTOR_LUT,
                              ( LEP_ATTRIBUTE_T_PTR )spatialNoisePtr,
                              attributeWordLength );

   return( result );
}
LEP_RESULT LEP_SetCfgSpatialNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_LUT8_T_PTR spatialNoisePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4;    /* 8 byte LUT */

   if( spatialNoisePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_SPATIAL_NOISE_FACTOR_LUT,
                              ( LEP_ATTRIBUTE_T_PTR )spatialNoisePtr,
                              attributeWordLength );

   return( result );
}

LEP_RESULT LEP_GetCfgBlindRowFrameStatistics( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                              LEP_CFG_BLIND_ROW_STATISTICS_T_PTR blindRowStatsPtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 4;    /* 4 16-bit values */

   if( blindRowStatsPtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }

   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_BLIND_ROW_STATS,
                              ( LEP_ATTRIBUTE_T_PTR )blindRowStatsPtr,
                              attributeWordLength );


   return( result );
}


LEP_RESULT LEP_SetCfgThermalShutdownEnable( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_THERMAL_SHUTDOWN_ENABLE_T ThermalShutdownEnableState )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */

   if( ThermalShutdownEnableState.cfgThermalShutdownEnable >= LEP_CFG_END_STATE )
   {
      return( LEP_RANGE_ERROR );
   }
   /* Perform Command
   ** Writing the Camera's current video freeze enable state
   */
   result = LEP_SetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_THERMAL_SHUTDOWN_ENABLE,
                              ( LEP_ATTRIBUTE_T_PTR ) & ThermalShutdownEnableState,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_GetCfgThermalShutdownEnable( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_THERMAL_SHUTDOWN_ENABLE_T_PTR ThermalShutdownEnableStatePtr )
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 attributeWordLength = 2; /* one enum = two words */

   if( ThermalShutdownEnableStatePtr == NULL )
   {
      return( LEP_BAD_ARG_POINTER_ERROR );
   }
   result = LEP_GetAttribute( portDescPtr,
                              ( LEP_COMMAND_ID )LEP_CID_CFG_THERMAL_SHUTDOWN_ENABLE,
                              ( LEP_ATTRIBUTE_T_PTR )ThermalShutdownEnableStatePtr,
                              attributeWordLength );
   return( result );
}

LEP_RESULT LEP_GetCfgBootTimeMemoryTest(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                        LEP_UINT16* memTestPtr)
{
   LEP_RESULT result = LEP_OK;

   if(memTestPtr == NULL)
   {
      return(LEP_BAD_ARG_POINTER_ERROR);
   }

   result = LEP_DirectReadRegister(portDescPtr, (LEP_UINT16)LEP_CFG_BOOT_TIME_MEMORY_TEST, memTestPtr);
#if 0
   memTestPtr->gppProgramWalkPattern = (LEP_CFG_TEST_RESULT_E)((regValue & 0x1));
   memTestPtr->gppDataWalkPattern = (LEP_CFG_TEST_RESULT_E)((regValue & 0x2) >> 1);
   memTestPtr->dspProgramWalkPattern = (LEP_CFG_TEST_RESULT_E)((regValue & 0x4) >> 2);
   memTestPtr->dspDataWalkPattern = (LEP_CFG_TEST_RESULT_E)((regValue & 0x8) >> 3);
   memTestPtr->gppDspSharedWalk = (LEP_CFG_TEST_RESULT_E)((regValue & 0x10) >> 4);
   memTestPtr->eccErrorCounter = (LEP_UINT8)((regValue & 0xFF00) >> 8);
#endif
   return(result);
}
LEP_RESULT LEP_GetCfgPOSTMemTest(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                 LEP_UINT16* bootPostPtr)
{
   LEP_RESULT result = LEP_OK;
   LEP_UINT16 iterations = 660;

   if(bootPostPtr == NULL)
   {
      return(LEP_BAD_ARG_POINTER_ERROR);
   }

#if 0
   while(1)
   {
      result = LEP_DirectReadRegister(portDescPtr, LEP_CFG_POST_MEM_TEST_REGISTER, bootPostPtr);
      if(*bootPostPtr & 0x8000)
      {
         break;
      }
      //Sleep(670);    //Fails at 670 and below, 671 passes only after several iterations.
      Sleep(iterations++);    //Seems to return 674 most times, meaning that 673 is too quick but 674 is long enough
   }
   return(iterations);

#else
   result = LEP_DirectReadRegister(portDescPtr, LEP_CFG_POST_MEM_TEST_REGISTER, bootPostPtr);
   if(result == LEP_OK)
   {
      if((*bootPostPtr & 0x8000) == 0x0)
      {
         result = LEP_NOT_READY;
      }
   }
#endif

#if 0
   bootPostPtr->otp0sec = (LEP_CFG_TEST_RESULT_E)((regValue & 0x1));
   bootPostPtr->otp0ded = (LEP_CFG_TEST_RESULT_E)((regValue & 0x2) >> 1);
   bootPostPtr->otp1sec = (LEP_CFG_TEST_RESULT_E)((regValue & 0x4) >> 2);
   bootPostPtr->otp1ded = (LEP_CFG_TEST_RESULT_E)((regValue & 0x8) >> 3);
   bootPostPtr->otp2sec = (LEP_CFG_TEST_RESULT_E)((regValue & 0x10) >> 4);
   bootPostPtr->otp2ded = (LEP_CFG_TEST_RESULT_E)((regValue & 0x20) >> 5);
   bootPostPtr->otp3sec = (LEP_CFG_TEST_RESULT_E)((regValue & 0x40) >> 6);
   bootPostPtr->otp3ded = (LEP_CFG_TEST_RESULT_E)((regValue & 0x80) >> 7);
   bootPostPtr->otp4sec = (LEP_CFG_TEST_RESULT_E)((regValue & 0x100) >> 8);
   bootPostPtr->otp4ded = (LEP_CFG_TEST_RESULT_E)((regValue & 0x200) >> 9);
   bootPostPtr->otp5sec = (LEP_CFG_TEST_RESULT_E)((regValue & 0x400) >> 10);
   bootPostPtr->otp5ded = (LEP_CFG_TEST_RESULT_E)((regValue & 0x800) >> 11);
   bootPostPtr->bootupOtp015sec = (LEP_CFG_TEST_RESULT_E)((regValue & 0x1000) >> 12);
   bootPostPtr->bootupOtp015ded = (LEP_CFG_TEST_RESULT_E)((regValue & 0x2000) >> 13);
   bootPostPtr->bootupVlb63Compare = (LEP_CFG_TEST_RESULT_E)((regValue & 0x4000) >> 14);
   bootPostPtr->memTestRunState = (LEP_CFG_TEST_RUN_STATE_E)((regValue & 0x8000) >> 15);
#endif

   return(result);
}

/******************************************************************************/
/** PRIVATE MODULE FUNCTIONS                                                 **/
/******************************************************************************/


