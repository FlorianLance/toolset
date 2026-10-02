import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import samples 1.0

Rectangle{
    // height: 5
    // implicitHeight: 4
    // radius: 8
    color: "#0FF0FF"
    anchors.fill: parent
    // property DiffuseGlSample sample : DiffuseGlSample{}
    property OitSample sample : OitSample{}


    Component.onCompleted: {
        print("gl diffuse completed")
    }

    RowLayout{
        anchors.fill: parent

        Button{
            text: "test1"
            onClicked: {
                // sample.color = Qt.rgba(1,0,0,1)//Qt.yellow
                // sample.test2(1)
                // print("color " + sample.color)
            }
        }

        Button{
            text: "test2"
            onClicked: {
                // sample.color = Qt.rgba(1,0,1,1)//Qt.yellow
                // sample.test2(2)
            }
        }

        Button{
            text: "test3"
            onClicked: {
                // sample.color = Qt.rgba(1,1,0,1)//Qt.yellow
                // sample.test2(3)
            }
        }
    }

}
