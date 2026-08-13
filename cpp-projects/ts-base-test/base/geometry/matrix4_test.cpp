
// std
#include <iostream>

// catch
#include <catch/catch.hpp>

// local
#include "utility/logger.hpp"
#include "base/glm_utility.hpp"
#include "geometry/matrix4.hpp"

using namespace tool;


using gv3f = geo::Vec3f;
using glv3f = glm::fvec3;
using gv3d = geo::Vec3d;
using glv3d = glm::dvec3;

static constexpr geo::Mat4f m1{
    7.f,2,1,-4,
    0,3,-1,1,
    -3,4,-2,-3,
    -4,4,6.2f,1.2f
};
static constexpr geo::Mat4f m2{
    3.f,1,-4,-5,
    -2,7,5,-1,
    1.2f,4,-4,1.2f,
    3.6f,-1.5,4.2f,2
};
static constexpr auto glmM1 = glm::mat4(
    glm::vec4(7,0,-3,-4),
    glm::vec4(2,3,4,4),
    glm::vec4(1,-1,-2,6.2),
    glm::vec4(-4,1,-3,1.2)
);
static constexpr auto glmM2 = glm::mat4(
    glm::vec4(3,-2,1.2,3.6),
    glm::vec4(1,7,4,-1.5),
    glm::vec4(-4,5,-4,4.2),
    glm::vec4(-5,-1,1.2,2)
);

static constexpr auto altglmM1 = glm::mat4(
    7,0,-3,-4,
    2,3,4,4,
    1,-1,-2,6.2,
    -4,1,-3,1.2
);

static constexpr auto v1    =  geo::Vec4f{1.3f,5.f,-2.f,2.5f};
static constexpr auto glmV1 =  glm::vec4(1.3f,5.f,-2.f,2.5f);

