
/*******************************************************************************
** Toolset-opengl-utility                                                     **
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

#include "shapes.hpp"

// local
#include "opengl/draw/triangles_renderer.hpp"

using namespace tool;
using namespace tool::gl;
using namespace tool::geo;




namespace TeapotData {
static constexpr int patchdata[][16] =
    {
        /* rim */
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

static constexpr float cpdata[][3] =
    {
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

}


void Teapot::generate_patches(std::vector<GLfloat> &p, std::vector<GLfloat> &n, std::vector<GLfloat> &tc, std::vector<GLuint> &el, int grid){

    std::vector<GLfloat> B(4*(grid+1));  // Pre-computed Bernstein basis functions
    std::vector<GLfloat> dB(4*(grid+1)); // Pre-computed derivitives of basis functions

    int idx = 0, elIndex = 0, tcIndex = 0;

    // Pre-compute the basis functions  (Bernstein polynomials)
    // and their derivatives
    compute_basis_functions(B, dB, grid);

    // Build each patch
    // The rim
    build_patch_reflect(0, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    // The body
    build_patch_reflect(1, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    build_patch_reflect(2, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    // The lid
    build_patch_reflect(3, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    build_patch_reflect(4, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    // The bottom
    build_patch_reflect(5, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true, true);
    // The handle
    build_patch_reflect(6, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
    build_patch_reflect(7, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
    // The spout
    build_patch_reflect(8, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
    build_patch_reflect(9, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false, true);
}

void Teapot::build_patch_reflect(
    int patchNum, std::vector<GLfloat> &B, std::vector<GLfloat> &dB, std::vector<GLfloat> &v,
    std::vector<GLfloat> &n, std::vector<GLfloat> &tc, std::vector<GLuint> &el,
    int &index, int &elIndex, int &tcIndex, int grid, bool reflectX, bool reflectY){

    geo::Vec3f patch[4][4];
    geo::Vec3f patchRevV[4][4];
    get_patch(patchNum, patch, false);
    get_patch(patchNum, patchRevV, true);

    // Patch without modification
    build_patch(patch, B, dB, v, n, tc, el,
               index, elIndex, tcIndex, grid, geo::Mat3f(true), true);

    // Patch reflected in x
    if( reflectX ) {
        build_patch(patchRevV, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
            geo::Mat3f{
                -1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f
            },
            false
        );
    }

    // Patch reflected in y
    if( reflectY ) {
        build_patch(patchRevV, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
            geo::Mat3f{
                1.0f, 0.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, 0.0f, 1.0f
            },
            false
        );
    }

    // Patch reflected in x and y
    if( reflectX && reflectY ) {
        build_patch(patch, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
            geo::Mat3f{
                -1.0f, 0.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, 0.0f, 1.0f
            },
            true
        );
    }
}

void Teapot::build_patch(geo::Vec3f patch[][4],
    std::vector<GLfloat> &B, std::vector<GLfloat> &dB, std::vector<GLfloat> &v,
    std::vector<GLfloat> &n, std::vector<GLfloat> &tc, std::vector<GLuint> &el,
    int &index, int &elIndex, int &tcIndex, int grid, geo::Mat3f reflect, bool invertNormal){

    int startIndex = index / 3;
    float tcFactor = 1.0f / grid;

    for( int i = 0; i <= grid; i++ ){

        for( int j = 0 ; j <= grid; j++){

//            Vec3f pt   = reflect * evaluate(i,j,B,patch);
//            Vec3f norm = reflect * evaluate_normal(i,j,B,dB,patch);
            Vec3f pt   = evaluate(i,j,B,patch)*reflect;
            Vec3f norm = evaluate_normal(i,j,B,dB,patch)*reflect;
            if( invertNormal ){
                norm = -1.f*norm;
            }

            v[index]    = pt.x();
            v[index+1]  = pt.y();
            v[index+2]  = pt.z();

            n[index]    = norm.x();
            n[index+1]  = norm.y();
            n[index+2]  = norm.z();

            tc[tcIndex] = i * tcFactor;
            tc[tcIndex+1] = j * tcFactor;

            index += 3;
            tcIndex += 2;
        }
    }

    for( int i = 0; i < grid; i++ ){

        int iStart = i * (grid+1) + startIndex;
        int nextiStart = (i+1) * (grid+1) + startIndex;

        for( int j = 0; j < grid; j++){

            el[elIndex] = iStart + j;
            el[elIndex+1] = nextiStart + j + 1;
            el[elIndex+2] = nextiStart + j;

            el[elIndex+3] = iStart + j;
            el[elIndex+4] = iStart + j + 1;
            el[elIndex+5] = nextiStart + j + 1;

            elIndex += 6;
        }
    }
}

void Teapot::get_patch(int patchNum, geo::Vec3f patch[][4], bool reverseV){

    for( int u = 0; u < 4; u++) {          // Loop in u direction
        for( int v = 0; v < 4; v++ ) {     // Loop in v direction
            if( reverseV ) {
                patch[u][v] = Vec3f{
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u*4+(3-v)]][0],
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u*4+(3-v)]][1],
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u*4+(3-v)]][2]
                };
            } else {
                patch[u][v] = Vec3f{
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u*4+v]][0],
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u*4+v]][1],
                    TeapotData::cpdata[TeapotData::patchdata[patchNum][u*4+v]][2]
                };
            }
        }
    }
}

