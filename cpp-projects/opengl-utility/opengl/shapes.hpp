
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

#pragma once

// base
#include "geometry/matrix3.hpp"
#include "geometry/matrix4.hpp"

// local
#include "opengl/draw/vao_renderer.hpp"

namespace tool::gl{

// class BaseShape{
// public:
//     virtual ~BaseShape(){}
//     std::unique_ptr<VAORenderer> vaoRenderer;
// };

// class Teapot : public BaseShape{
// public:
//     Teapot(int grid, const geo::Mat4f &lidTransform = geo::Mat4f::identity());

// private:

//     void generate_patches(std::vector<GLfloat> & p, std::vector<GLfloat> & n, std::vector<GLfloat> & tc, std::vector<GLuint> & el, int grid);
//     void build_patch_reflect(int patchNum,std::vector<GLfloat> & B, std::vector<GLfloat> & dB,std::vector<GLfloat> & v, std::vector<GLfloat> & n,std::vector<GLfloat> & tc, std::vector<GLuint> & el,int &index, int &elIndex, int &tcIndex, int grid,bool reflectX, bool reflectY);
//     void build_patch(geo::Vec3f patch[][4], std::vector<GLfloat> & B, std::vector<GLfloat> & dB, std::vector<GLfloat> & v, std::vector<GLfloat> & n,std::vector<GLfloat> & tc, std::vector<GLuint> & el, int &index, int &elIndex, int &tcIndex, int grid, geo::Mat3f reflect, bool invertNormal);
//     void get_patch( int patchNum, geo::Vec3f patch[][4], bool reverseV );
//     void compute_basis_functions( std::vector<GLfloat> & B, std::vector<GLfloat> & dB, int grid );
//     geo::Vec3f evaluate( int gridU, int gridV, std::vector<GLfloat> & B, geo::Vec3f patch[][4] );
//     geo::Vec3f evaluate_normal(  int gridU, int gridV, std::vector<GLfloat> & B, std::vector<GLfloat> & dB, geo::Vec3f patch[][4] );
//     void move_lid(int grid, std::vector<GLfloat> & p, const geo::Mat4f &lidTransform);
// };





}
