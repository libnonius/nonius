#include <nonius/environment.h++>
#include <nonius/run.h++>
#include <iostream>
#include <chrono>
#include <string>
#include <utility>

int main() {
    using orig = nonius::FloatDuration<nonius::default_clock>;
    using fns = std::chrono::duration<double, std::nano>;

    nonius::configuration cfg;

    std::cout << "Measuring environment...\n";
    auto env = nonius::measure_environment<nonius::default_clock>();
    std::cout << "* Clock resolution: " << fns(env.clock_resolution).count() << " ns\n";
    std::cout << "* Cost of clock call: " << fns(env.clock_cost).count() << " ns\n";

    std::cout << "Obtaining " << cfg.samples << " samples...\n";
    auto sample = nonius::run_benchmark(cfg, env, nonius::make_benchmark("to_string", []{ return std::to_string(42); }));

    std::cout << "Bootstrapping with " << cfg.resamples << " resamples...\n";
    auto est = nonius::analyse_sample(cfg.confidence_interval, cfg.resamples, sample.begin(), sample.end());
    std::cout << "Done.\n";

    auto print_bounded_stat = [](std::string const& name, nonius::stats::estimate e) {
        std::cout << "* " << name << ": " << fns(orig(e.point)).count() << " ns"
                  << ", lb " << fns(orig(e.lower_bound)).count() << " ns"
                  << ", ub " << fns(orig(e.upper_bound)).count() << " ns\n";
    };
    print_bounded_stat("Mean", est.mean);
    print_bounded_stat("Standard deviation", est.standard_deviation);

    auto print_outlier_group = [cfg](std::string const& name, int count, double threshold) {
        double ratio = count /(double) cfg.samples;
        if(ratio > threshold) {
            std::cout << " * " << name << ": " << count << " (" << 100. * ratio << "%)\n";
        }
    };
    auto o = nonius::stats::classify_outliers(sample.begin(), sample.end());
    std::cout << "* Outliers: " << o.total() << " (" << (100. * o.total() / cfg.samples) << "%)\n";
    print_outlier_group("Low severe", o.low_severe, 0);
    print_outlier_group("Low mild", o.low_mild, 0.01);
    print_outlier_group("High mild", o.high_mild, 0.01);
    print_outlier_group("High severe", o.high_severe, 0);

    std::cout << "* Variance introduced by outliers: " << est.outlier_variance << "%\n";

    std::string outlier_effect;
    if(est.outlier_variance < 0.01) {
        outlier_effect = "unaffected";
    } else if(est.outlier_variance < 0.1) {
        outlier_effect = "slightly inflated";
    } else if(est.outlier_variance < 0.5) {
        outlier_effect = "moderately inflated";
    } else {
        outlier_effect = "severely inflated";
    }
    std::cout << " * Variance is " << outlier_effect << " by outliers\n";
}

