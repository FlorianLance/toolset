
/*******************************************************************************
** Toolset-ts-global-app                                                      **
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
#include <string>

// stb
#include <stb/stb_image_write.h>

// base
#include "geometry/cloud_io.hpp"


auto simd() -> void;
auto time() -> void;

// devices
// # mic
auto microphone() -> void;
// # orbec
auto dc_device_idle() -> void;
auto femto_mega() -> void;
auto raw_orbbec_femto() -> void;
// # k2
auto raw_kinect2() -> void;
auto kinect2() -> void;
// # j4
auto test_kinect4() -> void;
auto test_raw_k4device() -> void;

// geometry
auto pcl_encoding() -> void;

// kvid
auto convert_kvid(const std::string &path, const std::string &dest) -> void;
auto process_kvid() -> void;
// dc player
auto dc_player_test() -> void;
auto test_dc_player_merge(std::string kvidPath) -> void;

auto dc_settings() -> void;

auto encode1() -> void;
auto encode2() -> void;

auto multi_devices_benchmark() -> void;

auto test_bdf_reader(const std::string &path) -> void;

