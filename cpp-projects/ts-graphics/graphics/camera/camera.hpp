
/*******************************************************************************
** Toolset-ts-graphics                                                        **
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
#include "screen.hpp"
#include "geometry/matrix4.hpp"
// #include "camera_matrices.hpp"

namespace tool::graphics{

class Camera{
public:

    enum class Mode : int{
        Perspective, Orhtographic
    };

    Camera(Screen *screen, geo::Pt3d position = {0.,0.,0.}, geo::Vec3d direction = {0.,0.,1.}, geo::Vec3d up = {0.,1.,0.});
    auto reset_init_values() -> void;

    // view    
    // # get
    [[nodiscard]] constexpr auto position() const noexcept -> geo::Pt3d{return m_position;}
    [[nodiscard]] constexpr auto up() const noexcept -> geo::Vec3d{return m_up;}
    [[nodiscard]] constexpr auto direction() const noexcept -> geo::Vec3d{return m_direction;}
    [[nodiscard]] constexpr auto view() const noexcept -> geo::Mat4d{return m_view;}
    [[nodiscard]] auto look_at() const -> geo::Mat4d;
    // # move
    auto move_up(double amount) -> void;
    auto move_down(double amount) -> void;
    auto move_front(double amount) -> void;
    auto move_left(double amount) -> void;
    auto move_right(double amount) -> void;
    auto move_back(double amount) -> void;
    // # set
    auto set_position(geo::Pt3d position) -> void;
    auto set_up_vector(geo::Vec3d up) -> void;
    auto set_direction(double yaw, double pitch, double roll) -> void;
    auto set_direction(geo::Vec3d direction, geo::Vec3d up = {0.,1.,0.}) -> void;

    // projection
    // # get
    [[nodiscard]] constexpr auto projection() const noexcept -> geo::Mat4d{return m_projection;}
    [[nodiscard]] constexpr auto fov() const noexcept -> double{return m_fov;}
    [[nodiscard]] constexpr auto z_range() const noexcept -> geo::Pt2d{return m_zRange;}
    [[nodiscard]] constexpr Screen *screen() const noexcept{return m_screen;}
    // # set
    auto set_mode(Mode mode) -> void;
    auto set_screen(Screen *screen) -> void;
    auto set_fov(double fov) -> void;
    auto set_range(double min, double max) -> void;
    // # update
    auto update_projection() -> void;


    auto screen_raycast(geo::Pt2<int> screenPos) -> geo::Vec3d;
    // static auto normal(const geo::Mat4d &modelView) -> geo::Mat3d{
    //     const auto &mv = modelView;
    //     return {
    //         mv.at(0,0),mv.at(0,1),mv.at(0,2),
    //         mv.at(1,0),mv.at(1,1),mv.at(1,2),
    //         mv.at(2,0),mv.at(2,1),mv.at(2,2),
    //     };
    // }

    // auto test() -> geo::Pt3d {
    //     geo::Pt4<int> pixel;

    //     auto cPix = pixel.conv<double>();
    //     cPix.x() /= m_screen->width();
    //     cPix.x() -= 0.5;
    //     cPix.x() *= 2.0;

    //     cPix.y() /= m_screen->height();
    //     cPix.y() -= 0.5;
    //     cPix.y() *= 2.0;

    //     cPix.z() = m_zRange.x();
    //     cPix.w() = 1.0;

    //     auto invP = inverse(m_projection);
    //     auto invV = inverse(m_view);

    //     cPix = invP.multiply_point(cPix);
    //     cPix = invV.multiply_point(cPix);
    //         // geo::Pt3d(m_screen->width(), m_screen->height());
    //     return cPix.
    // }


    // float imageAspectRatio = imageWidth / (float)imageHeight; // assuming width > height
    // float Px = (2 * ((x + 0.5) / imageWidth) - 1) * tan(fov / 2 * M_PI / 180) * imageAspectRatio;
    // float Py = (1 - 2 * ((y + 0.5) / imageHeight) * tan(fov / 2 * M_PI / 180);
    // Vec3f rayOrigin(0);
    // Vec3f rayDirection = Vec3f(Px, Py, -1) - rayOrigin; // note that this just equal to Vec3f(Px, Py, -1);
    // rayDirection = normalize(rayDirection); // it's a direction so don't forget to normalize

    // auto
    // auto generate_camera_matrices() const -> CameraMatrices;

    // rotation * (pixel.x - imageCentreX, pixel.y - imageCentreY, focalLength)

    // 0
    //
    // 0   0
    // inverse projection * inverse view

    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
    // auto dir_vector_from_pixel(double x, double y) -> geo::Pt3d{

    // }


private :

    auto update_right() -> void;
    auto update_up() -> void;
    auto update_view() -> void;

    Screen *m_screen = nullptr;

    geo::Pt3d m_position   = {0.,0.,0.};
    geo::Vec3d m_up        = {0.,1.,0.};
    geo::Vec3d m_direction = {0.,0.,1.};
    geo::Vec3d m_right;

    geo::Pt3d m_initPosition   = {0.,0.,0.};
    geo::Vec3d m_initUp        = {0.,1.,0.};
    geo::Vec3d m_initDirection = {1.,0.,0.};
    // geo::Mat4d m_initLookAt = geo::Mat4d::identity();

    geo::Mat4d m_view;
    geo::Mat4d m_projection;



    double m_fov = 60.0;
    geo::Pt2d m_zRange = {0.1, 10000.};

    Mode m_mode = Mode::Perspective;
};
}

