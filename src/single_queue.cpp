#include "para/single_queue.hpp"

namespace para::single {

task_system_sq::task_system_sq() : queue(task_queue{}) {
    auto thread_count = std::jthread::hardware_concurrency();
    threads           = std::vector<std::jthread>{};
    for(size_t thr = 0; thr < thread_count; ++thr) {
        threads.emplace_back([&] { run(); });
    }
}

task_system_sq::~task_system_sq() {
    queue.close();
    for(auto& thr: threads) {
        thr.join();
    }
}

auto task_system_sq::run() -> void {
    while(true) {
        if(auto func = queue.pop()) {
            (*func)();
        } else {
            break;
        }
    }
}

} // namespace para::single