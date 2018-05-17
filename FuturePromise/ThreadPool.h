#pragma once

#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <assert.h>

class ThreadPool {
public:
    ThreadPool(int size):size(size), terminated(false) {}
    ThreadPool(ThreadPool&& other) = delete;
    ThreadPool(const ThreadPool& other) = delete;

    ~ThreadPool();

    void AddTask(std::function<void()> task);
    void Terminate();

private:
    std::vector<std::thread> threads;
    std::vector<std::function<void()>> tasks;
    std::vector<bool> busy;
    std::mutex critical;
    std::condition_variable cv;
    int size;
    bool terminated;

    void initialize();
};

