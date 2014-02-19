#include <nonius/main.h++>

#include <string>
#include <vector>

NONIUS_BENCHMARK("std::string construction", [](nonius::chronometer meter)
{
	std::vector<nonius::storage_for<std::string>> storage(meter.runs());
	meter.measure([&](int i) { storage[i].construct("Foo"); });
})

NONIUS_BENCHMARK("std::string destruction", [](nonius::chronometer meter)
{
	std::vector<nonius::destructable_object<std::string>> storage(meter.runs());
	std::for_each(storage.begin(), storage.end(), [](nonius::destructable_object<std::string>& object) { object.construct("Bar"); });
	meter.measure([&](int i) { storage[i].destruct(); });
})
