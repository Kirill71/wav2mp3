#pragma once

#include <cstddef>
#include <string>

namespace app
{
    /*
     * System abstraction layer
     */
    namespace sal
    {
        /*
         *  return number of CPU cores.
         */

        size_t hardware_concurrency();

        /*
         *  yield the thread.
         */

        void thread_yield();

    }

    namespace utils
    {
        /*
         * helper function to avoid code duplication.
         * if ec > 0 it will throw std::system exception.
         * ec - error code
         * message - string error message
         */

        void throwSystemExceptionIfNeeded(int ec, const std::string& message);

        void throwSystemException(const std::string& message);

        /*
         * Check that is a .wav file
         */

        bool isWavFile(const std::string& filePath);
    }
}