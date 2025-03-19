

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
    base base-app base-test base-export base-export-app\
    opengl-utility opengl-utility-app \
    imgui-opengl-engine \
    qt-utility qt-utility-app qt-utility-qml-app\
    nodes demos \
    dc-grabber dc-manager dc-player guardian dc-monitoring\

# where to find the sub projects
## toolset
### base
base.subdir                     = cpp-projects/base
base-app.subdir                 = cpp-projects/base-app
base-test.subdir                = cpp-projects/base-test
base-export.subdir              = cpp-projects/base-export
base-export-app.subdir          = cpp-projects/base-export-app
### opengl-utility
opengl-utility.subdir           = cpp-projects/opengl-utility
opengl-utility-app.subdir       = cpp-projects/opengl-utility-app
### imgui-opengl-engine
imgui-opengl-engine.subdir      = cpp-projects/imgui-opengl-engine
### qt-utility
qt-utility.subdir               = cpp-projects/qt-utility
qt-utility-app.subdir           = cpp-projects/qt-utility-app
qt-utility-qml-app.subdir       = cpp-projects/qt-utility-qml-app
### nodes
nodes.subdir                    = cpp-projects/nodes
### demos
demos.subdir                    = cpp-projects/demos
### depth-camera
guardian.subdir                 = cpp-projects/guardian
dc-grabber.subdir               = cpp-projects/dc-grabber
dc-manager.subdir               = cpp-projects/dc-manager
dc-monitoring.subdir            = cpp-projects/dc-monitoring
dc-player.subdir                = cpp-projects/dc-player

# dependencies
## toolset
### base
base-app.depends                = base
base-test.depends               = base
base-export.depends             = base
base-export-app.depends         = base-export
### opengl-utility
opengl-utility.depends          = base
opengl-utility-app.depends      = opengl-utility
### imgui-opengl-engine
imgui-opengl-engine.depends     = opengl-utility
### qt-utility
qt-utility.depends              = opengl-utility
qt-utility-app.depends          = qt-utility
qt-utility-qml-app.depends      = qt-utility
### nodes
nodes.depends                   = base
demos.depends                   = imgui-opengl-engine
### depth-camera
dc-grabber.depends              = imgui-opengl-engine
dc-manager.depends              = imgui-opengl-engine
dc-player.depends               = imgui-opengl-engine
dc-monitoring.depends           = qt-utility
guardian.depends                = qt-utility



