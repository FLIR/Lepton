/*******************************************************************************
**
**    File NAME: LEPTON_CFG.h
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
#ifndef _LEPTON_CFG_H_
   #define _LEPTON_CFG_H_

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
   #define LEP_CFG_MODULE_BASE                     0x0D00

   #define LEP_CID_CFG_FLIR_SERIAL_NUMBER          (LEP_CFG_MODULE_BASE + 0x8000 )
   #define LEP_CID_CFG_FLIR_PART_NUMBER            (LEP_CFG_MODULE_BASE + 0x8004 )
   #define LEP_CID_CFG_MASTER_ID                   (LEP_CFG_MODULE_BASE + 0x8008 )
   #define LEP_CID_CFG_GAIN_MAP_SOURCE             (LEP_CFG_MODULE_BASE + 0x800C )
   #define LEP_CID_CFG_TEMPORAL_FILTER_SETTINGS    (LEP_CFG_MODULE_BASE + 0x8010 )
   #define LEP_CID_CFG_VIDEO_INPUT_CONSTANT        (LEP_CFG_MODULE_BASE + 0x8018 )
   #define LEP_CID_CFG_VIDEO_MODE_SELECT           (LEP_CFG_MODULE_BASE + 0x801C )
   #define LEP_CID_CFG_VIDEO_INPUT_SELECT          (LEP_CFG_MODULE_BASE + 0x8020 )
   #define LEP_CID_CFG_VIDEO_CROP_ROI_SELECT       (LEP_CFG_MODULE_BASE + 0x8024 )
   #define LEP_CID_CFG_SCNR_V2_SETTINGS            (LEP_CFG_MODULE_BASE + 0x8028 )
   #define LEP_CID_CFG_FFC_V2_SETTINGS             (LEP_CFG_MODULE_BASE + 0x802C )
   #define LEP_CID_CFG_NUC_ENABLE_STATES           (LEP_CFG_MODULE_BASE + 0x8030 )
   #define LEP_CID_CFG_BAD_PIXEL_REPLACEMENT       (LEP_CFG_MODULE_BASE + 0x8034 )
   #define LEP_CID_CFG_TEMPORAL_FILTER_CONTROL     (LEP_CFG_MODULE_BASE + 0x8038 )
   #define LEP_CID_CFG_COLUMN_NOISE_ESTIMATE       (LEP_CFG_MODULE_BASE + 0x803C )
   #define LEP_CID_CFG_ROW_NOISE_ESTIMATE          (LEP_CFG_MODULE_BASE + 0x8040 )
   #define LEP_CID_CFG_SBNUC_FFC_ESTIMATE          (LEP_CFG_MODULE_BASE + 0x8044 )
   #define LEP_CID_CFG_CUST_SERIAL_NUMBER          (LEP_CFG_MODULE_BASE + 0x8048 )
   #define LEP_CID_CFG_CUST_PART_NUMBER            (LEP_CFG_MODULE_BASE + 0x804C )
   #define LEP_CID_CFG_LAGRANGE_FILTER_CONTROL     (LEP_CFG_MODULE_BASE + 0x8050 )
   #define LEP_CID_CFG_CUSTOM_RAMP_SETTINGS        (LEP_CFG_MODULE_BASE + 0x8054 )
   #define LEP_CID_CFG_BLIND_ROW_PROCESSING_ENABLE_STATE       (LEP_CFG_MODULE_BASE + 0x8058)
   #define LEP_CID_CFG_FRAME_BOUNCE_PROCESSING_ENABLE_STATE    (LEP_CFG_MODULE_BASE + 0x805C)
   #define LEP_CID_CFG_FRAME_BOUNCE_PROCESSING_THRESHOLD       (LEP_CFG_MODULE_BASE + 0x8060)

   #define LEP_CID_CFG_COLUMN_NOISE_FACTOR_LUT     (LEP_CFG_MODULE_BASE + 0x8064)
   #define LEP_CID_CFG_TEMPORAL_NOISE_FACTOR_LUT   (LEP_CFG_MODULE_BASE + 0x8068)
   #define LEP_CID_CFG_SPATIAL_NOISE_FACTOR_LUT    (LEP_CFG_MODULE_BASE + 0x806C)
#if 1
   #define LEP_CID_CFG_GPIO_MODE_SELECT            (LEP_CFG_MODULE_BASE + 0x8070)      // clearer name
#else   
   #define LEP_CID_CFG_GPIO_VSYNC_ENABLE_STATE     (LEP_CFG_MODULE_BASE + 0x8070)      // not a good name
#endif      
   #define LEP_CID_CFG_GPIO_VSYNC_PHASE_DELAY      (LEP_CFG_MODULE_BASE + 0x8074)
   #define LEP_CID_CFG_BLIND_ROW_STATS             (LEP_CFG_MODULE_BASE + 0x8078)
   #define LEP_CID_CFG_THERMAL_SHUTDOWN_ENABLE     (LEP_CFG_MODULE_BASE + 0x807C)


   #define LEP_CFG_MAX_PART_NUMBER_CHAR_SIZE       32
   #define LEP_CFG_MAX_SERIAL_NUMBER_CHAR_SIZE     32
   #define LEP_CFG_POST_MEM_TEST_REGISTER          0x2C //e8
   #define LEP_CFG_BOOT_TIME_MEMORY_TEST           0x2A //e4

/******************************************************************************/
/** EXPORTED TYPE DEFINITIONS                                                **/
/******************************************************************************/

