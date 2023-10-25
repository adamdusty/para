#pragma once

#include <concepts>
#include <cstdint>
#include <future>
#include <thread>

#include "para/export.hpp"
#include "para/task_queue.hpp"

namespace para::multi {

using std::invocable;

PARA_EXPORT class task_system_mq {
    unsigned int thread_count = std::jthread::hardware_concurrency();
    std::uint32_t spin_factor{1};
    std::atomic<unsigned int> idx;

    std::vector<std::jthread> threads;
    std::vector<task_queue> queues;

    auto run(unsigned int index) -> void;

public:
    task_system_mq(const task_system_mq&) = delete;
    task_system_mq(task_system_mq&&)      = delete;
    auto operator=(const task_system_mq&) = delete;
    auto operator=(task_system_mq&&)      = delete;

    task_system_mq();
    explicit task_system_mq(std::uint32_t spin);
    ~task_system_mq();

    template<typename F, typename... Args, typename R = std::invoke_result<F, Args...>::type>
        requires invocable<F, Args...>
    auto enqueue(F&& func, Args&&... args) -> std::future<R> {
        using std::forward;
        using std::packaged_task;

        auto task = std::make_shared<packaged_task<R()>>(std::bind(forward<F>(func), forward<Args>(args)...));
        auto res  = task->get_future();

        auto call = [task]() {
            (*task)();
        };

        for(unsigned int n = 0; n != thread_count * spin_factor; ++n) {
            if(queues.at((idx + n) % thread_count).try_push(call)) {
                ++idx;
                return res;
            }
        }

        queues.at(idx++ % thread_count).push(call);
        return res;
    }
};

} // namespace para::multi