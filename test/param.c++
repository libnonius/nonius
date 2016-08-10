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

namespace {

struct x_tag {
    using type = int;
    static const char* name() { return "x"; }
};

struct y_tag {
    using type = std::string;
    static const char* name() { return "y"; }
};

} // annon namespace

TEST_CASE("param declaration") {
    SECTION("local") {
        auto reg = param_registry{};
        auto decl = param_declaration<x_tag>{42, reg};

        CHECK(decl.registry.defaults().size() == 1);
        CHECK(global_param_registry().defaults().size() == 0);
        CHECK(reg.defaults().at("x").as<int>() == 42);
    }

    SECTION("global") {
        auto&& reg = global_param_registry(); {
            auto decl = scoped_param_declaration<x_tag>{42};

            CHECK(reg.defaults().size() == 1);
            CHECK(reg.defaults().at("x").as<int>() == 42);
        }
        // cleanup, for testing
        CHECK(reg.defaults().size() == 0);
    }
}

struct custom_t {
    int x;
    std::string y;

    friend bool operator== (custom_t const& x, custom_t const& y) {
        return x.x == y.x && x.y == y.y;
    }

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
        CHECK(param{42}.as<int>() == 42);
        CHECK((param{2} + param{3}).as<int>() == 5);
        CHECK((param{2} * param{3}).as<int>() == 6);
    }

    SECTION("float") {
        CHECK(param{42.0f}.as<float>() == 42.0f);
        CHECK((param{2.0f} + param{3.5f}).as<float>() == Approx(5.5f));
        CHECK((param{2.0f} * param{3.5f}).as<float>() == Approx(7.0f));
    }

    SECTION("string") {
        CHECK(param{std::string{"foo"}}.as<std::string>() == "foo");
        CHECK((param{std::string{"foo"}} + param{std::string{"bar"}}).as<std::string>() == "foobar");
        CHECK_THROWS(param{std::string{"foo"}} * param{std::string{"bar"}});
    }

    SECTION("custom") {
        CHECK((param{custom_t{42, "pepe"}}.as<custom_t>() == custom_t{42, "pepe"}));
        CHECK_THROWS(param{custom_t{}} + param{custom_t{}});
        CHECK(((param{custom_t{42, "pepe"}} * param{custom_t{12, "manolo"}}).as<custom_t>() == custom_t{504, "pepe*manolo"}));
    }

    SECTION("types must match") {
        CHECK_THROWS(param{42}.as<float>());
        CHECK_THROWS(param{42.0f}.as<int>());
    }
}

TEST_CASE("parameter maps") {
    SECTION("get") {
        auto decl1 = scoped_param_declaration<y_tag>{""};
        auto params = parameters{{"y", param{std::string{"foo"}}}};

        CHECK(params.get<y_tag>() == "foo");
        CHECK_THROWS_AS(params.get<x_tag>(), std::out_of_range);
    }

    SECTION("merge") {
        using str = std::string;
        auto params = parameters{{"x", str{"foo"}},
                                 {"y", str{"bar"}}}.merged({
                                         {"y", str{"baz"}},
                                         {"z", str{"oof"}}});
        CHECK(params.size() == 3);
        CHECK(params.at("x").as<str>() == "foo");
        CHECK(params.at("y").as<str>() == "baz");
        CHECK(params.at("z").as<str>() == "oof");
    }
}

} // namespace nonius
