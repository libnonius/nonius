#include <nonius/environment.h++>
#include <iostream>

int main() {
    //using s = std::chrono::seconds;
    using fs = std::chrono::duration<double, std::ratio<1>>;
    int seed = 10000;
    nonius::warmup(seed);
    auto resolution = nonius::estimate_clock_resolution(seed);
    std::cout << fs(nonius::estimate_clock_cost(resolution)).count();
}