typedef LEP_UINT64   LEP_CFG_FLIR_SERIAL_NUMBER_T, *LEP_CFG_FLIR_SERIAL_NUMBER_T_PTR;
typedef LEP_UINT32   LEP_CFG_FBS_THRESHOLD_T, *LEP_CFG_FBS_THRESHOLD_T_PTR;
typedef LEP_UINT8    LEP_CFG_LUT8_T, *LEP_CFG_LUT8_T_PTR;

/* Serial Number: A (32 byte string) identifier unique to a 
** specific configuration of module; essentially a module
** Configuration ID.
*/

#if USE_DEPRECATED_SERIAL_NUMBER_INTERFACE
typedef LEP_CHAR8    *LEP_CFG_CUST_SERIAL_NUMBER_T, *LEP_CFG_CUST_SERIAL_NUMBER_T_PTR;
#else
typedef struct LEP_CFG_CUST_SERIAL_NUMBER_T_TAG
{
   LEP_CHAR8 value[LEP_CFG_MAX_SERIAL_NUMBER_CHAR_SIZE];
} LEP_CFG_CUST_SERIAL_NUMBER_T, *LEP_CFG_CUST_SERIAL_NUMBER_T_PTR;
#endif
/* Part Number: A (32 byte string) identifier unique to a
** specific configuration of module; essentially a module
** Configuration ID.
*/
typedef LEP_CHAR8 *LEP_CFG_PART_NUMBER_T, *LEP_CFG_PART_NUMBER_T_PTR;
typedef struct LEP_CFG_PN_T_TAG
{
   LEP_CHAR8 value[LEP_CFG_MAX_PART_NUMBER_CHAR_SIZE];
}LEP_CFG_PN_T, *LEP_CFG_PN_T_PTR;

/* Video Mode Selection
*/
typedef enum LEP_CFG_VIDEO_MODE_SELECT_TAG
{
   LEP_VIDEO_MODE_NORMAL = 0,
   LEP_VIDEO_MODE_CALIBRATION,
   LEP_VIDEO_MODE_TEST,
   LEP_END_VIDEO_MODE_SELECT

}LEP_CFG_VIDEO_MODE_SELECT_E, *LEP_CFG_VIDEO_MODE_SELECT_E_PTR;

/*
*/
typedef enum LEP_CFG_STATE_E_TAG
{
   LEP_CFG_DISABLE = 0,
   LEP_CFG_ENABLE,
   LEP_CFG_END_STATE

}LEP_CFG_STATE_E,*LEP_CFG_STATE_E_PTR;

typedef enum LEP_CFG_TEST_RESULT_E_TAG
{
   LEP_CFG_TEST_PASSED = 0,
   LEP_CFG_TEST_FAILED = 1,
   LEP_CFG_END_TEST_STATE
}LEP_CFG_TEST_RESULT_E, *LEP_CFG_TEST_RESULT_E_PTR;

typedef enum LEP_CFG_TEST_RUN_STATE_E_TAG
{
   LEP_CFG_TEST_STATE_RUNNING = 0,
   LEP_CFG_TEST_STATE_COMPLETE,
   LEP_CFG_END_TEST_RUN_STATE
} LEP_CFG_TEST_RUN_STATE_E, *LEP_CFG_TEST_RUN_STATE_E_PTR;

