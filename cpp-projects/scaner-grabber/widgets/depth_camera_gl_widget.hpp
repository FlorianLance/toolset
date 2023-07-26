
/*******************************************************************************
** Toolset-scaner-grabber                                                     **
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

// opengl-utility
#include "opengl/draw/drawer.hpp"

// qt_utility
#include "gui/widgets/sfmlqt_gl_widget.hpp"
#include "wrapper/qt_gl_wrapper.hpp"

// lib
#include "camera/kinect2/k2_types.hpp"

namespace tool::ui{

class DepthCameraGlWidget : public SfmlQtGlWidget{

    Q_OBJECT
public:

    DepthCameraGlWidget(){}

public slots:

    // update data
    void update_cloud(camera::K2CloudDisplayData *cloudData);
    void update_mesh(camera::K2MeshDisplayData *meshData);

    // update
    void update_joints_state(std::array<camera::K2BodyInfos, 6> *bodies);

protected:

    // sfml
    virtual void OnInit()   override;
    virtual void OnUpdate() override;
    virtual void onResize() override;

private:

    void init_joints();

public:

    float sizePtsCloud = 3.f;
    bool displayJoints = true;
    bool displayGrid = false;
    bool displayAxes = false;

    int bodyTracked = 0;
    int nbJointsTracked = 0;
    int nbJointsInferred = 0;
    int nbJointsNotTracked = 0;

    size_t sizeCloud = 0;
    size_t sizeMesh = 0;

private:

    // joints
    std::array<camera::K2BodyInfos, 6> m_bodies;
    std::unordered_map<std::uint64_t, geo::Pt4f> m_bodiesColors;

    // objects to draw
    int vaoToDisplay = -1;

    geo::Mesh mesh;

    gl::CloudPointsDrawer kinectCloud;
    gl::MeshDrawer kinectMesh2;

    // ...
    gl::DrawJointMesh jointsMesh;
};

}
