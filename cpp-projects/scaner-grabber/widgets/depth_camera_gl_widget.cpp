
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

#include "depth_camera_gl_widget.hpp"

// base
#include "utility/math.hpp"
#include "geometry/quaternion.hpp"

// qt-utility
#include "qt_logger.hpp"

using namespace tool;
using namespace tool::ui;
using namespace tool::gl;
using namespace tool::cam;


void DepthCameraGlWidget::update_cloud(K2CloudDisplayData *cloudData){

    cloudData->dataLocker.lock();
        sizeCloud = cloudData->sizePts;
        // kinectCloud.init(cloudData->sizePts, cloudData->points->data(), cloudData->colors->data());
    cloudData->dataLocker.unlock();
    vaoToDisplay = 1;
}

void DepthCameraGlWidget::update_mesh(K2MeshDisplayData *meshData){

    meshData->dataLocker.lock();
        sizeMesh = meshData->sizePts;
        mesh.vertices.resize(meshData->sizePts);
        mesh.normals.resize(meshData->sizePts);
        mesh.colors.resize(meshData->sizePts);
        mesh.triIds.resize(meshData->sizeTris);

        std::copy(std::begin(*meshData->points),  std::begin(*meshData->points) + meshData->sizePts, std::begin(mesh.vertices));
        std::copy(std::begin(*meshData->normals), std::begin(*meshData->normals) + meshData->sizePts, std::begin(mesh.normals));

        for(size_t ii = 0; ii < meshData->sizePts; ++ii){
            mesh.colors[ii] = geo::to_pt4((*meshData->colors)[ii], 1.f);
        }
        for(size_t ii = 0; ii < meshData->sizeTris; ++ii){
            // const auto &tri = (*meshData->triangles)[ii];
            // mesh.triIds[ii].ids = {tri.x(), tri.y(), tri.z()};
        }
    meshData->dataLocker.unlock();

    kinectMesh2.init(&mesh);
    vaoToDisplay = 2;
}

void DepthCameraGlWidget::init_joints(){
    jointsMesh.init();
}

void DepthCameraGlWidget::update_joints_state(std::array<K2BodyInfos, 6> *bodies){
    m_bodies = *bodies;

    bodyTracked = 0;
    nbJointsTracked = 0;
    nbJointsInferred = 0;
    nbJointsNotTracked = 0;

    for(const auto &body : m_bodies){
        if(body.tracked){
            ++bodyTracked;
            for(const auto &joint : body.joints){
                if(joint.second.state == K2TrackingStateT::tracked){
                    ++nbJointsTracked;
                }else if(joint.second.state == K2TrackingStateT::inferred){
                    ++nbJointsInferred;
                }else{
                    ++nbJointsNotTracked;
                }
            }
        }
    }
}

void DepthCameraGlWidget::OnInit(){

    if(!cloudShader.init()){
        QtLogger::error("Cannot load cloud shader.");
        return;
    }

    if(!meshShader.init()){
        QtLogger::error("Cannot load mesh shader.");
        return;
    }

    init_axes();
    init_grid();
    init_joints();

    // enable / disable
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
}

void DepthCameraGlWidget::OnUpdate(){


    // time
    currrentTime = clock.getElapsedTime();

    float currentFrame = currrentTime.asMilliseconds();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // clean
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // viewport
    glViewport(0,0, width(), height());

    // set polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(displayGrid){
        draw_grid();
    }

    if(displayAxes){
        draw_axes();
    }

    if(vaoToDisplay == 1){ // draw cloud

        // cloud shader
        cloudShader.use();
        cloudShader.set_uniform("view", camera.view().conv<float>());
        cloudShader.set_uniform("model", (model).conv<float>());
        cloudShader.set_uniform("projection", camera.projection().conv<float>());
        cloudShader.set_uniform("size_pt", sizePtsCloud);
        cloudShader.set_uniform("camera_position", camera.position().conv<float>());
        cloudShader.set_uniform("enable_unicolor", false);
        kinectCloud.draw();


    }else if (vaoToDisplay == 2){

        // mesh shader
        meshShader.use();
        meshShader.set_uniform("view",         camera.view().conv<float>());
        meshShader.set_uniform("model",        (model).conv<float>());
        meshShader.set_uniform("projection",   camera.projection().conv<float>());
        meshShader.set_uniform("enable_unicolor", false);
        kinectMesh2.draw();
    }

    // draw joints
    displayJoints = true;




    if(displayJoints){

        meshShader.use();
        meshShader.set_uniform("view",         camera.view().conv<float>());
        meshShader.set_uniform("projection",   camera.projection().conv<float>());
        meshShader.set_uniform("enable_unicolor", true);        

        for(const auto &body : m_bodies){

            if(!body.tracked){
                continue;
            }

            if(m_bodiesColors.count(body.id) == 0){
                m_bodiesColors[body.id] = geo::Pt4f{0.001f*(rand()%1000),0.001f*(rand()%1000),0.001f*(rand()%1000),1};
            }
            meshShader.set_uniform("unicolor", m_bodiesColors[body.id]);

            for(const auto &joint : body.joints){
                if(joint.second.state == K2TrackingStateT::not_tracked){
                    continue;
                }

                const auto &p = joint.second.pos;
                const auto q = joint.second.rotQuaternion.conv<double>();

                geo::Quatd quat(q.x(), q.y(), q.z(), q.w());
                geo::Pt3d angle = euler_angles(quat) * d180_PI<double>;

                geo::Mat4d jointsModel = geo::transform( // transform2
                    geo::Vec3d{0.02,0.02,0.02},
                    angle,
                    geo::Vec3d{static_cast<double>(p.x()),static_cast<double>(p.y()),static_cast<double>(p.z())}
                );
                meshShader.set_uniform("model", (jointsModel).conv<float>());
                jointsMesh.draw();
            }
        }
    }
}

void DepthCameraGlWidget::onResize(){
}
