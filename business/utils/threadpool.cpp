#include "threadpool.h"

#include <iostream>

ThreadPool::ThreadPool() {
    size_t threadsCount = std::thread::hardware_concurrency();
    std::cout << "std::thread::hardware_concurrency() -> " << threadsCount << std::endl;
    workers.reserve(threadsCount);
    for (size_t i = 0; i < threadsCount; ++i)
        workers.emplace_back([this] { workerLoop(); });
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard lock(mtx);
        stopping = true;
    }
    cv.notify_all();
    for (auto& w : workers)
        if (w.joinable()) w.join();
}

void ThreadPool::submit(std::function<void ()> job, std::function<void ()> onDone) {
    {
        std::lock_guard lock(mtx);
        queue.push({std::move(job), std::move(onDone)});
    }

    cv.notify_one();
}

void ThreadPool::workerLoop() {
    while (true) {
        Task task;
        {
            std::unique_lock lock(mtx);
            cv.wait(lock, [this] { return stopping || !queue.empty(); });
            if (stopping && queue.empty()) return;
            task = std::move(queue.front());
            queue.pop();
        }

        try {
            task.job();
        } catch (...) {
        }
        if (task.onDone) task.onDone();
    }
}
