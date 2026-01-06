
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

#include "texture.hpp"

// std
// #include <iostream>
#include <string>
#include <filesystem>

// stb_image
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

// local
#include "utility/logger.hpp"

using namespace tool::img;
using namespace std::string_view_literals;

auto Texture::copy_2d_data(int width, int height, int nbChannels, unsigned char *data) -> void{

    m_sizes = {width, height, 1};
    m_nbChannels = nbChannels;
    m_hdr = false;

    m_data.resize(width*height*nbChannels);
    std::copy(data, data + width*height*m_nbChannels, std::begin(m_data));
}

auto Texture::copy_2d_data(int width, int height, int nbChannels, const std::vector<unsigned char> &data) -> void{

    if(data.size() < static_cast<size_t>(width*height*nbChannels)){
        Log::error("[Texture::copy_2d_data] Cannot copy data to texture, invalid array size.\n"sv);
        return;
    }
    m_sizes = {width, height, 1};
    m_nbChannels = nbChannels;
    m_hdr = false;
    m_data = data;
}

auto Texture::copy_2d_data(int width, int height, const std::vector<geo::Pt3<unsigned char>> &data) -> void{

    if(data.size() < static_cast<size_t>(width*height)){
        Log::error("[Texture::copy_2d_data] Cannot copy data to texture, invalid array size.\n"sv);
        return;
    }
    m_sizes = {width, height, 1};
    m_nbChannels = 3;
    m_hdr = false;

    m_data.resize(static_cast<size_t>(width*height*m_nbChannels)*4);
    std::copy(std::begin(data), std::end(data), reinterpret_cast<geo::Pt3<unsigned char>*>(m_data.data()));
}

auto Texture::copy_2d_data(int width, int height, int nbChannels, const std::vector<float> &data) -> void{

    if(data.size() < static_cast<size_t>(width*height*nbChannels)){
        Log::error("[Texture::copy_2d_data] Cannot copy data to texture, invalid array size.\n"sv);
        return;
    }
    m_sizes = {width, height, 1};
    m_nbChannels = nbChannels;
    m_hdr = true;

    m_data.resize(static_cast<size_t>(width*height*m_nbChannels)*4);
    std::copy(std::begin(data), std::end(data), reinterpret_cast<float*>(m_data.data()));
}

auto Texture::copy_2d_data(int width, int height, const std::vector<geo::Pt3<float>> &data) -> void{

    if(data.size() < static_cast<size_t>(width*height)){
        Log::error("[Texture::copy_2d_data] Cannot copy data to texture, invalid array size.\n"sv);
        return;
    }

    m_sizes = {width, height, 1};
    m_nbChannels = 3;
    m_hdr = true;

    m_data.resize(static_cast<size_t>(width*height*m_nbChannels)*4);
    std::copy(std::begin(data), std::end(data), reinterpret_cast<geo::Pt3<float>*>(m_data.data()));
}


auto Texture::load_2d_image_file_data(const std::string &pathTexture, bool flip, int targetNbChannels) -> bool{

    if(targetNbChannels <= 0 || targetNbChannels > 4){
        Log::error(std::format("[Texture::load_2d_image_file_data] Invalid input target nb of channels with texture: {}\n"sv, pathTexture));
        return false;
    }

    // set flipping
    stbi_set_flip_vertically_on_load(flip);

    // N=#comp components
    // 1 grey
    // 2 grey, alpha
    // 3 red, green, blue
    // 4 red, green, blue, alpha

    int width;
    int height;

    // check extension
    namespace fs = std::filesystem;
    if(fs::path(pathTexture).extension() == u8".hdr"){

        m_hdr = true;

        float *loadedDataF = nullptr;

        // load data with stb
        loadedDataF = stbi_loadf(pathTexture.c_str(), &width, &height, &m_nbChannels, targetNbChannels);
        if(!loadedDataF){
            Log::error(std::format("[Texture::load_2d_image_file_data] Cannot load texture: {}\n"sv, pathTexture));
            return false;
        }

        m_data.resize(static_cast<size_t>(width*height*targetNbChannels)*4);
        std::copy(loadedDataF, loadedDataF + width*height*targetNbChannels, reinterpret_cast<float*>(m_data.data()));

        // clean
        stbi_image_free(loadedDataF);
    }else{

        unsigned char *loadedData = nullptr;

        // load data with stb
        loadedData = stbi_load(pathTexture.c_str(), &width, &height, &m_nbChannels, targetNbChannels);
        if(!loadedData){
            Log::error(std::format("[Texture::load_2d_image_file_data] Cannot load texture: {}\n"sv, pathTexture));
            return false;
        }

        m_data.resize(static_cast<size_t>(width*height*targetNbChannels));
        std::copy(loadedData, loadedData + width*height*targetNbChannels, m_data.data());

        // clean
        stbi_image_free(loadedData);
    }

    m_sizes = {width, height, 1};
    if(targetNbChannels == 0){
        targetNbChannels = m_nbChannels;
    }

    // update channels nb
    m_nbChannels = targetNbChannels;

    return true;
}

