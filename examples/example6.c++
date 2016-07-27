#define NONIUS_RUNNER
#include <nonius.h++>

#include <string>
#include <vector>

NONIUS_PARAM(size, 100u);

NONIUS_BENCHMARK("push_back vector", [](nonius::chronometer meter)
{
    auto n = meter.param("size");
    auto storage = std::vector<std::vector<char>>(meter.runs());
    meter.measure([&](int i) {
        auto& x = storage[i];
        for (auto j = 0; j < n; ++j)
            x.push_back(static_cast<char>(j));
    });
})

NONIUS_BENCHMARK("push_back string", [](nonius::chronometer meter)
{
    auto n = meter.param<std::size_t>("size");
    auto storage = std::vector<std::string>(meter.runs());
    meter.measure([&](int i) {
        auto& x = storage[i];
        for (auto j = 0u; j < n; ++j)
            x.push_back(static_cast<char>(j));
    });
})
