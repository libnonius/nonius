#include <nonius/environment.h++>
#include <iostream>
#include <chrono>

int main() {
    using fns = std::chrono::duration<double, std::nano>;
    auto env = nonius::measure_environment();
    std::cout << "Clock resolution: " << fns(env.clock_resolution).count() << " ns\n";
    std::cout << "Cost of clock call: " << fns(env.clock_cost).count() << " ns\n";
}
