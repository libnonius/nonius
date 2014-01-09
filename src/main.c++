#include <nonius/environment.h++>
#include <nonius/run.h++>
#include <iostream>
#include <chrono>
#include <string>
#include <utility>

struct mean {
    template <typename It>
    std::pair<nonius::detail::IteratorValue<It>, nonius::detail::IteratorValue<It>> operator()(It first, It last) {
        using value = nonius::detail::IteratorValue<It>;
        auto count = last - first;
        auto mean = nonius::stats::mean(first, last);
        double stddev_sum = std::accumulate(first, last, 0.0, [mean](double a, value b) {
                    double diff = (b - mean).count();
                    return a + diff*diff;
                });
        auto stddev = decltype(mean)(std::sqrt(stddev_sum / count));
        return { mean, stddev };
    }
};

int main() {
    using fns = std::chrono::duration<double, std::nano>;

    nonius::configuration cfg;

    std::cout << "Measuring environment...\n";
    auto env = nonius::measure_environment<nonius::default_clock>();
    std::cout << "* Clock resolution: " << fns(env.clock_resolution).count() << " ns\n";
    std::cout << "* Cost of clock call: " << fns(env.clock_cost).count() << " ns\n";

    std::cout << "Obtaining " << cfg.samples << " samples...\n";
    auto sample = nonius::run_benchmark(cfg, env, nonius::make_benchmark("to_string", []{ return std::to_string(42); }));

    auto mean = nonius::stats::mean(sample.begin(), sample.end());
    auto o = nonius::stats::classify_outliers(sample.begin(), sample.end());

    std::cout << "* Mean: " << fns(mean).count() << " ns\n";
    std::cout << "* Outliers:\n";
    std::cout << "\t* Low severe: " << o.low_severe << '\n';
    std::cout << "\t* Low mild: " << o.low_mild << '\n';
    std::cout << "\t* High mild: " << o.high_mild << '\n';
    std::cout << "\t* High severe: " << o.high_severe << '\n';

    std::cout << "Resampling " << cfg.resamples << " times...\n";

    std::mt19937 rng;
    auto resample = nonius::stats::resample(rng, cfg.resamples, sample.begin(), sample.end(), ::mean{});
    for(auto r : resample) {
        std::cout << fns(r.first).count() << " ns\t" << fns(r.second).count() << " ns\t\t";
    }
    std::cout << '\n';
}

