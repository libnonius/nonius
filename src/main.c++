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
    nonius::benchmark benchmarks[] = {
        { "to_string(42)", []{ return std::to_string(42); } },
        { "to_string(4.2)", []{ return std::to_string(4.2); } },
    };

    std::cout << "Measuring environment...\n";
    auto env = nonius::measure_environment<nonius::default_clock>();
    std::cout << "Running benchmark...\n";
    auto results = nonius::run_analyse(cfg, env, benchmarks, benchmarks + 2);
    std::cout << "Done.\n";

    auto print_bounded_stat = [](std::string const& name, nonius::stats::estimate e) {
        std::cout << "* " << name << ": " << fns(orig(e.point)).count() << " ns"
                << ", lb " << fns(orig(e.lower_bound)).count() << " ns"
                << ", ub " << fns(orig(e.upper_bound)).count() << " ns\n";
    };
    auto print_outlier_group = [cfg](std::string const& name, int count, double threshold) {
        double ratio = count /(double) cfg.samples;
        if(ratio > threshold) {
            std::cout << " * " << name << ": " << count << " (" << 100. * ratio << "%)\n";
        }
    };
    for(auto&& p : results) {
        std::string name;
        nonius::benchmark_results r;
        std::tie(name, r) = p;
        std::cout << "===== " << name << " =====\n";

        print_bounded_stat("Mean", r.analysis.mean);
        print_bounded_stat("Standard deviation", r.analysis.standard_deviation);

        std::cout << "* Outliers: " << r.outliers.total() << " (" << (100. * r.outliers.total() / cfg.samples) << "%)\n";
        print_outlier_group("Low severe", r.outliers.low_severe, 0);
        print_outlier_group("Low mild", r.outliers.low_mild, 0.01);
        print_outlier_group("High mild", r.outliers.high_mild, 0.01);
        print_outlier_group("High severe", r.outliers.high_severe, 0);

        std::cout << "* Variance introduced by outliers: " << r.analysis.outlier_variance << "%\n";

        std::string outlier_effect;
        if(r.analysis.outlier_variance < 0.01) {
            outlier_effect = "unaffected";
        } else if(r.analysis.outlier_variance < 0.1) {
            outlier_effect = "slightly inflated";
        } else if(r.analysis.outlier_variance < 0.5) {
            outlier_effect = "moderately inflated";
        } else {
            outlier_effect = "severely inflated";
        }
        std::cout << " * Variance is " << outlier_effect << " by outliers\n";
    }
}

