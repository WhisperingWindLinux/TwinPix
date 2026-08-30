#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>


class ThreadPool {
public:
    explicit ThreadPool();

    ~ThreadPool();

    void submit(std::function<void()> job, std::function<void()> onDone);

private:
    struct Task {
        std::function<void()> job;
        std::function<void()> onDone;
    };

    void workerLoop();

    std::vector<std::thread> workers;
    std::queue<Task> queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool stopping = false;
};
#endif // THREADPOOL_H
