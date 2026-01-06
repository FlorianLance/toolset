
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

#include "cube_map.hpp"

// local
#include "utility/logger.hpp"

using namespace tool::img;

auto CubeMap::load_2d_images_files(const std::array<std::string, 6> &pathTextures, bool flip, int targetNbChannels) -> bool{

    for(size_t ii = 0; ii < pathTextures.size(); ++ii){
        if(!textures[ii].load_2d_image_file_data(pathTextures[ii], flip, targetNbChannels)){
            Log::error("[CubeMap] Error during files textures loading.\n");
            return false;
        }
    }
    return true;
}

auto CubeMap::load_2d_images_files(const std::string &basePath, std::array<std::string, 6> &extensions, bool flip, int targetNbChannels) -> bool{

    for(size_t ii = 0; ii < extensions.size(); ++ii){
        if(!textures[ii].load_2d_image_file_data(basePath + extensions[ii], flip, targetNbChannels)){
            Log::error("[CubeMap] Error during files textures loading.\n");
            return false;
        }
    }
    return true;
}
