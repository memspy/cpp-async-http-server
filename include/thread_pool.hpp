#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t thread_count) {
        workers_.reserve(thread_count);
        for (size_t i = 0; i < thread_count; ++i) {
            workers_.emplace_back([this] { workerLoop(); });
        }
    }
    template<typename F>
    void submit(F&& task) {
        {
            std::unique_lock lock(mutex_);
            tasks_.push(std::forward<F>(task));
        }
        cv_.notify_one();
    }

    ~ThreadPool() {
        stop_.store(true);
        cv_.notify_all();
        for (auto& t : workers_) t.join();
    }

private:
    void workerLoop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock lock(mutex_);
                cv_.wait(lock, [this] {
                    return stop_ || !tasks_.empty();
                });

                if (stop_ && tasks_.empty()) return;

                task = std::move(tasks_.front());
                tasks_.pop();
            } 

            task(); 
        }
    }

    std::vector<std::thread>             workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex                            mutex_;
    std::condition_variable               cv_;
    std::atomic<bool>                     stop_{false};
};