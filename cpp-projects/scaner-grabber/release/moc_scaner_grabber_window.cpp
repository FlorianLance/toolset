/****************************************************************************
** Meta object code from reading C++ file 'scaner_grabber_window.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../scaner_grabber_window.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scaner_grabber_window.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS = QtMocHelpers::stringData(
    "tool::ui::ScanerGrabberWindow",
    "start_camera_loop_signal",
    "",
    "kill_camera_loop_signal",
    "send_frame_signal",
    "cam::K2TcpPacket",
    "std::shared_ptr<cam::K2Frame>",
    "process_and_display_signal",
    "save_camera_intrinsics_signal",
    "path",
    "save_camera_space_depth_table_signal",
    "save_settings_config_file_signal",
    "read_new_frame_signal",
    "new_frame",
    "std::int64_t",
    "time",
    "frame",
    "save_cloud",
    "start_saving_cloud",
    "stop_saving_cloud",
    "update_display_state",
    "state",
    "set_display_parameters",
    "DisplayOptions",
    "parameters",
    "read_tcp_command",
    "command",
    "new_tcp_connection_signal",
    "address",
    "port",
    "tcp_connection_ended",
    "update_tcp_connection_state",
    "readingAddress",
    "readingPort",
    "update_udp_connection_state",
    "writingAddress",
    "writingPort",
    "update_udp_packet_failure",
    "size_t",
    "count",
    "update_udp_frame_sent",
    "update_udp_nb_bytes_sent",
    "timeStamp",
    "nbBytesSent",
    "update_cloud_data",
    "cam::K2CloudDisplayData*",
    "cloudData",
    "update_mesh_data",
    "cam::K2MeshDisplayData*",
    "meshData",
    "set_process_time",
    "erase_time_get_data_timeout",
    "erase_time_display_timeout",
    "erase_time_send_data_timeout",
    "erase_time_send_rate_timeout",
    "save_camera_intrinsics",
    "save_camera_depth_space_table",
    "save_settings_config_file",
    "load_settings_config_file"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS_t {
    uint offsetsAndSizes[118];
    char stringdata0[30];
    char stringdata1[25];
    char stringdata2[1];
    char stringdata3[24];
    char stringdata4[18];
    char stringdata5[17];
    char stringdata6[30];
    char stringdata7[27];
    char stringdata8[30];
    char stringdata9[5];
    char stringdata10[37];
    char stringdata11[33];
    char stringdata12[22];
    char stringdata13[10];
    char stringdata14[13];
    char stringdata15[5];
    char stringdata16[6];
    char stringdata17[11];
    char stringdata18[19];
    char stringdata19[18];
    char stringdata20[21];
    char stringdata21[6];
    char stringdata22[23];
    char stringdata23[15];
    char stringdata24[11];
    char stringdata25[17];
    char stringdata26[8];
    char stringdata27[26];
    char stringdata28[8];
    char stringdata29[5];
    char stringdata30[21];
    char stringdata31[28];
    char stringdata32[15];
    char stringdata33[12];
    char stringdata34[28];
    char stringdata35[15];
    char stringdata36[12];
    char stringdata37[26];
    char stringdata38[7];
    char stringdata39[6];
    char stringdata40[22];
    char stringdata41[25];
    char stringdata42[10];
    char stringdata43[12];
    char stringdata44[18];
    char stringdata45[25];
    char stringdata46[10];
    char stringdata47[17];
    char stringdata48[24];
    char stringdata49[9];
    char stringdata50[17];
    char stringdata51[28];
    char stringdata52[27];
    char stringdata53[29];
    char stringdata54[29];
    char stringdata55[23];
    char stringdata56[30];
    char stringdata57[26];
    char stringdata58[26];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 29),  // "tool::ui::ScanerGrabberWindow"
        QT_MOC_LITERAL(30, 24),  // "start_camera_loop_signal"
        QT_MOC_LITERAL(55, 0),  // ""
        QT_MOC_LITERAL(56, 23),  // "kill_camera_loop_signal"
        QT_MOC_LITERAL(80, 17),  // "send_frame_signal"
        QT_MOC_LITERAL(98, 16),  // "cam::K2TcpPacket"
        QT_MOC_LITERAL(115, 29),  // "std::shared_ptr<cam::K2Frame>"
        QT_MOC_LITERAL(145, 26),  // "process_and_display_signal"
        QT_MOC_LITERAL(172, 29),  // "save_camera_intrinsics_signal"
        QT_MOC_LITERAL(202, 4),  // "path"
        QT_MOC_LITERAL(207, 36),  // "save_camera_space_depth_table..."
        QT_MOC_LITERAL(244, 32),  // "save_settings_config_file_signal"
        QT_MOC_LITERAL(277, 21),  // "read_new_frame_signal"
        QT_MOC_LITERAL(299, 9),  // "new_frame"
        QT_MOC_LITERAL(309, 12),  // "std::int64_t"
        QT_MOC_LITERAL(322, 4),  // "time"
        QT_MOC_LITERAL(327, 5),  // "frame"
        QT_MOC_LITERAL(333, 10),  // "save_cloud"
        QT_MOC_LITERAL(344, 18),  // "start_saving_cloud"
        QT_MOC_LITERAL(363, 17),  // "stop_saving_cloud"
        QT_MOC_LITERAL(381, 20),  // "update_display_state"
        QT_MOC_LITERAL(402, 5),  // "state"
        QT_MOC_LITERAL(408, 22),  // "set_display_parameters"
        QT_MOC_LITERAL(431, 14),  // "DisplayOptions"
        QT_MOC_LITERAL(446, 10),  // "parameters"
        QT_MOC_LITERAL(457, 16),  // "read_tcp_command"
        QT_MOC_LITERAL(474, 7),  // "command"
        QT_MOC_LITERAL(482, 25),  // "new_tcp_connection_signal"
        QT_MOC_LITERAL(508, 7),  // "address"
        QT_MOC_LITERAL(516, 4),  // "port"
        QT_MOC_LITERAL(521, 20),  // "tcp_connection_ended"
        QT_MOC_LITERAL(542, 27),  // "update_tcp_connection_state"
        QT_MOC_LITERAL(570, 14),  // "readingAddress"
        QT_MOC_LITERAL(585, 11),  // "readingPort"
        QT_MOC_LITERAL(597, 27),  // "update_udp_connection_state"
        QT_MOC_LITERAL(625, 14),  // "writingAddress"
        QT_MOC_LITERAL(640, 11),  // "writingPort"
        QT_MOC_LITERAL(652, 25),  // "update_udp_packet_failure"
        QT_MOC_LITERAL(678, 6),  // "size_t"
        QT_MOC_LITERAL(685, 5),  // "count"
        QT_MOC_LITERAL(691, 21),  // "update_udp_frame_sent"
        QT_MOC_LITERAL(713, 24),  // "update_udp_nb_bytes_sent"
        QT_MOC_LITERAL(738, 9),  // "timeStamp"
        QT_MOC_LITERAL(748, 11),  // "nbBytesSent"
        QT_MOC_LITERAL(760, 17),  // "update_cloud_data"
        QT_MOC_LITERAL(778, 24),  // "cam::K2CloudDisplayData*"
        QT_MOC_LITERAL(803, 9),  // "cloudData"
        QT_MOC_LITERAL(813, 16),  // "update_mesh_data"
        QT_MOC_LITERAL(830, 23),  // "cam::K2MeshDisplayData*"
        QT_MOC_LITERAL(854, 8),  // "meshData"
        QT_MOC_LITERAL(863, 16),  // "set_process_time"
        QT_MOC_LITERAL(880, 27),  // "erase_time_get_data_timeout"
        QT_MOC_LITERAL(908, 26),  // "erase_time_display_timeout"
        QT_MOC_LITERAL(935, 28),  // "erase_time_send_data_timeout"
        QT_MOC_LITERAL(964, 28),  // "erase_time_send_rate_timeout"
        QT_MOC_LITERAL(993, 22),  // "save_camera_intrinsics"
        QT_MOC_LITERAL(1016, 29),  // "save_camera_depth_space_table"
        QT_MOC_LITERAL(1046, 25),  // "save_settings_config_file"
        QT_MOC_LITERAL(1072, 25)   // "load_settings_config_file"
    },
    "tool::ui::ScanerGrabberWindow",
    "start_camera_loop_signal",
    "",
    "kill_camera_loop_signal",
    "send_frame_signal",
    "cam::K2TcpPacket",
    "std::shared_ptr<cam::K2Frame>",
    "process_and_display_signal",
    "save_camera_intrinsics_signal",
    "path",
    "save_camera_space_depth_table_signal",
    "save_settings_config_file_signal",
    "read_new_frame_signal",
    "new_frame",
    "std::int64_t",
    "time",
    "frame",
    "save_cloud",
    "start_saving_cloud",
    "stop_saving_cloud",
    "update_display_state",
    "state",
    "set_display_parameters",
    "DisplayOptions",
    "parameters",
    "read_tcp_command",
    "command",
    "new_tcp_connection_signal",
    "address",
    "port",
    "tcp_connection_ended",
    "update_tcp_connection_state",
    "readingAddress",
    "readingPort",
    "update_udp_connection_state",
    "writingAddress",
    "writingPort",
    "update_udp_packet_failure",
    "size_t",
    "count",
    "update_udp_frame_sent",
    "update_udp_nb_bytes_sent",
    "timeStamp",
    "nbBytesSent",
    "update_cloud_data",
    "cam::K2CloudDisplayData*",
    "cloudData",
    "update_mesh_data",
    "cam::K2MeshDisplayData*",
    "meshData",
    "set_process_time",
    "erase_time_get_data_timeout",
    "erase_time_display_timeout",
    "erase_time_send_data_timeout",
    "erase_time_send_rate_timeout",
    "save_camera_intrinsics",
    "save_camera_depth_space_table",
    "save_settings_config_file",
    "load_settings_config_file"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  212,    2, 0x06,    1 /* Public */,
       3,    0,  213,    2, 0x06,    2 /* Public */,
       4,    2,  214,    2, 0x06,    3 /* Public */,
       7,    1,  219,    2, 0x06,    6 /* Public */,
       8,    1,  222,    2, 0x06,    8 /* Public */,
      10,    1,  225,    2, 0x06,   10 /* Public */,
      11,    1,  228,    2, 0x06,   12 /* Public */,
      12,    0,  231,    2, 0x06,   14 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      13,    2,  232,    2, 0x08,   15 /* Private */,
      17,    0,  237,    2, 0x08,   18 /* Private */,
      18,    0,  238,    2, 0x08,   19 /* Private */,
      19,    0,  239,    2, 0x08,   20 /* Private */,
      20,    1,  240,    2, 0x08,   21 /* Private */,
      22,    1,  243,    2, 0x08,   23 /* Private */,
      25,    1,  246,    2, 0x08,   25 /* Private */,
      27,    2,  249,    2, 0x08,   27 /* Private */,
      30,    2,  254,    2, 0x08,   30 /* Private */,
      31,    3,  259,    2, 0x08,   33 /* Private */,
      34,    3,  266,    2, 0x08,   37 /* Private */,
      37,    1,  273,    2, 0x08,   41 /* Private */,
      40,    1,  276,    2, 0x08,   43 /* Private */,
      41,    2,  279,    2, 0x08,   45 /* Private */,
      44,    1,  284,    2, 0x08,   48 /* Private */,
      47,    1,  287,    2, 0x08,   50 /* Private */,
      50,    1,  290,    2, 0x08,   52 /* Private */,
      51,    0,  293,    2, 0x08,   54 /* Private */,
      52,    0,  294,    2, 0x08,   55 /* Private */,
      53,    0,  295,    2, 0x08,   56 /* Private */,
      54,    0,  296,    2, 0x08,   57 /* Private */,
      55,    0,  297,    2, 0x08,   58 /* Private */,
      56,    0,  298,    2, 0x08,   59 /* Private */,
      57,    0,  299,    2, 0x08,   60 /* Private */,
      58,    0,  300,    2, 0x08,   61 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 6,    2,    2,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 6,   15,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   21,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 5,   26,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   28,   29,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   28,   29,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Bool,   32,   33,   21,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Bool,   35,   36,   21,
    QMetaType::Void, 0x80000000 | 38,   39,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 38,   42,   43,
    QMetaType::Void, 0x80000000 | 45,   46,
    QMetaType::Void, 0x80000000 | 48,   49,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ui::ScanerGrabberWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScanerGrabberWindow, std::true_type>,
        // method 'start_camera_loop_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'kill_camera_loop_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'send_frame_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2TcpPacket, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>,
        // method 'process_and_display_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>,
        // method 'save_camera_intrinsics_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'save_camera_space_depth_table_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'save_settings_config_file_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'read_new_frame_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'new_frame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::int64_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>,
        // method 'save_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'start_saving_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stop_saving_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'update_display_state'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'set_display_parameters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<DisplayOptions, std::false_type>,
        // method 'read_tcp_command'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2TcpPacket, std::false_type>,
        // method 'new_tcp_connection_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'tcp_connection_ended'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'update_tcp_connection_state'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'update_udp_connection_state'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'update_udp_packet_failure'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'update_udp_frame_sent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::int64_t, std::false_type>,
        // method 'update_udp_nb_bytes_sent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'update_cloud_data'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2CloudDisplayData *, std::false_type>,
        // method 'update_mesh_data'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2MeshDisplayData *, std::false_type>,
        // method 'set_process_time'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::int64_t, std::false_type>,
        // method 'erase_time_get_data_timeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'erase_time_display_timeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'erase_time_send_data_timeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'erase_time_send_rate_timeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_camera_intrinsics'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_camera_depth_space_table'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_settings_config_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'load_settings_config_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void tool::ui::ScanerGrabberWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ScanerGrabberWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->start_camera_loop_signal(); break;
        case 1: _t->kill_camera_loop_signal(); break;
        case 2: _t->send_frame_signal((*reinterpret_cast< std::add_pointer_t<cam::K2TcpPacket>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[2]))); break;
        case 3: _t->process_and_display_signal((*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[1]))); break;
        case 4: _t->save_camera_intrinsics_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->save_camera_space_depth_table_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->save_settings_config_file_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->read_new_frame_signal(); break;
        case 8: _t->new_frame((*reinterpret_cast< std::add_pointer_t<std::int64_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[2]))); break;
        case 9: _t->save_cloud(); break;
        case 10: _t->start_saving_cloud(); break;
        case 11: _t->stop_saving_cloud(); break;
        case 12: _t->update_display_state((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->set_display_parameters((*reinterpret_cast< std::add_pointer_t<DisplayOptions>>(_a[1]))); break;
        case 14: _t->read_tcp_command((*reinterpret_cast< std::add_pointer_t<cam::K2TcpPacket>>(_a[1]))); break;
        case 15: _t->new_tcp_connection_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 16: _t->tcp_connection_ended((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 17: _t->update_tcp_connection_state((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 18: _t->update_udp_connection_state((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 19: _t->update_udp_packet_failure((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1]))); break;
        case 20: _t->update_udp_frame_sent((*reinterpret_cast< std::add_pointer_t<std::int64_t>>(_a[1]))); break;
        case 21: _t->update_udp_nb_bytes_sent((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[2]))); break;
        case 22: _t->update_cloud_data((*reinterpret_cast< std::add_pointer_t<cam::K2CloudDisplayData*>>(_a[1]))); break;
        case 23: _t->update_mesh_data((*reinterpret_cast< std::add_pointer_t<cam::K2MeshDisplayData*>>(_a[1]))); break;
        case 24: _t->set_process_time((*reinterpret_cast< std::add_pointer_t<std::int64_t>>(_a[1]))); break;
        case 25: _t->erase_time_get_data_timeout(); break;
        case 26: _t->erase_time_display_timeout(); break;
        case 27: _t->erase_time_send_data_timeout(); break;
        case 28: _t->erase_time_send_rate_timeout(); break;
        case 29: _t->save_camera_intrinsics(); break;
        case 30: _t->save_camera_depth_space_table(); break;
        case 31: _t->save_settings_config_file(); break;
        case 32: _t->load_settings_config_file(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ScanerGrabberWindow::*)();
            if (_t _q_method = &ScanerGrabberWindow::start_camera_loop_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ScanerGrabberWindow::*)();
            if (_t _q_method = &ScanerGrabberWindow::kill_camera_loop_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ScanerGrabberWindow::*)(cam::K2TcpPacket , std::shared_ptr<cam::K2Frame> );
            if (_t _q_method = &ScanerGrabberWindow::send_frame_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ScanerGrabberWindow::*)(std::shared_ptr<cam::K2Frame> );
            if (_t _q_method = &ScanerGrabberWindow::process_and_display_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ScanerGrabberWindow::*)(QString );
            if (_t _q_method = &ScanerGrabberWindow::save_camera_intrinsics_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ScanerGrabberWindow::*)(QString );
            if (_t _q_method = &ScanerGrabberWindow::save_camera_space_depth_table_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ScanerGrabberWindow::*)(QString );
            if (_t _q_method = &ScanerGrabberWindow::save_settings_config_file_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ScanerGrabberWindow::*)();
            if (_t _q_method = &ScanerGrabberWindow::read_new_frame_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject *tool::ui::ScanerGrabberWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ui::ScanerGrabberWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerGrabberWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int tool::ui::ScanerGrabberWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 33;
    }
    return _id;
}

// SIGNAL 0
void tool::ui::ScanerGrabberWindow::start_camera_loop_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void tool::ui::ScanerGrabberWindow::kill_camera_loop_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void tool::ui::ScanerGrabberWindow::send_frame_signal(cam::K2TcpPacket _t1, std::shared_ptr<cam::K2Frame> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void tool::ui::ScanerGrabberWindow::process_and_display_signal(std::shared_ptr<cam::K2Frame> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void tool::ui::ScanerGrabberWindow::save_camera_intrinsics_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void tool::ui::ScanerGrabberWindow::save_camera_space_depth_table_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void tool::ui::ScanerGrabberWindow::save_settings_config_file_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void tool::ui::ScanerGrabberWindow::read_new_frame_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
