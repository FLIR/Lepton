#include<stdio.h>
#include<stdlib.h>
#if defined(WINDOWSS) || defined(WIN32)
#include<windows.h>

#ifdef __linux
#include<dlfcn.h>
#endif

#include "libMPSSE_i2c.h"
#include "ftd2xx.h"
#ifdef _WIN32
	#define GET_FUN_POINTER	GetProcAddress
	#define CHECK_ERROR(exp) {if(exp==NULL){printf("%s:%d:%s():  NULL expression encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};
#endif

#ifdef __linux
	#define GET_FUN_POINTER	dlsym
	#define CHECK_ERROR(exp) {if(dlerror() != NULL){printf("line %d: ERROR dlsym\n",__LINE__);}}
#endif

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) != FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

typedef FT_STATUS (*pfunc_I2C_GetNumChannels)(uint32 *numChannels);
pfunc_I2C_GetNumChannels p_I2C_GetNumChannels;
typedef FT_STATUS (*pfunc_I2C_GetChannelInfo)(uint32 index, FT_DEVICE_LIST_INFO_NODE *chanInfo);
pfunc_I2C_GetChannelInfo p_I2C_GetChannelInfo;
typedef FT_STATUS (*pfunc_I2C_OpenChannel)(uint32 index, FT_HANDLE *handle);
pfunc_I2C_OpenChannel p_I2C_OpenChannel;
typedef FT_STATUS (*pfunc_I2C_CloseChannel)(FT_HANDLE handle);
pfunc_I2C_CloseChannel p_I2C_CloseChannel;
typedef FT_STATUS (*pfunc_I2C_InitChannel)(FT_HANDLE handle, ChannelConfig *config);
pfunc_I2C_InitChannel p_I2C_InitChannel;
typedef FT_STATUS (*pfunc_I2C_DeviceRead)(FT_HANDLE handle,uint32 deviceAddress,uint32 bytesToTransfer, uint8 *buffer, uint32 *bytesTransfered, uint32 options);
pfunc_I2C_DeviceRead p_I2C_DeviceRead;
typedef FT_STATUS (*pfunc_I2C_DeviceWrite)(FT_HANDLE handle, uint32 deviceAddress,uint32 bytesToTransfer, uint8 *buffer, uint32 *bytesTransfered, uint32 options);
pfunc_I2C_DeviceWrite p_I2C_DeviceWrite;

#endif  // WINDOWSS || WIN32
