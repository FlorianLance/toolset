
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


# projects list
TOOLSET_CPP_PROJECTS +=\
    # tse
    ts-base \
    ts-base-test \
    ts-data \
    ts-network \
    ts-mesh \
    ts-depth-camera \
    ts-opengl \
    ts-opengl-app \
    ts-imgui-gl-engine \
    ts-demos \
    ts-export \
    ts-export-app \
    ts-global-app \
    ts-qt \
    ts-qt-gl \
    ts-qt-nodes \
    ts-qt-app \    
    # dc
    dc-grabber \
    dc-manager \
    dc-player \
    dc-monitoring \
    guardian \

TOOLSET_CPP_PROJECTS_DIR    = $$TOOLSET_REPOSITORY_DIR"/cpp-projects"
TOOLSET_CPP_THIRDPARTY_DIR  = $$TOOLSET_CPP_PROJECTS_DIR"/_thirdparty"
TOOLSET_CPP_RESOURCES_DIR   = $$TOOLSET_CPP_PROJECTS_DIR"/_resources"


# define functions
defineTest(generate_variables) {

    # $${1} -> PROJECT DIRECTORY -> project_dir (C:/toolset/cpp-projects/ts-base)
    # $${2} -> PROJECT NAME -> ts-base

    UPT = $$upper($${2}) # ts-base -> TS-BASE
    UPT = $$replace(UPT, "-", "_") # TS-BASE -> TS_BASE

    # directories
    ## includes
    eval($$UPT"_INCLUDES" = $${1}/$${2}) # TS_BASE_INCLUDES -> project_dir/ts_base
    eval(export($$UPT"_INCLUDES"))
    ## obj
    eval($$UPT"_OBJ" = $${1}/_build/temp/obj/$${CFG}/$${2}) # TS_BASE_OBJ -> project_dir/_build/temp/obj/[release|debug]/ts_base
    eval(export($$UPT"_OBJ"))
    ## moc
    eval($$UPT"_MOC" = $${1}/_build/temp/moc/$${CFG}/$${2}) # TS_BASE_MOC -> project_dir/_build/temp/moc/[release|debug]/ts_base
    eval(export($$UPT"_MOC"))
    ## rcc
    eval($$UPT"_RCC" = $${1}/_build/temp/rcc/$${CFG}/$${2}) # TS_BASE_RCC -> project_dir/_build/temp/rcc/[release|debug]/ts_base
    eval(export($$UPT"_RCC"))
    ## ui
    eval($$UPT"_UI" = $${1}/_build/temp/ui/$${CFG}/$${2}) # TS_BASE_UI -> project_dir/_build/temp/ui/[release|debug]/ts_base
    eval(export($$UPT"_UI"))
    ## dest
    eval($$UPT"_DEST" = $${1}/_build/bin/$${2}) # TS_BASE_DEST -> project_dir/_build/bin/ts_base
    eval(export($$UPT"_DEST"))

    # files
    ## obj
    eval($$UPT"_OBJ_FILES" = $${1}/_build/temp/obj/$${CFG}/$${2}/*.obj) # TS_BASE_OBJ_FILES -> project_dir/_build/temp/obj/[release|debug]/ts_base/*.obj
    eval(export($$UPT"_OBJ_FILES"))
    ## moc obj
    eval($$UPT"_MOC_OBJ_FILES" = $${1}/_build/temp/moc/$${CFG}/$${2}/*.obj) # TS_BASE_OBJ_FILES -> project_dir/_build/temp/obj/[release|debug]/ts_base/*.obj
    eval(export($$UPT"_MOC_OBJ_FILES"))

    # lib
    equals(CFG, "debug"){
        eval($$UPT"_LIB" = "-L"$${1}/_build/bin/$${2} "-l"$${2}d)       # TS_BASE_LIB -> -Lproject_dir/_build/bin/ts_base  -lts_based
        eval($$UPT"_LIB_FILE" = $${1}/_build/bin/$${2}/$${2}d".lib")    # TS_BASE_LIB -> -Lproject_dir/_build/bin/ts_base/ts-based.lib
    }
    equals(CFG, "release"){
        eval($$UPT"_LIB" = "-L"$${1}/_build/bin/$${2} "-l"$${2})        # TS_BASE_LIB -> -Lproject_dir/_build/bin/ts_base  -lts_base
        eval($$UPT"_LIB_FILE" = $${1}/_build/bin/$${2}/$${2}".lib")     # TS_BASE_LIB -> -Lproject_dir/_build/bin/ts_base/ts-base.lib
    }
    eval(export($$UPT"_LIB"))
    eval(export($$UPT"_LIB_FILE"))
}


for(PROJECT, TOOLSET_CPP_PROJECTS):{
    generate_variables($$TOOLSET_CPP_PROJECTS_DIR, $$PROJECT)
}


