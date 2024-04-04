

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

#include "triangles_drawers.hpp"

// base
#include "utility/logger.hpp"

// local
#include "triangles_renderer.hpp"


using namespace tool::geo;
using namespace tool::gl;

static constexpr std::array<Vec2f, 24> cubeTexCoords = {
    Vec2f
    // Front
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
    // Right
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
    // Back
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
    // Left
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
    // Bottom
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
    // Top
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
};

static constexpr std::array<Vec3f, 24> cubeNormals= {
    Vec3f
    // Front
    {0.0f, 0.0f, 1.0f},     {0.0f, 0.0f, 1.0f},     {0.0f, 0.0f, 1.0f},     {0.0f, 0.0f, 1.0f},
    // Right
    {1.0f, 0.0f, 0.0f},     {1.0f, 0.0f, 0.0f},     {1.0f, 0.0f, 0.0f},     {1.0f, 0.0f, 0.0f},
    // Back
    {0.0f, 0.0f, -1.0f},    {0.0f, 0.0f, -1.0f},    {0.0f, 0.0f, -1.0f},    {0.0f, 0.0f, -1.0f},
    // Left
    {-1.0f, 0.0f, 0.0f},    {-1.0f, 0.0f, 0.0f},    {-1.0f, 0.0f, 0.0f},    {-1.0f, 0.0f, 0.0f},
    // Bottom
    {0.0f, -1.0f, 0.0f},    {0.0f, -1.0f, 0.0f},    {0.0f, -1.0f, 0.0f},    {0.0f, -1.0f, 0.0f},
    // Top
    {0.0f, 1.0f, 0.0f},     {0.0f, 1.0f, 0.0f},     {0.0f, 1.0f, 0.0f},     {0.0f, 1.0f, 0.0f}
};

static constexpr std::array<Pt3<GLuint>, 12> cubeElements{
    Pt3<GLuint>
    {0,1,2},    {0,2,3},
    {4,5,6},    {4,6,7},
    {8,9,10},   {8,10,11},
    {12,13,14}, {12,14,15},
    {16,17,18}, {16,18,19},
    {20,21,22}, {20,22,23}
};


