

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

auto LinesDrawer2::initialize(bool dynamic, std::span<const GLuint> indices, std::span<const geo::Pt3f> vertices, std::span<const geo::Pt3f> colors) -> void{
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(dynamic){
        lm->indicesBufferUsage   = GL_DYNAMIC_STORAGE_BIT;
        lm->positionBufferUsage  = GL_DYNAMIC_STORAGE_BIT;
        lm->colorBufferUsage     = GL_DYNAMIC_STORAGE_BIT;
    }
    lm->initialize(!colors.empty());
    if(!lm->load_data(indices, vertices, colors)){
        Logger::error("[LinesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto LinesDrawer2::update(std::span<const GLuint> indices, std::span<const geo::Pt3f> vertices, std::span<const geo::Pt3f> colors) -> void{
    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(!lm->update_data(indices, 0, vertices, 0, colors, 0)){
        Logger::error("[LinesDrawer::update] Error during update.\n"sv);
        return;
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

auto GridLinesDrawer::initialize(float width, float height, int nbX, int nbY, bool addDiagonals) -> void{

    size_t nbVertices = nbX*nbY;
    auto hW = nbX*width*0.5f;
    auto hH = nbY*height*0.5f;
    auto start = Pt3f(-hW, 0.f, -hH);

    std::vector<Pt3f> vertices;
    vertices.reserve(nbVertices);
    for(int ii = 0; ii < nbX; ++ii){
        for(int jj = 0; jj < nbY; ++jj){
            vertices.push_back(start + Vec3f{ii*width, 0.f, jj*height});
        }
    }

    std::vector<GLuint> indices;
    indices.reserve(nbX + nbY);// + (addDiagonals ? (std::max(nbX,nbY)*2) : 0));
    for(int ii = 0; ii < nbY; ++ii){
        indices.push_back(ii);
        indices.push_back(ii + nbY * (nbX-1));
    }
    for(int ii = 0; ii < nbX; ++ii){
        indices.push_back(ii*nbY);
        indices.push_back(ii*nbY + (nbY-1));
    }
    if(addDiagonals){

        // int ii = 0;
        // indices.push_back(ii);
        // indices.push_back(ii);

        // for(int ii = 0; ii < nbX-1; ++ii){
        //     int id1 = nbX-2 -ii;
        //     int id2 = id1 + ii*(nbX+1);
        //     // int id2 = 2*nbX-1+ (ii*nbX);
        //     indices.push_back(id1);
        //     indices.push_back(id2);
        // }
    }


    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    lm->initialize(false);
    if(!lm->load_data(indices, vertices)){
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

    if(nearDist > farDist){
        nearDist = farDist;
    }

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

auto OrientedBoundingBoxLinesDrawer::initialize(bool dynamic, const geo::OBB3<float> &obb) -> void{

    static constexpr std::array<GLuint,48> indices = {
        0, 1, 0, 2, 1, 3, 2, 3,
        4, 5, 4, 6, 5, 7, 6, 7,
        2, 6, 3, 7, 0, 4, 1, 5,
        0, 3, 1, 2, 1, 7, 3, 5,
        2, 7, 3, 6, 0, 6, 2, 4,
        0, 5, 1, 4, 4, 7, 5, 6
    };

    auto tr = geo::transform(obb.size*0.5f, obb.rotation, obb.position);
    std::array<Pt3f,8> vertices = {
        Pt3f
        {-1.f,-1.f,-1.f},
        {1.f,-1.f,-1.f},
        {-1.f,1.f,-1.f},
        {1.f,1.f,-1.f},
        {-1.f,-1.f,1.f},
        {1.f,-1.f,1.f},
        {-1.f,1.f,1.f},
        {1.f,1.f,1.f},
    };

    for(auto &v : vertices){
        v = tr.multiply_point(Pt4f{v.x(),v.y(),v.z(),1.f}).xyz();
    }

    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(dynamic){
        lm->positionBufferUsage = GL_DYNAMIC_STORAGE_BIT;
    }
    lm->initialize(false);
    if(!lm->load_data(indices, vertices)){
        Logger::error("[OrientedBoundingBoxLinesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto OrientedBoundingBoxLinesDrawer::update(const geo::OBB3<float> &obb) -> void{


    auto tr = geo::transform(obb.size*0.5f, obb.rotation, obb.position);
    std::array<Pt3f,8> vertices = {
        Pt3f
        {-1.f,-1.f,-1.f},
        {1.f,-1.f,-1.f},
        {-1.f,1.f,-1.f},
        {1.f,1.f,-1.f},
        {-1.f,-1.f,1.f},
        {1.f,-1.f,1.f},
        {-1.f,1.f,1.f},
        {1.f,1.f,1.f},
    };

    for(auto &v : vertices){
        v = tr.multiply_point(Pt4f{v.x(),v.y(),v.z(),1.f}).xyz();
    }


    auto lm = dynamic_cast<LinesRenderer*>(m_vaoRenderer.get());
    if(!lm->update_data({}, 0, vertices, 0)){
        Logger::error("[OrientedBoundingBoxLinesDrawer::update] Error during update.\n"sv);
    }
}

