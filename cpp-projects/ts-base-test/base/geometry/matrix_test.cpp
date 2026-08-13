
// catch
#include <catch/catch.hpp>

// local
#include "utility/logger.hpp"
#include "base/glm_utility.hpp"
#include "geometry/matrix.hpp"

using namespace tool;

static constexpr auto pt00 = geo::Point<float,2>{{0.f,0.f}};
static constexpr auto pt10 = geo::Point<float,2>{{1.f}};
static constexpr auto pt12 = geo::Point<float,2>{{1.f,2.f}};

TEST_CASE("B. geo::Matrix"){
    Log::message("geo::Matrix\n");

    SECTION("Constructor"){
        Log::message("  Constructor\n");

        REQUIRE(almost_equal(pt00.x(),0.f));
        REQUIRE(almost_equal(pt00.y(),0.f));
        REQUIRE(almost_equal(pt10.x(),1.f));
        REQUIRE(almost_equal(pt10.y(),0.f));
        REQUIRE(almost_equal(pt12.x(),1.f));
        REQUIRE(almost_equal(pt12.y(),2.f));

        // Validity
        {geo::Matrix<float, 1,2> m = {}; REQUIRE(m == pt00);} // OK
        {geo::Matrix<float, 1,2> m{}; REQUIRE(m == pt00);} // OK
        {geo::Matrix<float, 1,2> m{{}}; REQUIRE(m == pt00);} // OK
        {geo::Matrix<float, 1,2> m({1.f}); REQUIRE(m == pt10);} // OK
        {geo::Matrix<float, 1,2> m({1.f,2.f}); REQUIRE(m == pt12);} // OK
        std::array<float,2> a2 = {1.f,2.f};
        {geo::Matrix<float, 1,2> m(a2); REQUIRE(m == pt12);} // OK
        {geo::Matrix<float, 1,2> m(std::move(a2)); REQUIRE(m == pt12);} // OK
        {geo::Matrix<float, 1,2> m(std::array<float,2>{}); REQUIRE(m == pt00);} // OK
        {geo::Matrix<float, 1,2> m(std::array<float,2>{1.f}); REQUIRE(m == pt10);} // OK
        {geo::Matrix<float, 1,2> m(std::array<float,2>{1.f,2.f}); REQUIRE(m == pt12);} // OK
        // {geo::Matrix<float, 1,2> m = {1.f};} // NOK
        // {geo::Matrix<float, 1,2> m = {1.f,2.f};} // NOK
        // {geo::Matrix<float, 1,2> m = {1.f,2.f,3.f};} // NOK
        // {geo::Matrix<float, 1,2> m{1.f};} // NOK
        // {geo::Matrix<float, 1,2> m{1.f,2.f};} // NOK
        // {geo::Matrix<float, 1,2> m{1.f,2.f,3.f};} // NOK
        // {geo::Matrix<float, 1,2> m({1.f,2.f,3.f});} // NOK
        // {geo::Matrix<float, 1,2> m();} // NOK (warning)
        // {geo::Matrix<float, 1,2> m(1.f);} // NOK
        // {geo::Matrix<float, 1,2> m(1.f,2.f);} // NOK
        // {geo::Matrix<float, 1,2> m(1.f,2.f,3.f);} // NOK
        // {std::array<float,1> a1 = {1.f}; geo::Matrix<float, 1,2> m(a1);} // NOK
        // {geo::Matrix<float, 1,2> m(std::array<float,2>{1.f,2.f,3.f});} // NOK
    }

    SECTION("Functions"){
        Log::message("   Functions\n");
        // {constexpr auto m = geo::Matrix<float, 3,3>::identity();}
        // {constexpr geo::Matrix<float,1, 2> r = m1.row(0);}
    }
}