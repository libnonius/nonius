#include <nonius/environment.h++>
#include <nonius/run.h++>
#include <iostream>
#include <chrono>
#include <string>

int main() {
    using fns = std::chrono::duration<double, std::nano>;

    nonius::configuration cfg;

    auto env = nonius::measure_environment<nonius::default_clock>();

    auto sample = nonius::run_benchmark(cfg, env, nonius::make_benchmark("to_string", []{ return std::to_string(42); }));

    auto mean = nonius::analyse_mean<nonius::default_clock>(sample.begin(), sample.end());
    auto o = nonius::classify_outliers(sample.begin(), sample.end());

    std::cout << "Clock resolution: " << fns(env.clock_resolution).count() << " ns\n";
    std::cout << "Cost of clock call: " << fns(env.clock_cost).count() << " ns\n";
    std::cout << "Mean: " << fns(mean).count() << " ns\n";
    std::cout << "Outliers:\n";
    std::cout << "\tLow severe: " << o.low_severe << '\n';
    std::cout << "\tLow mild: " << o.low_mild << '\n';
    std::cout << "\tHigh mild: " << o.high_mild << '\n';
    std::cout << "\tHigh severe: " << o.high_severe << '\n';
}

