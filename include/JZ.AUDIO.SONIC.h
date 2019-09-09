/******************************************************************************
 Copyright (C), 2001-2011, JZ Tech. Co., Ltd.
 ******************************************************************************
 File Name     : JZ.AUDIO.SONIC.h
 Version       : Initial Draft
 Author        : JZ platform group
 Created       :
 Last Modified :
 Description   :
 Function List :
 History       :
******************************************************************************/
#ifndef __JZ_AUDIO_SONIC_H__
#define __JZ_AUDIO_SONIC_H__

#include <stdio.h>
#include "sonic.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef unsigned int    JZ_U32;
typedef signed int      JZ_S32;
typedef float           JZ_FLOAT;
typedef unsigned short  JZ_U16;
typedef signed short    JZ_S16;
typedef unsigned char   JZ_U8;
typedef signed char     JZ_S8;



typedef enum JZ_ERRORTYPE_E
{
    JZ_ErrorNone = 0,
    JZ_ErrorNullPointer = (JZ_S32) 0X80001000,
    JZ_ErrorInvalidParameter = (JZ_S32) 0x80001001,
    JZ_ErrorEmptyBuffer = (JZ_S32) 0x80001002,
} JZ_ERRORTYPE_E;

typedef struct
{
    JZ_U32                         u32SampleRate;
    JZ_U32                         u32Ch;
    JZ_U32                         u32BitPerSample;
    JZ_FLOAT                       fSpeed;
    sonicStream                    pstream;
    JZ_U32                         u32OutBufSize;
    JZ_S16*                        s16OutBuffer;
    FILE*                          pInFile;
    FILE*                          pOutFile;
} stSonicInstance;

JZ_S32 JZ_SonicInit();
JZ_S32 JZ_SonicSetConfig(stSonicInstance* pSonicParam);
JZ_S8* JZ_SonicProcess(void* pInBuffer, JZ_U32* u32samples);
JZ_S32 JZ_SonicDeinit();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

