

/*******************************************************************************
** Toolset-ts-mesh                                                            **
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

#include "mesh_generation.hpp"

using namespace tool::geo;
using namespace tool::mesh;

auto MeshGeneration::plane(float xsize, float zsize, size_t xdivs, size_t zdivs, float smax, float tmax) -> Mesh{

    Mesh mesh;
    auto &points     = mesh.vertices;
    auto &normals    = mesh.normals;
    auto &tCoords    = mesh.tCoords;
    auto &tangents   = mesh.tangents;
    auto &triIds     = mesh.triIds;

    size_t nPoints = (xdivs + 1) * (zdivs + 1);
    points.resize(nPoints);
    normals.resize(nPoints);
    tCoords.resize(nPoints);
    tangents.resize(nPoints);
    triIds.resize(xdivs * zdivs * 2);

    float x2 = xsize / 2.0f;
    float z2 = zsize / 2.0f;
    float iFactor = static_cast<float>(zsize) / zdivs;
    float jFactor = static_cast<float>(xsize) / xdivs;
    float texi = smax / xdivs;
    float texj = tmax / zdivs;

    size_t idVertex = 0;
    for(size_t ii = 0; ii <= zdivs; ii++ ) {
        float z = iFactor * ii - z2;
        for( size_t jj = 0; jj <= xdivs; jj++ ) {
            float x = jFactor * jj - x2;
            points[idVertex]        = {x, 0.0f, z};
            tCoords[idVertex]    = {jj * texi, (zdivs - ii) * texj};
            ++idVertex;
        }
    }
    std::fill(std::begin(normals), std::end(normals), geo::Pt3f{0.0f,1.0f,0.0f});
    std::fill(std::begin(tangents), std::end(tangents), geo::Pt4f{1.0f,0.0f,0.0f,1.0f});

    size_t rowStart, nextRowStart;
    size_t idTri = 0;
    for( size_t ii = 0; ii < zdivs; ii++ ) {
        rowStart = static_cast<size_t>( ii * (xdivs+1) );
        nextRowStart = static_cast<size_t>( (ii+1) * (xdivs+1));
        for( size_t jj = 0; jj < xdivs; jj++ ) {

            triIds[idTri] = {
                static_cast<std::uint32_t>(rowStart + jj),
                static_cast<std::uint32_t>(nextRowStart + jj),
                static_cast<std::uint32_t>(nextRowStart + jj + 1)
            };
            ++idTri;
            triIds[idTri] = {
                static_cast<std::uint32_t>(rowStart + jj),
                static_cast<std::uint32_t>(nextRowStart + jj + 1),
                static_cast<std::uint32_t>(rowStart + jj + 1)
            };
            ++idTri;
        }
    }

    return mesh;
}
