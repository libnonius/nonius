
// Tests for parameter related stuff

#include <nonius/go.h++>
#include <catch.hpp>

namespace nonius {

TEST_CASE("generate params") {
    SECTION("no parameters") {
        auto v = generate_params({});
        CHECK(v.size() == 1);
        CHECK(v[0] == parameters{});
    }

    SECTION("linear") {
        auto decl = scoped_param_declaration<int>{"x", 0};
        auto v = generate_params(run_configuration{
                "x", "+", "42", "2", 4 });

        CHECK(v == (std::vector<parameters>{
                    {{"x", "42"}},
                    {{"x", "44"}},
                    {{"x", "46"}},
                    {{"x", "48"}}}));
    }

    SECTION("exponential") {
        auto decl = scoped_param_declaration<int>{"x", 0};
        auto v = generate_params(run_configuration{
                "x", "*", "42", "10", 3 });

        CHECK(v == (std::vector<parameters>{
                    {{"x", "42"}},
                    {{"x", "420"}},
                    {{"x", "4200"}}}));
    }

    SECTION("at least one step") {
        auto decl = scoped_param_declaration<int>{"x", 0};
        auto v = generate_params(run_configuration{
                "x", "*", "42", "10", 0 });

        CHECK(v == (std::vector<parameters>{
                    {{"x", "42"}}}));
    }
}

} // namespace nonius
