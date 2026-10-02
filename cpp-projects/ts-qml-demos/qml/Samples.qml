// Qt
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

// import "settings"

import items 1.0
import samples 1.0

Page {

    visible: false

    property bool firstTime : true

    // property Component comp : null
    property QtObject qComp : null

    // associated to sample ui control


    Component.onCompleted: {


        // for (var i=0; i<5; i++) {
        //     var object = component.createObject(container);
        //     object.x = (object.width + 10) * i;
        // }
    }


    // property var pSample : Sample3dItemViewer{}

    onVisibleChanged: {
        // debug
        if(firstTime){
            firstTime = false
            print("ttt")
            var comp = Qt.createComponent("samples/DiffuseGlSampleControl.qml");
            if (comp.status === Component.Ready) {
                print("ready")
                qComp = comp.createObject(uiControl)
                sampleV.set_sample(qComp.sample)
            }
            print("state " + comp.status)
        }
    }

    ColumnLayout {

        anchors.fill: parent

        Sample3dItemViewer{
            id: sampleV
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height * 0.8
        }

        Item{
            id: uiControl
            Layout.fillWidth: true
            // Layout.fillHeight: true
        }

        Button{

            text: "DESTROY"
            onClicked: {
                qComp.destroy()
            }
        }

        PageIndicator {
            count: appW.pagesCounts
            currentIndex: appW.currentPageId
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
