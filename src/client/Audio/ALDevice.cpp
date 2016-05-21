#include "ALDevice.h"
#include "CWaves.h"
//#include <FLAC++/decoder.h>
#include <Ogg/ogg.h>
#include <Vorbis/vorbisfile.h>
#include <vector>
#include <iterator>
#include <cstring>
#include <memory>
#include <cstdlib>
using namespace std;
unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels)
{
    int current_section;
    unsigned long ulBytesDone = 0;
    while (1)
    {
        long lDecodeSize = ov_read(psOggVorbisFile, pDecodeBuffer + ulBytesDone, ulBufferSize - ulBytesDone, 0, 2, 1, &current_section);
        if (lDecodeSize > 0)
        {
            ulBytesDone += lDecodeSize;

            if (ulBytesDone >= ulBufferSize)
                break;
        }
        else
        {
            break;
        }
    }

    // Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
    // however 6-Channels files need to be re-ordered
    if (ulChannels == 6)
    {
        short * pSamples = (short*)pDecodeBuffer;
        for (unsigned long ulSamples = 0; ulSamples < (ulBufferSize >> 1); ulSamples += 6)
        {
            // WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
            // OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
            swap(pSamples[ulSamples + 1], pSamples[ulSamples + 2]);
            swap(pSamples[ulSamples + 3], pSamples[ulSamples + 5]);
            swap(pSamples[ulSamples + 4], pSamples[ulSamples + 5]);
        }
    }

    return ulBytesDone;
}

