
/*******************************************************************************
** Toolset-opengl-utility                                                     **
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

#include "drawer.hpp"

// local
#include "opengl/texture/texture_2d_tbo.hpp"
#include "opengl/gl_draw.hpp"
#include "utility/logger.hpp"

using namespace tool;
using namespace tool::gl;

Drawer::~Drawer(){
    clean();
}

auto Drawer::draw(ShaderProgram *shader) -> void{
    static_cast<void>(shader);
    if(drawableObject != nullptr){
        if(texturesNames.size() > 0){
            TBO::bind(texturesNames,0);
        }
        drawableObject->data->render();
    }
}

auto Drawer::draw_adjacency(ShaderProgram *shader) -> void{
    static_cast<void>(shader);
    if(drawableObject != nullptr){
        if(texturesNames.size() > 0){
            TBO::bind(texturesNames, 0);
        }
        drawableObject->data->render_adjacency();
    }
}

auto Drawer::draw_patches(ShaderProgram *shader) -> void{
    static_cast<void>(shader);
    if(drawableObject != nullptr){
        if(texturesNames.size() > 0){
            TBO::bind(texturesNames, 0);
        }
        drawableObject->data->render_patches();
    }
}

auto Drawer::draw_instances(ShaderProgram *shader) -> void{

    static_cast<void>(shader);
    if(drawableObject != nullptr){
        if(texturesNames.size() > 0){
            TBO::bind(texturesNames, 0);
        }
        // ...
    }
}


AxesDrawer::AxesDrawer(float length){
    init(length);
}

auto AxesDrawer::init(float length) -> void{
    clean();
    drawableObject = std::make_unique<Axes>(length);
}

GridDrawer::GridDrawer(float width, float height, int nbX, int nbY){
    init(width, height, nbX, nbY);
}

auto GridDrawer::init(float width, float height, int nbX, int nbY) -> void{
    clean();
    drawableObject = std::make_unique<Grid>(width, height, nbX, nbY);
}

FullscreenQuadDrawer::FullscreenQuadDrawer(){
    init();
}

auto FullscreenQuadDrawer::init() -> void{
    clean();
    drawableObject = std::make_unique<FullscreenQuad>();
}

TorusDrawer::TorusDrawer(float outerRadius, float innerRadius, int nsides, int nrings, std::vector<GLuint> textures){
    init(outerRadius, innerRadius, nsides, nrings, textures);
}

auto TorusDrawer::init(float outerRadius, float innerRadius, int nsides, int nrings, std::vector<GLuint> textures) -> void{
    clean();
    texturesNames  = textures;
    drawableObject = std::make_unique<Torus>(outerRadius, innerRadius, nsides, nrings);
}

PlaneDrawer::PlaneDrawer(float xSize, float zSize, std::vector<GLuint> textures){
    init(xSize, zSize, textures);
}

auto PlaneDrawer::init(float xSize, float zSize, std::vector<GLuint> textures) -> void{
    clean();
    texturesNames  = textures;
    drawableObject = std::make_unique<Plane>( static_cast<GLfloat>(xSize), static_cast<GLfloat>(zSize), 1, 1, GLfloat{10}, GLfloat{7});
}

SkyboxDrawer::SkyboxDrawer(float size, std::optional<GLuint> cubemap){
    init(size, cubemap);
}

auto SkyboxDrawer::init(float size, std::optional<GLuint> cubemap) -> void{
    clean();
    if(cubemap.has_value()){
        texturesNames.emplace_back(cubemap.value());
    }
    drawableObject = std::make_unique<Skybox>(size);
}

SphereDrawer::SphereDrawer(float radius, size_t slices, size_t nbStacks, std::vector<GLuint> textures){
    init(radius, slices, nbStacks, textures);
}

auto SphereDrawer::init(float radius, size_t slices, size_t nbStacks, std::vector<GLuint> textures) -> void{
    clean();
    texturesNames  = textures;
    drawableObject = std::make_unique<SphereShape>(radius, static_cast<GLuint>(slices), static_cast<GLuint>(nbStacks));
}

CubeDrawer::CubeDrawer(float side, std::vector<GLuint> textures){
    init(side, textures);
}

auto CubeDrawer::init(float side, std::vector<GLuint> textures) -> void{
    clean();
    texturesNames  = textures;
    drawableObject = std::make_unique<Cube>(side);
}

auto CloudPointsDrawer::init(std::span<const geo::Pt2f> points, std::span<const geo::Pt3f> colors, std::span<const geo::Pt2f> normals) -> void {
    clean();
    if(!points.empty()){
        drawableObject = std::make_unique<gl::Cloud>(points, colors, normals);
    }else{
        drawableObject = nullptr;
    }
}

auto CloudPointsDrawer::init(std::span<const geo::Pt3f> points, std::span<const geo::Pt3f> colors, std::span<const geo::Pt3f> normals) -> void {
    clean();
    if(!points.empty()){
        drawableObject = std::make_unique<gl::Cloud>(points, colors, normals);
    }else{
        drawableObject = nullptr;
    }
}

VoxelsDrawer::VoxelsDrawer(std::span<const geo::Pt3<int>> voxels, std::span<const geo::Pt3f> colors){
    init(voxels, colors);
}

auto VoxelsDrawer::init(std::span<const geo::Pt3<int>> voxels, std::span<const geo::Pt3f> colors) -> void{
    clean();
    if(!voxels.empty()){
        drawableObject = std::make_unique<gl::Voxels>(voxels, colors);
    }else{
        drawableObject = nullptr;
    }
}

MeshDrawer::MeshDrawer(graphics::Mesh *mesh){
    init(mesh);
}

auto MeshDrawer::init(graphics::Mesh *mesh) -> void{
    clean();
    drawableObject = std::make_unique<gl::Mesh>(mesh);
}

auto GMeshDrawer::init(const std::shared_ptr<graphics::SubModelMesh> &gmesh, const std::vector<tool::graphics::TextureInfo> &texturesInfo) -> void{

    clean();

    if(texturesInfo.empty()){
        for(auto &texturesT : gmesh->material->texturesInfo){
            for(auto textureInfo : texturesT.second){
                gl::Texture2D tbo;
                tbo.load_texture(textureInfo.texture, textureInfo.options);
                m_textures[textureInfo.options.type].emplace_back(std::make_pair(GLuint{tbo.id()},std::move(tbo)));
            }
        }
    }else{
        for(auto textureInfo : texturesInfo){
            gl::Texture2D tbo;
            tbo.load_texture(textureInfo.texture);
            m_textures[textureInfo.options.type].emplace_back(std::make_pair(GLuint{tbo.id()},std::move(tbo)));
        }
    }

    // add textures in good order
    if(m_textures.count(TextureType::diffuse)!=0){
        texturesNames.emplace_back(m_textures[TextureType::diffuse][0].second.id());
    }
    if(m_textures.count(TextureType::normal)!=0){
        texturesNames.emplace_back(m_textures[TextureType::normal][0].second.id());
    }
    if(m_textures.count(TextureType::height)!=0){
        texturesNames.emplace_back(m_textures[TextureType::height][0].second.id());
    }

    drawableObject = std::make_unique<gl::Mesh>(&gmesh->mesh);
}

auto ModelDrawer::init(const std::weak_ptr<graphics::ModelMesh> &model, const std::vector<graphics::TextureInfo> &texturesInfo) -> void{

    clean();

    modelP = model.lock();
    if(modelP){
        for(auto &gmesh : modelP->meshes){
            auto gmeshD = std::make_unique<GMeshDrawer>();
            gmeshD->init(gmesh, texturesInfo);
            drawableObjects.emplace_back(std::move(gmeshD));
        }

        for(auto &modelChild : modelP->children){
            auto modelDrawer = std::make_unique<ModelDrawer>();
            modelDrawer->init(modelChild, {});
            children.emplace_back(std::move(modelDrawer));
        }
    }
}

auto ModelDrawer::draw(gl::ShaderProgram *shader) -> void{

    if(modelP){

        set_bones_uniform(modelP.get(), shader);

        for(auto &gmesh : drawableObjects){
            gmesh->draw();
        }

        for(auto &child : children){            
            child->draw(shader);
        }
    }
}

auto ModelDrawer::draw_adjacency(ShaderProgram *shader) -> void{

    if(modelP){

        set_bones_uniform(modelP.get(), shader);

        for(auto &gmesh : drawableObjects){
            gmesh->draw_adjacency();
        }

        for(auto &child : children){
            child->draw_adjacency(shader);
        }
    }
}

auto ModelDrawer::update_animation(std::string_view animationName, float time) -> void{

    if(modelP){
        modelP->update_animation(animationName, time);
    }

    for(auto &child : children){
        dynamic_cast<ModelDrawer*>(child.get())->update_animation(animationName, time);
    }
}

auto ModelDrawer::set_bones_uniform(graphics::ModelMesh *model, ShaderProgram *shader) -> void{

    if(shader == nullptr){
        return;
    }

    if(model->animations.size() > 0){

        std::vector<geo::Mat4f> transforms;
        for(size_t ii = 0; ii < model->bonesInfo.size(); ++ii){
            transforms.push_back(model->bonesInfo[ii].final);
        }
        if(transforms.size()> 100){
            transforms.resize(100);
        }

        shader->set_uniform("BonesM[0]", transforms);
    }
}

TeapotDrawer::TeapotDrawer(int grid, std::vector<GLuint> textures){
    init(grid, textures);
}

auto TeapotDrawer::init(int grid, std::vector<GLuint> textures) -> void{
    clean();
    texturesNames  = textures;
    drawableObject = std::make_unique<Teapot>(grid);
}

FrustumDrawer::FrustumDrawer(){
    init();
}

auto FrustumDrawer::init() -> void{
    clean();
    drawableObject = std::make_unique<Frustum>();
}

TriangleLineDrawer::TriangleLineDrawer(){
    init(geo::Pt3f{1.f,0.f,0.f},geo::Pt3f{0.f,1.f,0.f},geo::Pt3f{0.f,0.f,1.f});
}

auto TriangleLineDrawer::init(const geo::Pt3f &p1, const geo::Pt3f &p2, const geo::Pt3f &p3) -> void{
    clean();
    drawableObject = std::make_unique<TriangleLines>(p1,p2,p3);
}

CircleLinesDrawer::CircleLinesDrawer(float radius){

}

auto CircleLinesDrawer::init(float radius) -> void{}



CylinderDrawer::CylinderDrawer(geo::Pt3f a, geo::Pt3f b, float radius){

}

auto CylinderDrawer::init(geo::Pt3f a, geo::Pt3f b, float radius) -> void{

}

