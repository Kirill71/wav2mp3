#pragma once

#include <string>
#include <fstream>
#include <cassert>
#include <vector>
#include <algorithm>

#include "../conversion/audio_storage.h"
#include "../lame/lame.h"
#include "../utils/utils.h"

namespace app 
{

namespace exporting
{
    class Mp3Writer final
    {
        using ByteBuffer = std::vector<uint8_t>;
        static constexpr const size_t BufferSize = 8192;
        static constexpr const char* const Mp3Extension = ".mp3";

        static constexpr size_t Mp3QualityLevel = 5;

        using LamePtr = std::unique_ptr<lame_global_flags, decltype(&lame_close)>;

        void configureLame(const audio::AudioSettings& settings);

        int resampleIfNeeded(const audio::AudioSettings& settings, const ByteBuffer& wavBuffer, ByteBuffer& mp3Buffer);

    public:
        Mp3Writer();

        void write(audio::AudioStoragePtr audioStorage);
    private:
        LamePtr _lame;
    };
}
}