typedef enum LEP_CFG_GAIN_MAP_SOURCE_SELECT_E_TAG
{
   LEP_CFG_GAIN_MAP_SOURCE_SRAM = 0,
   LEP_CFG_GAIN_MAP_SOURCE_OTP,
   LEP_CFG_END_GAIN_MAP_SOURCE

}LEP_CFG_GAIN_MAP_SOURCE_SELECT_E, *LEP_CFG_GAIN_MAP_SOURCE_SELECT_E_PTR;

typedef struct LEP_CFG_NUC_PATH_CONTROL_T_TAG
{
   LEP_CFG_STATE_E cfgNucGainEnable;                 // NUC Gain application in the video path
   LEP_CFG_STATE_E cfgNucFfcOffsetEnable;             // NUC FFC Offsets application in the video path
   LEP_CFG_STATE_E cfgNucTemperatureOffsetsEnable;    // NUC Temperature Offsets application in the video path
   LEP_CFG_STATE_E cfgNucLaGrangOffsetEnable;         // NUC LaGrange Offsets application in the video path
   LEP_CFG_STATE_E cfgNucColNoiseOffsetEnable;        // NUC Column Noise Offsets application in the video path
   LEP_CFG_STATE_E cfgNucRowNoiseOffsetEnable;        // NUC Row Noise Offsets application in the video path

}LEP_CFG_NUC_PATH_CONTROL_T, *LEP_CFG_NUC_PATH_CONTROL_T_PTR;

typedef struct LEP_CFG_BAD_PIXEL_REPLACE_CONTROL_T_TAG
{
   LEP_CFG_STATE_E cfgBadPixelReplaceEnable;          // Bad Pixel Replacment in the video path

}LEP_CFG_BAD_PIXEL_REPLACE_CONTROL_T, *LEP_CFG_BAD_PIXEL_REPLACE_CONTROL_T_PTR;
typedef struct LEP_CFG_TEMPORAL_FILTER_CONTROL_T_TAG
{
   LEP_CFG_STATE_E cfgTemporalFilterEnable;           // Temporal Filter in the video path

}LEP_CFG_TEMPORAL_FILTER_CONTROL_T, *LEP_CFG_TEMPORAL_FILTER_CONTROL_T_PTR;

typedef struct LEP_CFG_COLUMN_NOISE_ESTIMATE_CONTROL_T_TAG
{
   LEP_CFG_STATE_E cfgColumnNoiseEstimateEnable;      // Column Noise Estimate in the video path

}LEP_CFG_COLUMN_NOISE_ESTIMATE_CONTROL_T, *LEP_CFG_COLUMN_NOISE_ESTIMATE_CONTROL_T_PTR;

typedef struct LEP_CFG_ROW_NOISE_ESTIMATE_CONTROL_T_TAG
{
   LEP_CFG_STATE_E cfgRowNoiseEstimateEnable;         // Row Noise Estimate in the video path

}LEP_CFG_ROW_NOISE_ESTIMATE_CONTROL_T, *LEP_CFG_ROW_NOISE_ESTIMATE_CONTROL_T_PTR;

typedef struct LEP_CFG_SBNUC_FFC_ESTIMATE_CONTROL_T_TAG
{
   LEP_CFG_STATE_E cfgSbnucFfcEstimateEnable;         //  SBNUC FFC Estimate in the video path

}LEP_CFG_SBNUC_FFC_ESTIMATE_CONTROL_T, *LEP_CFG_SBNUC_FFC_ESTIMATE_CONTROL_T_PTR;

typedef struct LEP_CFG_LAGRANGE_FILTER_CONTROL_T_TAG
{
   LEP_CFG_STATE_E cfgLaGrangeFilterEnable;           //  LaGrange Filter in the video path

}LEP_CFG_LAGRANGE_FILTER_CONTROL_T, *LEP_CFG_LAGRANGE_FILTER_CONTROL_T_PTR;

typedef struct LEP_CFG_TEMPORAL_FILTER_SETTINGS_T_TAG
{
   LEP_UINT8 delta_nf;              // Scalar for wLUT
   LEP_UINT8 th_motion_delta;       // Specifies if pixel is in motion

}LEP_CFG_TEMPORAL_FILTER_SETTINGS_T, *LEP_CFG_TEMPORAL_FILTER_SETTINGS_T_PTR;

