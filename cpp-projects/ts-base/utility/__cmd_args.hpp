
///*******************************************************************************
//** Toolset-ts-base                                                            **
//** MIT License                                                                **
//** Copyright (c) [2018] [Florian Lance]                                       **
//**                                                                            **
//** Permission is hereby granted, free of charge, to any person obtaining a    **
//** copy of this software and associated documentation files (the "Software"), **
//** to deal in the Software without restriction, including without limitation  **
//** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
//** and/or sell copies of the Software, and to permit persons to whom the      **
//** Software is furnished to do so, subject to the following conditions:       **
//**                                                                            **
//** The above copyright notice and this permission notice shall be included in **
//** all copies or substantial portions of the Software.                        **
//**                                                                            **
//** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
//** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
//** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
//** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
//** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
//** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
//** DEALINGS IN THE SOFTWARE.                                                  **
//**                                                                            **
//********************************************************************************/

//#pragma once

//// std
//#include <variant>
//#include <optional>
//#include <string_view>
//#include <vector>

//namespace tool{

//struct CmdOption{
//    template<typename OptT>
//    using opt = std::optional<OptT>;
//    std::string_view name;
//    std::variant<opt<int>, opt<float>, opt<double>, opt<std::string>, std::monostate> value = std::monostate{};
//    std::string_view info = "";
//};

//struct CmdOptions{

//    auto add_no_value_option(std::string_view name, std::string_view info) -> void{
//        options.push_back({name,std::monostate{},info});
//    }

//    template<typename OptT>
//    auto add_option(std::string_view name, std::string_view info, std::optional<OptT> defaultValue) -> void{
//        options.push_back({name,defaultValue,info});
//    }

//    auto add_option(CmdOption &&option) -> void{
//        options.push_back(std::move((option)));
//    }

//    std::vector<CmdOption> options;
//};

//class CmdLineArgsParser{

//    template<typename OptT>
//    using opt = std::optional<OptT>;

//public:
//    static auto get() -> CmdLineArgsParser*;
//    auto add_options(std::vector<CmdOption> options) -> void;
//    auto parse(int argc, char *argv[]) -> void;

//    auto display_arguments() const -> void;

//    auto get_option(std::string_view name) const -> std::optional<std::variant<int,float,double,std::string>>;
//    auto get_int_option(std::string_view name) -> std::optional<int> {return std::nullopt;}

//private:

//    static auto initialize() -> void;
//};

//}
