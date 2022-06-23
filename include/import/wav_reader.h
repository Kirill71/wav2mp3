#pragma once

#include "conversion/audio_storage.h"

namespace app 
{

namespace importing
{
    class WavReader final
    {

    public:
        audio::AudioStoragePtr read(const std::string& filePath);
    };
}

}