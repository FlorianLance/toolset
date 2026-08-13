
// catch
#include <catch/catch.hpp>

// local
#include "utility/logger.hpp"
#include "base/glm_utility.hpp"
#include "geometry/point4.hpp"

using namespace tool;

TEST_CASE("B. geo::Point4"){

    Log::message("geo::Point4\n");

    SECTION("GLM comparison"){
        Log::message("  GLM comparison\n");
        {
           Log::message("geo::Point operations: Point4\n");
           constexpr glm::vec4 pt1(1,2,3,4);
           constexpr glm::vec4 pt2(5,6,7,8);
           constexpr geo::Pt4f pt3{1,2,3,4};
           constexpr geo::Pt4f pt4{5,6,7,8};
           constexpr auto dotR = geo::dot(pt3,pt4);
           constexpr auto inv  = invert(pt3);
           constexpr auto sqn  = geo::square_norm(pt3);

           const glm::vec3 v(0);
           auto v2 = glm::normalize(v);
           // getters
           REQUIRE(pt2.x == pt4.x());
           REQUIRE(pt2.y == pt4.y());
           REQUIRE(pt2.z == pt4.z());
           REQUIRE(pt2.w == pt4.w());
           // operators
           REQUIRE(from_glm(pt1) == pt3);
           REQUIRE(from_glm(pt2) == pt4);
           REQUIRE(from_glm(-pt2) == -1.f*pt4);
           REQUIRE(from_glm(pt1+pt2) == (pt3+pt4));
           REQUIRE(from_glm(pt1-pt2) == (pt3-pt4));
           REQUIRE(from_glm(pt1*pt2) == (pt3*pt4));
           auto aa = pt1/pt2;
           // REQUIRE(from_glm(pt1/pt2) == (pt3/pt4));
           REQUIRE(from_glm(pt1+pt2) == (add(pt3,pt4)));
           REQUIRE(from_glm(pt1-pt2) == (substract(pt3,pt4)));
           // REQUIRE(from_glm(pt1*pt2) == (multiply(pt3,pt4)));
           // REQUIRE(from_glm(pt1/pt2) == (geo::divide(pt3,pt4)));
           // functions
           REQUIRE(glm::dot(pt1,pt2) == geo::dot(pt3,pt4));
           REQUIRE(from_glm(-pt1) == invert(pt3));
           REQUIRE(tool::almost_equal(glm::length(pt1)*glm::length(pt1),sqn));
           REQUIRE(from_glm(glm::normalize(pt1)) == geo::normalize(pt3));
           REQUIRE(from_glm(glm::normalize(glm::vec4(5,6,7,8))) == normalize(geo::Pt4f{5,6,7,8}));
        }
    }
}
