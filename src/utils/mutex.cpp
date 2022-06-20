#include <cassert>

#include "../include/utils/mutex.h"
#include "../include/utils/utils.h"

namespace app
{
    namespace sal
    {
        void Mutex::lock()
        {
          int ec = pthread_mutex_lock(&_native_handler);
          utils::throwSystemExceptionIfNeeded(ec, "Unable to lock mutex");
        }

        void Mutex::unlock()
        {
            int ec = pthread_mutex_unlock(&_native_handler);
            (void)ec;
            assert(ec == 0 && "Unable to unlock mutex");
        }

        MutexGuard::MutexGuard(Mutex &mutex)
        :_mutex(mutex)
        {
            _mutex.lock();
        }

        MutexGuard::~MutexGuard()
        {
            _mutex.unlock();
        }
    }
}