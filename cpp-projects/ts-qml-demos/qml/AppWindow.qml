
// Qt
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts


// import cpp.enums 1.0

import "utility"

ApplicationWindow {
    id: appW

    // ui
    visible: true
    width: 1200
    height: 900
    minimumWidth: 1000
    minimumHeight: 900

    // // app settings
    // property int nbCameras : 0
    // property bool splitView : false

    // // pages
    property var samples: Samples {}
    // property var camerasCount: CamerasCount {}
    // property var camerasConnection: CamerasConnection {}
    // property var camerasOrientation: CamerasOrientation {}
    // property var camerasFrustumFiltering: CamerasFrustumFiltering {}
    // # pages info
    property int pagesCounts: 5
    property int currentPageId: 0
    property int lastPageId: 0

    // // 2D views
    // property var pColor1 : ColorCameraItem{idClient: 0}
    // property var pColor2 : ColorCameraItem{idClient: 1}
    // property var pColor3 : ColorCameraItem{idClient: 2}
    // property var pColor4 : ColorCameraItem{idClient: 3}
    // property var pColor5 : ColorCameraItem{idClient: 4}
    // property var pColor6 : ColorCameraItem{idClient: 5}

    // property var pDepth1 : DepthCameraItem{idClient: 0}
    // property var pDepth2 : DepthCameraItem{idClient: 1}
    // property var pDepth3 : DepthCameraItem{idClient: 2}
    // property var pDepth4 : DepthCameraItem{idClient: 3}
    // property var pDepth5 : DepthCameraItem{idClient: 4}
    // property var pDepth6 : DepthCameraItem{idClient: 5}

    // 3D views

    // property var pCloud2 : CloudCameraItem{idClient: 1}
    // property var pCloud3 : CloudCameraItem{idClient: 2}
    // property var pCloud4 : CloudCameraItem{idClient: 3}
    // property var pCloud5 : CloudCameraItem{idClient: 4}
    // property var pCloud6 : CloudCameraItem{idClient: 5}

    // // functions
    // function next_page(nPage){
    //     appW.lastPageId = appW.currentPageId
    //     appW.currentPageId++
    //     mainStack.push(nPage)
    // }

    // function previous_page(){
    //     appW.lastPageId = appW.currentPageId
    //     appW.currentPageId--
    //     mainStack.pop()
    // }

    StackView {
        id: mainStack
        anchors.fill: parent

        Transition {
            id: opacityOut
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 500
            }
        }
        Transition {
            id: opacityIn
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 500
            }
        }

        initialItem: samples
        pushEnter: opacityIn
        pushExit: opacityOut
    }

    // menuBar: MenuBar {
    //     // ...
    // }

    // header: ToolBar {
    //     // ...
    // }

    // footer: TabBar {
    //     // ...
    // }
}
