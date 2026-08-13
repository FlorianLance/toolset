
// std
#include <iostream>

// catch
#include <catch/catch.hpp>

// local
#include "utility/logger.hpp"
#include "base/glm_utility.hpp"
#include "geometry/quaternion.hpp"

using namespace tool;

using gv3f = geo::Vec3f;
using glv3f = glm::fvec3;
using gv3d = geo::Vec3d;
using glv3d = glm::dvec3;

static inline const auto v1  = geo::Vec3d{0.0, 0.0, -1.0};
static inline const auto gv1 = glm::dvec3(0.0, 0.0, -1.0);

static inline const auto gq1 = glm::angleAxis(glm::radians(67.),  glm::dvec3(1., 0., 0.));
static inline const auto gq2 = glm::angleAxis(glm::radians(135.), glm::dvec3(0., 1., 0.));
static inline const auto gq3 = glm::angleAxis(glm::radians(-54.), glm::dvec3(0., 0., 1.));
static inline const auto gq4 = glm::angleAxis(glm::radians(58.),  glm::normalize(glm::dvec3(1.,0.6,-0.5)));

static inline const auto q1 = geo::Quatd::from_axis({1.,0,0}, 67.);
static inline const auto q2 = geo::Quatd::from_axis({0.,1,0}, 135.);
static inline const auto q3 = geo::Quatd::from_axis({0.,0,1}, -54.);
static inline const auto q4 = geo::Quatd::from_axis(normalize(geo::Vec3d{1.,0.6,-0.5}), 58.0);

// T w, T x, T y, T z

static inline const auto e1 = geo::euler_angles(q1);
static inline const auto e2 = geo::euler_angles(q2);
static inline const auto e3 = geo::euler_angles(q3);
static inline const auto e4 = geo::euler_angles(q4);