auto Texture::write_2d_image_file_data(const std::string &pathTexture) -> bool{

    // check extension
    namespace fs = std::filesystem;
    const auto path = fs::path(pathTexture);
    const auto parentDir = path.parent_path();
    if(!fs::exists(parentDir)){
        Log::error(std::format("[Texture::write_2d_image_file_data] Cannot save texture at path: {}\n"sv, pathTexture));
        return false;
    }

    const auto extension = path.extension();
    const auto filePath  = path.string();

    int res = 0;
    if(extension == u8".png"){
        if (!m_hdr){
            res = stbi_write_png(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, m_data.data(), m_sizes[0]*m_nbChannels);
        }else{
            std::vector<unsigned char> data(m_sizes[0]*m_sizes[1]*m_nbChannels);
            auto hdrData = reinterpret_cast<float*>(m_data.data());
            for(size_t ii = 0; ii < data.size(); ++ii){
                data[ii] = static_cast<unsigned int>(hdrData[ii]*255);
            }
            res = stbi_write_png(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, data.data(), m_sizes[0]*m_nbChannels);
        }
    }else if(extension == u8".bmp"){
        if (!m_hdr){
            res = stbi_write_bmp(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, m_data.data());
        }else{
            std::vector<unsigned char> data(m_sizes[0]*m_sizes[1]*m_nbChannels);
            auto hdrData = reinterpret_cast<float*>(m_data.data());
            for(size_t ii = 0; ii < data.size(); ++ii){
                data[ii] = static_cast<unsigned int>(hdrData[ii]*255);
            }
            res = stbi_write_bmp(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, data.data());
        }
    }else if(extension == u8".tga"){
        if (!m_hdr){
            res = stbi_write_tga(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, m_data.data());
        }else{
            std::vector<unsigned char> data(m_sizes[0]*m_sizes[1]*m_nbChannels);
            auto hdrData = reinterpret_cast<float*>(m_data.data());
            for(size_t ii = 0; ii < data.size(); ++ii){
                data[ii] = static_cast<unsigned int>(hdrData[ii]*255);
            }
            res = stbi_write_tga(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, data.data());
        }
    }else if(extension == u8".jpg"){
        if (!m_hdr){
            res = stbi_write_jpg(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, m_data.data(), 100);
        }else{
            std::vector<unsigned char> data(m_sizes[0]*m_sizes[1]*m_nbChannels);
            auto hdrData = reinterpret_cast<float*>(m_data.data());
            for(size_t ii = 0; ii < data.size(); ++ii){
                data[ii] = static_cast<unsigned int>(hdrData[ii]*255);
            }
            res = stbi_write_jpg(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, data.data(), 100);
        }
    }else if(extension == u8".hdr"){
        if (!m_hdr){
            std::vector<float> data(m_sizes[0]*m_sizes[1]*m_nbChannels);
            for(size_t ii = 0; ii < m_data.size(); ++ii){
                data[ii] = static_cast<float>(m_data[ii])/255.f;
            }
            res = stbi_write_hdr(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, reinterpret_cast<float*>(data.data()));
        }else{
            res = stbi_write_hdr(filePath.c_str(), m_sizes[0], m_sizes[1], m_nbChannels, reinterpret_cast<float*>(m_data.data()));
        }
    }else{
        Log::error(std::format("[Texture::write_2d_image_file_data] Cannot save texture at path: {}, extension not managed.\n"sv, pathTexture));
        return false;
    }

    if(res == 0){
        Log::error(std::format("[Texture::write_2d_image_file_data] Fail to save texture: {}\n"sv, pathTexture));
        return false;
    }

    return true;
}

