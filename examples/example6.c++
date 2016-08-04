#define NONIUS_RUNNER
#include <nonius.h++>

#include <string>
#include <vector>

NONIUS_PARAM("size", 100u)
NONIUS_PARAM("string", std::string{"foo"})

NONIUS_BENCHMARK("push_back vector", [](nonius::chronometer meter)
{
    auto n = meter.param("size");
    auto s = meter.param<std::string>("string");
    auto storage = std::vector<std::vector<char>>(
        meter.runs(), std::vector<char>(s.begin(), s.end()));
    meter.measure([&](int i) {
        auto& x = storage[i];
        for (auto j = 0; j < n; ++j)
            x.push_back(static_cast<char>(j));
    });
})

// alternative syntax
NONIUS_BENCHMARK("push_back string", [](nonius::parameters params) {
    auto n = params.get<std::size_t>("size");
    auto s = params.get<std::string>("string");
    return [=](nonius::chronometer meter) {
        auto storage = std::vector<std::string>(meter.runs(), s);
        meter.measure([&](int i) {
            auto& x = storage[i];
            for (auto j = 0u; j < n; ++j)
                x.push_back(static_cast<char>(j));
        });
    };
})