TEST_CASE("B. geo::Mat4"){
    Log::message("geo::Mat4\n");

    SECTION("Operators"){
        Log::message("  Operators\n");

        // identity
        REQUIRE(compare(geo::Mat4f::identity(),glm::mat4(1.0f)));

        // comparison
        REQUIRE(glmM1 == altglmM1);
        REQUIRE(compare(m1,glmM1));
        REQUIRE(compare(m1,from_glm(glmM1)));
        REQUIRE(compare(m2,glmM2));
        REQUIRE(compare(m2,from_glm(glmM2)));
        REQUIRE(compare(v1, glmV1));

        auto glmTest1 = glm::mat4(0.f);
        glmTest1[0][0] = 0;
        glmTest1[0][1] = 0;
        glmTest1[0][2] = 0;
        glmTest1[0][3] = 0;
        glmTest1[1][0] = 1;
        glmTest1[1][1] = 1;
        glmTest1[1][2] = 1;
        glmTest1[1][3] = 1;
        glmTest1[2][0] = 2;
        glmTest1[2][1] = 2;
        glmTest1[2][2] = 2;
        glmTest1[2][3] = 2;
        glmTest1[3][0] = 3;
        glmTest1[3][1] = 3;
        glmTest1[3][2] = 3;
        glmTest1[3][3] = 3;

        auto test1 = geo::Mat4f();
        test1(0,0) = 0.f;
        test1(1,0) = 0.f;
        test1(2,0) = 0.f;
        test1(3,0) = 0.f;
        test1(0,1) = 1.f;
        test1(1,1) = 1.f;
        test1(2,1) = 1.f;
        test1(3,1) = 1.f;
        test1(0,2) = 2.f;
        test1(1,2) = 2.f;
        test1(2,2) = 2.f;
        test1(3,2) = 2.f;
        test1(0,3) = 3.f;
        test1(1,3) = 3.f;
        test1(2,3) = 3.f;
        test1(3,3) = 3.f;

        REQUIRE(compare(glmTest1,test1));

        // to glm / from glm
        REQUIRE(compare(from_glm(to_glm_mat4(m1)),from_glm(glmM1)));

        // transpose
        REQUIRE(compare(transpose(m1),from_glm(glm::transpose(glmM1))));
        REQUIRE(compare(transpose(transpose(m1)),from_glm(glmM1)));

        // subscript
        for(int ii = 0; ii < 4; ++ii){
            for(int jj = 0; jj < 4; ++jj){
                REQUIRE(compare(m1(ii,jj), glmM1[jj][ii]));
            }
        }

        // columns
        REQUIRE(compare(m1.col(0), glmM1[0]));
        REQUIRE(compare(m1.col(1), glmM1[1]));
        REQUIRE(compare(m1.col(2), glmM1[2]));
        REQUIRE(compare(m1.col(3), glmM1[3]));
        REQUIRE(compare(m1.col(0), glm::column(glmM1,0)));
        REQUIRE(compare(m1.col(1), glm::column(glmM1,1)));
        REQUIRE(compare(m1.col(2), glm::column(glmM1,2)));
        REQUIRE(compare(m1.col(3), glm::column(glmM1,3)));
        REQUIRE(compare(m1.col0(), glm::column(glmM1,0)));
        REQUIRE(compare(m1.col1(), glm::column(glmM1,1)));
        REQUIRE(compare(m1.col2(), glm::column(glmM1,2)));
        REQUIRE(compare(m1.col3(), glm::column(glmM1,3)));
        REQUIRE(compare(m2.col(0), glm::column(glmM2,0)));
        REQUIRE(compare(m2.col(1), glm::column(glmM2,1)));
        REQUIRE(compare(m2.col(2), glm::column(glmM2,2)));
        REQUIRE(compare(m2.col(3), glm::column(glmM2,3)));
        // rows
        REQUIRE(compare(m1.row(0), glm::row(glmM1,0)));
        REQUIRE(compare(m1.row(1), glm::row(glmM1,1)));
        REQUIRE(compare(m1.row(2), glm::row(glmM1,2)));
        REQUIRE(compare(m1.row(3), glm::row(glmM1,3)));
        REQUIRE(compare(m2.row(0), glm::row(glmM2,0)));
        REQUIRE(compare(m2.row(1), glm::row(glmM2,1)));
        REQUIRE(compare(m2.row(2), glm::row(glmM2,2)));
        REQUIRE(compare(m2.row(3), glm::row(glmM2,3)));        
        REQUIRE(compare(m1.row0(), glm::row(glmM1,0)));
        REQUIRE(compare(m1.row1(), glm::row(glmM1,1)));
        REQUIRE(compare(m1.row2(), glm::row(glmM1,2)));
        REQUIRE(compare(m1.row3(), glm::row(glmM1,3)));



        // determinant
        REQUIRE(determinant(m1) == glm::determinant(glmM1));

        // inverse
        std::cout << "1 P1" << (m1) << "\n";
        std::cout << "1 glmP" << from_glm((glmM1)) << "\n";
        std::cout << "1 inverse P1" << geo::inverse(m1) << "\n";
        std::cout << "1 inverse glmP" << from_glm(glm::inverse(glmM1)) << "\n";
        REQUIRE(compare(inverse(m1),glm::inverse(glmM1)));

        // multiplication
        // # m x v
        REQUIRE(compare(v1*m1, glmV1*glmM1));
        REQUIRE(compare(v1*m1, glmV1*glmM1));
        REQUIRE(compare(m1*transpose_mrxc(v1), glmM1*glmV1));
        // # m x m
        REQUIRE(compare(m1*m2, multiply_mrxc(m1,m2)));
        REQUIRE(compare(m2*m1, multiply_mrxc(m2,m1)));
        REQUIRE(compare(m1*m2, glmM1*glmM2));
        REQUIRE(compare(m2*m1, glmM2*glmM1));


    }

    SECTION("Camera"){
        Log::message("  Camera\n");

        // lookat
        {   
            REQUIRE(compare(
                clean( glm::lookAt(glv3f(-2.0f, 4.0f, -1.0f),glv3f( 0.0f, 2.0f,  0.0f), glv3f(0.0f, 1.0f,  0.0f))),
                clean(      look_at(gv3f{-2.0f, 4.0f, -1.0f},gv3f{  0.0f, 2.0f,  0.0f}, gv3f{ 0.0f, 1.0f,  0.0f}))
            ));

            REQUIRE(compare(
                clean( glm::lookAt(glv3f(15.0f, -7.0f, 3.0f),glv3f( 2.1f, 0.0f,  0.2f), glv3f(1.0f, 0.0f,  0.0f))),
                clean(      look_at(gv3f{15.0f, -7.0f, 3.0f},gv3f{  2.1f, 0.0f,  0.2f}, gv3f{ 1.0f, 0.0f,  0.0f}))
            ));

            REQUIRE(compare(
                clean( glm::lookAt(glv3f(-15.0f, -1.0f, 5.0f),glv3f( 10.0f, -2.0f,  -8.0f), glv3f(0.0f, 0.0f,  1.0f))),
                clean(      look_at(gv3f{-15.0f, -1.0f, 5.0f},gv3f{  10.0f, -2.0f,  -8.0f}, gv3f{ 0.0f, 0.0f,  1.0f}))
            ));
        }

        // perspective
        {
            auto glmP = glm::perspective(60.f*PI_180<float>, 1.4f, 0.02f, 1000.f);
            auto p1   = geo::perspective(60.f*PI_180<float>, 1.4f, 0.02f, 1000.f);
            REQUIRE(compare(glmP, p1));
            std::cout << "persp inverse P1" << geo::inverse(p1) << "\n";
            std::cout << "persp inverse glmP" << from_glm(glm::inverse(glmP)) << "\n";
            REQUIRE(compare(clean(glm::inverse(glmP)), clean(geo::inverse(p1))));
        }

        // orthographic
        {
            auto glmP = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
            auto p1   = geo::orthographic(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
            REQUIRE(compare(glmP, p1));
            std::cout << "ortho inverse P1" << geo::inverse(p1) << "\n";
            std::cout << "ortho inverse glmP" << from_glm(glm::inverse(glmP)) << "\n";
            REQUIRE(compare(clean(glm::inverse(glmP)), clean(geo::inverse(p1))));
        }

    }

    SECTION("Transformations"){

        Log::message("  Transformations\n");

        REQUIRE(
            from_glm(glm::translate(glm::identity<glm::fmat4>(), glm::fvec3{0.4f, 0.5f, 0.6f})) ==
            geo::translation_m4x4(geo::Vec3f{0.4f, 0.5f, 0.6f})
        );



        // scale
        auto scale    = geo::scale(geo::Mat4f::identity(),geo::Vec3f{0.5f, 0.5f, 0.5f});
        auto glmScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));;
        REQUIRE(compare(geo::scale_m4x4(geo::Vec3f{0.5f, 0.5f, 0.5f}),scale));
        REQUIRE(compare(clean(scale),clean(glmScale)));

        // translate
        auto tr    = geo::translate(geo::Mat4f::identity(), {{-1.f, 17.f, 56.f}});
        auto glmTr = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 17.0, 56.0));
        REQUIRE(compare(geo::translation_m4x4(geo::Vec3f{-1.f, 17.f, 56.f}),tr));
        REQUIRE(compare(clean(tr),clean(glmTr)));

        // rotation 1
        auto rot1    = geo::rotate(geo::Mat4f::identity(), geo::Vec3f{1.f,0.f,0.f}, 90.f);
        auto glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        REQUIRE(compare(clean(rot1),clean(glmRot1)));

        rot1    = geo::rotate(geo::Mat4f::identity(), geo::Vec3f{0.f,1.f,0.f}, 90.f);
        glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        REQUIRE(compare(clean(rot1),clean(glmRot1)));

        rot1    = geo::rotate(geo::Mat4f::identity(), geo::Vec3f{0.f,0.f,1.f}, 90.f);
        glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        REQUIRE(compare(clean(rot1),clean(glmRot1)));

        // rotation 2
        rot1    = geo::rotate(geo::Mat4f::identity(), geo::Vec3f{90.f,0.f,0.f});
        glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        REQUIRE(compare(clean(rot1),clean(glmRot1)));

        rot1    = geo::rotate(geo::Mat4f::identity(), geo::Vec3f{0.f,90.f,0.f});
        glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        REQUIRE(compare(clean(rot1),clean(glmRot1)));

        rot1    = geo::rotate(geo::Mat4f::identity(), geo::Vec3f{0.f,0.f,90.f});
        glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        REQUIRE(compare(clean(rot1),clean(glmRot1)));

        // rotation 3
        rot1    = geo::Mat4f::identity()*geo::rotation_m4x4(geo::Vec3f{90.f*tool::PI_180<float>,0.f,0.f});
        glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        REQUIRE(compare(clean(rot1),clean(glmRot1)));

        rot1    = geo::Mat4f::identity()*geo::rotation_m4x4(geo::Vec3f{0.f, 90.f*tool::PI_180<float>,0.f});
        glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        REQUIRE(compare(clean(rot1),clean(glmRot1)));

        rot1    = geo::Mat4f::identity()*geo::rotation_m4x4(geo::Vec3f{0.f, 0.f, 90.f*tool::PI_180<float>});
        glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        REQUIRE(compare(clean(rot1),clean(glmRot1)));

        // combination 1
        auto glmC1  = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
        auto c1     = geo::scale(geo::Mat4f::identity(), geo::Vec3f{0.5f, 0.5f, 0.5f});
        REQUIRE(compare(clean(c1),clean(glmC1)));
        glmC1       = glm::rotate(glmC1, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        c1          = geo::rotate(c1, geo::Vec3f{1.f,0.f,0.f}, 90.f);
        REQUIRE(compare(clean(c1),clean(glmC1)));

        // combination 2
        glmC1   = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
        c1      = geo::scale(geo::Mat4f::identity(), {{0.5f, 0.5f, 0.5f}});
        REQUIRE(compare(clean(c1),clean(glmC1)));
        glmC1   = glm::rotate(glmC1, glm::radians(-47.0f), glm::vec3(1.0, 0.0, 0.0));
        c1      = geo::rotate(c1, geo::Vec3f{1.f,0.f,0.f}, -47.f);
        REQUIRE(compare(clean(c1),clean(glmC1)));

        glmC1   = glm::translate(glmC1, glm::vec3(-1.0, 17.0, 56.0));
        c1      = geo::translate(c1, geo::Vec3f{-1.0, 17.0, 56.0});
        REQUIRE(compare(c1,glmC1));

        auto glm_tr = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 2.6f, -10.5));
        auto glm_sc = glm::scale(glm::mat4(1.0f), glm::vec3(1.2f,1.2f,1.2f));
        auto glm_rx = glm::rotate(glm::mat4(1.0f), glm::radians(-45.f), glm::vec3(1.0, 0.0, 0.0));
        auto glm_ry = glm::rotate(glm::mat4(1.0f), glm::radians(70.6f), glm::vec3(0.0, 1.0, 0.0));
        auto glm_rz = glm::rotate(glm::mat4(1.0f), glm::radians(7.9f),  glm::vec3(0.0, 0.0, 1.0));
        auto glm_model =  glm_tr * (glm_rz*glm_rx*glm_ry) * glm_sc;


        auto geo_tr     = geo::translate(geo::Mat4f::identity(), geo::Vec3f{1.5f, 2.6f, -10.5f});
        auto geo_scale  = geo::scale(geo::Mat4f::identity(), geo::Vec3f{1.2f,1.2f,1.2f});
        auto geo_rot    = geo::rotate(geo::Mat4f::identity(), geo::Vec3f{-45.f,70.6f,7.9f});
        geo::Mat4f geo_model  = geo_tr * geo_rot * geo_scale;
        REQUIRE(compare(glm_model, geo_model));

        // axis_angle_m3x3
        {
            auto rot1    = geo::rotate(geo::Vec3f{1.f, 0.f, 0.f}, 90.f);
            // auto rot2    = geo::axis_angle_m4x4(geo::Vec3f{1.f, 0.f, 0.f}, 90.f);
            auto glmRot1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));


            // std::cout << "ROT1:\n " << rot1 << "\n";
            // std::cout << "ROT2:\n " << rot2 << "\n";

            REQUIRE(compare(clean(rot1), clean(glmRot1)));
            // REQUIRE(compare(clean(rot1), clean(rot2)));
        }

        {
            auto test  = glm_model * glm::vec4(1.2f,1.2f,1.2f, 1.f);
            auto test2 = geo_model * geo::ColVec<float,4>{{1.2f,1.2f,1.2f, 1.f}};
            REQUIRE(compare(test, test2));
        }
        {
            auto test  = glm::vec4(1.2f,1.2f,1.2f, 1.f) * glm_model;
            auto test2 = geo::RowVec<float,4>{{1.2f,1.2f,1.2f, 1.f}} * geo_model;
            REQUIRE(compare(test, test2));
        }



    }
}