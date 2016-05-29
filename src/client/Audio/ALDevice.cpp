#include <Ogg/ogg.h>
#include <Vorbis/vorbisfile.h>
#include <vector>
#include <iterator>
#include <cstring>
#include <memory>
#include <cstdlib>
#include "ALDevice.h"
#include "CWaves.h"
using namespace std;
ALDevice device;

unsigned long DecodeOggVorbis(OggVorbis_File *file, char *buffer, unsigned long size, unsigned long channels)
{
    int current_section;
    unsigned long BytesDone = 0;
    while (BytesDone < size)
    {
        long lDecodeSize = ov_read(file, buffer + BytesDone, size - BytesDone, 0, 2, 1, &current_section);
        if (lDecodeSize > 0)
            BytesDone += lDecodeSize;
        else
            break;
    }

    // Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
    // however 6-Channels files need to be re-ordered
    if (channels == 6)
    {
        short * samples = (short*)buffer;
        for (unsigned long ulSamples = 0; ulSamples < (size >> 1); ulSamples += 6)
        {
            // WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
            // OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
            swap(samples[ulSamples + 1], samples[ulSamples + 2]);
            swap(samples[ulSamples + 3], samples[ulSamples + 5]);
            swap(samples[ulSamples + 4], samples[ulSamples + 5]);
        }
    }

    return BytesDone;
}

ALuint ALDevice::load(const string& FileName)
{
    ALuint ret;
    alGenBuffers(1, &ret);
    ALchar *pData = nullptr;
    ALint iDataSize = 0, iFrequency = 0;
    ALenum eBufferFormat = 0;
    //WaveLoader
    {
        CWaves WaveLoader;
        WAVEID WaveID;
        if (SUCCEEDED(WaveLoader.LoadWaveFile(FileName.c_str(), &WaveID)))
        {
            if ((SUCCEEDED(WaveLoader.GetWaveSize(WaveID, (unsigned long*)&iDataSize))) &&
                    (SUCCEEDED(WaveLoader.GetWaveData(WaveID, (void**)&pData))) &&
                    (SUCCEEDED(WaveLoader.GetWaveFrequency(WaveID, (unsigned long*)&iFrequency))) &&
                    (SUCCEEDED(WaveLoader.GetWaveALBufferFormat(WaveID, &alGetEnumValue, (unsigned long*)&eBufferFormat))))
            {
                alBufferData(ret, eBufferFormat, pData, iDataSize, iFrequency);
                WaveLoader.DeleteWaveFile(WaveID);
                return ret;
            }
        }
    }
    //OggLoader
    {
        OggVorbis_File sOggVorbisFile;
        vorbis_info *psVorbisInfo;
        FILE *pOggVorbisFile;

        pOggVorbisFile = fopen(FileName.c_str(), "rb");
        if (!pOggVorbisFile) return INVALID_BUFFER;
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

                if (eBufferFormat)
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
                        } while (ulBytesWritten);
                        alBufferData(ret, eBufferFormat, data.data(), data.size(), iFrequency);
                        free(pData);
                        data.clear();
                        ov_clear(&sOggVorbisFile);
                        fclose(pOggVorbisFile);
                        return ret;
                    }
                }
            }
        }
    }
    return INVALID_BUFFER;
}
ALuint ALDevice::play(ALuint uiBuffer, bool loop, float gain,  ALfloat sourcePos[], ALfloat sourceVel[])
{
    ALuint uiSource;
    alGenSources(1, &uiSource);
    alSourcei(uiSource, AL_BUFFER, uiBuffer);
    alSourcei(uiSource, AL_LOOPING, loop);
    alSourcef(uiSource, AL_GAIN, gain);  //设置音量大小，1.0f表示最大音量。openAL动态调节音量大小就用这个方法
    //为省事，直接统一设置衰减因子
    alSourcef(uiSource, AL_ROLLOFF_FACTOR, 5.0);
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
