
// catch
#include <catch/catch.hpp>

// local
#include "utility/logger.hpp"
#include "base/glm_utility.hpp"
#include "geometry/point2.hpp"

using namespace tool;

TEST_CASE("B. geo::Point2"){

    Log::message("geo::Point2\n");

    SECTION("Constructors"){
        Log::message("  Constructors\n");

        constexpr auto pt00 = geo::Point<float,2>{{0.f,0.f}};
        constexpr auto pt10 = geo::Point<float,2>{{1.f}};
        constexpr auto pt12 = geo::Point<float,2>{{1.f,2.f}};

        {constexpr geo::Point<float,2> pt;                              REQUIRE(pt == pt00);}
        {constexpr geo::Point<float,2> pt({1.f,2.f});                   REQUIRE(pt == pt12);}
        {constexpr geo::Point<float,2> pt{{1.f,2.f}};                   REQUIRE(pt == pt12);}
        {constexpr geo::Point<float,2> pt = {{1.f,2.f}};                REQUIRE(pt == pt12);}
        {
            constexpr geo::Point<float,2> pt1{{1.f,2.f}};
            constexpr geo::Point<float,2> pt2(pt1.array);               REQUIRE(pt2 == pt12);
            constexpr geo::Point<float,2> pt3(std::move(pt1.array));    REQUIRE(pt3 == pt12);
        }
        {
            constexpr geo::Point<float,2> pt1{{1.f,2.f}};
            constexpr geo::Point<float,2> pt2(pt1.array);               REQUIRE(pt2 == pt12);
        }

        {
            constexpr auto pt1 = geo::Pt2f{1.f,2.f};
            constexpr geo::Pt2f pt2(pt1);                               REQUIRE(pt2 == pt12);
        }
        {
            constexpr auto pt1 = geo::Pt2f{1.f,2.f};
            constexpr geo::Pt2f pt2(std::move(pt1));                    REQUIRE(pt2 == pt12);
        }
        {constexpr geo::Pt2f pt2;                                       REQUIRE(pt2 == pt00);}
        {constexpr geo::Pt2f pt2 = {};                                  REQUIRE(pt2 == pt00);}
        {constexpr geo::Pt2f pt2 = {1.f};                               REQUIRE(pt2 == pt10);}
        {constexpr geo::Pt2f pt2 = {1.f,2.f};                           REQUIRE(pt2 == pt12);}

        {constexpr geo::Pt2f pt2{};                                     REQUIRE(pt2 == pt00);}
        {constexpr geo::Pt2f pt2{1.f};                                  REQUIRE(pt2 == pt10);}
        {constexpr geo::Pt2f pt2{1.f,2.f};                              REQUIRE(pt2 == pt12);}
        {constexpr geo::Pt2f pt2(1.f);                                  REQUIRE(pt2 == pt10);}
        {constexpr geo::Pt2f pt2(1.f,2.f);                              REQUIRE(pt2 == pt12);}
        {constexpr geo::Pt2f pt2(geo::Matrix<float,1,2>{{1.f,2.f}});    REQUIRE(pt2 == pt12);}
        {
            constexpr geo::Matrix<float,1,2> mat{{1.f,2.f}};
            constexpr geo::Pt2f pt2(mat);                               REQUIRE(pt2 == pt12);
        }
        // {constexpr geo::Pt2f pt2{1.f,2.f,3.f};                       REQUIRE(pt2 == pt12);} // NOK
        // {constexpr geo::Pt2f pt2 = {1.f,2.f,3.f};                    REQUIRE(pt2 == pt12);} // NOK
    }

    SECTION("GLM comparison"){
        Log::message("  GLM comparison\n");

        constexpr glm::vec2 glmPt1(1,2);
        constexpr glm::vec2 glmPt2(4,5);
        constexpr geo::Pt2f pt1{1,2};
        constexpr geo::Pt2f pt2{4,5};
        // getters
        REQUIRE(almost_equal(glmPt2.x,pt2.x()));
        REQUIRE(almost_equal(glmPt2.y,pt2.y()));
        // operators
        REQUIRE(from_glm(glmPt1) == pt1);
        REQUIRE(from_glm(glmPt2) == pt2);
        REQUIRE(from_glm(-1.f*glmPt2) == -1.f*pt2);
        REQUIRE(from_glm(glmPt1+glmPt2) == (pt1+pt2));
        REQUIRE(from_glm(glmPt1-glmPt2) == (pt1-pt2));
        REQUIRE(from_glm(glmPt1*glmPt2) == (pt1*pt2));
        // functions
        REQUIRE(glm::dot(glmPt1,glmPt2) == geo::dot(pt1,pt2));
        REQUIRE(from_glm(-glmPt1) == invert(pt1));
        REQUIRE(tool::almost_equal(glm::length(glmPt1)*glm::length(glmPt1),geo::square_norm(pt1)));
        REQUIRE(from_glm(glm::normalize(glmPt1)) == geo::normalize(pt1));
        REQUIRE(from_glm(glm::normalize(glm::vec2(4,5))) == normalize(geo::Pt2f{4,5}));
    }
}
