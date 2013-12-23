#include <nonius/measurement.h++>
#include <iostream>

int main() {
    auto d = nonius::run_for_at_least(std::chrono::seconds(1), 1, [](int x) {
            return bool(std::cout << "foo" << x << '\n');
        });
    std::cout << std::get<0>(d).count();
}
