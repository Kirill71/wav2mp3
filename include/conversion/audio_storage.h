#pragma once

#include <cstdint>
#include <utility>
#include <memory>
#include <string>
#include <fstream>

namespace app
{
namespace audio
{
    /*
     * The internal audio storage without any data format(like .wav, mp3 etc)
     */

    struct AudioSettings
    {
        uint16_t audioFormat;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
    };

    class AudioStorage final
    {
        using SourceStreamPtr = std::shared_ptr<std::ifstream>;
    public:
        AudioStorage(std::string filePath, AudioSettings settings, SourceStreamPtr sourceStream)
        :_filePath(std::move(filePath))
        , _settings(std::move(settings))
        , _sourceStream(std::move(sourceStream))
        {

        }

        const AudioSettings& getAudioSettings() const
        {
            return _settings;
        }

        SourceStreamPtr getSourceStream() const
        {
            return _sourceStream;
        }

        const std::string& getFilePath() const
        {
            return _filePath;
        }

    private:
        std::string _filePath;
        AudioSettings _settings;
        SourceStreamPtr _sourceStream;

    };

    using AudioStoragePtr = std::unique_ptr<AudioStorage>;
}
}
