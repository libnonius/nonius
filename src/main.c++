#include <nonius/environment.h++>
#include <iostream>

int main() {
    //using s = std::chrono::seconds;
    //using fs = std::chrono::duration<double, std::ratio<1>>;
    std::cout << nonius::warmup(10000);
}
