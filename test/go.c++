
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

    SECTION("default parameters") {
        auto decl = scoped_param_declaration<int>{"x", 42};
        auto v = generate_params({});

        CHECK(v == (std::vector<parameters>{{{"x", "42"}}}));
    }

    SECTION("default and explicit parameters") {
        auto decl1 = scoped_param_declaration<int>{"x", 42};
        auto decl2 = scoped_param_declaration<std::string>{"y", ""};
        auto v = generate_params({{{"y", "foo"}}, {}});

        CHECK(v == (std::vector<parameters>{{{"x", "42"}, {"y", "foo"}}}));
    }

    SECTION("run, linear") {
        auto decl = scoped_param_declaration<int>{"x", 0};
        auto v = generate_params(param_configuration{{},
                    run_configuration{"x", "+", "42", "2", 4 }});

        CHECK(v == (std::vector<parameters>{
                    {{"x", "42"}},
                    {{"x", "44"}},
                    {{"x", "46"}},
                    {{"x", "48"}}}));
    }

    SECTION("run, exponential") {
        auto decl = scoped_param_declaration<int>{"x", 0};
        auto v = generate_params(param_configuration{{},
                    run_configuration{"x", "*", "42", "10", 3 }});

        CHECK(v == (std::vector<parameters>{
                    {{"x", "42"}},
                    {{"x", "420"}},
                    {{"x", "4200"}}}));
    }

    SECTION("run, at least one step") {
        auto decl = scoped_param_declaration<int>{"x", 0};
        auto v = generate_params(param_configuration{{},
                    run_configuration{"x", "*", "42", "10", 0 }});

        CHECK(v == (std::vector<parameters>{
                    {{"x", "42"}}}));
    }

    SECTION("run, with default and explicit") {
        auto decl1 = scoped_param_declaration<int>{"x", 0};
        auto decl2 = scoped_param_declaration<std::string>{"y", ""};
        auto decl3 = scoped_param_declaration<float>{"z", 42.0f};
        auto v = generate_params(param_configuration{{{"y", "foo"}},
                    run_configuration{"x", "+", "42", "10", 3 }});

        CHECK(v == (std::vector<parameters>{
                    {{"x", "42"}, {"y", "foo"}, {"z", "42"}},
                    {{"x", "52"}, {"y", "foo"}, {"z", "42"}},
                    {{"x", "62"}, {"y", "foo"}, {"z", "42"}}}));
    }
}

} // namespace nonius
