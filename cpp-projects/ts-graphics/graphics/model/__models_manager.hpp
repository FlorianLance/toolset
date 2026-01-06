
// /*******************************************************************************
// ** Toolset-ts-graphics                                                        **
// ** MIT License                                                                **
// ** Copyright (c) [2018] [Florian Lance]                                       **
// **                                                                            **
// ** Permission is hereby granted, free of charge, to any person obtaining a    **
// ** copy of this software and associated documentation files (the "Software"), **
// ** to deal in the Software without restriction, including without limitation  **
// ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
// ** and/or sell copies of the Software, and to permit persons to whom the      **
// ** Software is furnished to do so, subject to the following conditions:       **
// **                                                                            **
// ** The above copyright notice and this permission notice shall be included in **
// ** all copies or substantial portions of the Software.                        **
// **                                                                            **
// ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
// ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
// ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
// ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
// ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
// ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
// ** DEALINGS IN THE SOFTWARE.                                                  **
// **                                                                            **
// ********************************************************************************/


// #pragma once

// // local
// #include "utility/id_alias_map.hpp"
// #include "model_mesh.hpp"

// namespace tool::graphics {

// class ModelsManager{

//     using Alias  = std::string;
//     using Path   = std::string;
//     using AliasV = std::string_view;
//     using PathV  = std::string_view;

// public:

//     static auto get_instance() -> ModelsManager*;

//     auto add(std::vector<std::pair<Alias,Path>> &&aliasPaths, bool forceReload = false) -> bool;
//     auto add(const std::string &alias, const std::string &path) -> bool;
    
//     auto get(AliasV alias) const -> std::weak_ptr<graphics::ModelMesh>;
//     auto get_animations_count(AliasV alias) const -> size_t;
//     auto get_animation(AliasV alias, size_t id) const -> Animation;
//     auto get_animation_name(AliasV alias, size_t id) const -> std::string_view;

//     inline auto count() const noexcept -> size_t {return models.count();}

// private:
    
//     auto get_ptr(AliasV alias) const -> graphics::ModelMesh*;

//     s_umap<Path, AliasV> aliasPerPath;
//     IdAliasMapSharedPtr<graphics::ModelMesh> models;
// };
// }



