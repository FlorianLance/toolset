

/*******************************************************************************
** Toolset-opengl-utility                                                     **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "lines_drawers.hpp"

// base
#include "utility/logger.hpp"

// local
#include "lines_renderer.hpp"

using namespace tool::geo;
using namespace tool::gl;

auto LineDrawer2::initialize(bool dynamic) -> void{

    static constexpr std::array<GLuint,2> indices = {
        0, 1
    };

    static constexpr std::array<Pt3f, 2> vertices = {
        Pt3f
        {0.f,0.f,0.f}, {1.f,0.f,0.f}
    };
    
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(dynamic){
        lm->positionBufferUsage = GL_DYNAMIC_STORAGE_BIT;
        lm->colorBufferUsage    = GL_DYNAMIC_STORAGE_BIT;
    }
    lm->initialize(true);
    if(!lm->load_data(indices, vertices, vertices)){
        Logger::error("[LineDrawer::initialize] Error during loading.\n"sv);
    }
}

auto LineDrawer2::update(std::span<const Pt3f, 2> vertices, std::span<const Pt3f> colors) -> void{
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(!lm->update_data({}, 0, vertices, 0, colors, 0)){
        Logger::error("[LineDrawer::update] Error during update.\n"sv);
    }
}

auto AxesLinesDrawer::initialize(float length) -> void{

    static constexpr std::array<GLuint,6> indices = {
        0, 1, 2, 3, 4, 5
    };

    std::array<Pt3f, 6> vertices = {
        Pt3f
        {0.f,0.f,0.f}, {length,0.f,0.f},
        {0.f,0.f,0.f}, {0.f,length,0.f},
        {0.f,0.f,0.f}, {0.f,0.f,length}
    };

    static constexpr std::array<Pt3f,6> colors = {
        Pt3f
        {1.f,0.f,0.f}, {1.f,0.f,0.f},
        {0.f,1.f,0.f}, {0.f,1.f,0.f},
        {0.f,0.f,1.f}, {0.f,0.f,1.f}
    };
    
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    lm->initialize(true);
    if(!lm->load_data(indices, vertices, colors)){
        Logger::error("[AxesLinesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto TriangleLinesDrawer::initialize(bool dynamic) -> void{

    static constexpr std::array<GLuint,6> indices = {
        0, 1, 1, 2, 2, 0
    };

    static constexpr std::array<Pt3f,3> vertices = {
        Pt3f
        {-1.f, -1.f, 0.f},
        { 1.f, -1.f, 0.f},
        { 1.f,  1.f, 0.f}
    };

    static constexpr std::array<Pt3f,3> colors = {
        Pt3f
        {1.f, 1.f, 1.f},
        {1.f, 1.f, 1.f},
        {1.f, 1.f, 1.f}
    };

    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(dynamic){
        lm->positionBufferUsage = GL_DYNAMIC_STORAGE_BIT;
        lm->colorBufferUsage    = GL_DYNAMIC_STORAGE_BIT;
    }
    lm->initialize(true);
    if(!lm->load_data(indices, vertices, colors)){
        Logger::error("[TriangleLinesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto TriangleLinesDrawer::update(std::span<const geo::Pt3f, 3> vertices, std::span<const geo::Pt3f> colors) -> void{
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(!lm->update_data({}, 0, vertices, 0, colors, 0)){
        Logger::error("[TriangleLinesDrawer::update] Error during update.\n"sv);
    }
}

auto QuadLinesDrawer::initialize(bool dynamic) -> void{

    static constexpr std::array<GLuint,8> indices = {
                                                      0, 1, 1, 2, 2, 3, 3, 0
    };

    static constexpr std::array<Pt3f,4> vertices = {
        Pt3f
        {-1.f, -1.f, 0.f},
        { 1.f, -1.f, 0.f},
        { 1.f,  1.f, 0.f},
        {-1.f,  1.f, 0.f}
    };

    static constexpr std::array<Pt3f,4> colors = {
        Pt3f
        {1.f, 1.f, 1.f},
        {1.f, 1.f, 1.f},
        {1.f, 1.f, 1.f},
        {1.f, 1.f, 1.f}
    };
    
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(dynamic){
        lm->positionBufferUsage = GL_DYNAMIC_STORAGE_BIT;
        lm->colorBufferUsage    = GL_DYNAMIC_STORAGE_BIT;
    }
    lm->initialize(true);
    if(!lm->load_data(indices, vertices, colors)){
        Logger::error("[QuadLinesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto QuadLinesDrawer::update(std::span<const Pt3f, 4> vertices, std::span<const Pt3f> colors) -> void{
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(!lm->update_data({}, 0, vertices, 0, colors, 0)){
        Logger::error("[QuadLinesDrawer::update] Error during update.\n"sv);
    }
}

auto GridLinesDrawer::initialize(float width , float height, int nbX, int nbY) -> void{

    std::vector<GLuint> indices;
    indices.reserve(nbX*2+nbY*2);

    std::vector<GLfloat> vertices;
    vertices.reserve((nbX*2+nbY*2)*3);

    const GLfloat lX = nbX*width;
    const GLfloat lY = nbY*height;

    const GLfloat minX = -lX*0.5f;
    const GLfloat maxX = +lX*0.5f;
    for(GLuint ii = 0; ii < nbX; ++ii){
        indices.emplace_back(static_cast<GLuint>(vertices.size()));
        vertices.emplace_back(minX);
        vertices.emplace_back(0.f);
        vertices.emplace_back(minX+ii*width);

        indices.emplace_back(static_cast<GLuint>(vertices.size()));
        vertices.emplace_back(maxX);
        vertices.emplace_back(0.f);
        vertices.emplace_back(minX+ii*width);
    }

    const GLfloat minY = -lY*0.5f;
    const GLfloat maxY = +lY*0.5f;
    for(GLuint ii = 0; ii < nbY; ++ii){

        indices.emplace_back(static_cast<GLuint>(vertices.size()));
        vertices.emplace_back(minY+ii*height);
        vertices.emplace_back(0.f);
        vertices.emplace_back(minY);

        indices.emplace_back(static_cast<GLuint>(vertices.size()));
        vertices.emplace_back(minY+ii*height);
        vertices.emplace_back(0.f);
        vertices.emplace_back(maxY);
    }
    
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    lm->initialize(false);
    if(!lm->load_data(
        std::span<GLuint>(indices.data(),indices.size()),
        std::span<Pt3f>(reinterpret_cast<Pt3f*>(vertices.data()), vertices.size()/3)
    )){
        Logger::error("[GridLinesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto FrustumDrawerLinesDrawer::initialize(bool dynamic, float fovy, float ar, float nearDist, float farDist) -> void{

    static constexpr std::array<GLuint, 24> indices = {
        0, 5, 0, 6, 0, 7, 0, 8,
        // The near plane
        1, 2, 2, 3, 3, 4, 4, 1,
        // The far plane
        5, 6, 6, 7, 7, 8, 8, 5
    };

    float dy   = nearDist * tanf( deg_2_rad(fovy) / 2.0f );
    float dx   = ar * dy;
    float fdy  = farDist * tanf( deg_2_rad(fovy) / 2.0f );
    float fdx  = ar * fdy;

    std::array<Pt3f,9> vertices = {
        Pt3f
        {0,0,0},                {dx, dy, nearDist},    {-dx, dy,   nearDist},
        {-dx, -dy,  nearDist},  {dx, -dy, nearDist},   {fdx, fdy,  farDist},
        {-fdx, fdy, farDist},   {-fdx, -fdy, farDist}, {fdx, -fdy, farDist}
    };
    
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(dynamic){
        lm->positionBufferUsage = GL_DYNAMIC_STORAGE_BIT;
    }
    lm->initialize(false);
    if(!lm->load_data(indices, vertices)){
        Logger::error("[FrustumDrawerLinesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto FrustumDrawerLinesDrawer::update(float fovy, float ar, float nearDist, float farDist) -> void{

    float dy   = nearDist * tanf( deg_2_rad(fovy) / 2.0f );
    float dx   = ar * dy;
    float fdy  = farDist * tanf( deg_2_rad(fovy) / 2.0f );
    float fdx  = ar * fdy;

    std::array<Pt3f,9> vertices = {
        Pt3f
        {0,0,0},                {dx, dy, nearDist},    {-dx, dy,   nearDist},
        {-dx, -dy,  nearDist},  {dx, -dy, nearDist},   {fdx, fdy,  farDist},
        {-fdx, fdy, farDist},   {-fdx, -fdy, farDist}, {fdx, -fdy, farDist}
    };
    
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(!lm->update_data({}, 0, vertices, 0)){
        Logger::error("[FrustumDrawerLinesDrawer::update] Error during update.\n"sv);
    }
}



