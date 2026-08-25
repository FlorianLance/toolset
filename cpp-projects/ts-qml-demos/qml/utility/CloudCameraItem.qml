
// Qt
import QtQuick
import QtQuick.Layouts

import com.example 1.0
import cpp.enums 1.0

Cloud3dViewItem{
    Layout.fillWidth: true
    Layout.fillHeight: true
    property int idClient: 0
    Component.onCompleted:{
        bsc_controller.init_cloud_3d_view(idClient, this)
    }
}