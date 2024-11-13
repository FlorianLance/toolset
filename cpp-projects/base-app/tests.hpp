
#pragma once

// std
#include <string>

// stb
#include "thirdparty/stb/stb_image_write.h"

// base
#include "io/cloud_io.hpp"


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


