#pragma once

#include <cstdint>

namespace app
{
namespace audio
{
    // I've developed it based on this resource:
    // http://soundfile.sapp.org/doc/WaveFormat/

    // Some bytes in wav files are little-endian
    // and some are big-endian. In this test-task the endianness isn't handled

#pragma pack(push, 1)
    struct WavHeader final
    {
        uint32_t chunkID;
        uint32_t chunkSize;
        uint32_t format;
        uint32_t subChunk1ID;
        uint32_t subChunk1Size;
        uint16_t audioFormat;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
        uint32_t subChunk2ID;
        uint32_t subChunk2Size;
    };
#pragma pack(pop)

    constexpr uint16_t PcmFormat = 1;
    constexpr uint32_t ChunkID = 0x46464952; // riff header value in LE form (0x52494646 in BE form) 'RIFF'
    constexpr uint32_t Format = 0x45564157; // wave format value in LE form (0x57415645 in BE form) 'WAVE'
    constexpr uint32_t SubChunk1ID = 0x20746D66; // fmt header value in LE form (0x666d7420 in BE form) 'fmt '
    constexpr uint32_t SubChunk2ID = 0x61746164; // data header value in LE form (0x64617461 in BE form) 'data'
}
}