void Teapot::compute_basis_functions(std::vector<GLfloat> &B, std::vector<GLfloat> &dB, int grid){

    float inc = 1.0f / grid;
    for( int i = 0; i <= grid; i++ ){

        float t = i * inc;
        float tSqr = t * t;
        float oneMinusT = (1.0f - t);
        float oneMinusT2 = oneMinusT * oneMinusT;

        B[i*4 + 0] = oneMinusT * oneMinusT2;
        B[i*4 + 1] = 3.0f * oneMinusT2 * t;
        B[i*4 + 2] = 3.0f * oneMinusT * tSqr;
        B[i*4 + 3] = t * tSqr;

        dB[i*4 + 0] = -3.0f * oneMinusT2;
        dB[i*4 + 1] = -6.0f * t * oneMinusT + 3.0f * oneMinusT2;
        dB[i*4 + 2] = -3.0f * tSqr + 6.0f * t * oneMinusT;
        dB[i*4 + 3] = 3.0f * tSqr;
    }
}

Vec3f Teapot::evaluate(int gridU, int gridV, std::vector<GLfloat> &B, Vec3f patch[][4]){

    Vec3f p{0.0f,0.0f,0.0f};
    for( int i = 0; i < 4; i++) {
        for( int j = 0; j < 4; j++) {
            p += patch[i][j] * B[gridU*4+i] * B[gridV*4+j];
        }
    }
    return p;
}

Vec3f Teapot::evaluate_normal(int gridU, int gridV, std::vector<GLfloat> &B, std::vector<GLfloat> &dB, Vec3f patch[][4]){

    Vec3f du{0.0f,0.0f,0.0f};
    Vec3f dv{0.0f,0.0f,0.0f};

    for( int i = 0; i < 4; i++) {
        for( int j = 0; j < 4; j++) {
            du += patch[i][j] * dB[gridU*4+i] * B[gridV*4+j];
            dv += patch[i][j] * B[gridU*4+i] * dB[gridV*4+j];
        }
    }

    auto norm = geo::cross(du, dv);
    if(geo::norm(norm) != 0.0f){
        norm = geo::normalize(norm);
    }
    return norm;
}

void Teapot::move_lid(int grid, std::vector<GLfloat> &p, const Mat4f &lidTransform){

    static_cast<void>(lidTransform);
    int start = 3 * 12 * (grid+1) * (grid+1);
    int end = 3 * 20 * (grid+1) * (grid+1);

    for( int i = start; i < end; i+=3 ){

        geo::Vec4f vert = geo::Vec4f{p[i], p[i+1], p[i+2], 1.0f };
//        vert    = lidTransform.multiply_vector(vert); //lidTransform * vert;
        p[i]    = vert.x();
        p[i+1]  = vert.y();
        p[i+2]  = vert.z();
    }
}

Teapot::Teapot(int grid, const geo::Mat4f &lidTransform){

    int verts = 32 * (grid + 1) * (grid + 1);
    int faces = grid * grid * 32;
    std::vector<GLfloat> p( verts * 3 );
    std::vector<GLfloat> n( verts * 3 );
    std::vector<GLfloat> tc( verts * 2 );
    std::vector<GLuint> el( faces * 6 );

    generate_patches( p, n, tc, el, grid );
    move_lid(grid, p, lidTransform);
    
    auto tmd = std::make_unique<TrianglesRenderer>();
    tmd->init_buffers(&el,&p, &n, &tc);
    vaoRenderer = std::move(tmd);
}

