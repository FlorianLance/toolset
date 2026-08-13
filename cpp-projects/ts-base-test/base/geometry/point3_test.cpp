
// catch
#include <catch/catch.hpp>

// local
#include "utility/logger.hpp"
#include "base/glm_utility.hpp"
#include "geometry/point3.hpp"

using namespace tool;

TEST_CASE("B. geo::Point3"){

    Log::message("geo::Point3\n");

    SECTION("Constructors"){

        Log::message("  Constructors\n");

        constexpr auto pt000 = geo::Point<float,3>{{0.f,0.f,0.f}};
        constexpr auto pt100 = geo::Point<float,3>{{1.f}};
        constexpr auto pt120 = geo::Point<float,3>{{1.f,2.f}};
        constexpr geo::Pt3f pt123 = {1.f,2.f,3.f};

        {constexpr geo::Point<float,3> pt;                              REQUIRE(pt == pt000);}
        {constexpr geo::Point<float,3> pt({1.f,2.f,3.f});               REQUIRE(pt == pt123);}
        // {constexpr geo::Point<float,3> pt{1.f,2.f,3.f};              REQUIRE(pt == pt123);} // NOK
        // {constexpr geo::Point<float,3> pt = {1.f,2.f,3.f};           REQUIRE(pt == pt123);} // NOK
        {
            constexpr geo::Point<float,3> pt1{{1.f,2.f,3.f}};
            constexpr geo::Point<float,3> pt2(pt1.array);               REQUIRE(pt2 == pt123);
            constexpr geo::Point<float,3> pt3(std::move(pt1.array));    REQUIRE(pt3 == pt123);
        }
        {
            constexpr auto pt1 = geo::Pt3f{1.f,2.f,3.f};
            constexpr geo::Pt3f pt2(pt1);                               REQUIRE(pt2 == pt123);
        }
        {
            constexpr auto pt1 = geo::Pt3f{1.f,2.f,3.f};
            constexpr geo::Pt3f pt2(std::move(pt1));                    REQUIRE(pt2 == pt123);
        }
        {constexpr geo::Pt3f pt3;                                       REQUIRE(pt3 == pt000);}
        {constexpr geo::Pt3f pt3 = {};                                  REQUIRE(pt3 == pt000);}
        {constexpr geo::Pt3f pt3 = {1.f};                               REQUIRE(pt3 == pt100);}
        {constexpr geo::Pt3f pt3 = {1.f,2.f};                           REQUIRE(pt3 == pt120);}
        {constexpr geo::Pt3f pt3 = {1.f,2.f,3.f};                       REQUIRE(pt3 == pt123);}

        {constexpr geo::Pt3f pt3{};                                     REQUIRE(pt3 == pt000);}
        {constexpr geo::Pt3f pt3{1.f};                                  REQUIRE(pt3 == pt100);}
        {constexpr geo::Pt3f pt3{1.f,2.f};                              REQUIRE(pt3 == pt120);}
        {constexpr geo::Pt3f pt3{1.f,2.f,3.f};                          REQUIRE(pt3 == pt123);}
        {constexpr geo::Pt3f pt3(1.f);                                  REQUIRE(pt3 == pt100);}
        {constexpr geo::Pt3f pt3(1.f,2.f);                              REQUIRE(pt3 == pt120);}
        {constexpr geo::Pt3f pt3(1.f,2.f,3.f);                          REQUIRE(pt3 == pt123);}
        {constexpr geo::Pt3f pt3(geo::Matrix<float,1,3>{{1.f,2.f,3.f}});REQUIRE(pt3 == pt123);}
        {
            constexpr geo::Matrix<float,1,3> mat{{1.f,2.f,3.f}};
            constexpr geo::Pt3f pt3(mat);                               REQUIRE(pt3 == pt123);
        }
        // {constexpr geo::Pt3f pt3 = {1.f,2.f,3.f,4.f};                REQUIRE(pt3 == pt3);} // NOK
        // {constexpr geo::Pt3f pt3{1.f,2.f,3.f,4.f};                   REQUIRE(pt3 == pt3);} // NOK

        {
            constexpr geo::Pt3f pt3_1 = {1.f,2.f,3.f};
            constexpr geo::Pt3f pt3_2 = {1.f,1.f,3.f};
            constexpr geo::Pt3f pt3_3 = {3.f,2.f,1.f};
            REQUIRE(pt3_1 < pt3_3);
            REQUIRE(pt3_3 > pt3_1);
            REQUIRE(pt3_1 > pt3_2);
            REQUIRE(pt3_2 < pt3_1);
        }
    }

    SECTION("GLM comparison"){
        Log::message("  GLM comparison\n");
        {
            constexpr glm::vec3 glmPt1(1,2,3);
            constexpr glm::vec3 glmPt2(4,5,6);
            constexpr geo::Pt3f pt1{1,2,3};
            constexpr geo::Pt3f pt2{4,5,6};
            constexpr auto dotR = geo::dot(pt1,pt2);
            constexpr auto inv  = invert(pt1);
            constexpr auto sqn  = geo::square_norm(pt1);

           // getters
           REQUIRE(glmPt2.x == pt2.x());
           REQUIRE(glmPt2.y == pt2.y());
           REQUIRE(glmPt2.z == pt2.z());
           // operators
           REQUIRE(from_glm(glmPt1) == pt1);
           REQUIRE(from_glm(glmPt2) == pt2);
           REQUIRE(from_glm(-glmPt2) == -1.f*pt2);
           REQUIRE(from_glm(glmPt1+glmPt2) == (pt1+pt2));
           REQUIRE(from_glm(glmPt1-glmPt2) == (pt1-pt2));
           // REQUIRE(from_glm(glmPt1*glmPt2) == (pt1*pt2));
           // REQUIRE(from_glm(glmPt1/glmPt2) == (pt1/pt2));
           REQUIRE(from_glm(glmPt1+glmPt2) == (add(pt1,pt2)));
           REQUIRE(from_glm(glmPt1-glmPt2) == (substract(pt1,pt2)));
           // REQUIRE(from_glm(glmPt1*glmPt2) == (multiply(pt1,pt2)));
           // REQUIRE(from_glm(glmPt1/glmPt2) == (divide(pt1,pt2)));
           // functions
           REQUIRE(glm::dot(glmPt1,glmPt2) == geo::dot(pt1,pt2));
           REQUIRE(from_glm(glm::cross(glmPt1,glmPt2)) == geo::cross(pt1,pt2));
           REQUIRE(from_glm(-glmPt1) == invert(pt1));
           REQUIRE(tool::almost_equal(glm::length(glmPt1)*glm::length(glmPt1),sqn));
           REQUIRE(from_glm(glm::normalize(glmPt1)) == geo::normalize(pt1));
           REQUIRE(from_glm(glm::normalize(glm::vec3(4,5,6))) == normalize(geo::Pt3f{4,5,6}));

            // Log::message(std::format("cross {}\n", to_string(geo::cross(pt1,pt2))));
            // Log::message(std::format("glm cross {}\n", to_string(from_glm(glm::cross(glmPt1,glmPt2)))));
            // Log::message(std::format("glm mul {}\n", to_string(from_glm(glmPt1*glmPt2))));
        }
    }

}