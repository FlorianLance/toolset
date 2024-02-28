
/*******************************************************************************
** Toolset-base                                                               **
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


#include "camera.hpp"

using namespace tool::graphics;
using namespace tool::geo;

Camera::Camera(Screen *screen, geo::Pt3d position, geo::Vec3d direction, geo::Vec3d up) :
    m_screen(screen),m_position(position),m_up(up),m_direction(direction), m_initPosition(position), m_initUp(up), m_initDirection(direction) {
    update_right();
    update_view();
    update_projection();
}

auto Camera::look_at() const -> Mat4d{
    return geo::look_at(m_position, m_position+m_direction, m_up);
}

// auto Camera::generate_camera_matrices() const -> CameraMatrices{
//     CameraMatrices camM;
//     camM.v      = m_view;
//     camM.p      = m_projection;
//     camM.update();
//     return camM;
// }

auto Camera::move_up(double amount) -> void{
    m_position += m_up*amount;
    update_view();
}

auto Camera::move_down(double amount) -> void{
    m_position -= m_up*amount;
    update_view();
}

auto Camera::move_front(double amount) -> void{
    m_position += m_direction*amount;
    update_view();
}

auto Camera::move_left(double amount) -> void{
    m_position -= geo::cross(m_direction, m_up)*amount;
    update_view();
}

auto Camera::move_right(double amount) -> void{
    m_position += geo::cross(m_direction, m_up)*amount;
    update_view();
}

auto Camera::move_back(double amount) -> void{
    m_position -= m_direction*amount;
    update_view();
}

auto Camera::set_position(geo::Pt3d position) -> void{
    m_position = position;
    update_view();
}

auto Camera::set_up_vector(geo::Vec3d up) -> void{
    m_up = normalize(up);
    update_right();
    update_view();
}

auto Camera::set_direction(double yaw, double pitch, double roll) -> void{

    // compute direction vector
    m_direction = axis_angle_m3x3(m_up, yaw).multiply_vector(m_direction);
    m_direction = normalize(axis_angle_m3x3(m_right, pitch).multiply_vector(m_direction));
    // compute up vector
    update_up();
    m_up = axis_angle_m3x3(m_direction, roll).multiply_vector(m_up);
    // m_up.normalize();

    // update right vector
    update_right();
    // update view matrix
    update_view();
}

auto Camera::set_direction(geo::Vec3d direction, geo::Vec3d up) -> void{

    m_direction = normalize(direction);
    m_up        = normalize(up);

    // update right vector
    update_right();
    // update view matrix
    update_view();
}

auto Camera::set_mode(Mode mode) -> void{
    m_mode = mode;
    update_projection();
}

auto Camera::set_screen(Screen *screen) -> void{
    m_screen = screen;
    update_projection();
}

auto Camera::set_fov(double fov) -> void{
    m_fov = fov;
    if(m_mode == Mode::Perspective){
        update_projection();
    }
}

auto Camera::set_range(double min, double max) -> void{
    m_zRange = {min,max};
    update_projection();
}

auto Camera::update_projection() -> void{
    if(m_screen != nullptr){
        const auto ratio = 1.*m_screen->width()/ m_screen->height();
        if(m_mode == Mode::Perspective){
            m_projection = geo::perspective(deg_2_rad(m_fov), ratio, m_zRange.x(), m_zRange.y());
        }else if(m_mode == Mode::Orhtographic){
            m_projection = geo::orthographic(
                -ratio, ratio,
                -1., 1.,
                m_zRange.x(), m_zRange.y()
            );
        }
    }
}

auto Camera::reset_init_values() -> void{
    m_position    = m_initPosition;
    m_direction   = m_initDirection;
    m_up          = m_initUp;
    update_right();
    update_view();
}

auto Camera::update_right() -> void{
    m_right = geo::normalize(geo::cross(m_up, m_direction));
}

auto Camera::update_up() -> void{
    m_up    = geo::normalize(geo::cross(m_direction, m_right));
}

auto Camera::update_view() -> void{
    m_view = geo::look_at(m_position, m_position+m_direction, m_up);
}
