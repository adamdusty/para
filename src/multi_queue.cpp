#include "para/multi_queue.hpp"

namespace para::multi {

task_system_mq::task_system_mq() : queues(thread_count) {
    for(unsigned int thr = 0; thr < thread_count; ++thr) {
        threads.emplace_back([&, thr] { run(thr); });
    }

    if(threads.size() != queues.size()) {
        throw std::runtime_error{"Different amount of threads and queues"};
    }
}

task_system_mq::task_system_mq(std::uint32_t spin) : task_system_mq() {
    // NOLINTNEXTLINE
    spin_factor = spin;
}

task_system_mq::~task_system_mq() {
    for(auto& q: queues) {
        q.close();
    }

    for(auto& thr: threads) {
        thr.join();
    }
}

auto task_system_mq::run(unsigned int index) -> void {
    while(true) {

        task task;

        // Check each queue for work
        for(unsigned int n = 0; n != thread_count; ++n) {
            if(auto retrieved_task = queues.at((index + n) % thread_count).try_pop()) {
                task = *retrieved_task;
                break;
            }
        }

        if(!task) {
            if(auto retrieved_task = queues.at(index).pop()) {
                task = *retrieved_task;
            }
        }

        if(task) {
            task();
        } else {
            break;
        }
    }
}

} // namespace para::multi