TEST_CASE("B. geo::Quaternion"){
    Log::message("geo::Quaternion\n");

    SECTION("All"){
        Log::message("  All\n");

        // rotation = glm::quat_cast(rotation);

        REQUIRE(from_glm(gq1) == q1);
        REQUIRE(from_glm(gq2) == q2);
        REQUIRE(from_glm(gq3) == q3);

        // angle
        REQUIRE(geo::angle(q1) == glm::angle(gq1));
        REQUIRE(geo::angle(q2) == glm::angle(gq2));
        REQUIRE(geo::angle(q3) == glm::angle(gq3));

        // axis
        REQUIRE(geo::axis(q1) == from_glm(glm::axis(gq1)));
        REQUIRE(geo::axis(q2) == from_glm(glm::axis(gq2)));
        REQUIRE(geo::axis(q3) == from_glm(glm::axis(gq3)));

        // norm
        REQUIRE(geo::norm(q1) == glm::length(gq1));
        REQUIRE(geo::norm(q2) == glm::length(gq2));
        REQUIRE(geo::norm(q3) == glm::length(gq3));

        // operators
        REQUIRE((q1 + q2) == from_glm(gq1 + gq2));
        REQUIRE((q1 - q2) == from_glm(gq1 - gq2));
        REQUIRE((q1 * q2) == from_glm(gq1 * gq2));
        REQUIRE((q2 * q1) == from_glm(gq2 * gq1));
        REQUIRE((q1 * 5.) == from_glm(gq1 * 5.));
        REQUIRE((q1 / 5.) == from_glm(gq1 / 5.));

        // normalize
        REQUIRE(geo::normalize(q1) == from_glm(glm::normalize(gq1)));
        REQUIRE(geo::normalize(q2) == from_glm(glm::normalize(gq2)));
        REQUIRE(geo::normalize(q3) == from_glm(glm::normalize(gq3)));

        // inverse
        REQUIRE(geo::inverse(q1) == from_glm(glm::inverse(gq1)));
        REQUIRE(geo::inverse(q2) == from_glm(glm::inverse(gq2)));
        REQUIRE(geo::inverse(q3) == from_glm(glm::inverse(gq3)));

        // dot
        REQUIRE(geo::dot(q1,q2) == glm::dot(gq1, gq2));
        REQUIRE(geo::dot(q2,q1) == glm::dot(gq2, gq1));
        REQUIRE(geo::dot(q3,q2) == glm::dot(gq3, gq2));

        // conjugate
        REQUIRE(geo::conjugate(q1) == from_glm(glm::conjugate(gq1)));
        REQUIRE(geo::conjugate(q2) == from_glm(glm::conjugate(gq2)));
        REQUIRE(geo::conjugate(q3) == from_glm(glm::conjugate(gq3)));

        // slerp
        REQUIRE(geo::slerp(q1,q2, 0.4) == from_glm(glm::slerp(gq1, gq2, 0.4)));
        REQUIRE(geo::slerp(q1,q3, 0.19) == from_glm(glm::slerp(gq1, gq3, 0.19)));
        REQUIRE(geo::slerp(q2,q3, 0.89) == from_glm(glm::slerp(gq2, gq3, 0.89)));

        // pitch
        REQUIRE(compare(geo::pitch(q1), (glm::pitch(gq1))));
        REQUIRE(compare(geo::pitch(q2), (glm::pitch(gq2))));
        REQUIRE(compare(geo::pitch(q3), (glm::pitch(gq3))));
        REQUIRE(compare(geo::pitch(q4), (glm::pitch(gq4))));

        // yaw
        REQUIRE(compare(geo::yaw(q1), (glm::yaw(gq1))));
        REQUIRE(compare(geo::yaw(q2), (glm::yaw(gq2))));
        REQUIRE(compare(geo::yaw(q3), (glm::yaw(gq3))));
        REQUIRE(compare(geo::yaw(q4), (glm::yaw(gq4))));

        // roll
        REQUIRE(compare(geo::roll(q1), (glm::roll(gq1))));
        REQUIRE(compare(geo::roll(q2), (glm::roll(gq2))));
        REQUIRE(compare(geo::roll(q3), (glm::roll(gq3))));
        REQUIRE(compare(geo::roll(q4), (glm::roll(gq4))));

        // euler angles
        REQUIRE(geo::euler_angles(q1) == from_glm(glm::eulerAngles(gq1)));
        REQUIRE(geo::euler_angles(q2) == from_glm(glm::eulerAngles(gq2)));
        REQUIRE(geo::euler_angles(q3) == from_glm(glm::eulerAngles(gq3)));
        REQUIRE(geo::euler_angles(q4) == from_glm(glm::eulerAngles(gq4)));

        // rotate
        REQUIRE(geo::rotate(q1, v1) == from_glm(gq1 * gv1));
        REQUIRE(geo::rotate(q1, v1) == from_glm(glm::rotate(gq1, gv1)));
        REQUIRE(geo::rotate(q2, v1) == from_glm(glm::rotate(gq2, gv1)));
        REQUIRE(geo::rotate(q3, v1) == from_glm(glm::rotate(gq3, gv1)));
        REQUIRE(geo::rotate(q4, v1) == from_glm(glm::rotate(gq4, gv1)));

        {
            geo::Quaternion<double> orientation(1.0,0,0,0);
            auto yawQuat   = geo::Quatd::from_axis(geo::Vec3d{0., 1., 0.}, 10.);
            auto pitchQuat = geo::Quatd::from_axis(geo::Vec3d{1., 0., 0.}, -15.);
            auto rollQuat  = geo::Quatd::from_axis(geo::Vec3d{0., 0., 1.}, 45.);
            orientation    = geo::normalize(rollQuat * pitchQuat * yawQuat * orientation);

            REQUIRE(orientation == geo::Quatd::from_euler(10., -15., 45.));


            glm::dquat gorientation{1,0,0,0};
            glm::dquat gyawQuat      = glm::angleAxis(glm::radians(10.),    glm::dvec3(0., 1., 0.));
            glm::dquat gpitchQuat    = glm::angleAxis(glm::radians(-15.),   glm::dvec3(1., 0., 0.));
            glm::dquat grollQuat     = glm::angleAxis(glm::radians(45.),    glm::dvec3(0., 0., 1.));
            gorientation = glm::normalize(grollQuat * gpitchQuat * gyawQuat * gorientation);

            REQUIRE(yawQuat == from_glm(gyawQuat));
            REQUIRE(pitchQuat == from_glm(gpitchQuat));
            REQUIRE(rollQuat == from_glm(grollQuat));
            REQUIRE((rollQuat * pitchQuat) == (from_glm(grollQuat * gpitchQuat)));
            REQUIRE((rollQuat * pitchQuat * yawQuat) == (from_glm(grollQuat * gpitchQuat * gyawQuat)));
            REQUIRE(orientation == from_glm(gorientation));
        }


        REQUIRE(from_glm(glm::mat4_cast(gq1)) == rotation_m4x4(q1));

        {
            // glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

            glm::fvec3 position(1.0f, 2.0f, 3.0f);    // Translation
            glm::fvec3 rotation(45.0f, 10.0f, -12.0f);   // Euler angles (x, y, z)
            glm::fvec3 scale(2.0f, 2.0f, 2.0f);       // Scaling factors

            glm::fmat4 modelMatrix =
                glm::translate(glm::mat4(1.0f), position) *
                glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                // glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
                glm::scale(glm::mat4(1.0f), scale
            );

            geo::Mat4f transfo = geo::transform(
                geo::Vec3f{2.0f, 2.0f, 2.0f},
                // geo::Vec3f{45.0f, 10.0f, -12.0f},
                geo::Vec3f{0.f, 10.0f, -12.0f},
                geo::Vec3f{1.0f, 2.0f, 3.0f}
            );

            // std::cout << "modelMatrix " << from_glm(modelMatrix) << "\n";
            // std::cout << "transfo " << transfo << "\n";
            // REQUIRE(from_glm(modelMatrix) == transfo);


            auto r1 = geo::rotation_m4x4(geo::Vec3f{45.0f, 10.0f, -12.0f});
            // auto r2 = geo::rotation_m4x4(geo::Quatf::from_euler(45.0f, 10.0f, -12.0f));
            // auto r2 = geo::rotation_m4x4(geo::Quatf::from_euler(45.0f, 10.0f, -12.0f));

            // auto v = glm::cos(glm::vec3(1.0f, 0.0f, 0.0f));

            auto eAngles = geo::Vec3f{deg_2_rad(45.0f), deg_2_rad(10.0f), deg_2_rad(-12.0f)};

            // Convert degrees to radians
            glm::fvec3 eulerAnglesRad = glm::fvec3(
                eAngles.x(),  // Pitch (X-axis)
                eAngles.y(),  // Yaw (Y-axis)
                eAngles.z()   // Roll (Z-axis)
            );

            // Create quaternion from Euler angles (Pitch, Yaw, Roll order)
            glm::fquat quaternion = glm::fquat(eulerAnglesRad);
            geo::Quatf qq(eAngles);
            REQUIRE(from_glm(quaternion) == qq);
            // std::cout << "r1 " << r1 << "\n";
            // std::cout << "r2 " << r2 << "\n";

            // geo::Quatd::from_euler(10., -15., 45.));
        }

        auto glookat = glm::lookAt(glv3f(-2.0f, 4.0f, -1.0f),glv3f( 0.0f, 2.0f,  0.0f), glv3f(0.0f, 1.0f,  0.0f));
        auto lookat = look_at(gv3f{-2.0f, 4.0f, -1.0f},gv3f{  0.0f, 2.0f,  0.0f}, gv3f{ 0.0f, 1.0f,  0.0f});
        std::cout << "mat quat cast\n";
        std::cout << geo::Quatf::from_look_at(lookat) << "\n";
        std::cout << from_glm(glm::quat_cast(glookat)) << "\n";
        REQUIRE(geo::Quatf::from_look_at(lookat) == from_glm(glm::quat_cast(glookat)));

        // inline glm::mat4 rotationMatrix(const Camera& cam) {
            // return glm::mat4_cast(cam.rotation);
        // }

        // // Combine quaternions (order matters: Yaw → Pitch → Roll)
        // m_orientation = normalize(rollQuat * pitchQuat * yawQuat * m_orientation);


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