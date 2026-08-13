
/*******************************************************************************
** Toolset-ts-base                                                            **                      **
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

// local
#include "geometry/matrix4.hpp"
#include "utility/math.hpp"

namespace tool::geo {

template<typename acc>
struct Quaternion;

template<typename acc>
using Quat  = Quaternion<acc>;
using Quatf = Quat<float>;
using Quatd = Quat<double>;

template <typename acc>
auto normalize(const Quaternion<acc> &q) -> Quaternion<acc>;

template<class acc>
struct Quaternion{

    acc w{1}; /**< scalar   */
    acc x{0}; /**< i factor */
    acc y{0}; /**< j factor */
    acc z{0}; /**< k factor */    

    Quaternion() = default;
    constexpr Quaternion(acc w, acc x, acc y, acc z) : w(w), x(x), y(y), z(z){}

    Quaternion(Vec3<acc> eulerAngle){
        // rad
        // pitch / yaw / roll
        auto e = eulerAngle * acc{0.5};
        auto c = Vec3<acc>{std::cos(e.x()),std::cos(e.y()),std::cos(e.z())};
        auto s = Vec3<acc>{std::sin(e.x()),std::sin(e.y()),std::sin(e.z())};
        w = c.x() * c.y() * c.z() + s.x() * s.y() * s.z();
        x = s.x() * c.y() * c.z() - c.x() * s.y() * s.z();
        y = c.x() * s.y() * c.z() + s.x() * c.y() * s.z();
        z = c.x() * c.y() * s.z() - s.x() * s.y() * c.z();
    }

    static auto from_look_at(const Mat4<acc> &lookat) -> Quaternion{

        const auto &m = lookat;
        acc fourXSquaredMinus1 = m(0,0) - m(1,1) - m(2,2);
        acc fourYSquaredMinus1 = m(1,1) - m(0,0) - m(2,2);
        acc fourZSquaredMinus1 = m(2,2) - m(0,0) - m(1,1);
        acc fourWSquaredMinus1 = m(0,0) + m(1,1) + m(2,2);

        int biggestIndex = 0;
        acc fourBiggestSquaredMinus1 = fourWSquaredMinus1;
        if(fourXSquaredMinus1 > fourBiggestSquaredMinus1){
            fourBiggestSquaredMinus1 = fourXSquaredMinus1;
            biggestIndex = 1;
        }
        if(fourYSquaredMinus1 > fourBiggestSquaredMinus1){
            fourBiggestSquaredMinus1 = fourYSquaredMinus1;
            biggestIndex = 2;
        }
        if(fourZSquaredMinus1 > fourBiggestSquaredMinus1){
            fourBiggestSquaredMinus1 = fourZSquaredMinus1;
            biggestIndex = 3;
        }

        acc biggestVal = std::sqrt(fourBiggestSquaredMinus1 + static_cast<acc>(1)) * static_cast<acc>(0.5);
        acc mult       = static_cast<acc>(0.25) / biggestVal;

        switch(biggestIndex){
        case 0:
            return {biggestVal, (m(2,1) - m(1,2)) * mult, (m(0,2) - m(2,0)) * mult, (m(1,0) - m(0,1)) * mult};
        case 1:
            return {(m(2,1) - m(1,2)) * mult, biggestVal, (m(1,0) + m(0,1)) * mult, (m(0,2) + m(2,0)) * mult};
        case 2:
            return {(m(0,2) - m(2,0)) * mult, (m(1,0) + m(0,1)) * mult, biggestVal, (m(2,1) + m(1,2)) * mult};
        case 3:
            return {(m(1,0) - m(0,1)) * mult, (m(0,2) + m(2,0)) * mult, (m(2,1) + m(1,2)) * mult, biggestVal};
        default:
            // should never actually get here
            return {1, 0, 0, 0};
        }
    }


    static auto from_axis(const Vec3<acc> &axe, const acc angle) -> Quaternion{
        const acc radAngle = deg_2_rad(angle/2);
        const acc s = std::sin(radAngle);
        return {cos(radAngle), axe.x() * s, axe.y() * s, axe.z() * s};
    }

    static auto from_euler(acc yaw, acc pitch, acc roll) -> Quaternion{
        auto yawQuat   = geo::Quat<acc>::from_axis(geo::Vec3<acc>{0, 1, 0}, yaw);
        auto pitchQuat = geo::Quat<acc>::from_axis(geo::Vec3<acc>{1, 0, 0}, pitch);
        auto rollQuat  = geo::Quat<acc>::from_axis(geo::Vec3<acc>{0, 0, 1}, roll);
        return geo::normalize(rollQuat * pitchQuat * yawQuat * Quaternion());
    }    

    // operators
    constexpr auto operator+() const noexcept -> Quaternion{return *this;}
    constexpr auto operator-() const noexcept -> Quaternion{return {-w, -x, -y, -z};}
    constexpr auto operator+=(acc value) noexcept -> Quaternion&{(*this) = *this + value;return *this;}
    constexpr auto operator-=(acc value) noexcept -> Quaternion&{(*this) = *this - value;return *this;}
    constexpr auto operator*=(acc value) noexcept -> Quaternion&{(*this) = *this * value;return *this;}
    constexpr auto operator/=(acc value) -> Quaternion&{(*this) = *this / value;return *this;}
    constexpr auto operator+=(const Quaternion &q) noexcept -> Quaternion&{(*this) = *this + q;return *this;}
    constexpr auto operator-=(const Quaternion &q) noexcept -> Quaternion&{(*this) = *this + q;return *this;}
    constexpr auto operator*=(const Quaternion &q) noexcept -> Quaternion&{(*this) = *this * q;return *this;}
};

