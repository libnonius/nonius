#define NONIUS_RUNNER
#include <nonius.h++>

#include <list>
#include <forward_list>

NONIUS_BENCHMARK("list<long long>", [](nonius::chronometer meter) {
    std::list<long long> l;
    meter.measure([&](int i) { l.push_front(i); });
})

NONIUS_BENCHMARK("list<char>", [](nonius::chronometer meter) {
    std::list<char> l;
    meter.measure([&](int i) { l.push_front(i); });
})

NONIUS_BENCHMARK("forward_list<long long>", [](nonius::chronometer meter) {
    std::forward_list<long long> l;
    meter.measure([&](int i) { l.push_front(i); });
})

NONIUS_BENCHMARK("forward_list<char>", [](nonius::chronometer meter) {
    std::forward_list<char> l;
    meter.measure([&](int i) { l.push_front(static_cast<char>(i)); });
})
