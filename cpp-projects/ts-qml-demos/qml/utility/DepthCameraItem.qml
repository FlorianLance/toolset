// Qt
import QtQuick
import QtQuick.Layouts

import com.example 1.0
import cpp.enums 1.0

CustomPaintedItem {
    Layout.fillWidth: true
    Layout.fillHeight: true
    mode : CPPEnums.Depth
    property int idClient: 0

    Component.onCompleted:{
        bsc_controller.init_depth_view(idClient, this)
    }
}


