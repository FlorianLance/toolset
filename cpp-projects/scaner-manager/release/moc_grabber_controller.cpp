/****************************************************************************
** Meta object code from reading C++ file 'grabber_controller.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../managers/grabber_controller.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'grabber_controller.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS = QtMocHelpers::stringData(
    "tool::ui::GrabberController",
    "writing_connected_state_signal",
    "",
    "all_packets_received_signal",
    "std::vector<QByteArray*>",
    "data",
    "update_size_udp_packet_signal",
    "size",
    "open_camera_command_signal",
    "cam::K2FrameRequest",
    "mode",
    "ask_new_frame_command_signal",
    "shutdown_computer_signal",
    "restart_computer_signal",
    "ask_to_save_settings_file_signal",
    "update_ui_with_settings_signal",
    "cam::K2Settings",
    "settings",
    "copy_settings_parameters_signal",
    "update_display_parameters_signal",
    "size_t",
    "id",
    "DisplayOptions",
    "p",
    "process_frame_signal",
    "cam::K2UdpHeader",
    "header",
    "std::shared_ptr<cam::K2Frame>",
    "update_registers_clouds_data_signal",
    "std::vector<geo::Pt3f>*",
    "calibCloudData",
    "calibColorData",
    "update_cloud_data_signal",
    "cam::K2CloudDisplayData*",
    "cloudData",
    "update_mesh_data_signal",
    "cam::K2MeshDisplayData*",
    "meshData",
    "update_bodies_data_signal",
    "std::array<cam::K2BodyInfos,6>*",
    "bodiesData",
    "kill_workers_signal",
    "timeout_signal",
    "new_frame_to_save_signal",
    "camId",
    "frame",
    "connect_grabber",
    "disconnect_grabber",
    "ask_new_frame",
    "open_camera_mode",
    "close_camera_mode",
    "shutdown_computer",
    "restart_computer",
    "ask_to_save_settings_file",
    "send_ui_settings",
    "send_current_ui_settings",
    "update_current_ui_settings",
    "copy_current_ui_settings",
    "force_color_cloud",
    "state",
    "display_cloud",
    "hide_cloud",
    "set_size_cloud",
    "update_model_matrix",
    "geo::Mat4<double>",
    "m",
    "get_model_matrix",
    "send_current_ui_display_options",
    "register_cloud",
    "process_registered_clouds",
    "save_current_cloud",
    "path",
    "save_current_mesh",
    "cloud_data",
    "mesh_data"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS_t {
    uint offsetsAndSizes[150];
    char stringdata0[28];
    char stringdata1[31];
    char stringdata2[1];
    char stringdata3[28];
    char stringdata4[25];
    char stringdata5[5];
    char stringdata6[30];
    char stringdata7[5];
    char stringdata8[27];
    char stringdata9[20];
    char stringdata10[5];
    char stringdata11[29];
    char stringdata12[25];
    char stringdata13[24];
    char stringdata14[33];
    char stringdata15[31];
    char stringdata16[16];
    char stringdata17[9];
    char stringdata18[32];
    char stringdata19[33];
    char stringdata20[7];
    char stringdata21[3];
    char stringdata22[15];
    char stringdata23[2];
    char stringdata24[21];
    char stringdata25[17];
    char stringdata26[7];
    char stringdata27[30];
    char stringdata28[36];
    char stringdata29[24];
    char stringdata30[15];
    char stringdata31[15];
    char stringdata32[25];
    char stringdata33[25];
    char stringdata34[10];
    char stringdata35[24];
    char stringdata36[24];
    char stringdata37[9];
    char stringdata38[26];
    char stringdata39[32];
    char stringdata40[11];
    char stringdata41[20];
    char stringdata42[15];
    char stringdata43[25];
    char stringdata44[6];
    char stringdata45[6];
    char stringdata46[16];
    char stringdata47[19];
    char stringdata48[14];
    char stringdata49[17];
    char stringdata50[18];
    char stringdata51[18];
    char stringdata52[17];
    char stringdata53[26];
    char stringdata54[17];
    char stringdata55[25];
    char stringdata56[27];
    char stringdata57[25];
    char stringdata58[18];
    char stringdata59[6];
    char stringdata60[14];
    char stringdata61[11];
    char stringdata62[15];
    char stringdata63[20];
    char stringdata64[18];
    char stringdata65[2];
    char stringdata66[17];
    char stringdata67[32];
    char stringdata68[15];
    char stringdata69[26];
    char stringdata70[19];
    char stringdata71[5];
    char stringdata72[18];
    char stringdata73[11];
    char stringdata74[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 27),  // "tool::ui::GrabberController"
        QT_MOC_LITERAL(28, 30),  // "writing_connected_state_signal"
        QT_MOC_LITERAL(59, 0),  // ""
        QT_MOC_LITERAL(60, 27),  // "all_packets_received_signal"
        QT_MOC_LITERAL(88, 24),  // "std::vector<QByteArray*>"
        QT_MOC_LITERAL(113, 4),  // "data"
        QT_MOC_LITERAL(118, 29),  // "update_size_udp_packet_signal"
        QT_MOC_LITERAL(148, 4),  // "size"
        QT_MOC_LITERAL(153, 26),  // "open_camera_command_signal"
        QT_MOC_LITERAL(180, 19),  // "cam::K2FrameRequest"
        QT_MOC_LITERAL(200, 4),  // "mode"
        QT_MOC_LITERAL(205, 28),  // "ask_new_frame_command_signal"
        QT_MOC_LITERAL(234, 24),  // "shutdown_computer_signal"
        QT_MOC_LITERAL(259, 23),  // "restart_computer_signal"
        QT_MOC_LITERAL(283, 32),  // "ask_to_save_settings_file_signal"
        QT_MOC_LITERAL(316, 30),  // "update_ui_with_settings_signal"
        QT_MOC_LITERAL(347, 15),  // "cam::K2Settings"
        QT_MOC_LITERAL(363, 8),  // "settings"
        QT_MOC_LITERAL(372, 31),  // "copy_settings_parameters_signal"
        QT_MOC_LITERAL(404, 32),  // "update_display_parameters_signal"
        QT_MOC_LITERAL(437, 6),  // "size_t"
        QT_MOC_LITERAL(444, 2),  // "id"
        QT_MOC_LITERAL(447, 14),  // "DisplayOptions"
        QT_MOC_LITERAL(462, 1),  // "p"
        QT_MOC_LITERAL(464, 20),  // "process_frame_signal"
        QT_MOC_LITERAL(485, 16),  // "cam::K2UdpHeader"
        QT_MOC_LITERAL(502, 6),  // "header"
        QT_MOC_LITERAL(509, 29),  // "std::shared_ptr<cam::K2Frame>"
        QT_MOC_LITERAL(539, 35),  // "update_registers_clouds_data_..."
        QT_MOC_LITERAL(575, 23),  // "std::vector<geo::Pt3f>*"
        QT_MOC_LITERAL(599, 14),  // "calibCloudData"
        QT_MOC_LITERAL(614, 14),  // "calibColorData"
        QT_MOC_LITERAL(629, 24),  // "update_cloud_data_signal"
        QT_MOC_LITERAL(654, 24),  // "cam::K2CloudDisplayData*"
        QT_MOC_LITERAL(679, 9),  // "cloudData"
        QT_MOC_LITERAL(689, 23),  // "update_mesh_data_signal"
        QT_MOC_LITERAL(713, 23),  // "cam::K2MeshDisplayData*"
        QT_MOC_LITERAL(737, 8),  // "meshData"
        QT_MOC_LITERAL(746, 25),  // "update_bodies_data_signal"
        QT_MOC_LITERAL(772, 31),  // "std::array<cam::K2BodyInfos,6>*"
        QT_MOC_LITERAL(804, 10),  // "bodiesData"
        QT_MOC_LITERAL(815, 19),  // "kill_workers_signal"
        QT_MOC_LITERAL(835, 14),  // "timeout_signal"
        QT_MOC_LITERAL(850, 24),  // "new_frame_to_save_signal"
        QT_MOC_LITERAL(875, 5),  // "camId"
        QT_MOC_LITERAL(881, 5),  // "frame"
        QT_MOC_LITERAL(887, 15),  // "connect_grabber"
        QT_MOC_LITERAL(903, 18),  // "disconnect_grabber"
        QT_MOC_LITERAL(922, 13),  // "ask_new_frame"
        QT_MOC_LITERAL(936, 16),  // "open_camera_mode"
        QT_MOC_LITERAL(953, 17),  // "close_camera_mode"
        QT_MOC_LITERAL(971, 17),  // "shutdown_computer"
        QT_MOC_LITERAL(989, 16),  // "restart_computer"
        QT_MOC_LITERAL(1006, 25),  // "ask_to_save_settings_file"
        QT_MOC_LITERAL(1032, 16),  // "send_ui_settings"
        QT_MOC_LITERAL(1049, 24),  // "send_current_ui_settings"
        QT_MOC_LITERAL(1074, 26),  // "update_current_ui_settings"
        QT_MOC_LITERAL(1101, 24),  // "copy_current_ui_settings"
        QT_MOC_LITERAL(1126, 17),  // "force_color_cloud"
        QT_MOC_LITERAL(1144, 5),  // "state"
        QT_MOC_LITERAL(1150, 13),  // "display_cloud"
        QT_MOC_LITERAL(1164, 10),  // "hide_cloud"
        QT_MOC_LITERAL(1175, 14),  // "set_size_cloud"
        QT_MOC_LITERAL(1190, 19),  // "update_model_matrix"
        QT_MOC_LITERAL(1210, 17),  // "geo::Mat4<double>"
        QT_MOC_LITERAL(1228, 1),  // "m"
        QT_MOC_LITERAL(1230, 16),  // "get_model_matrix"
        QT_MOC_LITERAL(1247, 31),  // "send_current_ui_display_options"
        QT_MOC_LITERAL(1279, 14),  // "register_cloud"
        QT_MOC_LITERAL(1294, 25),  // "process_registered_clouds"
        QT_MOC_LITERAL(1320, 18),  // "save_current_cloud"
        QT_MOC_LITERAL(1339, 4),  // "path"
        QT_MOC_LITERAL(1344, 17),  // "save_current_mesh"
        QT_MOC_LITERAL(1362, 10),  // "cloud_data"
        QT_MOC_LITERAL(1373, 9)   // "mesh_data"
    },
    "tool::ui::GrabberController",
    "writing_connected_state_signal",
    "",
    "all_packets_received_signal",
    "std::vector<QByteArray*>",
    "data",
    "update_size_udp_packet_signal",
    "size",
    "open_camera_command_signal",
    "cam::K2FrameRequest",
    "mode",
    "ask_new_frame_command_signal",
    "shutdown_computer_signal",
    "restart_computer_signal",
    "ask_to_save_settings_file_signal",
    "update_ui_with_settings_signal",
    "cam::K2Settings",
    "settings",
    "copy_settings_parameters_signal",
    "update_display_parameters_signal",
    "size_t",
    "id",
    "DisplayOptions",
    "p",
    "process_frame_signal",
    "cam::K2UdpHeader",
    "header",
    "std::shared_ptr<cam::K2Frame>",
    "update_registers_clouds_data_signal",
    "std::vector<geo::Pt3f>*",
    "calibCloudData",
    "calibColorData",
    "update_cloud_data_signal",
    "cam::K2CloudDisplayData*",
    "cloudData",
    "update_mesh_data_signal",
    "cam::K2MeshDisplayData*",
    "meshData",
    "update_bodies_data_signal",
    "std::array<cam::K2BodyInfos,6>*",
    "bodiesData",
    "kill_workers_signal",
    "timeout_signal",
    "new_frame_to_save_signal",
    "camId",
    "frame",
    "connect_grabber",
    "disconnect_grabber",
    "ask_new_frame",
    "open_camera_mode",
    "close_camera_mode",
    "shutdown_computer",
    "restart_computer",
    "ask_to_save_settings_file",
    "send_ui_settings",
    "send_current_ui_settings",
    "update_current_ui_settings",
    "copy_current_ui_settings",
    "force_color_cloud",
    "state",
    "display_cloud",
    "hide_cloud",
    "set_size_cloud",
    "update_model_matrix",
    "geo::Mat4<double>",
    "m",
    "get_model_matrix",
    "send_current_ui_display_options",
    "register_cloud",
    "process_registered_clouds",
    "save_current_cloud",
    "path",
    "save_current_mesh",
    "cloud_data",
    "mesh_data"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      44,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      19,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  278,    2, 0x06,    1 /* Public */,
       3,    1,  281,    2, 0x06,    3 /* Public */,
       6,    1,  284,    2, 0x06,    5 /* Public */,
       8,    1,  287,    2, 0x06,    7 /* Public */,
      11,    0,  290,    2, 0x06,    9 /* Public */,
      12,    0,  291,    2, 0x06,   10 /* Public */,
      13,    0,  292,    2, 0x06,   11 /* Public */,
      14,    0,  293,    2, 0x06,   12 /* Public */,
      15,    1,  294,    2, 0x06,   13 /* Public */,
      18,    1,  297,    2, 0x06,   15 /* Public */,
      19,    2,  300,    2, 0x06,   17 /* Public */,
      24,    2,  305,    2, 0x06,   20 /* Public */,
      28,    3,  310,    2, 0x06,   23 /* Public */,
      32,    2,  317,    2, 0x06,   27 /* Public */,
      35,    2,  322,    2, 0x06,   30 /* Public */,
      38,    2,  327,    2, 0x06,   33 /* Public */,
      41,    0,  332,    2, 0x06,   36 /* Public */,
      42,    0,  333,    2, 0x06,   37 /* Public */,
      43,    2,  334,    2, 0x06,   38 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      46,    0,  339,    2, 0x0a,   41 /* Public */,
      47,    0,  340,    2, 0x0a,   42 /* Public */,
      48,    0,  341,    2, 0x0a,   43 /* Public */,
      49,    1,  342,    2, 0x0a,   44 /* Public */,
      50,    0,  345,    2, 0x0a,   46 /* Public */,
      51,    0,  346,    2, 0x0a,   47 /* Public */,
      52,    0,  347,    2, 0x0a,   48 /* Public */,
      53,    0,  348,    2, 0x0a,   49 /* Public */,
      54,    1,  349,    2, 0x0a,   50 /* Public */,
      55,    0,  352,    2, 0x0a,   52 /* Public */,
      56,    1,  353,    2, 0x0a,   53 /* Public */,
      57,    0,  356,    2, 0x0a,   55 /* Public */,
      58,    1,  357,    2, 0x0a,   56 /* Public */,
      60,    0,  360,    2, 0x0a,   58 /* Public */,
      61,    0,  361,    2, 0x0a,   59 /* Public */,
      62,    1,  362,    2, 0x0a,   60 /* Public */,
      63,    1,  365,    2, 0x0a,   62 /* Public */,
      66,    0,  368,    2, 0x10a,   64 /* Public | MethodIsConst  */,
      67,    0,  369,    2, 0x0a,   65 /* Public */,
      68,    1,  370,    2, 0x0a,   66 /* Public */,
      69,    0,  373,    2, 0x0a,   68 /* Public */,
      70,    1,  374,    2, 0x0a,   69 /* Public */,
      72,    1,  377,    2, 0x0a,   71 /* Public */,
      73,    0,  380,    2, 0x0a,   73 /* Public */,
      74,    0,  381,    2, 0x0a,   74 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 20, 0x80000000 | 22,   21,   23,
    QMetaType::Void, 0x80000000 | 25, 0x80000000 | 27,   26,    5,
    QMetaType::Void, 0x80000000 | 20, 0x80000000 | 29, 0x80000000 | 29,   21,   30,   31,
    QMetaType::Void, 0x80000000 | 20, 0x80000000 | 33,   21,   34,
    QMetaType::Void, 0x80000000 | 20, 0x80000000 | 36,   21,   37,
    QMetaType::Void, 0x80000000 | 20, 0x80000000 | 39,   21,   40,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 20, 0x80000000 | 27,   44,   45,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   59,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QReal,    7,
    QMetaType::Void, 0x80000000 | 64,   65,
    0x80000000 | 64,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   71,
    QMetaType::Void, QMetaType::QString,   71,
    0x80000000 | 33,
    0x80000000 | 36,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ui::GrabberController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GrabberController, std::true_type>,
        // method 'writing_connected_state_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'all_packets_received_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<QByteArray*>, std::false_type>,
        // method 'update_size_udp_packet_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'open_camera_command_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2FrameRequest, std::false_type>,
        // method 'ask_new_frame_command_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shutdown_computer_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'restart_computer_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ask_to_save_settings_file_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'update_ui_with_settings_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2Settings, std::false_type>,
        // method 'copy_settings_parameters_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2Settings, std::false_type>,
        // method 'update_display_parameters_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<DisplayOptions, std::false_type>,
        // method 'process_frame_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2UdpHeader, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>,
        // method 'update_registers_clouds_data_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<geo::Pt3f> *, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<geo::Pt3f> *, std::false_type>,
        // method 'update_cloud_data_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2CloudDisplayData *, std::false_type>,
        // method 'update_mesh_data_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2MeshDisplayData *, std::false_type>,
        // method 'update_bodies_data_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::array<cam::K2BodyInfos,6> *, std::false_type>,
        // method 'kill_workers_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'timeout_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'new_frame_to_save_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>,
        // method 'connect_grabber'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnect_grabber'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ask_new_frame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'open_camera_mode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2FrameRequest, std::false_type>,
        // method 'close_camera_mode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shutdown_computer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'restart_computer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ask_to_save_settings_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'send_ui_settings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2Settings, std::false_type>,
        // method 'send_current_ui_settings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'update_current_ui_settings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2Settings, std::false_type>,
        // method 'copy_current_ui_settings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'force_color_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'display_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'hide_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_size_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        // method 'update_model_matrix'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const geo::Mat4<double> &, std::false_type>,
        // method 'get_model_matrix'
        QtPrivate::TypeAndForceComplete<geo::Mat4<double>, std::false_type>,
        // method 'send_current_ui_display_options'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'register_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2CloudDisplayData *, std::false_type>,
        // method 'process_registered_clouds'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_current_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'save_current_mesh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'cloud_data'
        QtPrivate::TypeAndForceComplete<cam::K2CloudDisplayData *, std::false_type>,
        // method 'mesh_data'
        QtPrivate::TypeAndForceComplete<cam::K2MeshDisplayData *, std::false_type>
    >,
    nullptr
} };

