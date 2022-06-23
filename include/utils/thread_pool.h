#pragma once
#include <vector>
#include <queue>
#include <functional>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <atomic>

#include "mutex.h"
#include "utils.h"

namespace app {
    namespace utils {
        template<typename Type>
        class ThreadSafeQueue final {
        public:
            ThreadSafeQueue() = default;

            void push(Type &&type) {
                sal::MutexGuard mutexGuard(_mutex);
                _queue.push(std::forward<Type>(type));
            }

            bool tryPop(Type &value) {
                sal::MutexGuard mutexGuard(_mutex);
                if (_queue.empty()) {
                    return false;
                }
                value = std::move(_queue.front());
                _queue.pop();
                return true;
            }

            bool empty() const {
                sal::MutexGuard mutexGuard(_mutex);
                return _queue.empty();
            }

        private:
            mutable sal::Mutex _mutex;
            std::queue<Type> _queue;
        };

        class ThreadPool final {
            template<typename TaskType>
            using Task = std::function<TaskType>;
            using DefaultTask = Task<void()>;
            using ThreadImpl = pthread_t;
            template<typename Type>
            using ThreadPoolContainer = std::vector<Type>;

        private:
            class ThreadJoiner final {
            public:
                explicit ThreadJoiner(ThreadPoolContainer<ThreadImpl> &threads)
                        : _threads(threads) {
                }

                ~ThreadJoiner() {
                    for (auto &thread: _threads)
                    {
                        int ec = pthread_join(thread, nullptr);
                        (void) ec;
                        assert(ec == 0 && "Unable to join thread");
                    }
                }

            private:
                ThreadPoolContainer<ThreadImpl>& _threads;
            };

            static void *ThreadFunc(void *args) {
                auto *ptrToThis = static_cast<ThreadPool *>(args);
                ptrToThis->worker();
                return nullptr;
            }

            void worker() {
                while (true) {

                    if (_shutdown && _tasksQueue.empty())
                    {
                        break;
                    }
                    DefaultTask task;
                    if (_tasksQueue.tryPop(task)) {
                        task();
                    } else {
                        sal::thread_yield();
                    }
                }
            }

        public:
            ThreadPool()
                    : _shutdown(false)
                    , _pool(std::max(size_t(1), sal::hardware_concurrency() - 1))
                    , _joiner(_pool) {
                try {
                    for (auto &thread: _pool) {
                        int ec = pthread_create(&thread, nullptr, &ThreadPool::ThreadFunc, this);
                        utils::throwSystemExceptionIfNeeded(ec, "Unable create thread");
                    }
                }
                catch (std::exception &ex) {
                    std::cerr << ex.what() << std::endl;
                    _shutdown = true;
                    throw;
                }
                catch (...) {
                    std::cerr << "Thread pool has generated an exception." << std::endl;
                    _shutdown = true;
                    throw;
                }
            }

        public:
            ThreadPool(const ThreadPool &) = delete;

            ThreadPool &operator=(const ThreadPool &) = delete;

            ~ThreadPool()
            {
                _shutdown = true;
            }

            template<typename Func, typename... Args>
            void addTask(Func &&func, Args &&...args) {
                using retType = decltype(func(args...));
                Task<retType()> task(std::bind(func, args...));
                _tasksQueue.push(DefaultTask(std::move(task)));
            }

        private:
            std::atomic_bool _shutdown;
            ThreadSafeQueue<DefaultTask> _tasksQueue;
            ThreadPoolContainer<ThreadImpl> _pool;
            ThreadJoiner _joiner;
        };
    }
}