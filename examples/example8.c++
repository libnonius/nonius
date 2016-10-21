#define NONIUS_RUNNER
#include <nonius.h++>

#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include <cstdlib>

NONIUS_PARAM(N, std::size_t{100})
NONIUS_PARAM(QUADRATIC_MAX_N, std::size_t{10000})

template <typename IntT=int>
std::vector<IntT> make_random_vector(std::size_t n)
{
    auto v = std::vector<IntT>(n);
    std::iota(v.begin(), v.end(), IntT{});
    std::shuffle(v.begin(), v.end(), std::default_random_engine{42});
    return v;
}

template <typename Iter>
void bubble_sort(Iter first, Iter last)
{
    using std::swap;
    auto sorted = false;
    if (first == last) return;
    do {
        sorted = true;
        for (auto it = first, nxt = next(it);
             nxt != last;
             it = nxt, nxt = next(it))
        {
            if (*it > *nxt) {
                swap(*it, *nxt);
                sorted = false;
            }
        }
    } while (!sorted);
}

template <typename Iter>
void insert_sort(Iter first, Iter last)
{
    using std::swap;
    for (auto it = first; it != last; ++it)
        swap(*it, *std::min_element(it, last));
}

NONIUS_BENCHMARK("std::sort", [](nonius::chronometer meter)
{
    auto n = meter.param<N>();
    auto v = make_random_vector(n);
    auto vs = std::vector<decltype(v)>(meter.runs(), v);
    meter.measure([&] (int r) {
        std::sort(vs[r].begin(), vs[r].end());
    });
})

NONIUS_BENCHMARK("std::qsort", [](nonius::chronometer meter)
{
    auto n = meter.param<N>();
    auto v = make_random_vector(n);
    auto vs = std::vector<decltype(v)>(meter.runs(), v);
    meter.measure([&] (int r) {
        std::qsort(
            &vs[r][0], vs[r].size(), sizeof(int),
            [] (const void* a, const void* b) {
                return *static_cast<const int*>(a) - *static_cast<const int*>(b);
            });
    });
})

NONIUS_BENCHMARK("bubble_sort", [](nonius::chronometer meter)
{
    auto n = meter.param<N>();
    if (n >= meter.param<QUADRATIC_MAX_N>())
        nonius::skip();
    auto v = make_random_vector(n);
    auto vs = std::vector<decltype(v)>(meter.runs(), v);
    meter.measure([&] (int r) {
        bubble_sort(vs[r].begin(), vs[r].end());
    });
})

NONIUS_BENCHMARK("insert_sort", [](nonius::parameters params)
{
    auto n = params.get<N>();
    if (n >= params.get<QUADRATIC_MAX_N>())
        nonius::skip();
    return [=](nonius::chronometer meter) {
        auto v = make_random_vector(n);
        auto vs = std::vector<decltype(v)>(meter.runs(), v);
        meter.measure([&] (int r) {
            insert_sort(vs[r].begin(), vs[r].end());
        });
    };
})
