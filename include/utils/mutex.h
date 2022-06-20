#pragma once
#include <pthread.h>

namespace app
{
    namespace sal
    {
        /*
         *  Abstraction layer which allows to hide platform specific locking primitives
         */
        class Mutex final
        {
        public:
            Mutex() = default;

            Mutex(const Mutex&) = delete;
            Mutex& operator=(const Mutex&) = delete;
            ~Mutex() = default;

            void lock();
            void unlock();

        private:
            pthread_mutex_t _native_handler = PTHREAD_MUTEX_INITIALIZER;
        };

        class MutexGuard final
        {
        public:
            MutexGuard(const MutexGuard&) = delete;
            MutexGuard& operator=(const MutexGuard&) = delete;
            explicit MutexGuard(Mutex& mutex);
            ~MutexGuard();

        private:
            Mutex& _mutex;
        };
    }
}
