#include "export/mp3_writer.h"

namespace app
{
    namespace exporting
    {
        Mp3Writer::Mp3Writer()
        :_lame(lame_init(), lame_close)
        {
        }

        void Mp3Writer::configureLame(const audio::AudioSettings &settings)
        {
            const auto numChannels = settings.numChannels;
            if (numChannels != 1 && numChannels != 2)
            {
                app::utils::throwSystemException("Lame: Unexpected number of channels - "
                                                 + std::to_string(numChannels));
            }

            const auto bitsPerSample = settings.bitsPerSample;
            if (bitsPerSample != 8 && bitsPerSample != 16)
            {
                app::utils::throwSystemException("Lame: Unexpected bits per sample - "
                                                 + std::to_string(bitsPerSample));
            }

            if (lame_set_num_channels(_lame.get(), static_cast<int>(numChannels)))
            {
                app::utils::throwSystemException("Lame: Unable to set number of channels.");
            }

            const bool isMono = numChannels == 1;
            if(lame_set_mode(_lame.get(), isMono ? MONO : STEREO))
            {
                app::utils::throwSystemException("Lame: Unable to to set mode.");
            }
            if (lame_set_in_samplerate(_lame.get(), static_cast<int>(settings.sampleRate)))
            {
                app::utils::throwSystemException("Lame: Unable to set sample rate.");
            }

            if (lame_set_quality(_lame.get(), Mp3QualityLevel))
            {
                app::utils::throwSystemException("Lame: Unable to set mp3 quality.");
            }

            if (lame_init_params(_lame.get()))
            {
                app::utils::throwSystemException("Lame: Unable to configure.");
            }
        }

        int Mp3Writer::resampleIfNeeded(const audio::AudioSettings& settings, const ByteBuffer& wavBuffer, ByteBuffer& mp3Buffer)
        {
            int16_t* tempBuffer = nullptr;
            const int numSamples = static_cast<int>(BufferSize / settings.blockAlign);
            const auto numChannels = settings.numChannels;
            const auto bitsPerSample = settings.bitsPerSample;
            const auto ResampledBufferSize = numSamples * numChannels;
            auto resampledBuffer = std::make_unique<int16_t[]>(ResampledBufferSize);
            constexpr size_t BitMask = 0x80;

            if (bitsPerSample == 8)
            {
                tempBuffer = resampledBuffer.get();
                for(int i = 0; i < ResampledBufferSize; ++i)
                {
                    //I've found this conversion here:
                    // https://stackoverflow.com/questions/24449957/converting-a-8-bit-pcm-to-16-bit-pcm

                    tempBuffer[i] = static_cast<int16_t>((wavBuffer[i] - BitMask) << 8);
                }

            }
            else if(bitsPerSample == 16)
            {
                tempBuffer = const_cast<int16_t*>(reinterpret_cast<const int16_t*>(wavBuffer.data()));
            }
            else
            {
                // This impl doesn't support 24 bits 32 bits for now.
                app::utils::throwSystemException("Unsupported bits per sample");
            }

            int numberWrittenBytes = 0;
            if (numChannels == 1)
            {
                numberWrittenBytes = lame_encode_buffer(_lame.get(), tempBuffer, nullptr, numSamples, mp3Buffer.data(), static_cast<int>(mp3Buffer.size()));
            }
            else if (numChannels == 2)
            {
                numberWrittenBytes = lame_encode_buffer_interleaved(_lame.get(), tempBuffer, numSamples, mp3Buffer.data(), static_cast<int>(mp3Buffer.size()));
            }
            else
            {
                app::utils::throwSystemException("Unsupported number of channels");
            }
            return numberWrittenBytes;
        }

        void Mp3Writer::write(audio::AudioStoragePtr audioStorage)
        {
            if (!audioStorage) {
                app::utils::throwSystemException("Audio storage is empty.");
            }

            if (!_lame) {
                app::utils::throwSystemException("Unable to initialize Lame lib.");
            }

            const auto &filePath = audioStorage->getFilePath();
            assert(!filePath.empty() && "File path is empty.");
            const auto &newFilePath = filePath + Mp3Extension;

            std::ofstream destinationStream(newFilePath, std::ios::binary);

            if (!destinationStream.is_open()) {
                app::utils::throwSystemException("Unable to write: " + newFilePath);
            }

            const auto &audioSettings = audioStorage->getAudioSettings();
            configureLame(audioSettings);

            std::vector<uint8_t> wavBuffer(BufferSize * 2);
            std::vector<uint8_t> mp3Buffer(BufferSize);

            std::streamsize read{};
            int write{};
            auto sourceStream = audioStorage->getSourceStream();
            if (sourceStream && !sourceStream->is_open()) {
                app::utils::throwSystemException("Unable to read: " + filePath);
            }
            do {
                sourceStream->read(reinterpret_cast<char*>(wavBuffer.data()), BufferSize);
                read = sourceStream->gcount();
                if (read == 0)
                {
                    write = lame_encode_flush(_lame.get(), mp3Buffer.data(), BufferSize);
                }
                else
                {
                    write = resampleIfNeeded(audioSettings, wavBuffer, mp3Buffer);
                }
                destinationStream.write(reinterpret_cast<char*>(mp3Buffer.data()), write);
            }while(read != 0);

        }

    }
}