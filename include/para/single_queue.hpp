#pragma once

#include <concepts>
#include <cstdint>
#include <future>
#include <thread>
#include <vector>

#include "para/export.hpp"
#include "para/task_queue.hpp"

namespace para::single {

using std::invocable;

class PARA_EXPORT task_system_sq {
    std::vector<std::jthread> threads;
    task_queue queue;

    auto run() -> void;

public:
    task_system_sq(const task_system_sq&) = delete;
    task_system_sq(task_system_sq&&)      = delete;
    auto operator=(const task_system_sq&) = delete;
    auto operator=(task_system_sq&&)      = delete;

    task_system_sq();
    ~task_system_sq();

    template<typename F, typename... Args, typename R = std::invoke_result<F, Args...>::type>
        requires invocable<F, Args...>
    auto enqueue(F&& func, Args&&... args) -> std::future<R> {
        auto task =
            std::make_shared<std::packaged_task<R()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        auto res = task->get_future();

        auto call = [task] {
            (*task)();
        };

        queue.push(call);

        return res;
    }
};

} // namespace para::single