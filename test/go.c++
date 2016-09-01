
// Tests for parameter related stuff

#include <nonius/go.h++>
#include <catch.hpp>

namespace nonius {

namespace {

struct x_tag {
    using type = int;
    static const char* name() { return "x"; }
};

struct y_tag {
    using type = std::string;
    static const char* name() { return "y"; }
};

struct z_tag {
    using type = float;
    static const char* name() { return "z"; }
};

} // anon namespace

TEST_CASE("generate params") {
    using str = std::string;

    SECTION("no parameters") {
        auto v = generate_params({});
        CHECK(v.size() == 1);
        CHECK(v[0] == parameters{});
    }

    SECTION("default parameters") {
        auto decl = scoped_param_declaration<x_tag>{42};
        auto v = generate_params({});

        CHECK(v == (std::vector<parameters>{{{"x", 42}}}));
    }

    SECTION("default and explicit parameters") {
        auto decl1 = scoped_param_declaration<x_tag>{42};
        auto decl2 = scoped_param_declaration<y_tag>{""};
        auto v = generate_params({{{"y", str{"foo"}}}, {}});

        CHECK(v == (std::vector<parameters>{{{"x", 42}, {"y", str{"foo"}}}}));
    }

    SECTION("run, linear") {
        auto decl = scoped_param_declaration<x_tag>{0};
        auto v = generate_params(param_configuration{{},
                    run_configuration{"x", "+", 42, 2, 4 }});

        CHECK(v == (std::vector<parameters>{
                    {{"x", 42}},
                    {{"x", 44}},
                    {{"x", 46}},
                    {{"x", 48}}}));
    }

    SECTION("run, exponential") {
        auto decl = scoped_param_declaration<x_tag>{0};
        auto v = generate_params(param_configuration{{},
                    run_configuration{"x", "*", 42, 10, 3 }});

        CHECK(v == (std::vector<parameters>{
                    {{"x", 42}},
                    {{"x", 420}},
                    {{"x", 4200}}}));
    }

    SECTION("run, at least one step") {
        auto decl = scoped_param_declaration<x_tag>{0};
        auto v = generate_params(param_configuration{{},
                    run_configuration{"x", "*", 42, 10, 0 }});

        CHECK(v == (std::vector<parameters>{
                    {{"x", 42}}}));
    }

    SECTION("run, with default and explicit") {
        auto decl1 = scoped_param_declaration<x_tag>{0};
        auto decl2 = scoped_param_declaration<y_tag>{""};
        auto decl3 = scoped_param_declaration<z_tag>{42.0f};
        auto v = generate_params(param_configuration{{{"y", str{"foo"}}},
                    run_configuration{"x", "+", 42, 10, 3 }});

        CHECK(v == (std::vector<parameters>{
                    {{"x", 42}, {"y", str{"foo"}}, {"z", 42.0f}},
                    {{"x", 52}, {"y", str{"foo"}}, {"z", 42.0f}},
                    {{"x", 62}, {"y", str{"foo"}}, {"z", 42.0f}}}));
    }
}

} // namespace nonius
