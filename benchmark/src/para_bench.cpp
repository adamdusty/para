#include <nanobench.h>
#include <para/multi_queue.hpp>
#include <para/single_queue.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using para::multi::task_system_mq;
using para::single::task_system_sq;
using std::array;

template<size_t N, size_t M>
struct matrix {
    array<array<float, M>, N> data = {};

    constexpr auto at(size_t row, size_t col) -> float& { return data.at(row).at(col); }
    constexpr auto at(size_t row, size_t col) const -> float { return data.at(row).at(col); }

    auto to_string() const -> std::string {
        auto str = std::stringstream{};

        for(auto& row: data) {
            for(auto& col: row) {
                str << col << ", ";
            }
            str << "\n";
        }

        return str.str();
    }

    auto randomize() -> void {
        auto seed   = std::random_device{};
        auto engine = std::mt19937{seed()};
        auto dist   = std::uniform_real_distribution<float>{0, 10};

        for(auto& row: data) {
            for(auto& col: row) {
                col = dist(engine);
            }
        }
    }
};

template<size_t N, size_t M>
auto matrix_dot(const matrix<N, M>& lhs, const matrix<M, N>& rhs) -> matrix<N, N> {
    auto result = matrix<N, N>{};

    for(auto i = 0; i < N; ++i) {
        for(auto j = 0; j < N; ++j) {
            for(auto k = 0; k < M; ++k) {
                result.at(i, j) += lhs.at(i, k) * rhs.at(k, j);
            }
        }
    }

    return result;
};

const size_t matrix_size{100};

auto main(int argc, char** argv) -> int {
    auto num_iters = 1;
    if(argc == 2) {
        num_iters = std::stoi(argv[1]);
    }

    auto mat1 = matrix<matrix_size, matrix_size>{};
    auto mat2 = matrix<matrix_size, matrix_size>{};

    auto seq = [&] {
        for(size_t i = 0; i < num_iters; ++i) {
            mat1.randomize();
            mat2.randomize();
            matrix_dot(mat1, mat2);
        }
    };

    auto par_sq = [&] {
        auto ts = task_system_sq{};
        std::vector<std::future<matrix<matrix_size, matrix_size>>> res{};

        for(size_t i = 0; i < num_iters; ++i) {
            auto work = [&]() {
                mat1.randomize();
                mat2.randomize();
                return matrix_dot(mat1, mat2);
            };

            res.emplace_back(ts.enqueue(work));
        }

        auto mats = std::vector<matrix<matrix_size, matrix_size>>{};
        for(auto& fut: res) {
            mats.emplace_back(fut.get());
        }
    };

    auto par_mq = [&](std::uint32_t spin) {
        auto ts = task_system_mq{spin};
        std::vector<std::future<matrix<matrix_size, matrix_size>>> res{};

        for(size_t i = 0; i < num_iters; ++i) {
            auto work = [&]() {
                mat1.randomize();
                mat2.randomize();
                return matrix_dot(mat1, mat2);
            };

            res.emplace_back(ts.enqueue(work));
        }

        auto mats = std::vector<matrix<matrix_size, matrix_size>>{};
        for(auto& fut: res) {
            mats.emplace_back(fut.get());
        }
    };

    auto mq_100 = [par_mq] {
        return par_mq(100);
    };
    auto mq_1000 = [par_mq] {
        return par_mq(1000);
    };
    auto mq_10000 = [par_mq] {
        return par_mq(10000);
    };
    auto mq_100000 = [par_mq] {
        return par_mq(100000);
    };
    auto mq_1000000 = [par_mq] {
        return par_mq(1000000);
    };

    // ankerl::nanobench::Bench().epochs(1).run("seq", seq);
    // ankerl::nanobench::Bench().epochs(1).run("par_sq", par_sq);
    ankerl::nanobench::Bench().epochs(1).run("par_mq", mq_100);
    ankerl::nanobench::Bench().epochs(1).run("par_mq", mq_1000);
    ankerl::nanobench::Bench().epochs(1).run("par_mq", mq_10000);
    ankerl::nanobench::Bench().epochs(1).run("par_mq", mq_100000);
    ankerl::nanobench::Bench().epochs(1).run("par_mq", mq_1000000);

    return 0;
}