auto SphereTrianglesDrawer::initialize(float radius, size_t nSlices, size_t nStacks) -> void{

    GLuint nVerts    = static_cast<GLuint>((nSlices+1) * (nStacks + 1));
    GLuint nElements = static_cast<GLuint>((nSlices * 2 * (nStacks-1)));

    std::vector<Pt3<GLuint>> elements(nElements);
    std::vector<Pt3f> vertices(nVerts);
    std::vector<Pt3f> normals(nVerts);
    std::vector<Pt2f> texCoords(nVerts);

    // generate positions and normals
    GLfloat thetaFac = two_PI<GLfloat>/ nSlices;
    GLfloat phiFac   = PI<GLfloat>/ nStacks;
    GLuint idx = 0;
    for(GLuint ii = 0; ii <= nSlices; ++ii) {

        GLfloat theta = ii * thetaFac;
        GLfloat s = static_cast<GLfloat>(ii) / nSlices;

        for(GLuint jj = 0; jj <= nStacks; ++jj) {

            GLfloat phi = jj * phiFac;
            GLfloat t = static_cast<GLfloat>(jj) / nStacks;
            GLfloat nx = sinf(phi) * cosf(theta);
            GLfloat ny = sinf(phi) * sinf(theta);
            GLfloat nz = cosf(phi);

            vertices[idx]   = {radius * nx, radius * ny, radius * nz};
            normals[idx]    = {nx, ny, nz};
            texCoords[idx]  = {s,t};
            ++idx;
        }
    }

    // generate the element list
    idx = 0;
    for(GLuint ii = 0; ii < nSlices; ++ii) {
        GLuint stackStart     = static_cast<GLuint>(ii * (nStacks + 1));
        GLuint nextStackStart = static_cast<GLuint>((ii+1) * (nStacks+1));
        for(GLuint jj = 0; jj < nStacks; ++jj) {
            if( jj == 0 ) {
                elements[idx] = {stackStart, stackStart + 1, nextStackStart + 1};
                ++idx;
            } else if( jj == nStacks - 1) {
                elements[idx] = {stackStart + jj, stackStart + jj + 1, nextStackStart + jj};
                ++idx;
            } else {
                elements[idx] = {stackStart + jj, stackStart + jj + 1, nextStackStart + jj + 1};
                ++idx;
                elements[idx] = {nextStackStart + jj, stackStart + jj, nextStackStart + jj + 1};
                ++idx;
            }
        }
    }

    auto tm = dynamic_cast<TrianglesRenderer*>(m_vaoRenderer.get());
    tm->initialize(true, true);
    if(!tm->load_data(elements, vertices, normals, texCoords)){
        Logger::error("[SphereTrianglesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto CubeTrianglesDrawer::initialize(float side) -> void{

    GLfloat hSide = side / 2.0f;
    std::array<Pt3f, 24> vertices= {
        Pt3f
        // Front
        {-hSide, -hSide, hSide},    {hSide, -hSide, hSide},     {hSide,  hSide, hSide},     {-hSide,  hSide, hSide},
        // Right
        {hSide, -hSide, hSide},     {hSide, -hSide, -hSide},    {hSide,  hSide, -hSide},    {hSide,  hSide, hSide},
        // Back
        {-hSide, -hSide, -hSide},   {-hSide, hSide, -hSide},    {hSide,  hSide, -hSide},    {hSide, -hSide, -hSide},
        // Left
        {-hSide, -hSide, hSide},    {-hSide,  hSide, hSide},    {-hSide,  hSide, -hSide},   {-hSide, -hSide, -hSide},
        // Bottom
        {-hSide, -hSide, hSide},    {-hSide, -hSide, -hSide},   {hSide, -hSide, -hSide},    {hSide, -hSide, hSide},
        // Top
        {-hSide, hSide, hSide},     {hSide,  hSide, hSide},     {hSide,  hSide, -hSide},    {-hSide,  hSide, -hSide}
    };

    auto tm = dynamic_cast<TrianglesRenderer*>(m_vaoRenderer.get());
    tm->initialize(true, true);
    if(!tm->load_data(cubeElements, vertices, cubeNormals, cubeTexCoords)){
        Logger::error("[CubeTrianglesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto SkyboxTrianglesDrawer::initialize(float side, std::optional<GLuint> cubemap) -> void{

    GLfloat hSide = side / 2.0f;
    std::array<Pt3f, 24> vertices= {
        Pt3f
        // Front
        {-hSide, -hSide, hSide},    {hSide, -hSide, hSide},     {hSide,  hSide, hSide},     {-hSide,  hSide, hSide},
        // Right
        {hSide, -hSide, hSide},     {hSide, -hSide, -hSide},    {hSide,  hSide, -hSide},    {hSide,  hSide, hSide},
        // Back
        {-hSide, -hSide, -hSide},   {-hSide, hSide, -hSide},    {hSide,  hSide, -hSide},    {hSide, -hSide, -hSide},
        // Left
        {-hSide, -hSide, hSide},    {-hSide,  hSide, hSide},    {-hSide,  hSide, -hSide},   {-hSide, -hSide, -hSide},
        // Bottom
        {-hSide, -hSide, hSide},    {-hSide, -hSide, -hSide},   {hSide, -hSide, -hSide},    {hSide, -hSide, hSide},
        // Top
        {-hSide, hSide, hSide},     {hSide,  hSide, hSide},     {hSide,  hSide, -hSide},    {-hSide,  hSide, -hSide}
    };

    if(cubemap.has_value()){
        texturesId.push_back(cubemap.value());
    }

    auto tm = dynamic_cast<TrianglesRenderer*>(m_vaoRenderer.get());
    tm->initialize(false, true);
    if(!tm->load_data(cubeElements, vertices, {}, cubeTexCoords)){
        Logger::error("[SkyboxTrianglesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto GridQuadTrianglesDrawer::initialize(float xsize, float zsize, size_t xdivs, size_t zdivs, float smax, float tmax) -> void{

    GLuint nVerts    = static_cast<GLuint>((xdivs + 1) * (zdivs + 1));
    GLuint nElements = static_cast<GLuint>(6 * xdivs * zdivs);

    std::vector<Pt3f> vertices(nVerts);
    std::vector<Pt3f> normals(nVerts);
    std::fill(std::begin(normals), std::end(normals), Pt3f{0.f, 1.f, 0.f});

    std::vector<Pt4f> tangents(nVerts);
    std::fill(std::begin(tangents), std::end(tangents), Pt4f{1.f, 0.f, 1.f, 0.f});

    std::vector<Pt2f> texCoords(nVerts);
    std::vector<Pt3<GLuint>> elements(nElements);

    float x2        = xsize / 2.0f;
    float z2        = zsize / 2.0f;
    float iFactor   = static_cast<float>(zsize) / zdivs;
    float jFactor   = static_cast<float>(xsize) / xdivs;
    float texi      = smax / xdivs;
    float texj      = tmax / zdivs;

    size_t id = 0;
    for( size_t ii = 0; ii <= zdivs; ii++ ) {
        float z = iFactor * ii - z2;
        for( size_t jj = 0; jj <= xdivs; jj++ ) {
            float x = jFactor * jj - x2;
            vertices[id]  = {x, 0.f, z};
            texCoords[id] = {jj * texi, (zdivs - ii) * texj};
            ++id;
        }
    }

    id = 0;
    for( size_t ii = 0; ii < zdivs; ii++ ) {
        GLuint rowStart = static_cast<GLuint>( ii * (xdivs+1) );
        GLuint nextRowStart = static_cast<GLuint>( (ii+1) * (xdivs+1));
        for( size_t jj = 0; jj < xdivs; jj++ ) {
            elements[id++] = {static_cast<GLuint>(rowStart + jj), static_cast<GLuint>(nextRowStart + jj), static_cast<GLuint>(nextRowStart + jj + 1)};
            elements[id++] = {static_cast<GLuint>(rowStart + jj), static_cast<GLuint>(nextRowStart + jj + 1), static_cast<GLuint>(rowStart + jj + 1)};
        }
    }

    auto tm = dynamic_cast<TrianglesRenderer*>(m_vaoRenderer.get());
    tm->initialize(true, true, true);
    if(!tm->load_data(elements, vertices, normals, texCoords, tangents)){
        Logger::error("[GridQuadTrianglesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto QuadTrianglesDrawer::initialize(bool dynamic) -> void{

    constexpr static std::array<Pt3<GLuint>,2> elements = {
        Pt3<GLuint>{0,1,2},
        Pt3<GLuint>{0,2,3}
    };

    constexpr static std::array<Pt3f,4> vertices = {
        Pt3f{-1.f, -1.f, 0.f},
        Pt3f{ 1.f, -1.f, 0.f},
        Pt3f{ 1.f,  1.f, 0.f},
        Pt3f{-1.f,  1.f, 0.f}
    };

    constexpr static std::array<Pt2f,4> texCoords = {
        Pt2f{0.f, 1.f},
        Pt2f{1.f, 1.f},
        Pt2f{1.f, 0.f},
        Pt2f{0.f, 0.f},
    };

    constexpr static std::array<Pt3f,4> normals = {
        Pt3f{0.f, 0.f, 1.f},
        Pt3f{0.f, 0.f, 1.f},
        Pt3f{0.f, 0.f, 1.f},
        Pt3f{0.f, 0.f, 1.f}
    };

    auto tm = dynamic_cast<TrianglesRenderer*>(m_vaoRenderer.get());
    if(dynamic){
        tm->positionBufferUsage     = GL_DYNAMIC_STORAGE_BIT;
        tm->normalBufferUsage       = GL_DYNAMIC_STORAGE_BIT;
    }

    tm->initialize(true, true);
    if(!tm->load_data(elements, vertices, normals, texCoords
    )){
        Logger::error("[QuadTrianglesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto QuadTrianglesDrawer::update(std::span<const geo::Pt3f, 4> vertices) -> void{

    auto normalV = normalize((normalize(vec(vertices[0],vertices[1])), normalize(vec(vertices[0],vertices[2]))));
    std::array<Pt3f,4> normals;
    std::fill(std::begin(normals), std::end(normals), normalV);

    auto tm = dynamic_cast<TrianglesRenderer*>(m_vaoRenderer.get());
    if(!tm->update_data(
        {},         0,
        vertices,   0,
        normals,    0
    )){
        Logger::error("[QuadTrianglesDrawer::update] Error during update.\n"sv);
    }
}

auto TorusTrianglesDrawer::initialize(float outerRadius, float innerRadius, size_t nSides, size_t nRings) -> void{

    GLuint nVerts    = static_cast<GLuint>(nSides * (nRings+1));
    GLuint nElements = static_cast<GLuint>(6 * nSides * nRings);

    std::vector<Pt3<GLuint>> elements(nElements);
    std::vector<Pt3f> vertices(nVerts);
    std::vector<Pt3f> normals(nVerts);
    std::vector<Pt2f> texCoords(nVerts);    

    float ringFactor = two_PI<float> / nRings;
    float sideFactor = two_PI<float> / nSides;
    size_t id = 0;
    for( GLuint ring = 0; ring <= nRings; ring++ ) {
        float u = ring * ringFactor;
        float cu = cos(u);
        float su = sin(u);
        for( GLuint side = 0; side < nSides; side++ ) {
            float v = side * sideFactor;
            float cv = cos(v);
            float sv = sin(v);
            float r = (outerRadius + innerRadius * cv);
            vertices[id]    = {r * cu,              r * su,         innerRadius * sv};
            normals[id]     = {cv * cu * r,         cv * su * r,    sv * r};
            texCoords[id]   = {u / two_PI<float>,   v / two_PI<float>};
            normals[id]     = normalize(normals[id]);
            ++id;
        }
    }

    id = 0;
    for(GLuint ring = 0; ring < nRings; ring++) {
        auto ringStart      = static_cast<GLuint>(ring * nSides);
        auto nextRingStart  = static_cast<GLuint>((ring + 1) * nSides);
        for( GLuint side = 0; side < nSides; side++ ) {
            GLuint nextSide = (side+1) % nSides;
            elements[id++] = {ringStart + side, nextRingStart + side,       nextRingStart + nextSide};
            elements[id++] = {ringStart + side, nextRingStart + nextSide,   ringStart + nextSide};
        }
    }

    auto tm = dynamic_cast<TrianglesRenderer*>(m_vaoRenderer.get());
    tm->initialize(true, true);
    if(!tm->load_data(elements, vertices, normals, texCoords)){
        Logger::error("[TorusTrianglesDrawer::initialize] Error during loading.\n"sv);
    }
}

auto TeapotTrianglesDrawer::initialize(int grid) -> void{

    static constexpr std::array<std::array<int,16>,10> patchdata ={
        /* rim */
        std::array<int,16>
        {102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        /* body */
        {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
        {24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40},
        /* lid */
        {96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,},
        {0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117},
        /* bottom */
        {118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120, 40, 39, 38, 37},
        /* handle */
        {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56},
        {53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67},
        /* spout */
        {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83},
        {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95}
    };

    static constexpr std::array<Pt3f, 127> cpdata = {
        Pt3f
        {0.2f, 0.f, 2.7f},
        {0.2f, -0.112f, 2.7f},
        {0.112f, -0.2f, 2.7f},
        {0.f, -0.2f, 2.7f},
        {1.3375f, 0.f, 2.53125f},
        {1.3375f, -0.749f, 2.53125f},
        {0.749f, -1.3375f, 2.53125f},
        {0.f, -1.3375f, 2.53125f},
        {1.4375f, 0.f, 2.53125f},
        {1.4375f, -0.805f, 2.53125f},
        {0.805f, -1.4375f, 2.53125f},
        {0.f, -1.4375f, 2.53125f},
        {1.5f, 0.f, 2.4f},
        {1.5f, -0.84f, 2.4f},
        {0.84f, -1.5f, 2.4f},
        {0.f, -1.5f, 2.4f},
        {1.75f, 0.f, 1.875f},
        {1.75f, -0.98f, 1.875f},
        {0.98f, -1.75f, 1.875f},
        {0.f, -1.75f, 1.875f},
        {2.f, 0.f, 1.35f},
        {2.f, -1.12f, 1.35f},
        {1.12f, -2.f, 1.35f},
        {0.f, -2.f, 1.35f},
        {2.f, 0.f, 0.9f},
        {2.f, -1.12f, 0.9f},
        {1.12f, -2.f, 0.9f},
        {0.f, -2.f, 0.9f},
        {-2.f, 0.f, 0.9f},
        {2.f, 0.f, 0.45f},
        {2.f, -1.12f, 0.45f},
        {1.12f, -2.f, 0.45f},
        {0.f, -2.f, 0.45f},
        {1.5f, 0.f, 0.225f},
        {1.5f, -0.84f, 0.225f},
        {0.84f, -1.5f, 0.225f},
        {0.f, -1.5f, 0.225f},
        {1.5f, 0.f, 0.15f},
        {1.5f, -0.84f, 0.15f},
        {0.84f, -1.5f, 0.15f},
        {0.f, -1.5f, 0.15f},
        {-1.6f, 0.f, 2.025f},
        {-1.6f, -0.3f, 2.025f},
        {-1.5f, -0.3f, 2.25f},
        {-1.5f, 0.f, 2.25f},
        {-2.3f, 0.f, 2.025f},
        {-2.3f, -0.3f, 2.025f},
        {-2.5f, -0.3f, 2.25f},
        {-2.5f, 0.f, 2.25f},
        {-2.7f, 0.f, 2.025f},
        {-2.7f, -0.3f, 2.025f},
        {-3.f, -0.3f, 2.25f},
        {-3.f, 0.f, 2.25f},
        {-2.7f, 0.f, 1.8f},
        {-2.7f, -0.3f, 1.8f},
        {-3.f, -0.3f, 1.8f},
        {-3.f, 0.f, 1.8f},
        {-2.7f, 0.f, 1.575f},
        {-2.7f, -0.3f, 1.575f},
        {-3.f, -0.3f, 1.35f},
        {-3.f, 0.f, 1.35f},
        {-2.5f, 0.f, 1.125f},
        {-2.5f, -0.3f, 1.125f},
        {-2.65f, -0.3f, 0.9375f},
        {-2.65f, 0.f, 0.9375f},
        {-2.f, -0.3f, 0.9f},
        {-1.9f, -0.3f, 0.6f},
        {-1.9f, 0.f, 0.6f},
        {1.7f, 0.f, 1.425f},
        {1.7f, -0.66f, 1.425f},
        {1.7f, -0.66f, 0.6f},
        {1.7f, 0.f, 0.6f},
        {2.6f, 0.f, 1.425f},
        {2.6f, -0.66f, 1.425f},
        {3.1f, -0.66f, 0.825f},
        {3.1f, 0.f, 0.825f},
        {2.3f, 0.f, 2.1f},
        {2.3f, -0.25f, 2.1f},
        {2.4f, -0.25f, 2.025f},
        {2.4f, 0.f, 2.025f},
        {2.7f, 0.f, 2.4f},
        {2.7f, -0.25f, 2.4f},
        {3.3f, -0.25f, 2.4f},
        {3.3f, 0.f, 2.4f},
        {2.8f, 0.f, 2.475f},
        {2.8f, -0.25f, 2.475f},
        {3.525f, -0.25f, 2.49375f},
        {3.525f, 0.f, 2.49375f},
        {2.9f, 0.f, 2.475f},
        {2.9f, -0.15f, 2.475f},
        {3.45f, -0.15f, 2.5125f},
        {3.45f, 0.f, 2.5125f},
        {2.8f, 0.f, 2.4f},
        {2.8f, -0.15f, 2.4f},
        {3.2f, -0.15f, 2.4f},
        {3.2f, 0.f, 2.4f},
        {0.f, 0.f, 3.15f},
        {0.8f, 0.f, 3.15f},
        {0.8f, -0.45f, 3.15f},
        {0.45f, -0.8f, 3.15f},
        {0.f, -0.8f, 3.15f},
        {0.f, 0.f, 2.85f},
        {1.4f, 0.f, 2.4f},
        {1.4f, -0.784f, 2.4f},
        {0.784f, -1.4f, 2.4f},
        {0.f, -1.4f, 2.4f},
        {0.4f, 0.f, 2.55f},
        {0.4f, -0.224f, 2.55f},
        {0.224f, -0.4f, 2.55f},
        {0.f, -0.4f, 2.55f},
        {1.3f, 0.f, 2.55f},
        {1.3f, -0.728f, 2.55f},
        {0.728f, -1.3f, 2.55f},
        {0.f, -1.3f, 2.55f},
        {1.3f, 0.f, 2.4f},
        {1.3f, -0.728f, 2.4f},
        {0.728f, -1.3f, 2.4f},
        {0.f, -1.3f, 2.4f},
        {0.f, 0.f, 0.f},
        {1.425f, -0.798f, 0.f},
        {1.5f, 0.f, 0.075f},
        {1.425f, 0.f, 0.f},
        {0.798f, -1.425f, 0.f},
        {0.f, -1.5f, 0.075f},
        {0.f, -1.425f, 0.f},
        {1.5f, -0.84f, 0.075f},
        {0.84f, -1.5f, 0.075f}
    };

    GLuint nVerts    = static_cast<GLuint>(32 * (grid + 1) * (grid + 1));
    GLuint nElements = static_cast<GLuint>(grid * grid * 32 * 2);

    std::vector<Pt3<GLuint>> elements(nElements);
    std::vector<Pt3f> vertices(nVerts);
    std::vector<Pt3f> normals(nVerts);
    std::vector<Pt2f> texCoords(nVerts);

    auto compute_basis_functions = [](std::span<Pt4f> B, std::span<Pt4f> dB, int grid){
        float inc = 1.0f / grid;
        for( int i = 0; i <= grid; i++ ){

            float t = i * inc;
            float tSqr = t * t;
            float oneMinusT = (1.0f - t);
            float oneMinusT2 = oneMinusT * oneMinusT;

            B[i]  = {oneMinusT * oneMinusT2, 3.0f * oneMinusT2 * t,                     3.0f * oneMinusT * tSqr,                t * tSqr};
            dB[i] = {-3.0f * oneMinusT2,    -6.0f * t * oneMinusT + 3.0f * oneMinusT2,  -3.0f * tSqr + 6.0f * t * oneMinusT,    3.0f * tSqr};
        }
    };

    // auto generate_patches(std::span<)

    // float inc = 1.0f / grid;
    // for( int i = 0; i <= grid; i++ ){

    //     float t = i * inc;
    //     float tSqr = t * t;
    //     float oneMinusT = (1.0f - t);
    //     float oneMinusT2 = oneMinusT * oneMinusT;

    //     B[i*4 + 0] = oneMinusT * oneMinusT2;
    //     B[i*4 + 1] = 3.0f * oneMinusT2 * t;
    //     B[i*4 + 2] = 3.0f * oneMinusT * tSqr;
    //     B[i*4 + 3] = t * tSqr;

    //     dB[i*4 + 0] = -3.0f * oneMinusT2;
    //     dB[i*4 + 1] = -6.0f * t * oneMinusT + 3.0f * oneMinusT2;
    //     dB[i*4 + 2] = -3.0f * tSqr + 6.0f * t * oneMinusT;
    //     dB[i*4 + 3] = 3.0f * tSqr;
    // }

}



auto MeshDrawer2::initialize(const graphics::Mesh &mesh) -> void{

    auto tm = dynamic_cast<TrianglesRenderer*>(m_vaoRenderer.get());
    tm->initialize(!mesh.normals.empty(), !mesh.tCoords.empty(), !mesh.tangents.empty(), !mesh.bones.empty(), !mesh.colors.empty());
    if(!tm->load_data(
            std::span<const Pt3<GLuint>>(reinterpret_cast<const Pt3<GLuint>*>(mesh.triIds.get_data()), mesh.triIds.size()),
            mesh.vertices.span(),
            mesh.normals.span(),
            mesh.tCoords.span(),
            mesh.tangents.span(),
            mesh.bones.span(),
            mesh.colors.span()
        )){
        Logger::error("[MeshDrawer::initialize] Error during loading.\n"sv);
    }
}

auto GMeshDrawer2::initialize(const graphics::SubModelMesh &gmesh, const std::vector<graphics::TextureInfo> &texturesInfo) -> void{

    // clean previous textures
    if(!m_textures.empty()){
        for(auto &texturesT : m_textures){
            for(auto &texture : texturesT.second){
                texture->clean();
            }
        }
        m_textures.clear();
    }

    if(texturesInfo.empty()){
        // look from gmesh material
        for(auto &texturesT : gmesh.material->texturesInfo){
            for(auto textureInfo : texturesT.second){
                m_textures[textureInfo.options.type].push_back(std::make_unique<Texture2D>());
                m_textures[textureInfo.options.type].back()->load_texture(textureInfo.texture, textureInfo.options);
            }
        }
    }else{
        // look from input infos
        for(auto textureInfo : texturesInfo){
            m_textures[textureInfo.options.type].push_back(std::make_unique<Texture2D>());
            m_textures[textureInfo.options.type].back()->load_texture(textureInfo.texture);
        }
    }

    // add textures in good order
    if(m_textures.count(TextureType::diffuse)!=0){
        texturesId.push_back(m_textures[TextureType::diffuse].front()->id());
    }
    if(m_textures.count(TextureType::normal)!=0){
        texturesId.push_back(m_textures[TextureType::normal].front()->id());
    }
    if(m_textures.count(TextureType::height)!=0){
        texturesId.push_back(m_textures[TextureType::height].front()->id());
    }

    auto tm = dynamic_cast<TrianglesRenderer*>(m_vaoRenderer.get());
    const auto &mesh = gmesh.mesh;
    tm->initialize(!mesh.normals.empty(), !mesh.tCoords.empty(), !mesh.tangents.empty(), !mesh.bones.empty(), !mesh.colors.empty());
    if(!tm->load_data(
            std::span<const Pt3<GLuint>>(reinterpret_cast<const Pt3<GLuint>*>(mesh.triIds.get_data()), mesh.triIds.size()),
            mesh.vertices.span(),
            mesh.normals.span(),
            mesh.tCoords.span(),
            mesh.tangents.span(),
            mesh.bones.span(),
            mesh.colors.span()
        )){
        Logger::error("[GMeshDrawer::initialize] Error during loading.\n"sv);
    }
}

auto ModelMeshDrawer2::initialize(const graphics::ModelMesh &modelMesh, const std::vector<graphics::TextureInfo> &texturesInfo) -> void{

    // clean previous hierarchy
    clean();

    // initialize current level drawers
    for(const auto &gmesh : modelMesh.meshes){
        auto gmeshD = std::make_unique<GMeshDrawer2>();
        gmeshD->initialize(*gmesh, texturesInfo);
        drawers.push_back(std::move(gmeshD));
    }

    // initialize children drawers
    for(const auto &modelChild : modelMesh.children){
        auto modelDrawer = std::make_unique<ModelMeshDrawer2>();
        modelDrawer->initialize(*modelChild, {});
        children.push_back(std::move(modelDrawer));
    }
}