// functions
template <typename acc>
[[nodiscard]] constexpr auto add(const Quaternion<acc> &q1, const Quaternion<acc> &q2) noexcept -> Quaternion<acc>{
    return {q1.w+q2.w, q1.x+q2.x,q1.y+q2.y,q1.z+q2.z};
}

template <typename acc>
[[nodiscard]] constexpr auto add(const Quaternion<acc> &q, acc value) noexcept -> Quaternion<acc>{
    return {q.w, q.x+value, q.y, q.z};
}

template <typename acc>
[[nodiscard]] constexpr auto substract(const Quaternion<acc> &q1, const Quaternion<acc> &q2) noexcept -> Quaternion<acc>{
    return {q1.w-q2.w, q1.x-q2.x,q1.y-q2.y,q1.z-q2.z};
}

template <typename acc>
[[nodiscard]] constexpr auto substract(const Quaternion<acc> &q, acc value) noexcept -> Quaternion<acc>{
    return {q.w, q.x-value, q.y, q.z};
}

template <typename acc>
[[nodiscard]] constexpr auto multiply(const Quaternion<acc> &q, acc value) noexcept -> Quaternion<acc>{
    return {q.w*value, q.x*value, q.y*value, q.z*value};
}

template <typename acc>
[[nodiscard]] constexpr auto rotate(const Quaternion<acc> &q, const Vec3<acc> &v) noexcept -> Vec3<acc>{
    auto u = geo::Vec3<acc>{q.x, q.y, q.z};
    return {
        acc{2} * dot(u, v) * u      +
        (q.w*q.w - dot(u, u)) * v   +
        acc{2} * q.w * cross(u, v)
    };
}

template <typename acc>
[[nodiscard]] constexpr auto multiply(const Quaternion<acc> &q1, const Quaternion<acc> &q2) noexcept -> Quaternion<acc>{
    return {
        q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z,
        q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y,
        q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x,
        q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w
    };
}

template <typename acc>
[[nodiscard]] constexpr auto divide(const Quaternion<acc> &q, acc value) noexcept -> Quaternion<acc>{
    if(value > acc{0}){
        return {q.w/value, q.x/value, q.y/value, q.z/value};
    }
    return q;
}

template <typename acc>
[[nodiscard]] constexpr auto compare(const Quaternion<acc> &l, const Quaternion<acc> &r, int ulp = 3) noexcept -> bool{
    return
        almost_equal(l.w, r.w, ulp) &&
        almost_equal(l.x, r.x, ulp) &&
        almost_equal(l.y, r.y, ulp) &&
        almost_equal(l.z, r.z, ulp);
}