typedef struct LEP_CFG_SCNRv2_SETTINGS_T_TAG
{
   LEP_UINT16 th_col_sum;           // Typically 3.5 * FPA_ROWS
   LEP_UINT8 th_pix_in;             // Pixel threshold beyond which pixels are not processed
   LEP_UINT8 th_max_correction;     // Maximum correction allowed (total)

} LEP_CFG_SCNRv2_SETTINGS_T, *LEP_CFG_SCNRv2_SETTINGS_T_PTR;

typedef struct LEP_CFG_FFCv2_SETTINGS_T_TAG
{
   LEP_UINT8 th_pix_sum;            // Sets the majority threshold specifying minimum neighbors
                                    // to make a majority
   LEP_UINT8 th_pix_in;             // Pixel Delta threshold beyond which pixels are not processed
   LEP_UINT8 th_max_correction;     // Maximum correction allowed (total)

} LEP_CFG_FFCv2_SETTINGS_T, *LEP_CFG_FFCv2_SETTINGS_T_PTR;

/* Video Input source selection
*/
typedef enum LEP_CFG_VIDEO_INPUT_SOURCE_SELECT_TAG
{
   LEP_VIDEO_INPUT_SOURCE_FPA = 0,
   LEP_VIDEO_INPUT_SOURCE_CONSTANT,
   LEP_VIDEO_INPUT_SOURCE_RAMP_2D,
   LEP_VIDEO_INPUT_SOURCE_RAMP_COL,
   LEP_VIDEO_INPUT_SOURCE_RAMP_ROW,
   LEP_VIDEO_INPUT_SOURCE_RAMP_CUSTOM,
   LEP_VIDEO_INPUT_SOURCE_RAMP_TEST,
   LEP_VIDEO_INPUT_SOURCE_FRAME_BUFFER_0,
   LEP_VIDEO_INPUT_SOURCE_FRAME_BUFFER_1,
   LEP_VIDEO_INPUT_SOURCE_FRAME_BUFFER_2,
   LEP_VIDEO_INPUT_SOURCE_FRAME_BUFFER_3,
   LEP_VIDEO_INPUT_SOURCE_FRAME_BUFFER_4,

   LEP_END_VIDEO_INPUT_SOURCE_SELECT

}LEP_CFG_VIDEO_INPUT_SOURCE_SELECT_E, *LEP_CFG_VIDEO_INPUT_SOURCE_SELECT_E_PTR;

typedef struct LEP_CFG_CUSTOM_RAMP_SETTINGS_T_TAG
{
   LEP_UINT16 startValue;
   LEP_UINT16 endValue;
   LEP_UINT16 colIncrementValue;
   LEP_UINT16 rowIncrementValue;
}LEP_CFG_CUSTOM_RAMP_SETTINGS_T, *LEP_CFG_CUSTOM_RAMP_SETTINGS_T_PTR;

typedef enum LEP_CFG_VSYNC_DELAY_E_TAG
{
   LEP_CFG_VSYNC_DELAY_MINUS_3 = -3,
   LEP_CFG_VSYNC_DELAY_MINUS_2 = -2,
   LEP_CFG_VSYNC_DELAY_MINUS_1 = -1,
   LEP_CFG_VSYNC_DELAY_NONE = 0,
   LEP_CFG_VSYNC_DELAY_PLUS_1 = 1,
   LEP_CFG_VSYNC_DELAY_PLUS_2 = 2,
   LEP_CFG_VSYNC_DELAY_PLUS_3 = 3,

   LEP_END_CFG_VSYNC_DELAY
} LEP_CFG_VSYNC_DELAY_E, *LEP_CFG_VSYNC_DELAY_E_PTR;

typedef enum LEP_CFG_GPIO_MODE_E_TAG
{
   LEP_CFG_GPIO_MODE_GPIO = 0,
   LEP_CFG_GPIO_MODE_I2C_MASTER = 1,
   LEP_CFG_GPIO_MODE_SPI_MASTER_VLB_DATA = 2,
   LEP_CFG_GPIO_MODE_SPIO_MASTER_REG_DATA = 3,
   LEP_CFG_GPIO_MODE_SPI_SLAVE_VLB_DATA = 4,
   LEP_CFG_GPIO_MODE_VSYNC = 5,

   LEP_CFG_END_GPIO_MODE,
}LEP_CFG_GPIO_MODE_E, *LEP_CFG_GPIO_MODE_E_PTR;

