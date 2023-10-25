#pragma once

#include <condition_variable>
#include <deque>
#include <format>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>

namespace para {

using std::optional;
using std::try_to_lock;
using task   = std::function<void()>;
using lock_t = std::unique_lock<std::mutex>;

class task_queue {
    bool done = false;
    std::deque<task> tasks;
    std::mutex mutex;
    std::condition_variable ready;

public:
    auto close() -> void {
        {
            auto lock = lock_t{mutex};
            done      = true;
        }
        ready.notify_all();
    }

    auto push(task&& t) -> void {
        {
            auto lock = lock_t{mutex};
            tasks.emplace_back(t);
        }

        ready.notify_one();
    }

    auto pop() -> optional<task> {
        auto lock = lock_t{mutex};
        while(tasks.empty() && !done) {
            ready.wait(lock);
        }

        if(tasks.empty()) {
            return std::nullopt;
        }

        auto t = std::move(tasks.front());
        tasks.pop_front();
        return t;
    }

    auto try_push(task&& t) -> bool {
        {
            auto lock = lock_t{mutex, try_to_lock};
            if(!lock) {
                return false;
            }
            tasks.emplace_back(t);
        }

        ready.notify_one();
        return true;
    }

    auto try_pop() -> optional<task> {
        auto lock = lock_t{mutex, try_to_lock};
        if(done || !lock || tasks.empty()) {
            return std::nullopt;
        }

        auto t = std::move(tasks.front());
        tasks.pop_front();
        return t;
    }
};

} // namespace para