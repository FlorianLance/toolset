
/*******************************************************************************
** Toolset-ts-base                                                            **
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
#include "quaternion.hpp"

namespace tool::geo{

class Camera{
public:

    // order: yaw / pitch / roll
    Camera(geo::Pt3d position = {0.,0.,0.}, geo::Pt3d eulerAngles = {0.,0.,0.}){

        // store initial
        m_initPosition = position;
        m_initRotation = eulerAngles;

        // apply
        m_position = m_initPosition;
        rotate(m_initRotation);

        update_vectors();
    }

    auto reset_init_values() noexcept -> void{
        m_position    = m_initPosition;
        m_orientation = {};
        rotate(m_initRotation);
        update_vectors();
    }

    // view    
    // # get
    [[nodiscard]] constexpr auto position()             const noexcept  -> geo::Pt3d{   return m_position;}
    [[nodiscard]] constexpr auto up()                   const noexcept  -> geo::Vec3d{  return m_up;}
    [[nodiscard]] constexpr auto direction()            const noexcept  -> geo::Vec3d{  return m_direction;}
    [[nodiscard]] constexpr auto view_matrix()          const noexcept  -> geo::Mat4d{  return geo::look_at(m_position, m_position + m_direction, m_up);}
    [[nodiscard]] constexpr auto translation_matrix()   const noexcept  -> Mat4d{       return geo::translation_m4x4(m_position);}
    [[nodiscard]] constexpr auto rotation_matrix()      const noexcept  -> Mat4d{       return geo::rotation_m4x4(m_orientation);}

    [[nodiscard]] constexpr auto init_position()        const noexcept  -> geo::Pt3d{   return m_initPosition;}
    [[nodiscard]] constexpr auto init_rotation()        const noexcept  -> geo::Vec3d{  return m_initRotation;}

    // move absolute
    constexpr auto set_position(geo::Pt3d position) -> void{m_position = position;}
    // move relative to camera orientation
    constexpr auto translate(geo::Vec3d direction)  noexcept -> void{m_position += geo::rotate(m_orientation, direction);}
    constexpr auto move_up(double amount)           noexcept -> void{m_position += m_up*amount;}
    constexpr auto move_down(double amount)         noexcept -> void{m_position -= m_up*amount;}
    constexpr auto move_front(double amount)        noexcept -> void{m_position += m_direction*amount;}
    constexpr auto move_left(double amount)         noexcept -> void{m_position -= m_right * amount;}
    constexpr auto move_right(double amount)        noexcept -> void{m_position += m_right * amount;;}
    constexpr auto move_back(double amount)         noexcept -> void{m_position -= m_direction*amount;}

    // rotate absolute
    auto set_rotation(geo::Pt3d eulerAngles) -> void;
    // rotate relative
    auto rotate(geo::Pt3d eulerAngles) -> void;

private :

    constexpr auto update_vectors() noexcept -> void{
        m_direction = geo::rotate(m_orientation, Vec3d{0.0f, 0.0f, -1.0f});
        m_right     = geo::rotate(m_orientation, Vec3d{1.0f, 0.0f, 0.0f});
        m_up        = geo::rotate(m_orientation, Vec3d{0.0f, 1.0f, 0.0f});
    }

    // current
    geo::Quatd m_orientation;
    geo::Pt3d m_position;

    // init values
    geo::Pt3d m_initPosition;
    geo::Vec3d m_initRotation;

    // computed vectors
    geo::Vec3d m_up        = {0.,1.,0.};
    geo::Vec3d m_direction = {0.,0.,1.};
    geo::Vec3d m_right     = {1.,0.,0.};
};


}

