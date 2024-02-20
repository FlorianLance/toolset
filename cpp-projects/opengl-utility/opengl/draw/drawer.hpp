
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

#pragma once

// base
#include "graphics/model.hpp"

// local
#include "opengl/shapes.hpp"
#include "opengl/shader/shader.hpp"
#include "opengl/buffer/texture_buffer_object.hpp"

namespace tool::gl {

class Drawer{

public:

    Drawer() = default;
    Drawer(Drawer&&) = default;
    virtual ~Drawer();

    virtual auto draw(gl::ShaderProgram *shader = nullptr) -> void;
    virtual auto draw_adjacency(gl::ShaderProgram *shader = nullptr) -> void;
    virtual auto draw_patches(gl::ShaderProgram *shader = nullptr) -> void;
    virtual auto draw_instances(gl::ShaderProgram *shader = nullptr) -> void;

    inline auto object() const -> gl::BaseShape *{
        return drawableObject.get();
    }  

    auto clean() -> void{
        if(drawableObject){
            drawableObject->data->clean();
        }
    }

    float scaleHint = 1.f;

protected:
    std::unique_ptr<gl::BaseShape> drawableObject = nullptr;
    std::vector<TextureName> texturesNames;
};


class HierarchyDrawer : public Drawer{
protected:
    std::vector<std::unique_ptr<Drawer>> drawableObjects;
    std::vector<std::unique_ptr<Drawer>> children;
};


// geometry3D
// Drawer

// drawer_list

// associate geometry to drawer
// -> if geometry udpated
// -> update gl data

// drawer
//  -> add geometry
//  if(geometry type == triangle)
//

// geometry
//  type
// gl geometry
//  points / lines / triangles (data)


// static points generation -> cloud / coloredCloud
// generate_cloud

// static lines generation -> lines / coloredLines
// generate_grid()
// generate_axes()
// generate_frustum()

// static triangle generation -> mesh
// generate_torus()
// generate_teapot()
// generate_quad()
// generate_plane()
// generate_skybox()
// generate_cube()
// generate_sphere()

// gl_drawer
//  -> initialize_from_geometry

// procedural geometry
// # 3D lines
class AxesDrawer : public Drawer{
public:
    AxesDrawer(float length = 1.f);
    auto init(float length) -> void;
};

class FrustumDrawer : public Drawer{
public:
    FrustumDrawer();
    auto init() -> void;
};

// # 2D shapes
class FullscreenQuadDrawer : public Drawer{
public:
    FullscreenQuadDrawer();
    auto init() -> void;
};

class PlaneDrawer : public Drawer{
public:
    PlaneDrawer(float xSize = 8.f, float zSize = 8.f, std::vector<TextureName> textures = {});
    auto init(float xSize, float zSize, std::vector<TextureName> textures) -> void;
};

class TriangleLineDrawer : public Drawer{
public:
    TriangleLineDrawer();
    auto init(const geo::Pt3f &p1, const geo::Pt3f &p2, const geo::Pt3f &p3) -> void;
};

class CircleLinesDrawer : public Drawer{
    CircleLinesDrawer(float radius);
    auto init(float radius) -> void;
};

class GridDrawer : public Drawer{
public:
    GridDrawer(float width = 0.2f, float height = 0.2f, int nbX = 100.f, int nbY = 100.f);
    auto init(float width, float height, int nbX, int nbY) -> void;
};

// # 3D shapes
// ## points
class CloudPointsDrawer : public Drawer{
public:

    auto init(size_t size, const geo::Pt2f *points, const geo::Pt3f *colors = nullptr) -> void;;
    auto init(std::vector<geo::Pt2f> *points, std::vector<geo::Pt3f> *colors = nullptr) -> void;;

    auto init(size_t size, const geo::Pt3f *points, const geo::Pt3f *colors = nullptr, const geo::Pt3f *normals = nullptr) -> void;;

    auto init(std::vector<geo::Pt3f> *points, std::vector<geo::Pt3f> *colors = nullptr) -> void;;

    auto init(size_t size, const std::vector<geo::Pt3f> *points, const std::vector<geo::Pt3f> *colors = nullptr) -> void;;
};

class VoxelsDrawer : public Drawer{
public:
    VoxelsDrawer(size_t size, geo::Pt3<int> *voxels, geo::Pt3f *colors = nullptr);
    auto init(size_t size, geo::Pt3<int> *voxels, geo::Pt3f *colors) -> void;
};


// ## triangles
class SkyboxDrawer : public Drawer{
public:
    SkyboxDrawer(float size = 100.f, std::optional<TextureName> cubemap = std::nullopt);
    auto init(float size, std::optional<TextureName> cubemap) -> void;
};

class SphereDrawer : public Drawer{
public:
    SphereDrawer(float radius = 1.0f, size_t slices = 20, size_t nbStacks = 20, std::vector<TextureName> textures = {});
    auto init(float radius, size_t slices, size_t nbStacks, std::vector<TextureName> textures) -> void;
};

class CubeDrawer : public Drawer{
public:
    explicit CubeDrawer(float side = 1.f, std::vector<TextureName> textures = {});
    auto init(float side, std::vector<TextureName> textures) -> void;
};

class TorusDrawer : public Drawer{
public:
    TorusDrawer(float outerRadius = 0.7f * 1.5f, float innerRadius = 0.3f * 1.5f, int nsides = 50, int nrings = 50, std::vector<TextureName> textures = {});
    auto init(float outerRadius, float innerRadius, int nsides, int nrings, std::vector<TextureName> textures) -> void;
};

class TeapotDrawer : public Drawer{
public:
    TeapotDrawer(int grid = 14, std::vector<TextureName> textures = {});
    auto init(int grid, std::vector<TextureName> textures) -> void;
};

class CylinderDrawer : public Drawer{
public:
    CylinderDrawer(geo::Pt3f a, geo::Pt3f b, float radius);
    auto init(geo::Pt3f a, geo::Pt3f b, float radius) -> void;
};

// loaded geometry
// # mesh
class MeshDrawer : public Drawer{
public:
    MeshDrawer() = default;
    MeshDrawer(geo::Mesh *mesh);
    auto init(geo::Mesh *mesh) -> void;
};

class GMeshDrawer : public Drawer{
    umap<TextureType, std::vector<std::pair<TextureName,TBO>>> m_textures;
public:
    // if texturesInfo empty use textures from material
    auto init(const std::shared_ptr<graphics::GMesh<float>> &gmesh, const std::vector<graphics::TextureInfo> &texturesInfo = {}) -> void;
};

// # model
class ModelDrawer : public HierarchyDrawer{
    std::shared_ptr<graphics::Model> modelP = nullptr;
public:
    ModelDrawer() = default;
    ModelDrawer(const std::weak_ptr<graphics::Model> &model, const std::vector<graphics::TextureInfo> &texturesInfo = {}){
        init(model, texturesInfo);
    }
    auto init(const std::weak_ptr<graphics::Model> &model, const std::vector<graphics::TextureInfo> &texturesInfo) -> void;
    auto draw(gl::ShaderProgram *shader = nullptr)  -> void override;
    auto draw_adjacency(gl::ShaderProgram *shader = nullptr)  -> void override;
    auto update_animation(std::string_view animationName, float time) -> void;

    auto model() -> graphics::Model*{
        if(modelP){
            return modelP.get();
        }
        return nullptr;
    }

private:

    auto set_bones_uniform(graphics::Model *model, gl::ShaderProgram *shader) -> void;
};




}
