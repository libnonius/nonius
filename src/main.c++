#include <nonius/environment.h++>
#include <iostream>
#include <chrono>

int main() {
    using fns = std::chrono::duration<double, std::nano>;
    int seed = 10000;
    nonius::warmup(seed);
    auto resolution = nonius::estimate_clock_resolution(seed);
    std::cout << "Clock resolution: " << fns(resolution).count() << " ns\n";
    std::cout << "Cost of clock call: " << fns(nonius::estimate_clock_cost(resolution)).count() << " ns\n";
}
