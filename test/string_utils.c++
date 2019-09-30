// Nonius - C++ benchmarking tool
//
// Written in 2014- by the nonius contributors <nonius@rmf.io>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Tests for statistical stuff

#include <nonius/detail/string_utils.h++>

#include <catch.hpp>

#include <vector>

TEST_CASE("is_space") {
    std::string spaces = "a b\tc";
    std::string nospace = "ab";
    REQUIRE(std::count_if(spaces.begin(), spaces.end(), nonius::is_space()) == 2);
    REQUIRE(std::count_if(nospace.begin(), nospace.end(), nonius::is_space()) == 0);
}

TEST_CASE("is_any_of") {
    std::string str = "a:b;c:d";
    REQUIRE(std::count_if(str.begin(), str.end(), nonius::is_any_of("")) == 0);
    REQUIRE(std::count_if(str.begin(), str.end(), nonius::is_any_of(":")) == 2);
    REQUIRE(std::count_if(str.begin(), str.end(), nonius::is_any_of(":;")) == 3);
}

TEST_CASE("starts_with") {
    REQUIRE(nonius::starts_with("abc", "ab"));
    REQUIRE_FALSE(nonius::starts_with("abc", "bc"));
    REQUIRE_FALSE(nonius::starts_with("ab", "abc"));
}

TEST_CASE("trim_copy") {
    REQUIRE(nonius::trim_copy("") == std::string(""));
    REQUIRE(nonius::trim_copy("abc") == std::string("abc"));
    REQUIRE(nonius::trim_copy(" abc") == std::string("abc"));
    REQUIRE(nonius::trim_copy("abc ") == std::string("abc"));
    REQUIRE(nonius::trim_copy(" abc ") == std::string("abc"));
}

TEST_CASE("split") {
    const std::string str1("xx abc  xx abb");
    const std::string str2("Xx abc  xX abb xx");
    const std::string str3("xx");
    const std::string strempty("");
    std::vector<std::string> result;

    nonius::split(result, str2, nonius::is_any_of("xX"));
    REQUIRE(result.size() == 4);
    CHECK(result[0] == std::string(""));
    CHECK(result[1] == std::string(" abc  "));
    CHECK(result[2] == std::string(" abb "));
    CHECK(result[3] == std::string(""));

    nonius::split(result, str1, nonius::is_any_of("x"));
    REQUIRE(result.size() == 3);
    CHECK(result[0] == std::string(""));
    CHECK(result[1] == std::string(" abc  "));
    CHECK(result[2] == std::string(" abb"));

    nonius::split(result, str1, nonius::is_space());
    REQUIRE(result.size() == 4);
    CHECK(result[0] == std::string("xx"));
    CHECK(result[1] == std::string("abc"));
    CHECK(result[2] == std::string("xx"));
    CHECK(result[3] == std::string("abb"));

    nonius::split(result, str3, nonius::is_any_of(","));
    REQUIRE(result.size() == 1);
    CHECK(result[0] == std::string("xx"));

    nonius::split(result, strempty, nonius::is_space());
    REQUIRE(result.size() == 1);
    CHECK(result[0] == std::string(""));
}

