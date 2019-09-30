#define NONIUS_RUNNER
#include <nonius.h++>

NONIUS_BENCHMARK("to_string(42)", []{
    return std::to_string(42);
})

// new syntax
NONIUS_BENCHMARK("to_string(4.2)") []{
    return std::to_string(4.2);
};
