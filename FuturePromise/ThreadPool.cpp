#include <ThreadPool.h>


ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> guard(critical);
        terminated = true;
        cv.notify_all();
    }
    for(int i = 0; i < threads.size(); ++i) {
        try {
            threads[i].join();
        } catch(...) {
            std::terminate();
        }
    }
}

void ThreadPool::AddTask(std::function<void ()> task)
{
    std::lock_guard<std::mutex> guard(critical);
    if(threads.size() == 0) {
        initialize();
    }
    tasks.push_back(task);
    cv.notify_all();
}

void ThreadPool::Terminate()
{
    std::unique_lock<std::mutex> lk(this->critical);
    cv.wait(lk, [this]() {
        if (tasks.size() != 0) {
            return false;
        }

        for(int i = 0; i < busy.size(); ++i) {
            if(busy[i]) {
                return false;
            }
        }
        terminated = true;
        return true;
    });
}

void ThreadPool::initialize() {
    assert(threads.size() == 0);
    std::function<void(int id)> workingFunction = [this](int id) {
        std::unique_lock<std::mutex> lk(this->critical);
        while(!terminated) {
            //lk.unlock();
            cv.wait(lk, [this]() { return this->tasks.size() > 0 || terminated; });
            if(terminated) {
                lk.unlock();
                return;
            }
            std::function <void()> task = tasks.back();
            tasks.pop_back();
            busy[id] = true;
            lk.unlock();
            cv.notify_all();
            task();
            lk.lock();
            busy[id] = false;
            lk.unlock();
            cv.notify_all();
        }
    };

    for(int i = 0; i < size; ++i) {
        threads.push_back(std::thread(workingFunction, i));
        busy.push_back(false);
    }
}
