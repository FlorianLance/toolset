
///*******************************************************************************
//** Toolset-base                                                               **
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

//#include "cmd_args.hpp"

//// std
//#include <memory>
//#include <format>
//#include <any>

//// boost
//#include <boost/program_options.hpp>

//// local
//#include "utility/logger.hpp"

//using namespace tool;
//namespace po = boost::program_options;


//struct CmdLineOptions{

//    CmdLineOptions() : desc("Allowed options"){

//    }
//    po::variables_map vm;
//    po::options_description desc;
//};

//struct Global{
//    static inline std::unique_ptr<CmdLineArgsParser> instance = nullptr;
//    static inline std::unique_ptr<CmdLineOptions> args = nullptr;
//};

//auto CmdLineArgsParser::initialize() -> void{
//    Global::instance = std::make_unique<CmdLineArgsParser>();
//    Global::args     = std::make_unique<CmdLineOptions>();
//}

//auto CmdLineArgsParser::get() -> CmdLineArgsParser*{
//    if(Global::instance == nullptr){
//        initialize();
//    }
//    return Global::instance.get();
//}

//auto CmdLineArgsParser::add_options(std::vector<CmdOption> options) -> void{

//    for(const auto &option : options){
//        auto addOpt = Global::args.get()->desc.add_options();
//        if(const auto* pval = std::get_if<std::optional<int>>(&option.value)){
//            if(pval->has_value()){
//                addOpt(option.name.data(), po::value<int>()->default_value(pval->value()), option.info.data());
//            }else{
//                addOpt(option.name.data(), po::value<int>(), option.info.data());
//            }
//        }else if(const std::optional<float>* pval = std::get_if<std::optional<float>>(&option.value)){
//            if(pval->has_value()){
//                addOpt(option.name.data(), po::value<float>()->default_value(pval->value()), option.info.data());
//            }else{
//                addOpt(option.name.data(), po::value<float>(), option.info.data());
//            }
//        }else if(const std::optional<double>* pval = std::get_if<std::optional<double>>(&option.value)){
//            if(pval->has_value()){
//                addOpt(option.name.data(), po::value<double>()->default_value(pval->value()), option.info.data());
//            }else{
//                addOpt(option.name.data(), po::value<double>(), option.info.data());
//            }
//        }else if(const std::optional<std::string>* pval = std::get_if<std::optional<std::string>>(&option.value)){
//            if(pval->has_value()){
//                addOpt(option.name.data(), po::value<std::string>()->default_value(pval->value()), option.info.data());
//            }else{
//                addOpt(option.name.data(), po::value<std::string>(), option.info.data());
//            }
//        }else if(const std::monostate* pval = std::get_if<std::monostate>(&option.value)){
//            addOpt(option.name.data(), option.info.data());
//        }
//    }
//}

//auto CmdLineArgsParser::parse(int argc, char *argv[]) -> void{
//    auto *args = Global::args.get();
//    po::store(po::parse_command_line(argc, argv, args->desc), args->vm);
//    po::notify(args->vm);
//}

//auto CmdLineArgsParser::display_arguments() const -> void{
//    Logger::message("Command line arguments:\n");
//    for(const auto &value : Global::args->vm){
//        if(value.second.value().type() == typeid(int)){
//            Logger::message(std::format("Arg [{}] with value [{}]\n", value.first, value.second.as<int>()));
//        } else if(value.second.value().type() == typeid(float)){
//            Logger::message(std::format("Arg [{}] with value [{}]\n", value.first, value.second.as<float>()));
//        } else if(value.second.value().type() == typeid(double)){
//            Logger::message(std::format("Arg [{}] with value [{}]\n", value.first, value.second.as<double>()));
//        } else if(value.second.value().type() == typeid(std::string)){
//            Logger::message(std::format("Arg [{}] with value [{}]\n", value.first, value.second.as<std::string>()));
//        }else{
//            Logger::message(std::format("Arg [{}] with no value.\n",value.first));
//        }
//    }
//}

//auto CmdLineArgsParser::get_option(std::string_view name) const -> std::optional<std::variant<int,float,double,std::string>>{

//    if(Global::args->vm.contains(name.data())){
//        const auto &v = Global::args->vm[name.data()];
//        std::variant<int, float, double, std::string> value;
//        if(v.value().type() == typeid(int)){
//            value = v.as<int>();
//        } else if(v.value().type() == typeid(float)){
//            value = v.as<float>();
//        } else if(v.value().type() == typeid(double)){
//            value = v.as<double>();
//        } else if(v.value().type() == typeid(std::string)){
//            value = v.as<std::string>();
//        }

//        return value;
//    }


//    return std::nullopt;
//}


////int foo(int argc, char *argv[]){

////    {
////    // Declare the supported options.
////    po::options_description desc("Allowed options");
////    desc.add_options()
////        ("help", "produce help message")
////        ("compression", po::value<int>(), "set compression level")
////    ;

////    po::variables_map vm;
////    po::store(po::parse_command_line(argc, argv, desc), vm);
////    po::notify(vm);

////    if (vm.count("help")) {
////        std::cout << desc << "\n";
////        return 1;
////    }

////    if (vm.count("compression")) {
////        std::cout << "Compression level was set to "
////     << vm["compression"].as<int>() << ".\n";
////    } else {
////        std::cout << "Compression level was not set.\n";
////    }
////    }

////    {
////        int opt;
////        po::options_description desc("Allowed options");
////        desc.add_options()
////            ("help", "produce help message")
////            ("optimization",    po::value<int>(&opt)->default_value(10), "optimization level")
////            ("include-path,I",  po::value<std::vector<std::string>>(),"include path")
////            ("input-file",      po::value<std::vector<std::string>>(), "input file")
////        ;

////        po::positional_options_description p;
////        p.add("input-file", -1);

////        po::variables_map vm;
////        po::store(po::command_line_parser(argc, argv).
////                  options(desc).positional(p).run(), vm);
////        po::notify(vm);


////        // Declare a group of options that will be
////        // allowed only on command line
////        po::options_description generic("Generic options");
////        generic.add_options()
////            ("version,v", "print version string")
////            ("help", "produce help message")
////            ;

////        // Declare a group of options that will be
////        // allowed both on command line and in
////        // config file
////        po::options_description config("Configuration");
////        config.add_options()
////            ("optimization", po::value<int>(&opt)->default_value(10),
////                  "optimization level")
////            ("include-path,I",
////                 po::value< std::vector<std::string> >()->composing(),
////                 "include path")
////            ;

////        // Hidden options, will be allowed both on command line and
////        // in config file, but will not be shown to the user.
////        po::options_description hidden("Hidden options");
////        hidden.add_options()
////            ("input-file", po::value< std::vector<std::string> >(), "input file")
////            ;

////        po::options_description cmdline_options;
////        cmdline_options.add(generic).add(config).add(hidden);

////        po::options_description config_file_options;
////        config_file_options.add(config).add(hidden);

////        po::options_description visible("Allowed options");
////        visible.add(generic).add(config);

////    }

////    return 1;
////}
