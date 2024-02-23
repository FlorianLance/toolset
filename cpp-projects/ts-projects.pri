
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
# ********************************************************************************/*/

message("TARGET: "$$TARGET)

# projects list
TOOLSET_CPP_PROJECTS +=\
    base \
    base-app \
    base-test \
    base-export \
    base-export-app \
    opengl-utility \
    opengl-utility-app \
    3d-engine \
    demos \
    dc-grabber \
    dc-manager \
    dc-monitoring \
    qt-utility \
    nodes \
    scaner-component \
    scaner-grabber \
    scaner-manager \
    guardian \

# define functions
defineTest(generate_variables) {

    UPT = $$upper($${2})
    UPT = $$replace(UPT, "-", "_")

    # include
    eval($$UPT"_INCLUDES" = $${1}/$${2})
    eval(export($$UPT"_INCLUDES"))

    # objects files
    eval($$UPT"_OBJ" = $${1}/_build/temp/obj/$${CFG}/$${2})
    eval(export($$UPT"_OBJ"))

    # moc
    eval($$UPT"_MOC" = $${1}/_build/temp/moc/$${CFG}/$${2})
    eval(export($$UPT"_MOC"))

    # rcc
    eval($$UPT"_RCC" = $${1}/_build/temp/rcc/$${CFG}/$${2})
    eval(export($$UPT"_RCC"))

    # ui generated files
    eval($$UPT"_UI" = $${1}/_build/temp/ui/$${CFG}/$${2})
    eval(export($$UPT"_UI"))

    # destination
    eval($$UPT"_DEST" = $${1}/_build/bin/$${2})
    eval(export($$UPT"_DEST"))

    # lib
    equals(CFG, "debug"){
        eval($$UPT"_LIB" = "-L"$${1}/_build/bin/$${2} "-l"$${2}d)
        eval($$UPT"_LIB_DEP" = $${1}/_build/bin/$${2}/$${2}d".lib") # TODO: remove
        eval($$UPT"_LIB_FILE" = $${1}/_build/bin/$${2}/$${2}d".lib")
    }
    equals(CFG, "release"){
        eval($$UPT"_LIB" = "-L"$${1}/_build/bin/$${2} "-l"$${2})
        eval($$UPT"_LIB_DEP" = $${1}/_build/bin/$${2}/$${2}".lib") # TODO: remove
        eval($$UPT"_LIB_FILE" = $${1}/_build/bin/$${2}/$${2}".lib")
    }
    eval(export($$UPT"_LIB"))
    eval(export($$UPT"_LIB_DEP"))
    eval(export($$UPT"_LIB_FILE"))
}

# sub dir
TOOLSET_CPP_PROJECTS_DIR    = $$TOOLSET_REPOSITORY_DIR"/cpp-projects"
TOOLSET_CPP_THIRDPARTY_DIR  = $$TOOLSET_CPP_PROJECTS_DIR"/_thirdparty"
TOOLSET_CPP_RESOURCES_DIR   = $$TOOLSET_CPP_PROJECTS_DIR"/_resources"

# generate projects variables
for(project, TOOLSET_CPP_PROJECTS):{

    generate_variables($$TOOLSET_CPP_PROJECTS_DIR, $$project)

    ##  build files
    TLOW = $$lower($$TARGET)
    equals(TLOW, $$project){

        UPT = $$upper($$project)
        UPT = $$replace(UPT, "-", "_")

        OBJECTS_DIR = $$eval($$UPT"_OBJ")
        DESTDIR     = $$eval($$UPT"_DEST")
        RCC_DIR     = $$eval($$UPT"_RCC")
        UI_DIR      = $$eval($$UPT"_UI")
        MOC_DIR     = $$eval($$UPT"_MOC")
        message("OBJ: "$$OBJECTS_DIR)
        message("DEST:"$$DESTDIR)
        message("RCC: "$$RCC_DIR)
        message("UI:  "$$UI_DIR)
        message("MOC: "$$MOC_DIR)
    }
}


