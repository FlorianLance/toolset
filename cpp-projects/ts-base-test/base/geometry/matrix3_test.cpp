
// catch
#include <catch/catch.hpp>

// local
#include "utility/logger.hpp"
#include "base/glm_utility.hpp"
#include "geometry/matrix3.hpp"

using namespace tool;

static constexpr geo::Mat3f m1{
    7.f,2,1,
    0,3,-1,
    -3,4,-2
};
static constexpr geo::Mat3f m2{
    3.f,1,-4,
    -2,7,5,
    1.2f,4,-4
};
static constexpr auto glmM1 = glm::mat3(
    glm::vec3(7,0,-3),
    glm::vec3(2,3,4),
    glm::vec3(1,-1,-2)
);
static constexpr auto glmM2 = glm::mat3(
    glm::vec3(3,-2,1.2),
    glm::vec3(1,7,4),
    glm::vec3(-4,5,-4)
);
static constexpr auto altglmM1 = glm::mat3(
    7,0,-3,
    2,3,4,
    1,-1,-2
);

TEST_CASE("B. geo::Mat3"){
    Log::message("geo::Mat3\n");

    SECTION("GLM comparison"){

        Log::message("  GLM comparison\n");



        REQUIRE(glmM1 == altglmM1);

        REQUIRE(compare(m1.col(0), glm::column(glmM1,0)));
        REQUIRE(compare(m1.col(1), glm::column(glmM1,1)));
        REQUIRE(compare(m1.col(2), glm::column(glmM1,2)));
        REQUIRE(compare(m1.row(0), glm::row(glmM1,0)));
        REQUIRE(compare(m1.row(1), glm::row(glmM1,1)));
        REQUIRE(compare(m1.row(2), glm::row(glmM1,2)));
        REQUIRE(compare(m1,glmM1));
        REQUIRE(compare(m1,from_glm(glmM1)));

        REQUIRE(compare(m2.col(0), glm::column(glmM2,0)));
        REQUIRE(compare(m2.col(1), glm::column(glmM2,1)));
        REQUIRE(compare(m2.col(2), glm::column(glmM2,2)));
        REQUIRE(compare(m2.row(0), glm::row(glmM2,0)));
        REQUIRE(compare(m2.row(1), glm::row(glmM2,1)));
        REQUIRE(compare(m2.row(2), glm::row(glmM2,2)));
        REQUIRE(compare(m2,glmM2));
        REQUIRE(compare(m2,from_glm(glmM2)));

        REQUIRE(determinant(m1) == glm::determinant(glmM1));
        REQUIRE(compare(inverse(m1),glm::inverse(glmM1)));
        REQUIRE(compare(m1*m2, glmM1*glmM2));
        REQUIRE(compare(m2*m1, glmM2*glmM1));

        constexpr auto v1    =  geo::Vec3f{1.3f,5.f,-2.f};
        constexpr auto glmV1 =  glm::vec3(1.3f,5.f,-2.f);
        REQUIRE(compare(v1, glmV1));
        REQUIRE(compare(v1*m1, glmV1*glmM1));
        REQUIRE(compare(m1*m2, multiply_mrxc(m1,m2)));
        REQUIRE(compare(m2*m1, multiply_mrxc(m2,m1)));
        REQUIRE(compare(m1*transpose_mrxc(v1), glmM1*glmV1));




//        REQUIRE(compare(m1.multiply_point(v1), v1*m1));
    }
}