
/*******************************************************************************
** Toolset-base                                                               **
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

#include "dc_depth_indices.hpp"


using namespace tool::cam;

auto DCDepthIndices::initialize(bool hasDepth, size_t depthWidth, size_t depthHeight) -> void{

    // set depth indices
    if(hasDepth){

        auto depthSize = depthWidth*depthHeight;

        // resize
        // # 1D
        depths1D.resize(depthSize);
        depths1DNoBorders.clear();
        depths1DNoBorders.reserve((depthWidth-2)*(depthHeight-2));

        neighbours2HDepth1D.clear();
        neighbours2HDepth1D.reserve(depthSize);
        neighbours2VDepth1D.clear();
        neighbours2VDepth1D.reserve(depthSize);

        neighbours4Depth1D.clear();
        neighbours4Depth1D.reserve(depthSize);
        neighbours8Depth1D.clear();
        neighbours8Depth1D.reserve(depthSize);

        // # 3D
        depths3D.resize(depthSize);
        // # correspondance
        depthVertexCorrrespondance.resize(depthSize);
        // depthsSortedCorrespondanceNoBorders.resize((infos.depth_width()-2)*(infos.depth_height()-2));

        // fill
        std::iota(std::begin(depths1D), std::end(depths1D), 0);
        std::int32_t id = 0;
        for(size_t ii = 0; ii < depthHeight; ++ii){
            for(size_t jj = 0; jj < depthWidth; ++jj){

                depths3D[id] = {static_cast<size_t>(id),jj,ii};

                // A B C
                // D I E
                // F G H
                std::int32_t idA = -1;
                std::int32_t idD = -1;
                std::int32_t idF = -1;
                std::int32_t idC = -1;
                std::int32_t idE = -1;
                std::int32_t idH = -1;
                std::int32_t idB = -1;
                std::int32_t idG = -1;

                bool notOnLeft   = jj > 0;
                bool notOnRight  = jj < depthWidth - 1;
                bool notOnTop    = ii > 0;
                bool notOnBottom = ii < depthHeight-1;

                if(notOnLeft){
                    idD = id - 1;
                    if(notOnTop){
                        idA = id - static_cast<int>(depthWidth)-1;
                    }
                    if(notOnBottom){
                        idF = id + static_cast<int>(depthWidth)-1;
                    }
                }
                if(notOnRight){
                    idE = id + 1;
                    if(notOnTop){
                        idC = id - static_cast<int>(depthWidth) + 1;
                    }
                    if(notOnBottom){
                        idH = id + static_cast<int>(depthWidth) + 1;
                    }
                }
                if(notOnTop){
                    idB = id - static_cast<int>(depthWidth);
                }
                if(notOnBottom){
                    idG = id + static_cast<int>(depthWidth);
                }

                neighbours2HDepth1D.push_back({idD,idE});
                neighbours2VDepth1D.push_back({idB,idG});
                neighbours4Depth1D.push_back({idB,idD,idE,idG});
                neighbours8Depth1D.push_back({idA,idB,idC,idD,idE,idF,idG,idH});

                if(notOnLeft && notOnRight && notOnTop && notOnBottom){
                    depths1DNoBorders.push_back(id);
                }
                ++id;
            }
        }
    }else{
        // # 1D
        depths1D.clear();
        depths1DNoBorders.clear();
        neighbours2HDepth1D.clear();
        neighbours2VDepth1D.clear();
        neighbours4Depth1D.clear();
        neighbours8Depth1D.clear();
        // # 3D
        depths3D.clear();
        // # correspondance
        depthVertexCorrrespondance.clear();
        // depthsSortedCorrespondanceNoBorders.clear();
    }

    // init iterators
    depths1DStart                   = depths1D.begin();
    depths1DEnd                     = depths1D.end();

    depths1DNoBordersStart          = depths1DNoBorders.begin();
    depths1DNoBordersEnd            = depths1DNoBorders.end();

    neighbours2HDepth1DStart        = neighbours2HDepth1D.begin();
    neighbours2HDepth1DEnd          = neighbours2HDepth1D.end();

    neighbours2VDepth1DStart        = neighbours2VDepth1D.begin();
    neighbours2VDepth1DEnd          = neighbours2VDepth1D.end();

    neighbours4Depth1DStart         = neighbours4Depth1D.begin();
    neighbours4Depth1DEnd           = neighbours4Depth1D.end();

    neighbours8Depth1DStart         = neighbours8Depth1D.begin();
    neighbours8Depth1DEnd           = neighbours8Depth1D.end();

    depths3DStart                   = depths3D.begin();
    depths3DEnd                     = depths3D.end();

    depthVertexCorrrespondanceStart = depthVertexCorrrespondance.begin();
    depthVertexCorrrespondanceEnd   = depthVertexCorrrespondance.end();
}