typedef struct LEP_CFG_BLIND_ROW_STATISTICS_T_TAG
{
   LEP_UINT16     meanIntensity;
   LEP_UINT16     maxIntensity;
   LEP_UINT16     minIntensity;
   LEP_UINT16     numPixels;

} LEP_CFG_BLIND_ROW_STATISTICS_T, *LEP_CFG_BLIND_ROW_STATISTICS_T_PTR;


typedef struct LEP_CFG_THERMAL_SHUTDOWN_ENABLE_T_TAG
{
   LEP_CFG_STATE_E cfgThermalShutdownEnable;         

}LEP_CFG_THERMAL_SHUTDOWN_ENABLE_T, *LEP_CFG_THERMAL_SHUTDOWN_ENABLE_T_PTR;

#if 0
typedef struct LEP_CFG_BOOT_TIME_MEM_TEST_T_TAG
{
   LEP_CFG_TEST_RESULT_E gppProgramWalkPattern;
   LEP_CFG_TEST_RESULT_E gppDataWalkPattern;
   LEP_CFG_TEST_RESULT_E dspProgramWalkPattern;
   LEP_CFG_TEST_RESULT_E dspDataWalkPattern;
   LEP_CFG_TEST_RESULT_E gppDspSharedWalk;
   LEP_UINT8            eccErrorCounter;
} LEP_CFG_BOOT_TIME_MEM_TEST_T, *LEP_CFG_BOOT_TIME_MEM_TEST_T_PTR;

typedef struct LEP_CFG_BOOT_POST_T_TAG
{
   LEP_CFG_TEST_RESULT_E otp0sec;
   LEP_CFG_TEST_RESULT_E otp0ded;
   LEP_CFG_TEST_RESULT_E otp1sec;
   LEP_CFG_TEST_RESULT_E otp1ded;
   LEP_CFG_TEST_RESULT_E otp2sec;
   LEP_CFG_TEST_RESULT_E otp2ded;
   LEP_CFG_TEST_RESULT_E otp3sec;
   LEP_CFG_TEST_RESULT_E otp3ded;
   LEP_CFG_TEST_RESULT_E otp4sec;
   LEP_CFG_TEST_RESULT_E otp4ded;
   LEP_CFG_TEST_RESULT_E otp5sec;
   LEP_CFG_TEST_RESULT_E otp5ded;
   LEP_CFG_TEST_RESULT_E bootupOtp015sec;
   LEP_CFG_TEST_RESULT_E bootupOtp015ded;
   LEP_CFG_TEST_RESULT_E bootupVlb63Compare;
   LEP_CFG_TEST_RUN_STATE_E memTestRunState;

} LEP_CFG_BOOT_POST_T, *LEP_CFG_BOOT_POST_T_PTR;
#endif

/******************************************************************************/
/** EXPORTED PUBLIC DATA                                                     **/
/******************************************************************************/

extern LEP_RESULT LEP_SetCfgFlirSerialNumber( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                              LEP_CFG_FLIR_SERIAL_NUMBER_T serialNumber );
#if USE_DEPRECATED_SERIAL_NUMBER_INTERFACE
/* Deprecated */
extern LEP_RESULT LEP_SetCfgCustSerialNumber( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                              LEP_CFG_CUST_SERIAL_NUMBER_T serialNumber );
#else
extern LEP_RESULT LEP_SetCfgCustSerialNumber(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                             LEP_CFG_CUST_SERIAL_NUMBER_T_PTR serialNumberPtr);
#endif


#if USE_DEPRECATED_PART_NUMBER_INTERFACE
/* Deprecated */
extern LEP_RESULT LEP_SetCfgFlirPartNumber( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_PART_NUMBER_T partNumber );
/* Deprecated */
extern LEP_RESULT LEP_SetCfgCustPartNumber( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_PART_NUMBER_T partNumber );
#else


extern LEP_RESULT LEP_SetCfgFlirPartNumber(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                           LEP_CFG_PART_NUMBER_T_PTR partNumberPtr);

extern LEP_RESULT LEP_SetCfgCustPartNumber(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                           LEP_CFG_PART_NUMBER_T_PTR partNumberPtr);
#endif
extern LEP_RESULT LEP_GetCfgVideoModeSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                             LEP_CFG_VIDEO_MODE_SELECT_E_PTR videoModeSelectPtr );

extern LEP_RESULT LEP_SetCfgVideoModeSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                             LEP_CFG_VIDEO_MODE_SELECT_E videoModeSelect );

