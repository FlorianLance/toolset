
/*******************************************************************************
** Toolset-opengl-utility                                                     **
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

#pragma once

// base
#include "graphics/model/model_mesh.hpp"

// local
#include "opengl/texture/texture_2d_tbo.hpp"
#include "base_drawer.hpp"

namespace tool::gl {

class GridQuadTrianglesDrawer : public BaseDrawer{
public:
    GridQuadTrianglesDrawer() : BaseDrawer(DrawerType::Triangles){}
    auto initialize(float xsize, float zsize, size_t xdivs, size_t zdivs, float smax = 1.0f, float tmax = 1.0f) -> void;
};

class QuadTrianglesDrawer : public BaseDrawer{
public:
    QuadTrianglesDrawer() : BaseDrawer(DrawerType::Triangles){}
    auto initialize(bool dynamic) -> void;
    auto update(std::span<const geo::Pt3f,4> vertices) -> void;
};

class CubeTrianglesDrawer : public BaseDrawer{
public:
    CubeTrianglesDrawer() : BaseDrawer(DrawerType::Triangles){}
    auto initialize(float side) -> void;
};

class SkyboxTrianglesDrawer : public BaseDrawer{
public:
    SkyboxTrianglesDrawer() : BaseDrawer(DrawerType::Triangles){}
    auto initialize(float side = 100.f, std::optional<GLuint> cubemap = std::nullopt) -> void;
};

class SphereTrianglesDrawer : public BaseDrawer{
public:
    SphereTrianglesDrawer() : BaseDrawer(DrawerType::Triangles){}
    auto initialize(float radius, size_t nSlices = 20, size_t nStacks = 20) -> void;
};

class TorusTrianglesDrawer : public BaseDrawer{
public:
    TorusTrianglesDrawer() : BaseDrawer(DrawerType::Triangles){}
    auto initialize(float outerRadius = 0.7f * 1.5f, float innerRadius = 0.3f * 1.5f, size_t nSides = 50, size_t nRings = 50) -> void;
};

class TeapotTrianglesDrawer : public BaseDrawer{
public:
    TeapotTrianglesDrawer() : BaseDrawer(DrawerType::Triangles){}
    auto initialize(int grid) -> void;
};

class MeshDrawer2 : public BaseDrawer{
public:
    MeshDrawer2() : BaseDrawer(DrawerType::Triangles){}
    auto initialize(const graphics::Mesh &mesh) -> void;
};

class GMeshDrawer2 : public BaseDrawer{
public:
    GMeshDrawer2() : BaseDrawer(DrawerType::Triangles){}
    auto initialize(const graphics::SubModelMesh &gmesh, const std::vector<graphics::TextureInfo> &texturesInfo = {}) -> void;

private:
    umap<gl::TextureType, std::vector<std::unique_ptr<Texture2D>>> m_textures;
};

class ModelMeshDrawer2 : public HierarchyDrawer2{
public:
    ModelMeshDrawer2() : HierarchyDrawer2(DrawerType::Triangles){}
    auto initialize(const graphics::ModelMesh &modelMesh, const std::vector<graphics::TextureInfo> &texturesInfo = {}) -> void;
    auto update_bones(const graphics::ModelMesh &modelMesh) -> void{}
};

}

// class HierarchyDrawer : public Drawer{
// protected:
//     std::vector<std::unique_ptr<Drawer>> drawableObjects;
//     std::vector<std::unique_ptr<Drawer>> children;
// };

// class GMeshDrawer : public Drawer{
//     umap<TextureType, std::vector<std::pair<GLuint,TBO>>> m_textures;
// public:
//     // if texturesInfo empty use textures from material
//     auto init(const std::shared_ptr<graphics::SubModelMesh> &gmesh, const std::vector<graphics::TextureInfo> &texturesInfo = {}) -> void;
// };



// auto GMeshDrawer::init(const std::shared_ptr<graphics::SubModelMesh> &gmesh, const std::vector<tool::graphics::TextureInfo> &texturesInfo) -> void{

//     clean();

//     if(texturesInfo.empty()){
//         for(auto &texturesT : gmesh->material->texturesInfo){
//             for(auto textureInfo : texturesT.second){
//                 gl::Texture2D tbo;
//                 tbo.load_texture(textureInfo.texture, textureInfo.options);
//                 // m_textures[textureInfo.options.type].emplace_back(std::make_pair(GLuint{tbo.id()},std::move(tbo)));
//             }
//         }
//     }else{
//         for(auto textureInfo : texturesInfo){
//             gl::Texture2D tbo;
//             tbo.load_texture(textureInfo.texture);
//             // m_textures[textureInfo.options.type].emplace_back(std::make_pair(GLuint{tbo.id()},std::move(tbo)));
//         }
//     }

//     // add textures in good order
//     if(m_textures.count(TextureType::diffuse)!=0){
//         texturesNames.emplace_back(m_textures[TextureType::diffuse][0].second.id());
//     }
//     if(m_textures.count(TextureType::normal)!=0){
//         texturesNames.emplace_back(m_textures[TextureType::normal][0].second.id());
//     }
//     if(m_textures.count(TextureType::height)!=0){
//         texturesNames.emplace_back(m_textures[TextureType::height][0].second.id());
//     }

//     drawableObject = std::make_unique<gl::Mesh>(&gmesh->mesh);
// }



// // # model
// class ModelDrawer : public HierarchyDrawer{
//     std::shared_ptr<graphics::ModelMesh> modelP = nullptr;
// public:
//     ModelDrawer() = default;
//     ModelDrawer(const std::weak_ptr<graphics::ModelMesh> &model, const std::vector<graphics::TextureInfo> &texturesInfo = {}){
//         init(model, texturesInfo);
//     }
//     auto init(const std::weak_ptr<graphics::ModelMesh> &model, const std::vector<graphics::TextureInfo> &texturesInfo) -> void;
//     auto draw(gl::ShaderProgram *shader = nullptr)  -> void override;
//     auto draw_adjacency(gl::ShaderProgram *shader = nullptr)  -> void override;
//     auto update_animation(std::string_view animationName, float time) -> void;

//     auto model() -> graphics::ModelMesh*{
//         if(modelP){
//             return modelP.get();
//         }
//         return nullptr;
//     }

// private:

//     auto set_bones_uniform(graphics::ModelMesh *model, gl::ShaderProgram *shader) -> void;
// };


// auto ModelDrawer::init(const std::weak_ptr<graphics::ModelMesh> &model, const std::vector<graphics::TextureInfo> &texturesInfo) -> void{

//     clean();

//     modelP = model.lock();
//     if(modelP){
//         for(auto &gmesh : modelP->meshes){
//             auto gmeshD = std::make_unique<GMeshDrawer>();
//             gmeshD->init(gmesh, texturesInfo);
//             drawableObjects.emplace_back(std::move(gmeshD));
//         }

//         for(auto &modelChild : modelP->children){
//             auto modelDrawer = std::make_unique<ModelDrawer>();
//             modelDrawer->init(modelChild, {});
//             children.emplace_back(std::move(modelDrawer));
//         }
//     }
// }

// auto ModelDrawer::draw(gl::ShaderProgram *shader) -> void{

//     if(modelP){

//         set_bones_uniform(modelP.get(), shader);

//         for(auto &gmesh : drawableObjects){
//             gmesh->draw();
//         }

//         for(auto &child : children){
//             child->draw(shader);
//         }
//     }
// }

// auto ModelDrawer::draw_adjacency(ShaderProgram *shader) -> void{

//     if(modelP){

//         set_bones_uniform(modelP.get(), shader);

//         for(auto &gmesh : drawableObjects){
//             gmesh->draw_adjacency();
//         }

//         for(auto &child : children){
//             child->draw_adjacency(shader);
//         }
//     }
// }

// auto ModelDrawer::update_animation(std::string_view animationName, float time) -> void{

//     if(modelP){
//         modelP->update_animation(animationName, time);
//     }

//     for(auto &child : children){
//         dynamic_cast<ModelDrawer*>(child.get())->update_animation(animationName, time);
//     }
// }

// auto ModelDrawer::set_bones_uniform(graphics::ModelMesh *model, ShaderProgram *shader) -> void{

//     if(shader == nullptr){
//         return;
//     }

//     if(model->animations.size() > 0){

//         std::vector<geo::Mat4f> transforms;
//         for(size_t ii = 0; ii < model->bonesInfo.size(); ++ii){
//             transforms.push_back(model->bonesInfo[ii].final);
//         }
//         if(transforms.size()> 100){
//             transforms.resize(100);
//         }

//         shader->set_uniform("BonesM[0]", transforms);
//     }
// }

