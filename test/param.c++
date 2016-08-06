// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Tests for parameter related stuff

#include <nonius/param.h++>
#include <catch.hpp>

namespace nonius {

TEST_CASE("param declaration") {
    SECTION("local") {
        auto reg = param_registry{};
        auto decl = param_declaration<int>{"x", 42, reg};
        auto&& spec = reg.specs.at("x").get();

        CHECK(reg.specs.size() == 1);
        CHECK(global_param_registry().specs.size() == 0);
        CHECK(spec.default_value() == "42");
    }

    SECTION("global") {
        auto&& reg = global_param_registry(); {
            auto decl = scoped_param_declaration<int>{"x", 42};
            auto&& spec = reg.specs.at("x").get();

            CHECK(reg.specs.size() == 1);
            CHECK(spec.default_value() == "42");
        }
        // cleanup, for testing
        CHECK(reg.specs.size() == 0);
    }
}

struct custom_t {
    int x;
    std::string y;

    friend std::ostream& operator<< (std::ostream& os, custom_t const& x) {
        os << x.x << " " << x.y;
        return os;
    }
    friend std::istream& operator>> (std::istream& is, custom_t& x) {
        is.setf(std::ios::skipws);
        is >> x.x >> x.y;
        return is;
    }
    friend custom_t operator*(custom_t const& lhs, custom_t const& rhs) {
        return { lhs.x * rhs.x, lhs.y + "*" + rhs.y};
    }
};

TEST_CASE("parameter types") {
    SECTION("int") {
        auto spec = param_spec<int>{42};
        CHECK(spec.default_value() == "42");
        CHECK(spec.plus("2", "3") == "5");
        CHECK(spec.times("2", "3") == "6");
    }

    SECTION("float") {
        auto spec = param_spec<float>{42};
        CHECK(spec.default_value() == "42");
        CHECK(spec.plus("2", "3.5") == "5.5");
        CHECK(spec.times("2", "3.5") == "7");
    }

    SECTION("string") {
        auto spec = param_spec<std::string>{"foo"};
        CHECK(spec.default_value() == "foo");
        CHECK(spec.plus("foo", "bar") == "foobar");
        CHECK_THROWS(spec.times("2", "3.5"));
    }

    SECTION("custom") {
        auto spec = param_spec<custom_t>{{42, "pepe"}};
        CHECK(spec.default_value() == "42 pepe");
        CHECK_THROWS(spec.plus("42 pepe", "12 manolo"));
        CHECK(spec.times("42 pepe", "12 manolo") == "504 pepe*manolo");
    }
}

TEST_CASE("parameter maps") {
    SECTION("get") {
        auto decl1 = scoped_param_declaration<int>{"x", 42};
        auto decl2 = scoped_param_declaration<std::string>{"y", ""};
        auto params = parameters{{"y", "foo"}};

        CHECK(params.get<std::string>("y") == "foo");

        CHECK_THROWS_AS(params.get("x"), std::out_of_range);
        CHECK_THROWS_AS(params.get<int>("y"), boost::bad_lexical_cast);
    }

    SECTION("merge") {
        auto params = parameters{{"x", "foo"},
                                 {"y", "bar"}}.merged({
                                         {"y", "baz"},
                                         {"z", "oof"}});

        CHECK(params == (parameters{
                    {"x", "foo"},
                    {"y", "baz"},
                    {"z", "oof"}}));
    }
}

} // namespace nonius
