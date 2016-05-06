#define NONIUS_RUNNER
#include <nonius.h++>

#include <string>
#include <vector>

NONIUS_BENCHMARK("construct small", [](nonius::chronometer meter)
{
    std::vector<nonius::storage_for<std::string>> storage(meter.runs());
    meter.measure([&](int i) { storage[i].construct("small"); });
})

NONIUS_BENCHMARK("construct large", [](nonius::chronometer meter)
{
    std::vector<nonius::storage_for<std::string>> storage(meter.runs());
    meter.measure([&](int i) { storage[i].construct("It was big. Really, really big. No, bigger than that. Even bigger. Keep going. More. No, more. Look, we're talking krakens and dreadnoughts for jewelry. It was big!"); });
})

NONIUS_BENCHMARK("destroy small", [](nonius::chronometer meter)
{
    std::vector<nonius::destructable_object<std::string>> storage(meter.runs());
    for(auto&& o : storage)
        o.construct("small");
    meter.measure([&](int i) { storage[i].destruct(); });
})

NONIUS_BENCHMARK("destroy large", [](nonius::chronometer meter)
{
    std::vector<nonius::destructable_object<std::string>> storage(meter.runs());
    for(auto&& o : storage)
        o.construct("It was big. Really, really big. No, bigger than that. Even bigger. Keep going. More. No, more. Look, we're talking krakens and dreadnoughts for jewelry. It was big!");
    meter.measure([&](int i) { storage[i].destruct(); });
})
