#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <system_error>

#include "../include/utils/utils.h"

namespace app
{
    namespace sal
    {
        /*
         *  return number of CPU cores
         */
        size_t hardware_concurrency()
        {
            size_t coresNumber{};

#if defined(WIN32) || defined(WIN64)
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    coresNumber = static_cast<size_t>(systemInfo.dwNumberOfProcessors);
#else
     coresNumber = static_cast<size_t>(sysconf(_SC_NPROCESSORS_ONLN));
#endif
            return coresNumber;
        }

        /*
         *  yield current thread
         */
        void thread_yield()
        {
#if defined(WIN32) || defined(WIN64)
            (void)SwitchToThread();
#else
            sched_yield();
#endif
        }
    }

    namespace utils
    {
        void throwSystemExceptionIfNeeded(int ec, const std::string& message)
        {
            if (ec)
            {
                throw std::system_error(std::error_code(ec, std::system_category()), message);
            }
        }

        void throwSystemException(const std::string& message)
        {
            throw std::system_error(std::error_code(errno, std::system_category()), message);
        }

        namespace
        {
            bool hasExtension(const std::string& filePath, const std::string& extension)
            {
                // Obviously, better to use std::filesystem in C++ 17 or boost::filesystem
                const char* const Separator = ".";
                auto it = filePath.find_last_of(Separator);
                if (it == std::string::npos)
                {
                    return false;
                }

                return filePath.compare(it + 1, extension.size(), extension) == 0;
            }
        }

        bool isWavFile(const std::string& filePath)
        {
           const char* const WavExtension = "wav";
           const char* const CapitalWavExtension = "WAV";
           return hasExtension(filePath, WavExtension) || hasExtension(filePath, CapitalWavExtension);
        }
    }

}