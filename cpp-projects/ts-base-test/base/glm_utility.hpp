
/*******************************************************************************
** Toolset-ts-base-test                                                       **
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

#pragma once

// std
#include <sstream>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_access.hpp>

// geometry
#include "geometry/point2.hpp"
#include "geometry/point3.hpp"
#include "geometry/point4.hpp"
#include "geometry/matrix2.hpp"
#include "geometry/matrix3.hpp"
#include "geometry/matrix4.hpp"
#include "geometry/quaternion.hpp"

namespace  tool {

template<typename acc, int _rows, int _cols>
auto to_string(const geo::Matrix<acc,_rows,_cols> &m) -> std::string{
    std::stringstream str;
    str << m;
    return str.str();
}

auto from_glm(const glm::vec2 &v) -> geo::Vec2f{
    return geo::Vec2f{v.x, v.y};
}

auto from_glm(const glm::vec3 &v) -> geo::Vec3f{
    return {v.x, v.y, v.z};
}

auto from_glm(const glm::vec4 &v) -> geo::Vec4f{
    return {v.x, v.y, v.z, v.w};
}

auto from_glm(const glm::quat &q) -> geo::Quatf{
    return {q.x, q.y, q.z, q.w};
}

auto from_glm(const glm::mat2 &m) -> geo::Mat2f{
    return {
        m[0][0], m[1][0],
        m[0][1], m[1][1]
    };
}

auto from_glm(const glm::mat3 &m) -> geo::Mat3f{
    return {
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2],
    };
}
auto from_glm(const glm::mat4 &m) -> geo::Mat4f{
    return {
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3],
    };
}

auto to_gml_mat4(const geo::Mat4f &m) -> glm::mat4{
    return glm::mat4(
        glm::vec4(m(0,0),m(1,0),m(2,0),m(3,0)),
        glm::vec4(m(0,1),m(1,1),m(2,1),m(3,1)),
        glm::vec4(m(0,2),m(1,2),m(2,2),m(3,2)),
        glm::vec4(m(0,3),m(1,3),m(2,3),m(3,3))
    );
}


auto compare(float v1, float v2) -> bool{
    return almost_equal(v1,v2);
}

auto compare(const glm::vec2 &v1, const geo::RowVec<float,2> &v2) -> bool{
    return
        compare(v1.x, v2.x()) &&
        compare(v1.y, v2.y());
}
auto compare(const geo::RowVec<float,2> &v1, const glm::vec2 &v2) -> bool{
    return compare(v2,v1);
}
auto compare(const glm::vec3 &v1, const geo::RowVec<float,3> &v2) -> bool{
    return
        compare(v1.x, v2.x()) &&
        compare(v1.y, v2.y()) &&
        compare(v1.z, v2.z());
}
auto compare(const geo::RowVec<float,3> &v1, const glm::vec3 &v2) -> bool{
    return compare(v2,v1);
}
auto compare(const glm::vec4 &v1, const geo::RowVec<float,4> &v2) -> bool{
    return
        compare(v1.x, v2.x()) &&
        compare(v1.y, v2.y()) &&
        compare(v1.z, v2.z()) &&
        compare(v1.w, v2.w());
}
auto compare(const geo::RowVec<float,4> &v1, const glm::vec4 &v2) -> bool{
    return compare(v2,v1);
}

//
auto compare(const glm::vec2 &v1, const geo::ColVec<float,2> &v2) -> bool{
    return
        compare(v1.x, v2.x()) &&
        compare(v1.y, v2.y());
}
auto compare(const geo::ColVec<float,2> &v1, const glm::vec2 &v2) -> bool{
    return compare(v2,v1);
}
auto compare(const glm::vec3 &v1, const geo::ColVec<float,3> &v2) -> bool{
    return
        compare(v1.x, v2.x()) &&
        compare(v1.y, v2.y()) &&
        compare(v1.z, v2.z());
}
auto compare(const geo::ColVec<float,3> &v1, const glm::vec3 &v2) -> bool{
    return compare(v2,v1);
}
auto compare(const glm::vec4 &v1, const geo::ColVec<float,4> &v2) -> bool{
    return
        compare(v1.x, v2.x()) &&
        compare(v1.y, v2.y()) &&
        compare(v1.z, v2.z()) &&
        compare(v1.w, v2.w());
}
auto compare(const geo::ColVec<float,4> &v1, const glm::vec4 &v2) -> bool{
    return compare(v2,v1);
}


auto compare(const glm::mat2 &m1, const geo::Mat2f &m2) -> bool{
    for(int r = 0; r < 2; ++r){
        for(int c = 0; c < 2; ++c){
            if(!compare(m1[c][r], m2(r,c))){
                return false;
            }
        }
    }
    return true;
}

auto compare(const geo::Mat2f &m1, const glm::mat2 &m2) -> bool{
    return compare(m2,m1);
}

auto compare(const glm::mat3 &m1, const geo::Mat3f &m2) -> bool{
    for(int r = 0; r < 3; ++r){
        for(int c = 0; c < 3; ++c){
            if(!compare(m1[c][r], m2(r,c))){
                return false;
            }
        }
    }
    return true;
}

auto compare(const geo::Mat3f &m1, const glm::mat3 &m2) -> bool{
    return compare(m2,m1);
}

auto compare(const glm::mat4 &m1, const geo::Mat4f &m2) -> bool{
    for(int r = 0; r < 4; ++r){
        for(int c = 0; c < 4; ++c){
            if(!compare(m1[c][r], m2(r,c))){
                return false;
            }
        }
    }
    return true;
}

auto compare(const geo::Mat4f &m1, const glm::mat4 &m2) -> bool{
    return compare(m2,m1);
}

//auto compare(const geo::Vec2f &v1, const geo::Vec2f &v2) -> bool{
//    return v1 == v2;
//}

//auto compare(const geo::Vec3f &v1, const geo::Vec3f &v2) -> bool{
//    return v1 == v2;
//}

//auto compare(const geo::Vec4f &v1, const geo::Vec4f &v2) -> bool{
//    return v1 == v2;
//}

//auto compare(const geo::Mat2f &m1, const geo::Mat2f &m2) -> bool{
//    return m1 == m2;
//}

//auto compare(const geo::Mat3f &m1, const geo::Mat3f &m2) -> bool{
//    return m1 == m2;
//}

//auto compare(const geo::Mat4f &m1, const geo::Mat4f &m2) -> bool{
//    return m1 == m2;
//}

auto clean(geo::Mat2f m) -> geo::Mat2f{
    for(int ii = 0; ii < 4; ++ii){
        if(m.at(ii) < 0.0000001f){
            m.at(ii) = 0.f;
        }
    }
    return m;
}

auto clean(geo::Mat3f m) -> geo::Mat3f{
    for(int ii = 0; ii < 9; ++ii){
        if(m.at(ii) < 0.0000001f){
            m.at(ii) = 0.f;
        }
    }
    return m;
}

auto clean(geo::Mat4f m) -> geo::Mat4f{
    for(int ii = 0; ii < 16; ++ii){
        if(m.at(ii) < 0.0000001f){
            m.at(ii) = 0.f;
        }
    }
    return m;
}

auto clean(glm::mat3 m) -> glm::mat3 {
    for(int ii = 0; ii < 3; ++ii){
        for(int jj = 0; jj < 3; ++jj){
            if(m[ii][jj] < 0.0000001f){
                m[ii][jj] = 0.f;
            }
        }
    }
    return m;
}

auto clean(glm::mat4 m) -> glm::mat4 {
    for(int ii = 0; ii < 4; ++ii){
        for(int jj = 0; jj < 4; ++jj){
            if(m[ii][jj] < 0.0000001f){
                m[ii][jj] = 0.f;
            }
        }
    }
    return m;
}

auto clean(geo::Quatf q) -> geo::Quatf{
    if(q.x < 0.0000001f){
        q.x = 0.f;
    }
    if(q.y < 0.0000001f){
        q.y = 0.f;
    }
    if(q.z < 0.0000001f){
        q.z = 0.f;
    }
    if(q.w < 0.0000001f){
        q.w = 0.f;
    }
    return q;
}

// TEST

auto glm_x_rotation(float value) -> glm::mat4{
    return glm::rotate(glm::mat4(1.0f), glm::radians(value), glm::vec3(1.0, 0.0, 0.0));
}
auto glm_y_rotation(float value) -> glm::mat4{
    return glm::rotate(glm::mat4(1.0f), glm::radians(value), glm::vec3(0.0, 1.0, 0.0));
}
auto glm_z_rotation(float value) -> glm::mat4{
    return glm::rotate(glm::mat4(1.0f), glm::radians(value), glm::vec3(0.0, 0.0, 1.0));
}

auto glm_rotate(const glm::vec3 &rotation) -> glm::mat4{
    return glm_z_rotation(rotation.z)*glm_x_rotation(rotation.x)*glm_y_rotation(rotation.y);
}

auto transform1(const geo::Vec3f &scale, const geo::Vec3f &rotation, const geo::Vec3f &translate) -> geo::Mat4f{
    return scale_m4x4(scale)*rotation_m4x4(rotation*PI_180<float>)*translation_m4x4(translate);
}
auto transform2(const geo::Vec3f &scale, const geo::Vec3f &rotation, const geo::Vec3f &translate) -> geo::Mat4f{
    return translation_m4x4(translate)*rotation_m4x4(rotation*PI_180<float>)*scale_m4x4(scale);
}
auto transform3(const geo::Vec3f &scale, const geo::Vec3f &rotation, const geo::Vec3f &translate) -> geo::Mat4f{
    geo::Mat4f tr = scale_m4x4(scale);
    tr      = tr * rotation_m4x4(rotation*PI_180<float>);
    tr      = tr * translation_m4x4(translate);
    return tr;
}


}
