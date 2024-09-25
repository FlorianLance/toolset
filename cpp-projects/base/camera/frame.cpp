
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

#include "frame.hpp"

// local
#include "utility/io_data.hpp"
#include "utility/io_fstream.hpp"
#include "data/json_utility.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto Frame::data_size() const noexcept -> size_t{
    return
        sizeof(idDevice) +
        sizeof(idCapture) +
        sizeof(afterCaptureTS) +
        sizeof(receivedTS);
}

auto Frame::init_from_json(const nlohmann::json &json) -> void {

    size_t unreadCount = 0;
    read_and_update_value(json, unreadCount, "id_device"sv,         idDevice);
    read_and_update_value(json, unreadCount, "id_capture"sv,        idCapture);
    read_and_update_value(json, unreadCount, "after_capture_ts"sv,  afterCaptureTS);
    read_and_update_value(json, unreadCount, "received_ts"sv,       receivedTS);

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[Frame::init_from_json] [{}] values have not been initialized from json data.\n"sv, unreadCount));
    }
}

auto Frame::convert_to_json() const -> nlohmann::json {

    return json{
        {"id_device"sv,           idDevice},
        {"id_capture"sv,          idCapture},
        {"after_capture_ts"sv,    afterCaptureTS},
        {"received_ts"sv,         receivedTS},
    };
}

auto Frame::convert_to_json_str() const -> std::string{
    return convert_to_json().dump(4);
}

auto Frame::convert_to_json_binary() const -> std::vector<uint8_t>{
    return json::to_bson(convert_to_json());
}

auto Frame::init_from_json_binary(std::span<const uint8_t> jsonData) -> bool{
    try {
        init_from_json(json::from_bson(jsonData));
        return true;
    }catch (const json::parse_error& e){
        Logger::error(std::format("Frame::init_from_json_binary] Error while reading JSON binary [{}] \n", e.what()));
    }
    return false;
}

auto Frame::init_from_json_str(std::string_view jsonStr) -> bool{
    try {
        init_from_json(json::parse(jsonStr));
        return true;
    }catch (const json::parse_error& e){
        Logger::error(std::format("Frame::init_from_json_str] Error while reading JSON string [{}]\n", e.what()));
    }
    return false;
}

auto Frame::init_from_file_stream(std::ifstream &file) -> void{
    read(idDevice,        file);
    read(idCapture,       file);
    read(afterCaptureTS,  file);
    read(receivedTS,      file);
}

auto Frame::init_from_data(std::span<const std::byte> data, size_t &offset) -> void{
    read(idDevice,       data, offset);
    read(idCapture,       data, offset);
    read(afterCaptureTS,  data, offset);
    read(receivedTS,      data, offset);
}

auto Frame::write_to_file_stream(std::ofstream &file) -> void{
    write(idDevice,       file);
    write(idCapture,       file);
    write(afterCaptureTS,  file);
    write(receivedTS,      file);
}

auto Frame::write_to_data(std::span<std::byte> data, size_t &offset) -> void{
    write(idDevice,       data, offset);
    write(idCapture,       data, offset);
    write(afterCaptureTS,  data, offset);
    write(receivedTS,      data, offset);
}