extern LEP_RESULT LEP_GetCfgVideoSourceSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                               LEP_CFG_VIDEO_INPUT_SOURCE_SELECT_E_PTR videoSourceSelectPtr );

extern LEP_RESULT LEP_SetCfgVideoSourceSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                               LEP_CFG_VIDEO_INPUT_SOURCE_SELECT_E videoSourceSelect );

extern LEP_RESULT LEP_GetCfgNUCPathEnableStates( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                 LEP_CFG_NUC_PATH_CONTROL_T_PTR NUCPathEnableStatesPtr );

extern LEP_RESULT LEP_SetCfgNUCPathEnableStates( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                 LEP_CFG_NUC_PATH_CONTROL_T NUCPathEnableStates );

extern LEP_RESULT LEP_GetCfgBadPixelReplaceControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                    LEP_CFG_BAD_PIXEL_REPLACE_CONTROL_T_PTR BadPixelReplaceControlPtr );
extern LEP_RESULT LEP_SetCfgBadPixelReplaceControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                    LEP_CFG_BAD_PIXEL_REPLACE_CONTROL_T BadPixelReplaceControl );


extern LEP_RESULT LEP_GetCfgTemporalFilterControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                   LEP_CFG_TEMPORAL_FILTER_CONTROL_T_PTR TemporalFilterControlPtr );
extern LEP_RESULT LEP_SetCfgTemporalFilterControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                   LEP_CFG_TEMPORAL_FILTER_CONTROL_T TemporalFilterControl );


extern LEP_RESULT LEP_GetCfgColumnNoiseEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                        LEP_CFG_COLUMN_NOISE_ESTIMATE_CONTROL_T_PTR ColumnNoiseEstimateControlPtr );
extern LEP_RESULT LEP_SetCfgColumnNoiseEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                        LEP_CFG_COLUMN_NOISE_ESTIMATE_CONTROL_T ColumnNoiseEstimateControl );


extern LEP_RESULT LEP_GetCfgRowNoiseEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                     LEP_CFG_ROW_NOISE_ESTIMATE_CONTROL_T_PTR RowNoiseEstimateControlPtr );
extern LEP_RESULT LEP_SetCfgRowNoiseEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                     LEP_CFG_ROW_NOISE_ESTIMATE_CONTROL_T RowNoiseEstimateControl );

extern LEP_RESULT LEP_GetCfgSbnucFfcEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                     LEP_CFG_SBNUC_FFC_ESTIMATE_CONTROL_T_PTR SbnucFfcEstimateControlPtr );
extern LEP_RESULT LEP_SetCfgSbnucFfcEstimateControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                     LEP_CFG_SBNUC_FFC_ESTIMATE_CONTROL_T SbnucFfcEstimateControl );

extern LEP_RESULT LEP_GetCfgLagrangeFilterControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                   LEP_CFG_LAGRANGE_FILTER_CONTROL_T_PTR lagrangeFilterControlPtr );
extern LEP_RESULT LEP_SetCfgLagrangeFilterControl( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                   LEP_CFG_LAGRANGE_FILTER_CONTROL_T lagrangeFilterControl );

extern LEP_RESULT LEP_SetCfgNucPathFfcEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                   LEP_CFG_STATE_E enableState );

extern LEP_RESULT LEP_GetCfgTemporalFilterSettings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                    LEP_CFG_TEMPORAL_FILTER_SETTINGS_T_PTR temporalFilterSettingsPtr );
extern LEP_RESULT LEP_SetCfgTemporalFilterSettings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                    LEP_CFG_TEMPORAL_FILTER_SETTINGS_T temporalFilterSettings );

extern LEP_RESULT LEP_GetCfgScnrV2Settings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_SCNRv2_SETTINGS_T_PTR scnrV2SettingsPtr );
extern LEP_RESULT LEP_SetCfgScnrV2Settings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                            LEP_CFG_SCNRv2_SETTINGS_T scnrV2Settings );

extern LEP_RESULT LEP_GetCfgFfcV2Settings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                           LEP_CFG_FFCv2_SETTINGS_T_PTR ffcV2SettingsPtr );
extern LEP_RESULT LEP_SetCfgFfcV2Settings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                           LEP_CFG_FFCv2_SETTINGS_T ffcV2Settings );

extern LEP_RESULT LEP_GetCfgGainMapSourceSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                 LEP_CFG_GAIN_MAP_SOURCE_SELECT_E_PTR gainSourceSelectPtr );
extern LEP_RESULT LEP_SetCfgGainMapSourceSelect( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                 LEP_CFG_GAIN_MAP_SOURCE_SELECT_E gainSourceSelect );