bool ALDevice::init(ALCchar * DeviceName)//初始化
{
    Device = alcOpenDevice(DeviceName);
    if (Device)
    {
        Context = alcCreateContext(Device, NULL);
        if (Context)
        {
            alcMakeContextCurrent(Context);
#ifdef NEWORLD_TARGET_WINDOWS
            EFX::init();
#endif
            return true;
        }
        alcCloseDevice(Device);
    }
    return false;
}
void ALDevice::updateListener(ALfloat listenerPos[], ALfloat listenerVel[], ALfloat listenerOri[])
{
    alListenerfv(AL_POSITION, listenerPos);
    alListenerfv(AL_VELOCITY, listenerVel);
    alListenerfv(AL_ORIENTATION, listenerOri);
}
void ALDevice::updateSource(ALuint Source, ALfloat sourcePos[], ALfloat sourceVel[])
{
    alSourcefv(Source, AL_POSITION, sourcePos);
    alSourcefv(Source, AL_VELOCITY, sourceVel);
}
bool ALDevice::load(const char * FileName, ALuint *uiBuffer)
{
    alGenBuffers(1, uiBuffer);
    ALchar *pData = nullptr;
    ALint iDataSize = 0, iFrequency = 0;
    ALenum eBufferFormat = 0;
    //WaveLoader
    {
        CWaves WaveLoader;
        WAVEID            WaveID;
        if (SUCCEEDED(WaveLoader.LoadWaveFile(FileName, &WaveID)))
        {
            if ((SUCCEEDED(WaveLoader.GetWaveSize(WaveID, (unsigned long*)&iDataSize))) &&
                    (SUCCEEDED(WaveLoader.GetWaveData(WaveID, (void**)&pData))) &&
                    (SUCCEEDED(WaveLoader.GetWaveFrequency(WaveID, (unsigned long*)&iFrequency))) &&
                    (SUCCEEDED(WaveLoader.GetWaveALBufferFormat(WaveID, &alGetEnumValue, (unsigned long*)&eBufferFormat))))
            {
                alBufferData(*uiBuffer, eBufferFormat, pData, iDataSize, iFrequency);
                WaveLoader.DeleteWaveFile(WaveID);
                return true;
            }
        }
    }
    //OggLoader
    {
        OggVorbis_File sOggVorbisFile;
        vorbis_info *psVorbisInfo;
        FILE *pOggVorbisFile;

        pOggVorbisFile = fopen(FileName, "rb");
        if (!pOggVorbisFile)return false;
        if (ov_open(pOggVorbisFile, &sOggVorbisFile, NULL, 0) == 0)
        {
            psVorbisInfo = ov_info(&sOggVorbisFile, -1);
            if (psVorbisInfo)
            {
                iFrequency = psVorbisInfo->rate;
                switch (psVorbisInfo->channels)
                {
                case 1:
                    eBufferFormat = AL_FORMAT_MONO16;
                    // Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
                    iDataSize = iFrequency >> 1;
                    // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
                    iDataSize -= (iDataSize % 2);
                    break;
                case 2:
                    eBufferFormat = AL_FORMAT_STEREO16;
                    // Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
                    iDataSize = iFrequency;
                    // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
                    iDataSize -= (iDataSize % 4);
                    break;
                case 4:
                    eBufferFormat = alGetEnumValue("AL_FORMAT_QUAD16");
                    // Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
                    iDataSize = iFrequency * 2;
                    // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
                    iDataSize -= (iDataSize % 8);
                    break;
                case 6:
                    eBufferFormat = alGetEnumValue("AL_FORMAT_51CHN16");
                    // Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
                    iDataSize = iFrequency * 3;
                    // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
                    iDataSize -= (iDataSize % 12);
                    break;
                }

                if (eBufferFormat != 0)
                {
                    // Allocate a buffer to be used to store decoded data for all Buffers
                    pData = (char*)malloc(iDataSize);
                    if (!pData)ov_clear(&sOggVorbisFile);
                    else
                    {
                        vector<char> data;
                        unsigned long ulBytesWritten;
                        back_insert_iterator<vector<char> > iter(data);
                        do
                        {
                            ulBytesWritten = DecodeOggVorbis(&sOggVorbisFile, pData, iDataSize, psVorbisInfo->channels);
                            // for (unsigned long i = 0; i < ulBytesWritten; i++)data.push_back(pData[i]);
                            copy(pData, pData + ulBytesWritten, iter);
                        }
                        while (ulBytesWritten);
                        alBufferData(*uiBuffer, eBufferFormat, data.data(), data.size(), iFrequency);
                        free(pData);
                        data.clear();
                        ov_clear(&sOggVorbisFile);
                        fclose(pOggVorbisFile);
                        return true;
                    }
                }
            }
        }
    }
//FlacLoader
    {
        /*
        FLAC__StreamDecoder *decoder= FLAC__stream_decoder_new();
        void* data;
        auto turn=FLAC__stream_decoder_init_file(decoder, FileName,WriteCallback,nullptr,ErrorCallback,data );
        if (turn == FLAC__StreamDecoderInitStatus::FLAC__STREAM_DECODER_INIT_STATUS_OK) {

            return true;
        }
        FLAC__stream_decoder_delete(decoder);*/
    }
    return false;
}
ALuint ALDevice::play(ALuint uiBuffer, bool loop, float gain,  ALfloat sourcePos[], ALfloat sourceVel[])
{
    ALuint uiSource;
    alGenSources(1, &uiSource);
    alSourcei(uiSource, AL_BUFFER, uiBuffer);
    alSourcei(uiSource, AL_LOOPING, loop);  // 设置音频播放是否为循环播放，AL_FALSE是不循环
    alSourcef(uiSource, AL_GAIN, gain);  //设置音量大小，1.0f表示最大音量。openAL动态调节音量大小就用这个方法
    //为省事，直接统一设置衰减因子
    alSourcef(uiSource, AL_ROLLOFF_FACTOR, 5.0);
    //alSourcef(uiSource, AL_MAX_DISTANCE, 30.0);
    alSourcef(uiSource, AL_REFERENCE_DISTANCE, 1.0);
    //设置位置
    updateSource(uiSource,sourcePos,sourceVel);
    //开启EFX
#ifdef NEWORLD_TARGET_WINDOWS
    EFX::set(uiSource);
#endif
    alSourcePlay(uiSource);
    return uiSource;
}
void ALDevice::stop(ALuint Source)
{
    alSourceStop(Source);
    alDeleteSources(1, &Source);
}
void ALDevice::unload(ALuint uiBuffer)
{
    alDeleteBuffers(1, &uiBuffer);
}

void ALDevice::clean()
{
    alcMakeContextCurrent(0);
    alcDestroyContext(Context);
    alcCloseDevice(Device);
}

