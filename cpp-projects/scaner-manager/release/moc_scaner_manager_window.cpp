/****************************************************************************
** Meta object code from reading C++ file 'scaner_manager_window.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/scaner_manager_window.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scaner_manager_window.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS = QtMocHelpers::stringData(
    "tool::ui::ScanerManagerWindow",
    "set_loop_ask_frames_state_signal",
    "",
    "set_loop_ask_frames_state",
    "state",
    "copy_camera_parameters",
    "cam::K2Settings",
    "p",
    "quit_threads",
    "wait_for_threads",
    "connect_grabbers",
    "disconnect_grabbers",
    "shutdown_computers",
    "restart_computers",
    "open_cameras",
    "close_cameras",
    "focus_camera",
    "index",
    "save_grabbers_settings_config_files",
    "save_video_file",
    "save_calibration",
    "load_calibration_config_file",
    "calibrate",
    "register_clouds",
    "send_all_settings"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS_t {
    uint offsetsAndSizes[50];
    char stringdata0[30];
    char stringdata1[33];
    char stringdata2[1];
    char stringdata3[26];
    char stringdata4[6];
    char stringdata5[23];
    char stringdata6[16];
    char stringdata7[2];
    char stringdata8[13];
    char stringdata9[17];
    char stringdata10[17];
    char stringdata11[20];
    char stringdata12[19];
    char stringdata13[18];
    char stringdata14[13];
    char stringdata15[14];
    char stringdata16[13];
    char stringdata17[6];
    char stringdata18[36];
    char stringdata19[16];
    char stringdata20[17];
    char stringdata21[29];
    char stringdata22[10];
    char stringdata23[16];
    char stringdata24[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 29),  // "tool::ui::ScanerManagerWindow"
        QT_MOC_LITERAL(30, 32),  // "set_loop_ask_frames_state_signal"
        QT_MOC_LITERAL(63, 0),  // ""
        QT_MOC_LITERAL(64, 25),  // "set_loop_ask_frames_state"
        QT_MOC_LITERAL(90, 5),  // "state"
        QT_MOC_LITERAL(96, 22),  // "copy_camera_parameters"
        QT_MOC_LITERAL(119, 15),  // "cam::K2Settings"
        QT_MOC_LITERAL(135, 1),  // "p"
        QT_MOC_LITERAL(137, 12),  // "quit_threads"
        QT_MOC_LITERAL(150, 16),  // "wait_for_threads"
        QT_MOC_LITERAL(167, 16),  // "connect_grabbers"
        QT_MOC_LITERAL(184, 19),  // "disconnect_grabbers"
        QT_MOC_LITERAL(204, 18),  // "shutdown_computers"
        QT_MOC_LITERAL(223, 17),  // "restart_computers"
        QT_MOC_LITERAL(241, 12),  // "open_cameras"
        QT_MOC_LITERAL(254, 13),  // "close_cameras"
        QT_MOC_LITERAL(268, 12),  // "focus_camera"
        QT_MOC_LITERAL(281, 5),  // "index"
        QT_MOC_LITERAL(287, 35),  // "save_grabbers_settings_config..."
        QT_MOC_LITERAL(323, 15),  // "save_video_file"
        QT_MOC_LITERAL(339, 16),  // "save_calibration"
        QT_MOC_LITERAL(356, 28),  // "load_calibration_config_file"
        QT_MOC_LITERAL(385, 9),  // "calibrate"
        QT_MOC_LITERAL(395, 15),  // "register_clouds"
        QT_MOC_LITERAL(411, 17)   // "send_all_settings"
    },
    "tool::ui::ScanerManagerWindow",
    "set_loop_ask_frames_state_signal",
    "",
    "set_loop_ask_frames_state",
    "state",
    "copy_camera_parameters",
    "cam::K2Settings",
    "p",
    "quit_threads",
    "wait_for_threads",
    "connect_grabbers",
    "disconnect_grabbers",
    "shutdown_computers",
    "restart_computers",
    "open_cameras",
    "close_cameras",
    "focus_camera",
    "index",
    "save_grabbers_settings_config_files",
    "save_video_file",
    "save_calibration",
    "load_calibration_config_file",
    "calibrate",
    "register_clouds",
    "send_all_settings"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  128,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,  131,    2, 0x0a,    3 /* Public */,
       5,    1,  134,    2, 0x0a,    5 /* Public */,
       8,    0,  137,    2, 0x0a,    7 /* Public */,
       9,    0,  138,    2, 0x0a,    8 /* Public */,
      10,    0,  139,    2, 0x0a,    9 /* Public */,
      11,    0,  140,    2, 0x0a,   10 /* Public */,
      12,    0,  141,    2, 0x0a,   11 /* Public */,
      13,    0,  142,    2, 0x0a,   12 /* Public */,
      14,    0,  143,    2, 0x0a,   13 /* Public */,
      15,    0,  144,    2, 0x0a,   14 /* Public */,
      16,    1,  145,    2, 0x0a,   15 /* Public */,
      18,    0,  148,    2, 0x0a,   17 /* Public */,
      19,    0,  149,    2, 0x0a,   18 /* Public */,
      20,    0,  150,    2, 0x0a,   19 /* Public */,
      21,    0,  151,    2, 0x0a,   20 /* Public */,
      22,    0,  152,    2, 0x0a,   21 /* Public */,
      23,    0,  153,    2, 0x0a,   22 /* Public */,
      24,    0,  154,    2, 0x0a,   23 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ui::ScanerManagerWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScanerManagerWindow, std::true_type>,
        // method 'set_loop_ask_frames_state_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'set_loop_ask_frames_state'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'copy_camera_parameters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2Settings, std::false_type>,
        // method 'quit_threads'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'wait_for_threads'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connect_grabbers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnect_grabbers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'shutdown_computers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'restart_computers'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'open_cameras'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'close_cameras'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'focus_camera'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'save_grabbers_settings_config_files'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_video_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_calibration'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'load_calibration_config_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'calibrate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'register_clouds'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'send_all_settings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void tool::ui::ScanerManagerWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ScanerManagerWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->set_loop_ask_frames_state_signal((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->set_loop_ask_frames_state((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->copy_camera_parameters((*reinterpret_cast< std::add_pointer_t<cam::K2Settings>>(_a[1]))); break;
        case 3: _t->quit_threads(); break;
        case 4: _t->wait_for_threads(); break;
        case 5: _t->connect_grabbers(); break;
        case 6: _t->disconnect_grabbers(); break;
        case 7: _t->shutdown_computers(); break;
        case 8: _t->restart_computers(); break;
        case 9: _t->open_cameras(); break;
        case 10: _t->close_cameras(); break;
        case 11: _t->focus_camera((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->save_grabbers_settings_config_files(); break;
        case 13: _t->save_video_file(); break;
        case 14: _t->save_calibration(); break;
        case 15: _t->load_calibration_config_file(); break;
        case 16: _t->calibrate(); break;
        case 17: _t->register_clouds(); break;
        case 18: _t->send_all_settings(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ScanerManagerWindow::*)(bool );
            if (_t _q_method = &ScanerManagerWindow::set_loop_ask_frames_state_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *tool::ui::ScanerManagerWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ui::ScanerManagerWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEuiSCOPEScanerManagerWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int tool::ui::ScanerManagerWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void tool::ui::ScanerManagerWindow::set_loop_ask_frames_state_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