void tool::ui::GrabberController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GrabberController *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->writing_connected_state_signal((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->all_packets_received_signal((*reinterpret_cast< std::add_pointer_t<std::vector<QByteArray*>>>(_a[1]))); break;
        case 2: _t->update_size_udp_packet_signal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->open_camera_command_signal((*reinterpret_cast< std::add_pointer_t<cam::K2FrameRequest>>(_a[1]))); break;
        case 4: _t->ask_new_frame_command_signal(); break;
        case 5: _t->shutdown_computer_signal(); break;
        case 6: _t->restart_computer_signal(); break;
        case 7: _t->ask_to_save_settings_file_signal(); break;
        case 8: _t->update_ui_with_settings_signal((*reinterpret_cast< std::add_pointer_t<cam::K2Settings>>(_a[1]))); break;
        case 9: _t->copy_settings_parameters_signal((*reinterpret_cast< std::add_pointer_t<cam::K2Settings>>(_a[1]))); break;
        case 10: _t->update_display_parameters_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<DisplayOptions>>(_a[2]))); break;
        case 11: _t->process_frame_signal((*reinterpret_cast< std::add_pointer_t<cam::K2UdpHeader>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[2]))); break;
        case 12: _t->update_registers_clouds_data_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::vector<geo::Pt3f>*>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<std::vector<geo::Pt3f>*>>(_a[3]))); break;
        case 13: _t->update_cloud_data_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<cam::K2CloudDisplayData*>>(_a[2]))); break;
        case 14: _t->update_mesh_data_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<cam::K2MeshDisplayData*>>(_a[2]))); break;
        case 15: _t->update_bodies_data_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::array<cam::K2BodyInfos,6>*>>(_a[2]))); break;
        case 16: _t->kill_workers_signal(); break;
        case 17: _t->timeout_signal(); break;
        case 18: _t->new_frame_to_save_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[2]))); break;
        case 19: _t->connect_grabber(); break;
        case 20: _t->disconnect_grabber(); break;
        case 21: _t->ask_new_frame(); break;
        case 22: _t->open_camera_mode((*reinterpret_cast< std::add_pointer_t<cam::K2FrameRequest>>(_a[1]))); break;
        case 23: _t->close_camera_mode(); break;
        case 24: _t->shutdown_computer(); break;
        case 25: _t->restart_computer(); break;
        case 26: _t->ask_to_save_settings_file(); break;
        case 27: _t->send_ui_settings((*reinterpret_cast< std::add_pointer_t<cam::K2Settings>>(_a[1]))); break;
        case 28: _t->send_current_ui_settings(); break;
        case 29: _t->update_current_ui_settings((*reinterpret_cast< std::add_pointer_t<cam::K2Settings>>(_a[1]))); break;
        case 30: _t->copy_current_ui_settings(); break;
        case 31: _t->force_color_cloud((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 32: _t->display_cloud(); break;
        case 33: _t->hide_cloud(); break;
        case 34: _t->set_size_cloud((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1]))); break;
        case 35: _t->update_model_matrix((*reinterpret_cast< std::add_pointer_t<geo::Mat4<double>>>(_a[1]))); break;
        case 36: { geo::Mat4<double> _r = _t->get_model_matrix();
            if (_a[0]) *reinterpret_cast< geo::Mat4<double>*>(_a[0]) = std::move(_r); }  break;
        case 37: _t->send_current_ui_display_options(); break;
        case 38: _t->register_cloud((*reinterpret_cast< std::add_pointer_t<cam::K2CloudDisplayData*>>(_a[1]))); break;
        case 39: _t->process_registered_clouds(); break;
        case 40: _t->save_current_cloud((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 41: _t->save_current_mesh((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 42: { cam::K2CloudDisplayData* _r = _t->cloud_data();
            if (_a[0]) *reinterpret_cast< cam::K2CloudDisplayData**>(_a[0]) = std::move(_r); }  break;
        case 43: { cam::K2MeshDisplayData* _r = _t->mesh_data();
            if (_a[0]) *reinterpret_cast< cam::K2MeshDisplayData**>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GrabberController::*)(bool );
            if (_t _q_method = &GrabberController::writing_connected_state_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(std::vector<QByteArray*> );
            if (_t _q_method = &GrabberController::all_packets_received_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(int );
            if (_t _q_method = &GrabberController::update_size_udp_packet_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(cam::K2FrameRequest );
            if (_t _q_method = &GrabberController::open_camera_command_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)();
            if (_t _q_method = &GrabberController::ask_new_frame_command_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)();
            if (_t _q_method = &GrabberController::shutdown_computer_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)();
            if (_t _q_method = &GrabberController::restart_computer_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)();
            if (_t _q_method = &GrabberController::ask_to_save_settings_file_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(cam::K2Settings );
            if (_t _q_method = &GrabberController::update_ui_with_settings_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(cam::K2Settings );
            if (_t _q_method = &GrabberController::copy_settings_parameters_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(size_t , DisplayOptions );
            if (_t _q_method = &GrabberController::update_display_parameters_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(cam::K2UdpHeader , std::shared_ptr<cam::K2Frame> );
            if (_t _q_method = &GrabberController::process_frame_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(size_t , std::vector<geo::Pt3f> * , std::vector<geo::Pt3f> * );
            if (_t _q_method = &GrabberController::update_registers_clouds_data_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(size_t , cam::K2CloudDisplayData * );
            if (_t _q_method = &GrabberController::update_cloud_data_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(size_t , cam::K2MeshDisplayData * );
            if (_t _q_method = &GrabberController::update_mesh_data_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(size_t , std::array<cam::K2BodyInfos,6> * );
            if (_t _q_method = &GrabberController::update_bodies_data_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)();
            if (_t _q_method = &GrabberController::kill_workers_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)();
            if (_t _q_method = &GrabberController::timeout_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (GrabberController::*)(size_t , std::shared_ptr<cam::K2Frame> );
            if (_t _q_method = &GrabberController::new_frame_to_save_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 18;
                return;
            }
        }
    }
}

const QMetaObject *tool::ui::GrabberController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ui::GrabberController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEuiSCOPEGrabberControllerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::ui::GrabberController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 44)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 44;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 44)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 44;
    }
    return _id;
}

// SIGNAL 0
void tool::ui::GrabberController::writing_connected_state_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void tool::ui::GrabberController::all_packets_received_signal(std::vector<QByteArray*> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void tool::ui::GrabberController::update_size_udp_packet_signal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void tool::ui::GrabberController::open_camera_command_signal(cam::K2FrameRequest _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void tool::ui::GrabberController::ask_new_frame_command_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void tool::ui::GrabberController::shutdown_computer_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void tool::ui::GrabberController::restart_computer_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void tool::ui::GrabberController::ask_to_save_settings_file_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void tool::ui::GrabberController::update_ui_with_settings_signal(cam::K2Settings _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void tool::ui::GrabberController::copy_settings_parameters_signal(cam::K2Settings _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void tool::ui::GrabberController::update_display_parameters_signal(size_t _t1, DisplayOptions _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void tool::ui::GrabberController::process_frame_signal(cam::K2UdpHeader _t1, std::shared_ptr<cam::K2Frame> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void tool::ui::GrabberController::update_registers_clouds_data_signal(size_t _t1, std::vector<geo::Pt3f> * _t2, std::vector<geo::Pt3f> * _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void tool::ui::GrabberController::update_cloud_data_signal(size_t _t1, cam::K2CloudDisplayData * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void tool::ui::GrabberController::update_mesh_data_signal(size_t _t1, cam::K2MeshDisplayData * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void tool::ui::GrabberController::update_bodies_data_signal(size_t _t1, std::array<cam::K2BodyInfos,6> * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void tool::ui::GrabberController::kill_workers_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void tool::ui::GrabberController::timeout_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void tool::ui::GrabberController::new_frame_to_save_signal(size_t _t1, std::shared_ptr<cam::K2Frame> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}
QT_WARNING_POP
