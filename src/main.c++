#include <iostream>
#include <nonius/environment.h++>
#include <nonius/run.h++>
#include <chrono>
#include <sstream>

int main() {
    using fns = std::chrono::duration<double, std::nano>;
    auto env = nonius::measure_environment();
    auto sample = nonius::run_benchmark({ 100 }, env, [](int k) -> int {
                for(int i = 0; i < k; ++i) {
                    []() -> std::string {
                        std::stringstream ss;
                        ss << 42;
                        return ss.str();
                    }();
                }
                return 0;
            });
    std::cout << "Clock resolution: " << fns(env.clock_resolution).count() << " ns\n";
    std::cout << "Cost of clock call: " << fns(env.clock_cost).count() << " ns\n";
    auto mean = nonius::analyse_mean(sample.times.begin(), sample.times.end(), sample.times.size());
    std::cout << "Mean: " << fns(mean).count() << " ns\n";
}