template <typename acc>
[[nodiscard]] constexpr auto square_norm(const Quaternion<acc> &q) noexcept -> acc{
    return (q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
}

template <typename acc>
[[nodiscard]] auto norm(const Quaternion<acc> &q) noexcept -> acc{
    return sqrt(square_norm(q));
}

template <typename acc>
[[nodiscard]] constexpr auto conjugate(const Quaternion<acc> &q) noexcept -> Quaternion<acc>{
    return {q.w, -q.x,-q.y,-q.z};
}

template <typename acc>
[[nodiscard]] constexpr auto dot(const Quaternion<acc> &q1, const Quaternion<acc> &q2) noexcept -> acc{
    return {q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z};
}

template <typename acc>
[[nodiscard]] auto normalize(const Quaternion<acc> &q) -> Quaternion<acc>{
    return divide(q, norm(q));
}

template <typename acc>
[[nodiscard]] auto inverse(const Quaternion<acc> &q) -> Quaternion<acc>{
    return conjugate(normalize(q));
}

template <typename acc>
[[nodiscard]] auto angle(const Quaternion<acc> &q) noexcept -> acc{
    static constexpr acc cosOneOverTwo = static_cast<acc>(0.87758256189037271613028606820350319);
    if (std::abs(q.w) > cosOneOverTwo){
        return std::asin(sqrt(q.x * q.x + q.y * q.y + q.z * q.z)) * acc{2};
    }
    return std::acos(q.w) * acc{2};
}

template <typename acc>
[[nodiscard]] constexpr auto axis(const Quaternion<acc> &q) noexcept -> Vec3<acc>{
    auto q1 = normalize(q);
    return normalize(Vec3<acc>{q1.x,q1.y,q1.z});
}

template <typename acc>
[[nodiscard]] constexpr auto pitch(const Quaternion<acc> &q) -> acc{

    const acc y = acc{2} * (q.y * q.z + q.w * q.x);
    const acc x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

    if(almost_equal(x,acc{0}) && almost_equal(y,acc{0})){
        return acc{2} * std::atan2(q.x, q.w);
    }
    return std::atan2(y, x);
}

template <typename acc>
[[nodiscard]] constexpr auto yaw(const Quaternion<acc> &q) -> acc{
    return std::asin(std::clamp(acc{-2} * (q.x * q.z - q.w * q.y),acc{-1}, acc{1}));
}

template <typename acc>
[[nodiscard]] constexpr auto roll(const Quaternion<acc> &q) -> acc{
    return std::atan2(acc{2} * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);
}

template <typename acc>
[[nodiscard]] constexpr auto euler_angles(const Quaternion<acc> &q) -> Vec3<acc>{
    return {pitch(q),yaw(q),roll(q)};
}

template <typename acc>
[[nodiscard]] auto slerp(const Quaternion<acc> &q1, const Quaternion<acc> &q2, acc t) -> Quaternion<acc>{

    // only unit quaternions are valid rotations.
    Quaternion<acc> v1 = normalize(q1);
    Quaternion<acc> v2 = normalize(q2);

    // compute the cosine of the angle between the two vectors.
    acc dotV = dot(v1, v2);

    // if the dot product is negative, slerp won't take
    // the shorter path. Note that v0 and -v0 are equivalent when
    // the negation is applied to all four components. Fix by
    // reversing one quaternion.
    if (dotV < 0) {
        v2 = -v2;
        dotV = -dotV;
    }

    const acc dotThreshold = static_cast<acc>(0.9995);
    if (dotV > dotThreshold){
        // if the inputs are too close for comfort, linearly interpolateand normalize the result.
        return normalize(v1 + (v2 - v1)*t);
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    acc theta_0 = acos(dotV);        // theta_0 = angle between input vectors
    acc theta = theta_0*t;          // theta = angle between v0 and result
    acc sin_theta = sin(theta);     // compute this value only once
    acc sin_theta_0 = sin(theta_0); // compute this value only once
    acc s0 = cos(theta) - dotV * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    acc s1 = sin_theta / sin_theta_0;

    return (v1*s0) + (v2*s1);
}

template<typename acc>
[[nodiscard]] constexpr auto rotation_m4x4(const Quaternion<acc> &q) noexcept  -> Mat4<acc>{

    acc qxx{q.x * q.x};
    acc qyy{q.y * q.y};
    acc qzz{q.z * q.z};
    acc qxz{q.x * q.z};
    acc qxy{q.x * q.y};
    acc qyz{q.y * q.z};
    acc qwx{q.w * q.x};
    acc qwy{q.w * q.y};
    acc qwz{q.w * q.z};

    return {
        acc{1} - acc{2} * (qyy +  qzz), acc{2} * (qxy - qwz),               acc{2} * (qxz + qwy),           0,
        acc{2} * (qxy + qwz),           acc(1) - acc{2} * (qxx +  qzz),     acc{2} * (qyz - qwx),           0,
        acc{2} * (qxz - qwy),           acc{2} * (qyz + qwx),               acc{1} - acc{2} * (qxx +  qyy), 0,
        0,                              0,                                  0,                              1
    };
}

// operators
template <typename acc>
[[nodiscard]] constexpr auto operator+(const Quaternion<acc> &q, acc value) noexcept -> Quaternion<acc>{
    return add(q,value);
}

template <typename acc>
[[nodiscard]] constexpr auto operator+(const Quaternion<acc> &q1, const Quaternion<acc> &q2) noexcept -> Quaternion<acc>{
    return add(q1,q2);
}

template <typename acc>
[[nodiscard]] constexpr auto operator-(const Quaternion<acc> &q, acc value) noexcept -> Quaternion<acc>{
    return substract(q,value);
}

template <typename acc>
[[nodiscard]] constexpr auto operator-(const Quaternion<acc> &q1, const Quaternion<acc> &q2) noexcept -> Quaternion<acc>{
    return substract(q1,q2);
}

template <typename acc>
[[nodiscard]] constexpr auto operator*(const Quaternion<acc> &q, acc value) noexcept -> Quaternion<acc>{
    return multiply(q, value);
}

template <typename acc>
[[nodiscard]] constexpr auto operator*(const Quaternion<acc> &q1, const Quaternion<acc> &q2) noexcept -> Quaternion<acc>{
    return multiply(q1,q2);
}

template <typename acc>
[[nodiscard]] constexpr auto operator/(const Quaternion<acc> &q, acc value) -> Quaternion<acc>{
    return divide(q, value);
}

template <typename acc>
[[nodiscard]] constexpr static auto operator==(const Quaternion<acc> &l, const Quaternion<acc> &r) -> bool{
    return compare(l,r);
}

template <typename acc>
auto operator<<(std::ostream &flux, const Quaternion<acc> &q) -> std::ostream&{
    flux <<"[" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << "]";
    return flux;
}

}


