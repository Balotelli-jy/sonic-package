#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "JZ.AUDIO.SONIC.h"
#include "sonic.h"
#include <utils/Log.h>

//TODO:buffer 大一统
#define LOG_TAG "JZsonic"
#define SONIC_IN_PCM_FILE    "/mnt/sonic_in.pcm"
#define SONIC_OUT_PCM_FILE   "/mnt/sonic_out.pcm"

#define SONIC_DUMP_DEGUG

static stSonicInstance g_stSonicConfig;

JZ_S32 JZ_SonicInit()
{
    g_stSonicConfig.u32BitPerSample = 16;
    g_stSonicConfig.u32Ch           = 2;
    g_stSonicConfig.u32SampleRate   = 44100;
    g_stSonicConfig.fSpeed          = 1.0;
    g_stSonicConfig.u32OutBufSize   = 15052;
    g_stSonicConfig.pstream         = sonicCreateStream(g_stSonicConfig.u32SampleRate, g_stSonicConfig.u32Ch);
    g_stSonicConfig.s16OutBuffer    = (JZ_S8* )malloc(sizeof(JZ_S8) * g_stSonicConfig.u32OutBufSize * 4);
    if (!g_stSonicConfig.s16OutBuffer)
    {
        ALOGE("malloc outbuffer failed!");
        return JZ_ErrorNullPointer;
    }
    sonicSetSpeed(g_stSonicConfig.pstream, 1.0);
    sonicSetPitch(g_stSonicConfig.pstream, 1.0);
    sonicSetRate(g_stSonicConfig.pstream, 1.0);
    sonicSetVolume(g_stSonicConfig.pstream, 1.0);
    sonicSetChordPitch(g_stSonicConfig.pstream, 0);
    sonicSetQuality(g_stSonicConfig.pstream, 0);

#ifdef SONIC_DUMP_DEGUG
    g_stSonicConfig.pInFile = fopen(SONIC_IN_PCM_FILE, "wb");
    if (!g_stSonicConfig.pInFile)
    {
        ALOGE("jztech:failed to open sonic pcm_in file!");
    }
    g_stSonicConfig.pOutFile = fopen(SONIC_OUT_PCM_FILE, "wb");
    if (!g_stSonicConfig.pInFile)
    {
        ALOGE("jztech:failed to open sonic pcm_out file!");
    }
#endif
    return JZ_ErrorNone;
}

JZ_S32 JZ_SonicSetConfig(stSonicInstance* pSonicParam)
{
    if ((!pSonicParam) || (!g_stSonicConfig.pstream))
    {
        ALOGE("pSonicParam is a NULL point or sonic don't init!");
        return JZ_ErrorNullPointer;
    }
    if (pSonicParam->u32Ch > 2)
    {
        ALOGE("unsupport chennnels:%d", pSonicParam->u32Ch);
        return JZ_ErrorInvalidParameter;
    }
    if ((pSonicParam->u32SampleRate < 8000) || (pSonicParam->u32SampleRate > 96000))
    {
        ALOGE("unsupport samplerate:%d", pSonicParam->u32SampleRate);
        return JZ_ErrorInvalidParameter;
    }
    if ((pSonicParam->fSpeed < 0) || (pSonicParam->fSpeed > 2)) 
    {
        ALOGE("unspport speed:%f", pSonicParam->fSpeed);
        return JZ_ErrorInvalidParameter;
    }

    g_stSonicConfig.u32BitPerSample = 16;
    g_stSonicConfig.u32Ch           = pSonicParam->u32Ch;
    g_stSonicConfig.u32SampleRate   = pSonicParam->u32SampleRate;
    g_stSonicConfig.fSpeed          = pSonicParam->fSpeed;

    sonicSetNumChannels(g_stSonicConfig.pstream, g_stSonicConfig.u32Ch);
    sonicSetSampleRate(g_stSonicConfig.pstream, g_stSonicConfig.u32SampleRate);
    sonicSetSpeed(g_stSonicConfig.pstream, g_stSonicConfig.fSpeed);

    return JZ_ErrorNone;
}

JZ_S8* JZ_SonicProcess(void* pInBuffer, JZ_U32* u32samples)
{
    JZ_U32 u32FramesWritten = 0, u32InBufferFrames, u32NewSamples = 0;
    

    if ((!pInBuffer) || (!g_stSonicConfig.s16OutBuffer))
    {
        ALOGE("pInBuffer is the empty buffer or outbuffer don't malloc!");
        return JZ_ErrorEmptyBuffer;
    }
#ifdef SONIC_DUMP_DEGUG
    if (g_stSonicConfig.pInFile)
    {
        fwrite((JZ_S8* )pInBuffer, 1, *u32samples, g_stSonicConfig.pInFile);
    }
#endif
    /* 传进来的是采样点数，要转化成frame之后才能使用sonic进行倍速:
       转化计算：frames = u32samples / 声道 / 位宽 */
    u32InBufferFrames = *u32samples / 4;
    memset(g_stSonicConfig.s16OutBuffer, 0, g_stSonicConfig.u32OutBufSize);
    sonicWriteShortToStream(g_stSonicConfig.pstream, (JZ_S16* )pInBuffer, u32InBufferFrames);

    do {
       u32FramesWritten = sonicReadShortFromStream(g_stSonicConfig.pstream, g_stSonicConfig.s16OutBuffer + u32NewSamples,
                                                u32InBufferFrames);

       u32NewSamples = u32NewSamples + u32FramesWritten * 4;

       ALOGE("jztech:transformision befor:samples = %d, frames = %d    \
       after: samples = %d, frames = %d", *u32samples, u32InBufferFrames, u32NewSamples, u32FramesWritten);

    }
    while (u32FramesWritten > 0);
#ifdef SONIC_DUMP_DEGUG
    if (g_stSonicConfig.pOutFile)
    {
       fwrite(((JZ_S8* )g_stSonicConfig.s16OutBuffer), 1, u32NewSamples, g_stSonicConfig.pOutFile);         
    }
#endif
    *u32samples = u32NewSamples;
    return g_stSonicConfig.s16OutBuffer;
}

JZ_S32 JZ_SonicDeinit()
{
#ifdef SONIC_DUMP_DEGUG 
    if (g_stSonicConfig.pInFile)
    {
        fclose(g_stSonicConfig.pInFile);
        g_stSonicConfig.pInFile = NULL;
    }
    if (g_stSonicConfig.pOutFile)
    {
        fclose(g_stSonicConfig.pOutFile);
        g_stSonicConfig.pOutFile = NULL;
    }
#endif
    if (!g_stSonicConfig.s16OutBuffer)
    {
        free(g_stSonicConfig.s16OutBuffer);
    }
    sonicDestroyStream(g_stSonicConfig.pstream);

    return JZ_ErrorNone;
}
