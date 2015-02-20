#include <nonius.h++>

#include <iterator>
#include <string>

int main() {
    nonius::configuration cfg;
    cfg.output_file = "example2.csv";
    nonius::benchmark benchmarks[] = {
        nonius::benchmark("to_string(42)", []{ return std::to_string(42); }),
        nonius::benchmark("to_string(4.2)", []{ return std::to_string(4.2); })
    };

    nonius::go(cfg, std::begin(benchmarks), std::end(benchmarks), nonius::csv_reporter());
}


