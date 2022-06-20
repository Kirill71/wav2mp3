#include <string>
#include <fstream>
#include <memory>
#include <cassert>

#include "../include/conversion/wav_header.h"
#include "../include/import/wav_reader.h"
#include "../include/utils/utils.h"

namespace app
{

namespace importing {
    namespace {
        void validate(const app::audio::WavHeader &header) {
            using namespace app;
            if (header.chunkID != audio::ChunkID) {
                throw std::runtime_error("RIFF Header is absent");
            }
            else if (header.format != audio::Format) {
                throw std::runtime_error("Is not a WAV file");
            }
            else if (header.subChunk1ID != audio::SubChunk1ID) {
                throw std::runtime_error("Format Header is absent");
            }
            else if (header.subChunk2ID != audio::SubChunk2ID) {
                throw std::runtime_error("Data chunk is absent");
            }
            else if (header.audioFormat != audio::PcmFormat) {
                throw std::runtime_error(
                        "File is not in PCM format, current format is " + std::to_string(header.audioFormat));
            }
        }

        audio::AudioSettings fillAudioSettings(const audio::WavHeader &header) {
            audio::AudioSettings settings{};

            settings.audioFormat = header.audioFormat;
            settings.numChannels = header.numChannels;
            settings.sampleRate = header.sampleRate;
            settings.byteRate = header.byteRate;
            settings.blockAlign = header.blockAlign;
            settings.bitsPerSample = header.bitsPerSample;

            return settings;
        }
    }

    app::audio::AudioStoragePtr WavReader::read(const std::string &filePath) {
        assert(!filePath.empty() && "File path is empty.");

        auto sourceStream = std::make_shared<std::ifstream>(filePath, std::ios::binary);
        if (!sourceStream || !sourceStream->is_open()) {
            app::utils::throwSystemException("Unable to open file: " + filePath);
        }

        audio::WavHeader header{};
        sourceStream->read(reinterpret_cast<char *>(&header), sizeof(header));
        if (!sourceStream || sourceStream->gcount() != sizeof(header)) {
            app::utils::throwSystemException("File header is broken: " + filePath);
        }

        validate(header);

        auto settings = fillAudioSettings(header);

        constexpr size_t ExtensionSize = 4; // .wav
        auto noExtensionFilePath = filePath.substr(0, filePath.size() - ExtensionSize);
        return std::make_unique<audio::AudioStorage>(noExtensionFilePath, std::move(settings), std::move(sourceStream));
    }
}

}
