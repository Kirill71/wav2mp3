#pragma once

#include <vector>
#include <string>
#include <memory>

namespace app
{
    namespace sal
    {
        class IWavFilePathResolver
        {
        protected:
#ifdef WIN32
            const char* const Separator = "\\";
#else
            const char* const Separator = "/";
#endif
        public:
            virtual std::vector<std::string> getWavFiles(const std::string&) = 0;
            virtual ~IWavFilePathResolver() = default;
        };

        class PosixWavFilePathResolver : public IWavFilePathResolver
        {
            std::vector<std::string> getWavFiles(const std::string& dirPath) override;
        };

        class WindowsWavFilePathResolver : public IWavFilePathResolver
        {
            std::vector<std::string> getWavFiles(const std::string& dirPath) override;
        };

        class WavFilePathResolver final
        {
            using IWavFilePathResolverPtr = std::unique_ptr<IWavFilePathResolver>;
        public:
            explicit WavFilePathResolver(std::unique_ptr<IWavFilePathResolver> impl)
            : _impl(std::move(impl))
            {
            }

            auto getWavFiles(const std::string& filePath)
            {
                return _impl->getWavFiles(filePath);
            }
        private:
            IWavFilePathResolverPtr _impl;
        };
    }
}
