

# /*******************************************************************************
# ** Toolset                                                                    **
# ** MIT License                                                                **
# ** Copyright (c) [2018] [Florian Lance]                                       **
# **                                                                            **
# ** Permission is hereby granted, free of charge, to any person obtaining a    **
# ** copy of this software and associated documentation files (the "Software"), **
# ** to deal in the Software without restriction, including without limitation  **
# ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
# ** and/or sell copies of the Software, and to permit persons to whom the      **
# ** Software is furnished to do so, subject to the following conditions:       **
# **                                                                            **
# ** The above copyright notice and this permission notice shall be included in **
# ** all copies or substantial portions of the Software.                        **
# **                                                                            **
# ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
# ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
# ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
# ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
# ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
# ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
# ** DEALINGS IN THE SOFTWARE.                                                  **
# **                                                                            **
# ********************************************************************************/

PROJECT_NAME = toolset

TEMPLATE = subdirs

SUBDIRS =\
    # ts
    ts-base ts-base-test\
    ts-data ts-network ts-mesh ts-depth-camera\
    ts-opengl ts-opengl-app \
    ts-imgui-gl-engine ts-demos \
    ts-export ts-export-app \
    ts-global-app \
    ts-qt ts-qt-gl ts-qt-nodes ts-qt-app \
    # dc
    dc-grabber dc-manager dc-player dc-monitoring \
    # other
    guardian\

# where to find the sub projects
## ts
ts-base.subdir                  = cpp-projects/ts-base
ts-base-test.subdir             = cpp-projects/ts-base-test
ts-data.subdir                  = cpp-projects/ts-data
ts-network.subdir               = cpp-projects/ts-network
ts-mesh.subdir                  = cpp-projects/ts-mesh
ts-depth-camera.subdir          = cpp-projects/ts-depth-camera
ts-opengl.subdir                = cpp-projects/ts-opengl
ts-opengl-app.subdir            = cpp-projects/ts-opengl-app
ts-imgui-gl-engine.subdir       = cpp-projects/ts-imgui-gl-engine
ts-demos.subdir                 = cpp-projects/ts-demos
ts-export.subdir                = cpp-projects/ts-export
ts-export-app.subdir            = cpp-projects/ts-export-app
ts-global-app.subdir            = cpp-projects/ts-global-app
ts-qt.subdir                    = cpp-projects/ts-qt
ts-qt-gl.subdir                 = cpp-projects/ts-qt-gl
ts-qt-nodes.subdir              = cpp-projects/ts-qt-nodes
ts-qt-app.subdir                = cpp-projects/ts-qt-app
## dc
dc-grabber.subdir               = cpp-projects/dc-grabber
dc-manager.subdir               = cpp-projects/dc-manager
dc-player.subdir                = cpp-projects/dc-player
dc-monitoring.subdir            = cpp-projects/dc-monitoring
## others
guardian.subdir                 = cpp-projects/guardian

# dependencies
## ts
ts-base-test.depends            = ts-base
ts-data.depends                 = ts-base
ts-network.depends              = ts-base
ts-mesh.depends                 = ts-base
ts-depth-camera.depends         = ts-data ts-network ts-mesh
ts-opengl.depends               = ts-mesh
ts-opengl-app.depends           = ts-opengl
ts-imgui-gl-engine.depends      = ts-opengl#ts-depth-camera
ts-demos.depends                = ts-imgui-gl-engine
ts-export.depends               = ts-depth-camera
ts-export-app.depends           = ts-export
ts-global-app.depends           = ts-depth-camera
ts-qt.depends                   = ts-base
ts-qt-gl.depends                = ts-qt ts-opengl
ts-qt-nodes.depends             = ts-qt
ts-qt-app.depends               = ts-qt-gl ts-qt-nodes
## dc
dc-grabber.depends              = ts-imgui-gl-engine ts-depth-camera
dc-manager.depends              = ts-imgui-gl-engine ts-depth-camera
dc-player.depends               = ts-imgui-gl-engine ts-depth-camera
dc-monitoring.depends           = ts-qt
## others
guardian.depends                = ts-qt
