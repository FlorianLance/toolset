
/*******************************************************************************
** Toolset-base                                                               **
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

// std
#include <fstream>
#include <ostream>
#include <string_view>
#include <vector>

// local
#include "geometry/point2.hpp"
#include "geometry/point4.hpp"
#include "geometry/cloud.hpp"

namespace tool::io {


// struct ImageIO{

//     static auto save_image(const std::string &path, std::span<std::uint16_t> data, size_t width, size_t height, std::uint16_t min, std::uint16_t max, std::span<geo::Pt3f> gradient) -> bool{

//         std::ofstream file;
//         file.open(path);
//         if(!file.is_open()){
//             //Logger::error(std::format("[ImageIO::save_image] Cannot open file from path {}.\n", path));
//             return false;
//         }

//         const auto diff = max - min;

//         // file << std::format(vLine2, v1, v2);


//         return true;
//     }
// };


using namespace std::string_view_literals;
class CloudIO{

public:

    static auto save_cloud(const std::string &path, const geo::ColoredCloudData &cloud) -> bool{
        if(cloud.is_valid()){

            return save_cloud<float, float>(
                path,
                cloud.size(),
                cloud.vertices.get_data(),
                cloud.has_colors()  ? cloud.colors.get_data() : nullptr,
                cloud.has_normals() ? cloud.normals.get_data() : nullptr
            );
        }
        return false;
    }

    template<typename TV,typename TC>
    static auto save_cloud(const std::string &path, size_t size, const geo::Pt2<TV> *vertices, const geo::Pt3<TC> *colors = nullptr, const geo::Pt2<TV> *normals = nullptr) -> bool{
        return save_cloud<TC,TV,2,3>(
            path,
            size,
            reinterpret_cast<const TV*>(vertices),
            colors != nullptr ? reinterpret_cast<const TC*>(colors) : nullptr,
            normals != nullptr ? reinterpret_cast<const TV*>(normals) : nullptr
        );
    }

    template<typename TV,typename TC>
    static auto save_cloud(const std::string &path, size_t size, const geo::Pt2<TV> *vertices, const geo::Pt4<TC> *colors = nullptr, const geo::Pt2<TV> *normals = nullptr) -> bool{
        return save_cloud<TC,TV,2,4>(
            path,
            size,
            reinterpret_cast<const TV*>(vertices),
            colors != nullptr ? reinterpret_cast<const TC*>(colors) : nullptr,
            normals != nullptr ? reinterpret_cast<const TV*>(normals) : nullptr
        );
    }

    template<typename TV,typename TC>
    static auto save_cloud(const std::string &path, size_t size, const geo::Pt3<TV> *vertices, const geo::Pt3<TC> *colors = nullptr, const geo::Pt3<TV> *normals = nullptr) -> bool{
        return save_cloud<TC,TV,3,3>(
            path,
            size,
            reinterpret_cast<const TV*>(vertices),
            colors != nullptr ? reinterpret_cast<const TC*>(colors) : nullptr,
            normals != nullptr ? reinterpret_cast<const TV*>(normals) : nullptr
        );
    }

    template<typename TV,typename TC>
    static auto save_cloud(const std::string &path, size_t size, const geo::Pt3<TV> *vertices, const geo::Pt4<TC> *colors = nullptr, const geo::Pt3<TV> *normals = nullptr) -> bool{
        return save_cloud<TC,TV,3,4>(
            path,
            size,
            reinterpret_cast<const TV*>(vertices),
            colors != nullptr ? reinterpret_cast<const TC*>(colors) : nullptr,
            normals != nullptr ? reinterpret_cast<const TV*>(normals) : nullptr
        );
    }

    template <typename TV,typename TC, int dimension, int nbChannels>
    static auto save_cloud(const std::string &path, const std::vector<TV> &vertices, const std::vector<TC> &colors) -> bool{

        if(check_buffers_sizes(vertices.size(), colors.size())){
            if(colors.size() == 0){
                return save_cloud<TV, dimension, 0>(path, vertices.size(), vertices.data(), nullptr, nullptr);
            }else{
                return save_cloud<TV, TC, dimension, nbChannels>(path, vertices.size(), vertices.data(), colors.data(), nullptr);
            }
        }
        return false;
    }

    template <typename TV, typename TC, int dimension, int nbChannels>
    static auto save_cloud(const std::string &path, size_t size, const TV *vertices, const TC *colors, const TV *normals) -> bool{

        if(!check_input_values(size, vertices != nullptr, colors != nullptr, dimension, nbChannels)){
            return false;
        }

        std::ofstream file;
        if(!open_file(path, file)){
            return false;
        }

        if(colors != nullptr){
            for(size_t ii = 0; ii < size; ++ii){
                write_vertex_line<TV,TC,dimension,nbChannels>(file, &vertices[ii*dimension], &colors[ii*nbChannels]);
            }
        }else{
            for(size_t ii = 0; ii < size; ++ii){
                write_vertex_line<TV,dimension>(file, &vertices[ii*dimension]);
            }
        }

        if(normals != nullptr){
            for(size_t ii = 0; ii < size; ++ii){
                write_vertex_normal_line<TV,dimension>(file, &normals[ii*dimension]);
            }
        }

        file.close();
        return true;
    }

private:


    template <typename TV, int dimension>
    static auto write_vertex_line(std::ostream &file, const TV *v) -> void{
        if constexpr (dimension == 2){
            write_vertex(file, v[0], v[1]);
        } else if constexpr (dimension == 3){
            write_vertex(file, v[0], v[1], v[2]);
        }
    }

    template <typename TV,typename TC, int dimension, int nbChannels>
    static auto write_vertex_line(std::ostream &file, const TV *v, const TC *c) -> void{
        if constexpr (dimension == 2 && nbChannels == 3){
            write_vertex(file, v[0], v[1], c[0], c[1], c[2]);
        }else if constexpr (dimension == 2 && nbChannels == 4){
            write_vertex(file, v[0], v[1], c[0], c[1], c[2], c[3]);
        }else if constexpr (dimension == 3 && nbChannels == 3){
            write_vertex(file, v[0], v[1], v[2], c[0], c[1], c[2]);
        }else if constexpr (dimension == 3 && nbChannels == 4){
            write_vertex(file, v[0], v[1], v[2], c[0], c[1], c[2], c[3]);
        }
    }

    template <typename TV, int dimension>
    static auto write_vertex_normal_line(std::ostream &file, const TV *vn) -> void{
        if constexpr (dimension == 2){
            write_vertex_normal(file, vn[0], vn[1]);
        } else if constexpr (dimension == 3){
            write_vertex_normal(file, vn[0], vn[1], vn[2]);
        }
    }

    static auto write_vertex(std::ostream &file, float v1, float v2) -> void;
    static auto write_vertex(std::ostream &file, float v1, float v2, float v3) -> void;
    static auto write_vertex(std::ostream &file, float v1, float v2, float v3, float v4, float v5) -> void;
    static auto write_vertex(std::ostream &file, float v1, float v2, float v3, float v4, float v5, float v6) -> void;
    static auto write_vertex(std::ostream &file, float v1, float v2, float v3, float v4, float v5, float v6, float v7) -> void;

    static auto write_vertex_normal(std::ostream &file, float v1, float v2) -> void;
    static auto write_vertex_normal(std::ostream &file, float v1, float v2, float v3) -> void;

    static auto open_file(const std::string &path, std::ofstream &file) -> bool;
    static auto check_buffers_sizes(size_t sizeVertices, size_t sizeColors) -> bool;
    static auto check_input_values(size_t size, bool hasVertices, bool hasColors, int dimension, int nbChannels) -> bool;

    static constexpr std::string_view vLine2 = "v {} {}\n"sv;
    static constexpr std::string_view vLine3 = "v {} {} {}\n"sv;
    static constexpr std::string_view vLine5 = "v {} {} {} {} {}\n"sv;
    static constexpr std::string_view vLine6 = "v {} {} {} {} {} {}\n"sv;
    static constexpr std::string_view vLine7 = "v {} {} {} {} {} {} {}•\n"sv;

    static constexpr std::string_view vnLine2 = "vn {} {}\n"sv;
    static constexpr std::string_view vnLine3 = "vn {} {} {}\n"sv;
};

}
