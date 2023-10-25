#include <para/multi_queue.hpp>
#include <para/single_queue.hpp>
#include <snitch/snitch.hpp>

#include <chrono>
#include <concepts>
#include <iostream>

using para::multi::task_system_mq;
using para::single::task_system_sq;

TEST_CASE("Single queue") {
    task_system_sq ts{};

    const auto input = 41;

    auto fut = ts.enqueue([](int num) { return num + 1; }, input);

    auto expected = input + 1;
    auto actual   = fut.get();

    CHECK(actual == expected);
}

TEST_CASE("Multi queue") {
    task_system_mq ts{};

    const auto input = 41;

    auto fut = ts.enqueue([](auto i) { return i + 1; }, input);

    auto expected = input + 1;
    auto actual   = fut.get();

    CHECK(actual == expected);
}
