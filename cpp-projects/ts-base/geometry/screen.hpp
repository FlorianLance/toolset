

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
#include "point2.hpp"
#include "matrix4.hpp"

namespace tool::geo{

class Screen{

public:

    enum class Mode : int{
        Perspective, Orhtographic
    };

    constexpr Screen(unsigned int width = 800, unsigned int height = 800, int xPos = 0, int yPos = 0, double pixelRatio = 1.0)
        : m_size(width,height), m_position(xPos,yPos), m_pixelRatio(pixelRatio){
        update_projection();
    }    

    [[nodiscard]] constexpr auto width()        const noexcept -> unsigned int  {return static_cast<unsigned int>(m_size.x()*m_pixelRatio);}
    [[nodiscard]] constexpr auto height()       const noexcept -> unsigned int  {return static_cast<unsigned int>(m_size.y()*m_pixelRatio);}
    [[nodiscard]] constexpr auto size_pixels()  const noexcept -> unsigned int  {return width() * height();}
    [[nodiscard]] constexpr auto x_pos()        const noexcept -> int           {return m_position.x();}
    [[nodiscard]] constexpr auto y_pos()        const noexcept -> int           {return m_position.y();}
    [[nodiscard]] constexpr auto position()     const noexcept -> geo::Pt2<int> {return m_position;}
    [[nodiscard]] constexpr auto projection()   const noexcept -> geo::Mat4d    {return m_projection;}
    [[nodiscard]] constexpr auto fov()          const noexcept -> double        {return m_fov;}
    [[nodiscard]] constexpr auto z_range()      const noexcept -> geo::Pt2d     {return m_zRange;}

    constexpr auto resize(unsigned int width, unsigned int height, double pixelRatio = 1.0) noexcept -> void{
        m_size = {width, height};
        m_pixelRatio = pixelRatio;
        update_projection();
    }

    constexpr auto set_mode(Mode mode) -> void{
        m_mode = mode;
        update_projection();
    }

    constexpr auto set_fov(double fov) -> void{
        m_fov = fov;
        if(m_mode == Mode::Perspective){
            update_projection();
        }
    }

    constexpr auto set_range(double min, double max)  -> void{
        m_zRange = {min,max};
        update_projection();
    }

private:

    constexpr auto update_projection() -> void{

        const auto ratio = 1.*width()/ height();
        if(m_mode == Mode::Perspective){
            m_projection = geo::perspective(
                deg_2_rad(m_fov),
                ratio,
                m_zRange.x(),
                m_zRange.y()
            );
        }else if(m_mode == Mode::Orhtographic){
            m_projection = geo::orthographic(
                -ratio, ratio,
                -1., 1.,
                m_zRange.x(), m_zRange.y()
            );
        }
    }

    geo::Pt2<unsigned int> m_size = {800,800};
    geo::Pt2<int> m_position = {0,0};
    double m_pixelRatio = 1.0;

    geo::Mat4d m_projection;
    double m_fov = 60.0;
    geo::Pt2d m_zRange = {0.1, 10000.};
    Mode m_mode = Mode::Perspective;
};
}