extern LEP_RESULT LEP_GetCfgCustomRampSettings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                LEP_CFG_CUSTOM_RAMP_SETTINGS_T_PTR customSettingsPtr );
extern LEP_RESULT LEP_SetCfgCustomRampSettings( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                LEP_CFG_CUSTOM_RAMP_SETTINGS_T customSettings );

extern LEP_RESULT LEP_GetCfgVideoInputSourceConstant( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                      LEP_UINT16 *cfgVideoInputSourceConstantPtr );

extern LEP_RESULT LEP_SetCfgVideoInputSourceConstant( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                      LEP_UINT16 cfgVideoInputSourceConstant );

extern LEP_RESULT LEP_GetCfgBlindRowProcessingEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                           LEP_CFG_STATE_E_PTR enableStatePtr );
extern LEP_RESULT LEP_SetCfgBlindRowProcessingEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                           LEP_CFG_STATE_E enableState );
extern LEP_RESULT LEP_GetCfgFrameBounceProcessingEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                              LEP_CFG_STATE_E_PTR enableStatePtr );
extern LEP_RESULT LEP_SetCfgFrameBounceProcessingEnableState( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                              LEP_CFG_STATE_E enableState );
extern LEP_RESULT LEP_GetCfgFrameBounceProcessingThreshold( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                            LEP_CFG_FBS_THRESHOLD_T_PTR FBSThresholdPtr );
extern LEP_RESULT LEP_SetCfgFrameBounceProcessingThreshold( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                            LEP_CFG_FBS_THRESHOLD_T FBSThreshold );

extern LEP_RESULT LEP_GetCfgGpioMode( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                      LEP_CFG_GPIO_MODE_E_PTR gpioModePtr );
extern LEP_RESULT LEP_SetCfgGpioMode( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                      LEP_CFG_GPIO_MODE_E gpioMode );
extern LEP_RESULT LEP_GetCfgGpioVsyncPhaseDelay( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                 LEP_CFG_VSYNC_DELAY_E_PTR numHsyncLinesPtr );
extern LEP_RESULT LEP_SetCfgGpioVsyncPhaseDelay( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                 LEP_CFG_VSYNC_DELAY_E numHsyncLines );

extern LEP_RESULT LEP_GetCfgColumnNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                  LEP_CFG_LUT8_T_PTR columnNoisePtr );
extern LEP_RESULT LEP_SetCfgColumnNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                  LEP_CFG_LUT8_T_PTR columnNoisePtr );
extern LEP_RESULT LEP_GetCfgTemporalNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                    LEP_CFG_LUT8_T_PTR temporalNoisePtr );
extern LEP_RESULT LEP_SetCfgTemporalNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                    LEP_CFG_LUT8_T_PTR temporalNoisePtr );
extern LEP_RESULT LEP_GetCfgSpatialNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                   LEP_CFG_LUT8_T_PTR spatialNoisePtr );
extern LEP_RESULT LEP_SetCfgSpatialNoiseFactorLut( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                   LEP_CFG_LUT8_T_PTR spatialNoisePtr );

extern LEP_RESULT LEP_GetCfgBlindRowFrameStatistics( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                     LEP_CFG_BLIND_ROW_STATISTICS_T_PTR blindRowStatsPtr );

extern LEP_RESULT LEP_GetCfgThermalShutdownEnable( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                   LEP_CFG_THERMAL_SHUTDOWN_ENABLE_T_PTR ThermalShutdownEnableStatePtr );

extern LEP_RESULT LEP_SetCfgThermalShutdownEnable( LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                                   LEP_CFG_THERMAL_SHUTDOWN_ENABLE_T ThermalShutdownEnableState ); 

extern LEP_RESULT LEP_GetCfgBootTimeMemoryTest(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                               LEP_UINT16* memTestPtr);

extern LEP_RESULT LEP_GetCfgPOSTMemTest(LEP_CAMERA_PORT_DESC_T_PTR portDescPtr,
                                        LEP_UINT16* bootPostPtr);

/******************************************************************************/
/** EXPORTED PUBLIC FUNCTIONS                                                **/
/******************************************************************************/


/******************************************************************************/
   #ifdef __cplusplus
}
   #endif

#endif  /* _LEPTON_CFG_H_ */

