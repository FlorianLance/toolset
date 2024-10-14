
/*******************************************************************************
** Toolset-test                                                                  **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

// std
#include <iostream>

// base
#include "thirdparty/catch/catch.hpp"
#include "utility/logger.hpp"

// local
#include "glm_utility.hpp"


using namespace tool;


//geo::Mat4f transform4(const geo::Vec3f &scale, const geo::Vec3f &rotation, const geo::Vec3f &translate){
//    auto tr = scale_matrix(scale);
//    tr *= rotation_matrix(rotation*PI_180<float>);
//    tr *= translation_matrix(translate);
//    return tr;
//}
//geo::Mat4f transform5(const geo::Vec3f &scale, const geo::Vec3f &rotation, const geo::Vec3f &translate){
//    return scale_matrix(scale)*(rotation_matrix(rotation*PI_180<float>)*translation_matrix(translate));
//}

//geo::Mat4f transform6(const geo::Vec3f &scale, const geo::Vec3f &rotation, const geo::Vec3f &translate){
//    auto tr = scale_matrix(scale);
//    tr      = rotation_matrix(rotation*PI_180<float>)*tr;
//    tr      = translation_matrix(translate)*tr;
//    return tr;
//}

//template <typename acc, int _rowsL, int _colsL, int _rowsR, int _colsR>
//geo::Matrix<acc,_rowsL, _colsR> multiply2(const geo::Matrix<acc,_rowsL,_colsL> &l, const geo::Matrix<acc,_rowsR,_colsR> &r){
//    geo::Matrix<acc,_rowsL, _colsR> res;
//    for(int ii = 0; ii < l.rows(); ++ii){
//        for(int jj = 0; jj < r.cols(); ++jj){
//            for(int kk = 0; kk < r.rows(); ++kk){
//                res(r.cols() * ii +jj) += l(l.cols() * ii + kk) * r(r.cols() * kk + jj);
//            }
//        }
//    }
//    return res;
//}
//template <typename acc, int _rowsL, int _colsL, int _rowsR, int _colsR>
//geo::Matrix<acc,_rowsL, _colsR> multiply3(const geo::Matrix<acc,_rowsL,_colsL> &r, const geo::Matrix<acc,_rowsR,_colsR> &l){
//    geo::Matrix<acc,_rowsL, _colsR> res;
//    for(int ii = 0; ii < l.rows(); ++ii){
//        for(int jj = 0; jj < r.cols(); ++jj){
//            for(int kk = 0; kk < r.rows(); ++kk){
//                res(r.cols() * ii +jj) += l(l.cols() * ii + kk) * r(r.cols() * kk + jj);
//            }
//        }
//    }
//    return res;
//}


TEST_CASE("geo::Point"){ Logger::message("geo::Point\n");

    SECTION("geo::Point2 constructors"){ Logger::message("geo::Point2 constructors\n");

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
    SECTION("geo::Point2 operators"){ Logger::message("geo::Point2 operators\n");

        constexpr auto pt12  = geo::Point<float,2>{{1.f,2.f}};
        constexpr auto pt21  = geo::Point<float,2>{{2.f,1.f}};

        {constexpr auto res = pt12 + pt21;  REQUIRE(res == geo::Pt2f{3,3});}
        {constexpr auto res = pt12 + 1.f;   REQUIRE(res == geo::Pt2f{2,3});}
        {constexpr auto res = pt12 - pt21;  REQUIRE(res == geo::Pt2f{-1,1});}
        {constexpr auto res = pt12 - 1.f;   REQUIRE(res == geo::Pt2f{0,1});}
        {constexpr auto res = 2.f * pt12;   REQUIRE(res == geo::Pt2f{2,4});}
        {constexpr auto res = pt12 * 2.f;   REQUIRE(res == geo::Pt2f{2,4});}
        {constexpr auto res = pt12 / 2.f;   REQUIRE(res == geo::Pt2f{0.5,1});}

        // {constexpr auto res = 1.f + pt12;   REQUIRE(res == geo::Pt2f{2,3});} // NOK
        // {constexpr auto res = 1.f - pt12;   REQUIRE(res == geo::Pt2f{0,1});} // NOK
        // {constexpr auto res = 2.f / pt12;   REQUIRE(res == geo::Pt2f{2,4});} // NOK
        // {constexpr auto res = pt12 * pt21;} // NOK
    }
    SECTION("geo::Point2 getters"){ Logger::message("geo::Point2 getters\n");
        constexpr auto pt12  = geo::Pt2f{1.f,2.f};
        pt12.x();
        pt12.y();
//        pt12.z(); // NOK
//        pt12.xyz(); // NOK
    }
    SECTION("geo::Point2 GLM comparison"){ Logger::message("geo::Point2 GLM comparison\n");

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
//        REQUIRE(from_glm(glmPt1/glmPt2) == (pt3/pt4));
        // functions
        REQUIRE(glm::dot(glmPt1,glmPt2) == geo::dot(pt1,pt2));

//        REQUIRE(glm::cross(glmPt1,glmPt2) == geo::cross(pt1,pt2));

        Logger::message(std::format("dotglm {} dot {}\n", glm::dot(glmPt1,glmPt2), geo::dot(pt1,pt2)));
        Logger::message(std::format("pt3 {}\n", to_string(pt1)));
        Logger::message(std::format("pt4 {}\n", to_string(pt2)));



        REQUIRE(from_glm(-glmPt1) == invert(pt1));
        REQUIRE(tool::almost_equal(glm::length(glmPt1)*glm::length(glmPt1),geo::square_norm(pt1)));
        REQUIRE(from_glm(glm::normalize(glmPt1)) == geo::normalize(pt1));
        REQUIRE(from_glm(glm::normalize(glm::vec2(4,5))) == normalize(geo::Pt2f{4,5}));
    }



    SECTION("geo::Point3"){ Logger::message("geo::Point3\n");

        {
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
        }
        {
            constexpr glm::vec3 glmPt1(1,2,3);
            constexpr glm::vec3 glmPt2(4,5,6);
            constexpr geo::Pt3f pt1{1,2,3};
            constexpr geo::Pt3f pt2{4,5,6};
            constexpr auto dotR = geo::dot(pt1,pt2);
            constexpr auto inv  = invert(pt1);
            constexpr auto sqn  = geo::square_norm(pt1);

            constexpr glm::vec4 glmPt3(1,2,3,1);
            constexpr glm::vec4 glmPt4(4,5,6,1);

            Logger::message(std::format("cross {}\n", to_string(geo::cross(pt1,pt2))));
    //        Logger::message(std::format("glm cross {}\n", to_string(from_glm(glm::cross(glmPt3,glmPt4)))));
    //        const glm::vec3 v(0);
    //        auto v2 = glm::normalize(v);
    //        // getters
    //        REQUIRE(glmPt2.x == pt2.x());
    //        REQUIRE(glmPt2.y == pt2.y());
    //        REQUIRE(glmPt2.z == pt2.z());
    //        // operators
    //        REQUIRE(from_glm(glmPt1) == pt1);
    //        REQUIRE(from_glm(glmPt2) == pt2);
    //        REQUIRE(from_glm(-glmPt2) == -pt2);
    //        REQUIRE(from_glm(glmPt1+glmPt2) == (pt1+pt2));
    //        REQUIRE(from_glm(glmPt1-glmPt2) == (pt1-pt2));
    //        REQUIRE(from_glm(glmPt1*glmPt2) == (pt1*pt2));
    //        REQUIRE(from_glm(glmPt1/glmPt2) == (pt1/pt2));
    //        REQUIRE(from_glm(glmPt1+glmPt2) == (add(pt1,pt2)));
    //        REQUIRE(from_glm(glmPt1-glmPt2) == (substract(pt1,pt2)));
    //        REQUIRE(from_glm(glmPt1*glmPt2) == (multiply(pt1,pt2)));
    //        REQUIRE(from_glm(glmPt1/glmPt2) == (divide(pt1,pt2)));
    //        // functions
    //        REQUIRE(glm::dot(glmPt1,glmPt2) == geo::dot(pt1,pt2));
    //        REQUIRE(from_glm(glm::cross(glmPt1,glmPt2)) == geo::cross(pt1,pt2));
    //        REQUIRE(from_glm(-glmPt1) == invert(pt1));
    //        REQUIRE(tool::almost_equal(glm::length(glmPt1)*glm::length(glmPt1),sqn));
    //        REQUIRE(from_glm(glm::normalize(glmPt1)) == geo::normalize(pt1));
    //        REQUIRE(from_glm(glm::normalize(glm::vec3(4,5,6))) == normalize(geo::Pt3f{4,5,6}));
            Logger::message(std::format("cross {}\n", to_string(geo::cross(pt1,pt2))));
            Logger::message(std::format("glm cross {}\n", to_string(from_glm(glm::cross(glmPt1,glmPt2)))));
            Logger::message(std::format("glm mul {}\n", to_string(from_glm(glmPt1*glmPt2))));
        }
    }

    SECTION("Point4"){

//        Logger::message("geo::Point operations: Point4\n");
//        constexpr glm::vec4 pt1(1,2,3,4);
//        constexpr glm::vec4 pt2(5,6,7,8);
//        constexpr geo::Pt4f pt3{1,2,3,4};
//        constexpr geo::Pt4f pt4{5,6,7,8};
//        constexpr auto dotR = geo::dot(pt3,pt4);
//        constexpr auto inv  = invert(pt3);
//        constexpr auto sqn  = geo::square_norm(pt3);

//        const glm::vec3 v(0);
//        auto v2 = glm::normalize(v);
//        // getters
//        REQUIRE(pt2.x == pt4.x());
//        REQUIRE(pt2.y == pt4.y());
//        REQUIRE(pt2.z == pt4.z());
//        REQUIRE(pt2.w == pt4.w());
//        // operators
//        REQUIRE(from_glm(pt1) == pt3);
//        REQUIRE(from_glm(pt2) == pt4);
//        REQUIRE(from_glm(-pt2) == -pt4);
//        REQUIRE(from_glm(pt1+pt2) == (pt3+pt4));
//        REQUIRE(from_glm(pt1-pt2) == (pt3-pt4));
//        REQUIRE(from_glm(pt1*pt2) == (pt3*pt4));
//        REQUIRE(from_glm(pt1/pt2) == (pt3/pt4));
//        REQUIRE(from_glm(pt1+pt2) == (add(pt3,pt4)));
//        REQUIRE(from_glm(pt1-pt2) == (substract(pt3,pt4)));
//        REQUIRE(from_glm(pt1*pt2) == (multiply(pt3,pt4)));
//        REQUIRE(from_glm(pt1/pt2) == (divide(pt3,pt4)));
//        // functions
//        REQUIRE(glm::dot(pt1,pt2) == geo::dot(pt3,pt4));
//        REQUIRE(from_glm(-pt1) == invert(pt3));
//        REQUIRE(tool::almost_equal(glm::length(pt1)*glm::length(pt1),sqn));
//        REQUIRE(from_glm(glm::normalize(pt1)) == geo::normalize(pt3));
//        REQUIRE(from_glm(glm::normalize(glm::vec4(5,6,7,8))) == normalize(geo::Pt4f{5,6,7,8}));
    }
}

TEST_CASE("geo::Matrix"){ Logger::message("geo::Matrix\n");

    SECTION("Constructor"){ Logger::message("geo::Matrix - Constructor\n");

        constexpr auto pt00 = geo::Point<float,2>{{0.f,0.f}};
        REQUIRE(almost_equal(pt00.x(),0.f));
        REQUIRE(almost_equal(pt00.y(),0.f));
        constexpr auto pt10 = geo::Point<float,2>{{1.f}};
        REQUIRE(almost_equal(pt10.x(),1.f));
        REQUIRE(almost_equal(pt10.y(),0.f));
        constexpr auto pt12 = geo::Point<float,2>{{1.f,2.f}};
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

    SECTION("Functions"){ Logger::message("geo::Matrix - Functions\n");

//        {constexpr auto m = geo::Matrix<float, 3,3>::identity();}
//        {constexpr geo::Matrix<float,1, 2> r = m1.row(0);}

    }

    SECTION("Matrix 2x2"){ Logger::message("geo::Mat2x2\n");

        constexpr geo::Mat2f m1{
            7.f,2,
            0,3
        };
        constexpr geo::Mat2f m2{
            3.f,1,
            -2,7
        };
        constexpr auto glmM1 = glm::mat2(
            glm::vec2(7,0),
            glm::vec2(2,3)
        );
        constexpr auto glmM2 = glm::mat2(
            glm::vec2(3,-2),
            glm::vec2(1,7)
        );
        constexpr auto altglmM1 = glm::mat2(
            7,0,
            2,3
        );
        REQUIRE(glmM1 == altglmM1);

        REQUIRE(compare(m1.col(0), glm::column(glmM1,0)));
        REQUIRE(compare(m1.col(1), glm::column(glmM1,1)));
        REQUIRE(compare(m1.row(0), glm::row(glmM1,0)));
        REQUIRE(compare(m1.row(1), glm::row(glmM1,1)));
        REQUIRE(compare(m1,glmM1));
        REQUIRE(compare(m1,from_glm(glmM1)));        



        REQUIRE(compare(m2.col(0), glm::column(glmM2,0)));
        REQUIRE(compare(m2.col(1), glm::column(glmM2,1)));
        REQUIRE(compare(m2.row(0), glm::row(glmM2,0)));
        REQUIRE(compare(m2.row(1), glm::row(glmM2,1)));
        REQUIRE(compare(m2,glmM2));
        REQUIRE(compare(m2,from_glm(glmM2)));

        REQUIRE(determinant(m1) == glm::determinant(glmM1));
        REQUIRE(compare(inverse(m1),glm::inverse(glmM1)));

        REQUIRE(compare(m1*m2, glmM1*glmM2));
        REQUIRE(compare(m2*m1, glmM2*glmM1));

        constexpr auto v1    =  geo::Vec2f{1.3f,5.f};
        constexpr auto glmV1 =  glm::vec2(1.3f,5.f);
        REQUIRE(compare(v1, glmV1));
        REQUIRE(compare(v1*m1, glmV1*glmM1));
        REQUIRE(compare(m1*m2, multiply(m1,m2)));
        REQUIRE(compare(m2*m1, multiply(m2,m1)));
        REQUIRE(compare(m1*transpose(v1), glmM1*glmV1));


//        v1
//        m1*transpose(v1);

//        geo::Mat2f m3{
//            cos(PI<float>/2.f),  sin(PI<float>/2.f),
//            -sin(PI<float>/2.f), cos(PI<float>/2.f)
//        };
//        geo::Mat2f m4{
//            1,0,
//            0,-1
//        };

//        Logger::message(to_string(m3*m4));
//        Logger::message(to_string(m4*m3));
    }

    SECTION("Matrix 3x3"){ Logger::message("geo::Mat3x3\n");

        constexpr geo::Mat3f m1{
            7.f,2,1,
            0,3,-1,
            -3,4,-2
        };
        constexpr geo::Mat3f m2{
            3.f,1,-4,
            -2,7,5,
            1.2f,4,-4
        };
        constexpr auto glmM1 = glm::mat3(
            glm::vec3(7,0,-3),
            glm::vec3(2,3,4),
            glm::vec3(1,-1,-2)
        );
        constexpr auto glmM2 = glm::mat3(
            glm::vec3(3,-2,1.2),
            glm::vec3(1,7,4),
            glm::vec3(-4,5,-4)
        );
        constexpr auto altglmM1 = glm::mat3(
            7,0,-3,
            2,3,4,
            1,-1,-2
        );

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
        REQUIRE(compare(m1*m2, multiply(m1,m2)));
        REQUIRE(compare(m2*m1, multiply(m2,m1)));
        REQUIRE(compare(m1*transpose(v1), glmM1*glmV1));

//        REQUIRE(compare(m1.multiply_point(v1), v1*m1));
    }

    SECTION("Matrix 4x4"){ Logger::message("geo::Mat4x4\n");

        constexpr geo::Mat4f m1{
            7.f,2,1,-4,
            0,3,-1,1,
            -3,4,-2,-3,
            -4,4,6.2f,1.2f
        };
        constexpr geo::Mat4f m2{
            3.f,1,-4,-5,
            -2,7,5,-1,
            1.2f,4,-4,1.2f,
            3.6f,-1.5,4.2f,2
        };
        constexpr auto glmM1 = glm::mat4(
            glm::vec4(7,0,-3,-4),
            glm::vec4(2,3,4,4),
            glm::vec4(1,-1,-2,6.2),
            glm::vec4(-4,1,-3,1.2)
        );
        constexpr auto glmM2 = glm::mat4(
            glm::vec4(3,-2,1.2,3.6),
            glm::vec4(1,7,4,-1.5),
            glm::vec4(-4,5,-4,4.2),
            glm::vec4(-5,-1,1.2,2)
        );
        constexpr auto altglmM1 = glm::mat4(
            7,0,-3,-4,
            2,3,4,4,
            1,-1,-2,6.2,
            -4,1,-3,1.2
        );

        REQUIRE(glmM1 == altglmM1);


        std::cout << "mmm:\n" << to_string(transpose(transpose(m1))) << "\n";
        std::cout << to_string(from_glm(glmM1)) << "\n";
        REQUIRE(compare(from_glm(to_gml_mat4(m1)),from_glm(glmM1)));
        REQUIRE(compare(transpose(transpose(m1)),from_glm(glmM1)));

        REQUIRE(compare(m1.col(0), glm::column(glmM1,0)));
        REQUIRE(compare(m1.col(1), glm::column(glmM1,1)));
        REQUIRE(compare(m1.col(2), glm::column(glmM1,2)));
        REQUIRE(compare(m1.col(3), glm::column(glmM1,3)));
        REQUIRE(compare(m1.row(0), glm::row(glmM1,0)));
        REQUIRE(compare(m1.row(1), glm::row(glmM1,1)));
        REQUIRE(compare(m1.row(2), glm::row(glmM1,2)));
        REQUIRE(compare(m1.row(3), glm::row(glmM1,3)));
        REQUIRE(compare(m1,glmM1));
        REQUIRE(compare(m1,from_glm(glmM1)));

        REQUIRE(compare(m2.col(0), glm::column(glmM2,0)));
        REQUIRE(compare(m2.col(1), glm::column(glmM2,1)));
        REQUIRE(compare(m2.col(2), glm::column(glmM2,2)));
        REQUIRE(compare(m2.col(3), glm::column(glmM2,3)));
        REQUIRE(compare(m2.row(0), glm::row(glmM2,0)));
        REQUIRE(compare(m2.row(1), glm::row(glmM2,1)));
        REQUIRE(compare(m2.row(2), glm::row(glmM2,2)));
        REQUIRE(compare(m2.row(3), glm::row(glmM2,3)));
        REQUIRE(compare(m2,glmM2));
        REQUIRE(compare(m2,from_glm(glmM2)));

        REQUIRE(determinant(m1) == glm::determinant(glmM1));
        REQUIRE(compare(inverse(m1),glm::inverse(glmM1)));
        REQUIRE(compare(m1*m2, glmM1*glmM2));
        REQUIRE(compare(m2*m1, glmM2*glmM1));

        constexpr auto v1    =  geo::Vec4f{1.3f,5.f,-2.f,2.5f};
        constexpr auto glmV1 =  glm::vec4(1.3f,5.f,-2.f,2.5f);
        REQUIRE(compare(v1, glmV1));
        REQUIRE(compare(v1*m1, glmV1*glmM1));

        REQUIRE(compare(m1*m2, multiply(m1,m2)));
        REQUIRE(compare(m2*m1, multiply(m2,m1)));
        REQUIRE(compare(v1*m1, glmV1*glmM1));
        REQUIRE(compare(m1*transpose(v1), glmM1*glmV1));


    }

    SECTION("Look at"){ Logger::message("Look at\n");
        auto glmLookAt = glm::lookAt(
            glm::vec3(-2.0f, 4.0f, -1.0f),
            glm::vec3( 0.0f, 2.0f,  0.0f),
            glm::vec3( 0.0f, 1.0f,  0.0f)
        );


        auto lootAt = look_at(
            geo::Pt3f{-2.0f, 4.0f, -1.0f},
            geo::Pt3f{0.0f, 2.0f,  0.0f},
            geo::Vec3f{0.0f, 1.0f,  0.0f}
        );

//        std::cout << from_glm(glmLookAt) << "\n";
//        std::cout << lootAt << "\n";
//        REQUIRE(compare(clean(glmLookAt),clean(lootAt)));
    }



    SECTION("Transform combination 1"){Logger::message("Transform combination 1\n");

        // scale
        auto scale    = geo::scale(geo::Mat4f::identity(),geo::Vec3f{0.5f, 0.5f, 0.5f});
        auto glmScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));;
        REQUIRE(compare(clean(scale),clean(glmScale)));

        // translate
        auto tr    = geo::translate(geo::Mat4f::identity(), {{-1.f, 17.f, 56.f}});
        auto glmTr = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 17.0, 56.0));
        REQUIRE(compare(tr,glmTr));
        REQUIRE(compare(geo::translation_m4x4(geo::Vec3f{-1.f, 17.f, 56.f}),tr));

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

//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));

//        trans1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
//        trans2 = geo::Mat4f::identity()*geo::Mat4f::rotation_matrix({0.f,90.f*tool::PI_180<float>,0.f});
//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));

//        trans1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
//        trans2 = geo::Mat4f::identity()*geo::Mat4f::rotation_matrix({0.f,0.f,90.f*tool::PI_180<float>});
//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));

        // combination 1
        auto glmC1  = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
        auto c1     = geo::scale(geo::Mat4f::identity(), geo::Vec3f{0.5f, 0.5f, 0.5f});
        REQUIRE(compare(clean(c1),clean(glmC1)));
        glmC1       = glm::rotate(glmC1, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        c1          = geo::rotate(c1, geo::Vec3f{1.f,0.f,0.f}, 90.f);

//        std::cout << "RR:\n";
////        Logger::message(to_string(clean(from_glm(glmC1))));
//        Logger::message(to_string( geo::translate(geo::Mat4f::identity(), {{-1.0, 17.0, 56.0}})));
//        Logger::message(to_string( from_glm(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 17.0, 56.0)))));
////        CHECK(compare(c1,glmC1));

        glmC1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
        c1 = geo::scale(geo::Mat4f::identity(), {{0.5f, 0.5f, 0.5f}});
        CHECK(compare(c1,glmC1));
        glmC1 = glm::rotate(glmC1, glm::radians(-47.0f), glm::vec3(1.0, 0.0, 0.0));
        c1 = geo::rotate(c1, geo::Vec3f{1.f,0.f,0.f}, -47.f);
        CHECK(compare(c1,glmC1));
                                       Logger::message(to_string( c1));
                                       Logger::message(to_string( from_glm(glmC1)));
        glmC1 = glm::translate(glmC1, glm::vec3(-1.0, 17.0, 56.0));
        c1 = geo::translate(c1, geo::Vec3f{-1.0, 17.0, 56.0});
//        CHECK(compare(c1,glmC1));

        auto glm_tr = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 2.6f, -10.5));
        auto glm_sc = glm::scale(glm::mat4(1.0f), glm::vec3(1.2f,1.2f,1.2f));
        auto glm_rx = glm::rotate(glm::mat4(1.0f), glm::radians(-45.f), glm::vec3(1.0, 0.0, 0.0));
        auto glm_ry = glm::rotate(glm::mat4(1.0f), glm::radians(70.6f), glm::vec3(0.0, 1.0, 0.0));
        auto glm_rz = glm::rotate(glm::mat4(1.0f), glm::radians(7.9f), glm::vec3(0.0, 0.0, 1.0));
        auto glm_model =  glm_tr * (glm_rz*glm_rx*glm_ry) * glm_sc;

        auto geo_tr = geo::translate(geo::Mat4f::identity(), geo::Vec3f{1.5f, 2.6f, -10.5f});
        auto geo_scale = geo::scale(geo::Mat4f::identity(), geo::Vec3f{1.2f,1.2f,1.2f});
        auto geo_rot = geo::rotate(geo::Mat4f::identity(), geo::Vec3f{-45.f,70.6f,7.9f});
        auto geo_model = geo_tr * geo_rot * geo_scale;

//        std::cout << "### TRC:\n " << to_string(geo_model) << "\n" << to_string(from_glm(glm_model)) << "\n";
//        CHECK(compare(clean(geo_model),clean(glm_model)));

        auto glmP = glm::perspective(60.f*PI_180<float>, 1.4f, 0.02f, 1000.f);
        auto p1 = geo::perspective(60.f*PI_180<float>, 1.4f, 0.02f, 1000.f);
//        Logger::message(to_string(clean(from_glm(glmP))));
//        Logger::message(to_string(clean(p1)));
        CHECK(compare(clean(glmP),clean(p1)));


//        c1 = geo::rotate(c1, {{-47.f,0.f,0.f}});



//       std::cout << "RRé:\n";
//       Logger::message(to_string(clean(from_glm(glmC1))));
//       Logger::message(to_string(clean(c1)));


    }

//    SECTION("Transformation comparison"){

//        // rotation matrices
//        trans1 = glm_x_rotation(87.f);
//        trans2 = geo::Mat3f::x_rotation_matrix(87.f);
//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));

//        trans1 = glm_y_rotation(87.f);
//        trans2 = geo::Mat3f::y_rotation_matrix(87.f);
//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));

//        trans1 = glm_z_rotation(87.f);
//        trans2 = geo::Mat3f::z_rotation_matrix(87.f);
//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));

//        trans1 = glm_rotate(glm::vec3(10.0, 20., -30.));
//        trans2 = geo::Mat4f::rotate(geo::Mat4f::identity(), {10.0, 20., -30.});
//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));

//        // scale matrix
//        trans1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
//        trans2 = geo::Mat4f::scale(geo::Mat4f::identity(), {0.5f, 0.5f, 0.5f});
//        trans3 = geo::Mat4f::scale_matrix({0.5f, 0.5f, 0.5f});

//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));
//        REQUIRE(clean(trans2) == clean(trans3));

//        // translate matrix
//        trans1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 17.0, 56.0));
//        trans2 = geo::Mat4f::translate(geo::Mat4f::identity(), {-1.f, 17.f, 56.f});
//        trans3 = geo::Mat4f::translation_matrix({-1.f, 17.f, 56.f});

//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));
//        REQUIRE(clean(trans2) == clean(trans3));




//        trans3 = geo::Mat4f::scale_matrix({0.5f, 0.5f, 0.5f});
//        trans3 = geo::Mat4f::rotation_matrix(geo::Vec3f{-47.f,0.f,0.f}*tool::PI_180<float>)*trans3;
//        trans3 = geo::Mat4f::translation_matrix({-1.0, 17.0, 56.0})* trans3;

//        REQUIRE(clean(from_glm(trans1)) == clean(trans2));
//        REQUIRE(clean(trans2) == clean(trans3));

//        trans4 = geo::Mat4f::transform({0.5f, 0.5f, 0.5f}, {-47.f,0.f,0.f}, {-1.0, 17.0, 56.0});
//        CHECK(clean(trans3) == clean(trans4));
//    }


    auto gq1 = glm::angleAxis(glm::radians(67.f),  glm::vec3(1.f, 0.f, 0.f));
    auto gq2 = glm::angleAxis(glm::radians(135.f), glm::vec3(0.f, 1.f, 0.f));
    auto gq3 = glm::angleAxis(glm::radians(-54.f), glm::vec3(0.f, 0.f, 1.f));
    auto gq4 = glm::angleAxis(glm::radians(58.f),  glm::normalize(glm::vec3(1.f,0.6f,-0.5f)));

    auto q1 = geo::Quatf::from_axis({1.f,0,0}, 67.f);
    auto q2 = geo::Quatf::from_axis({0.f,1,0}, 135.f);
    auto q3 = geo::Quatf::from_axis({0.f,0,1}, -54.f);
    auto q4 = geo::Quatf::from_axis(normalize(geo::Vec3f{1.f,0.6f,-0.5f}), 58.f);

//    auto e1 = euler_angles(q1);
//    auto e2 = euler_angles(q2);
//    auto e3 = euler_angles(q3);
//    auto e4 = euler_angles(q4);

    SECTION("Quaternion"){ Logger::message("Quaternion\n");

        REQUIRE(from_glm(gq1) == q1);
        REQUIRE(from_glm(gq2) == q2);
        REQUIRE(from_glm(gq3) == q3);

        REQUIRE(angle(q1) == glm::angle(gq1));
        REQUIRE(angle(q2) == glm::angle(gq2));
        REQUIRE(angle(q3) == glm::angle(gq3));

        REQUIRE(axis(q1) == from_glm(glm::axis(gq1)));
        REQUIRE(axis(q2) == from_glm(glm::axis(gq2)));
        REQUIRE(axis(q3) == from_glm(glm::axis(gq3)));

        REQUIRE(norm(q1) == glm::length(gq1));
        REQUIRE(norm(q2) == glm::length(gq2));
        REQUIRE(norm(q3) == glm::length(gq3));

        REQUIRE((q1 + q2) == from_glm(gq1 + gq2));
        REQUIRE((q1 - q2) == from_glm(gq1 - gq2));
        REQUIRE((q1 * q2) == from_glm(gq1 * gq2));
        REQUIRE((q1 * 5.f) == from_glm(gq1 * 5.f));
        REQUIRE((q1 / 5.f) == from_glm(gq1 / 5.f));

        REQUIRE(normalize(q1) == from_glm(glm::normalize(gq1)));
        REQUIRE(normalize(q2) == from_glm(glm::normalize(gq2)));
        REQUIRE(normalize(q3) == from_glm(glm::normalize(gq3)));

        REQUIRE(inverse(q1) == from_glm(glm::inverse(gq1)));
        REQUIRE(inverse(q2) == from_glm(glm::inverse(gq2)));
        REQUIRE(inverse(q3) == from_glm(glm::inverse(gq3)));

        REQUIRE(dot(q1,q2) == glm::dot(gq1, gq2));
        REQUIRE(dot(q2,q1) == glm::dot(gq2, gq1));
        REQUIRE(dot(q3,q2) == glm::dot(gq3, gq2));

        REQUIRE(conjugate(q1) == from_glm(glm::conjugate(gq1)));
        REQUIRE(conjugate(q2) == from_glm(glm::conjugate(gq2)));
        REQUIRE(conjugate(q3) == from_glm(glm::conjugate(gq3)));

        REQUIRE(slerp(q1,q2, 0.4f) == from_glm(glm::slerp(gq1, gq2, 0.4f)));
        REQUIRE(slerp(q1,q3, 0.19f) == from_glm(glm::slerp(gq1, gq3, 0.19f)));
        REQUIRE(slerp(q2,q3, 0.89f) == from_glm(glm::slerp(gq2, gq3, 0.89f)));

        REQUIRE(pitch(q1) == (glm::pitch(gq1)));
        REQUIRE(pitch(q2) == (glm::pitch(gq2)));
        REQUIRE(pitch(q3) == (glm::pitch(gq3)));
        REQUIRE(pitch(q4) == (glm::pitch(gq4)));

        REQUIRE(yaw(q1) == (glm::yaw(gq1)));
        REQUIRE(yaw(q2) == (glm::yaw(gq2)));
        REQUIRE(yaw(q3) == (glm::yaw(gq3)));
        REQUIRE(yaw(q4) == (glm::yaw(gq4)));

        REQUIRE(roll(q1) == (glm::roll(gq1)));
        REQUIRE(roll(q2) == (glm::roll(gq2)));
        REQUIRE(roll(q3) == (glm::roll(gq3)));
        REQUIRE(roll(q4) == (glm::roll(gq4)));

        REQUIRE(euler_angles(q1) == from_glm(glm::eulerAngles(gq1)));
        REQUIRE(euler_angles(q2) == from_glm(glm::eulerAngles(gq2)));
        REQUIRE(euler_angles(q3) == from_glm(glm::eulerAngles(gq3)));
        REQUIRE(euler_angles(q4) == from_glm(glm::eulerAngles(gq4)));

//        CHECK(clean(geo::Quatf::from_euler({rad_2_deg(e1.x()),rad_2_deg(e1.y()),rad_2_deg(e1.z())})) == q1);
//        CHECK(clean(geo::Quatf::from_euler({rad_2_deg(e2.x()),rad_2_deg(e2.y()),rad_2_deg(e2.z())})) == q2);
//        CHECK(clean(geo::Quatf::from_euler({rad_2_deg(e3.x()),rad_2_deg(e3.y()),rad_2_deg(e3.z())})) == q3);
//        CHECK(clean(geo::Quatf::from_euler({rad_2_deg(e4.x()),rad_2_deg(e4.y()),rad_2_deg(e4.z())})) == q4);

//        REQUIRE(to_mat4(q1) == from_glm(glm::toMat4(gq1)));
//        REQUIRE(to_mat4(q2) == from_glm(glm::toMat4(gq2)));
//        REQUIRE(to_mat4(q3) == from_glm(glm::toMat4(gq3)));
//        REQUIRE(to_mat4(q4) == from_glm(glm::toMat4(gq4)));

//        REQUIRE(to_quaternion(to_mat4(q1)) == from_glm(gq1));
//        REQUIRE(to_quaternion(to_mat4(q2)) == from_glm(gq2));
//        REQUIRE(to_quaternion(to_mat4(q3)) == from_glm(gq3));
//        REQUIRE(to_quaternion(to_mat4(q4)) == from_glm(gq4));
    }

}

#include "geometry/color_cloud.hpp"
#include "depth-camera/frame/dc_frame.hpp"
#include "geometry/color_voxel_grid.hpp"

template <typename acc, int _rows, int _cols>
constexpr auto ccc(const geo::Matrix<acc,_rows,_cols> &lhs, const geo::Matrix<acc, _rows, _cols> &rhs) noexcept -> bool{
    if(lhs != rhs){
        return std::lexicographical_compare(lhs.array.begin(), lhs.array.end(), rhs.array.begin(), rhs.array.end());
    }
    return false;
}

// turbopfor
// #include "TurboPFor/vp4.h"

bool comparator(const tool::geo::CVoxel& lhs, const tool::geo::CVoxel& rhs) {
    auto *c1 = reinterpret_cast<const std::uint64_t*>(&lhs);
    auto *c2 = reinterpret_cast<const std::uint64_t*>(&rhs);
    return *c1 < *c2;
}

TEST_CASE("geo::Cloud"){ Logger::message("geo::Cloud\n");
    SECTION("ColoredCloudData"){ Logger::message("geo::ColoredCloudData\n");
        geo::ColorCloud cloud;
//        cloud.vertices.reserve(100);
//        for(size_t ii = 0; ii < 100; ++ii){
//            cloud.vertices.emplace_back(100.f-ii,2.f*ii,3.f*ii);
//        }

        cloud.vertices.push_back({2.f,1.f,3.f});
        cloud.vertices.push_back({-5.f, 3.f, -2.f});
        cloud.vertices.push_back({9.f, 7.f, 0.f});
        cloud.vertices.push_back({0.f, 0.f, 0.f});

        auto pt1 = cloud.vertices[0];
        auto pt2 = cloud.vertices[1];
        auto pt3 = cloud.vertices[2];

        cloud.vertices.resize(5);
        Logger::message(std::format("<: {}\n", pt1 < pt1));
        Logger::message(std::format(">: {}\n", pt1 > pt2));
        Logger::message(std::format("<: {}\n", pt1 < pt3));
        Logger::message(std::format(">: {}\n", pt1 > pt3));

        geo::Pt3f pt{1.f,2.f,3.f};
        Logger::message(std::format("sum: {}\n", geo::sum(pt)));

        Logger::message(std::format("sum: {}\n", geo::sum(pt1)));
        Logger::message(std::format("sum: {}\n", geo::sum(pt2)));

        Logger::message(std::format("sum: {}\n", to_string(cloud.vertices.sum())));
        Logger::message(std::format("mean: {}\n", to_string(cloud.vertices.mean_position())));
        Logger::message(std::format("minx: {}\n", std::to_string(cloud.vertices.min_x())));
        Logger::message(std::format("maxx: {}\n", std::to_string(cloud.vertices.max_x())));
        Logger::message(std::format("miny: {}\n", std::to_string(cloud.vertices.min_y())));
        Logger::message(std::format("maxy: {}\n", std::to_string(cloud.vertices.max_y())));
        Logger::message(std::format("minz: {}\n", std::to_string(cloud.vertices.min_z())));
        Logger::message(std::format("maxz: {}\n", std::to_string(cloud.vertices.max_z())));
        Logger::message(std::format("min: {}\n", to_string(cloud.vertices.min())));
        Logger::message(std::format("max: {}\n", to_string(cloud.vertices.max())));

        Logger::message(std::format("minx_id: {}\n", cloud.vertices.min_x_id()));
        Logger::message(std::format("maxx_id: {}\n", cloud.vertices.max_x_id()));
        Logger::message(std::format("miny_id: {}\n", cloud.vertices.min_y_id()));
        Logger::message(std::format("maxy_id: {}\n", cloud.vertices.max_y_id()));
        Logger::message(std::format("minz_id: {}\n", cloud.vertices.min_z_id()));
        Logger::message(std::format("maxz_id: {}\n", cloud.vertices.max_z_id()));
        Logger::message(std::format("min_id: {}\n", cloud.vertices.min_id()));
        Logger::message(std::format("max_id: {}\n", cloud.vertices.max_id()));
        
        
        cam::DCDeprecatedFrame f;
        for(size_t ii = 0; ii < 10000; ++ii){
//            f.cloud.vertices.push_back({(rand()%1000)*0.001f,(rand()%2000)*0.001f,(rand()%3000)*0.001f});
//            f.cloud.colors.push_back({(rand()%1000)*0.001f,(rand()%1000)*0.001f,(rand()%1000)*0.001f});

            f.cloud.vertices.push_back({(rand()%1000)*0.001f,(rand()%2000)*0.001f,(rand()%3000)*0.001f});
            f.cloud.colors.push_back({(rand()%1000)*0.001f,(rand()%1000)*0.001f,(rand()%1000)*0.001f});

        }

        Logger::message("TEST VOXELISATION\n");
        auto minBound = tool::geo::Pt3f{-1.f,-1.f,-1.f};
        auto maxBound = tool::geo::Pt3f{1.f,1.f,1.f};
        tool::geo::ColorVoxelGrid voxelGrid = tool::geo::ColorVoxelGrid::create_from_point_cloud_within_bounds(
            f.cloud,
            0.1f,
            minBound,
            maxBound
        );

        Logger::message(std::format("frame cloud: {}\n", cloud.vertices.size()));
        Logger::message(std::format("minx_id: {}\n", f.cloud.vertices.min_x()));
        Logger::message(std::format("maxx_id: {}\n", f.cloud.vertices.max_x()));
        Logger::message(std::format("miny_id: {}\n", f.cloud.vertices.min_y()));
        Logger::message(std::format("maxy_id: {}\n", f.cloud.vertices.max_y()));
        Logger::message(std::format("minz_id: {}\n", f.cloud.vertices.min_z()));
        Logger::message(std::format("maxz_id: {}\n", f.cloud.vertices.max_z()));

        Logger::message(std::format("voxel size: {}\n", sizeof(geo::Voxel)));
        Logger::message(std::format("CVoxel size: {}\n", sizeof(geo::CVoxel)));
        Logger::message(std::format("std::int64_t size: {}\n", sizeof(std::int64_t)));
        Logger::message(std::format("std::int32_t size: {}\n", sizeof(std::int32_t)));
        Logger::message(std::format("geo::Pt3<int> size: {}\n", sizeof(geo::Pt3<int>)));
        Logger::message(std::format("geo::Pt3f size: {}\n", sizeof(geo::Pt3f)));

        std::vector<tool::geo::CVoxel> cvoxels;
        cvoxels.reserve(voxelGrid.grid.size());
        for(const auto &grid : voxelGrid.grid){

            const auto &id = grid.second.index;
            const auto &col = grid.second.color;
            tool::geo::CVoxel cv;
            cv.red   = static_cast<int>(col.x()*255.f);
            cv.green = static_cast<int>(col.y()*255.f);
            cv.blue  = static_cast<int>(col.z()*255.f);
            cv.xIndex= id.x();
            cv.yIndex= id.y();
            cv.zIndex= id.z();
            cvoxels.push_back(cv);
        }

        // pass it to sort:
        sort(cvoxels.begin(), cvoxels.end(), &comparator);

        cvoxels.resize(cvoxels.size() + 128- cvoxels.size()%128);

        std::vector<std::uint8_t> encoded;
        encoded.resize(cvoxels.size()*16);
        Logger::message(std::format("cvoxels size: {} : bytes {}\n", cvoxels.size(), cvoxels.size()*sizeof(tool::geo::CVoxel)));
        Logger::message(std::format("encoded size: {}\n", encoded.size()));

        // size_t encodedBytesNb = p4nzenc64(
        //     reinterpret_cast<std::uint64_t*>(cvoxels.data()),
        //     cvoxels.size(),
        //     encoded.data()
        // );
        // Logger::message(std::format("p4nzenc64: {}\n", encodedBytesNb));

        //  encodedBytesNb = p4nd1enc64(
        //      reinterpret_cast<std::uint64_t*>(cvoxels.data()),
        //      cvoxels.size(),
        //      encoded.data()
        //  );
        //  Logger::message(std::format("p4nd1enc64: {}\n", encodedBytesNb));

        // encodedBytesNb = p4ndenc64(
        //   reinterpret_cast<std::uint64_t*>(cvoxels.data()),
        //   cvoxels.size(),
        //   encoded.data()
        // );
        // Logger::message(std::format("p4ndenc64: {}\n", encodedBytesNb));

        //  encodedBytesNb = p4nenc64(
        //    reinterpret_cast<std::uint64_t*>(cvoxels.data()),
        //    cvoxels.size(),
        //    encoded.data()
        //  );
        //  Logger::message(std::format("p4nenc64: {}\n", encodedBytesNb));

        //   encodedBytesNb = p4nenc128v64(
        //     reinterpret_cast<std::uint64_t*>(cvoxels.data()),
        //     cvoxels.size(),
        //     encoded.data()
        //   );
        //   Logger::message(std::format("p4nenc128v64: {}\n", encodedBytesNb));


//                                  p4nenc128v64
//                                 // compress depth buffer
//                                 cFrame->encodedDepthData.resize(depthSize*2);

//                                 // depth sizes for every mode have already a 128 padded size
//                                 size_t encodedBytesNb = p4nzenc128v16(
//                                     depthBuffer,
//                                     depthSize,
//                                     cFrame->encodedDepthData.data()
//                                 );
//                                 cFrame->encodedDepthData.resize(encodedBytesNb);

        //  // init cloud from voxel grid
        //  cloud.vertices.resize(grid.grid.size());
        //  cloud.colors.resize(grid.grid.size());

        //  size_t idVoxel = 0;
        //  for(const auto &voxel : grid.grid){

        //      const auto &id = voxel.second.index;
        //      cloud.vertices[idVoxel] = (minBound +
        //          geo::Pt3f{static_cast<float>(id.x()), static_cast<float>(id.y()), static_cast<float>(id.z())})
        //          *static_cast<float>(grid.voxelSize);
        //      cloud.colors[idVoxel++] = voxel.second.color;
        //  }


    }
}
