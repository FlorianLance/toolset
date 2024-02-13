
/*******************************************************************************
** Toolset-scaner-manager                                                     **
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
#include "utility/array.hpp"
#include "depth-camera/impl/k2_types.hpp"

// opengl-utility
#include "opengl/draw/drawer.hpp"

// qt_utility
#include "gui/widgets/sfmlqt_gl_widget.hpp"
#include "gui/widgets/grabber_parameters_widget.hpp"

namespace tool::ui{

class MultiDepthCameraGlWidget : public SfmlQtGlWidget{

    Q_OBJECT
public:

    void init(size_t nbCameras);

public slots:

    // update data
    void update_grabber_cloud_display_parameters(size_t id, tool::ui::DisplayOptions p);
    void update_cloud(size_t id, tool::cam::K2CloudDisplayData *cloudData);
    void update_mesh(size_t id, tool::cam::K2MeshDisplayData *meshData);
    void update_bodies(size_t id, std::array<tool::cam::K2BodyInfos, 6> *bodies);
    void update_raw_cloud(size_t id, std::vector<geo::Pt3f> *points, std::vector<geo::Pt3f> *colors);

protected:

    // sfml
    virtual void OnInit()   override;
    virtual void OnUpdate() override;
    virtual void onResize() override;

private:

    // init
    void init_joints();

public :

    float sizePtsCloud = 3.f;
    bool displayJoints = false;

    bool displayGrid = false;
    bool displayAxes = false;

private:

    // shaders
    gl::ColoredCloudShader cloudShader;

    // settings
    std::vector<geo::Mat4<double>> models;
    std::vector<bool> visibility;
    std::vector<QColor> colors;
    std::vector<bool> forceColors;
    std::vector<float> sizePts;

    // bodies
    std::vector<std::array<cam::K2BodyInfos, 6>> kinectsBodies;
    std::unordered_map<std::uint64_t, geo::Pt4f> m_bodiesColors;

    // objects to draw
    int vaoToDisplay = -1;
    std::vector<geo::Mesh> meshes;
    std::vector<std::unique_ptr<gl::CloudPointsDrawer>> kinectClouds;
    std::vector<std::unique_ptr<gl::MeshDrawer>> kinectMeshes;
    gl::DrawJointMesh jointsMesh;

    gl::FrustumDrawer frustum;

};
}
