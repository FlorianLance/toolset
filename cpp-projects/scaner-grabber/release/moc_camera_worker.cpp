/****************************************************************************
** Meta object code from reading C++ file 'camera_worker.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../workers/camera_worker.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camera_worker.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS = QtMocHelpers::stringData(
    "tool::scan::CameraWorker",
    "camera_opened_signal",
    "",
    "state",
    "new_frame_signal",
    "std::int64_t",
    "time",
    "std::shared_ptr<cam::K2Frame>",
    "frame",
    "no_frame_signal",
    "loop_ended_signal",
    "kill_loop",
    "open_camera",
    "cam::K2FrameRequest",
    "mode",
    "manager_read_frame",
    "update_camera_parameters",
    "cam::K2Settings",
    "parameters",
    "set_loop_state",
    "close_camera",
    "save_camera_intrisics",
    "path",
    "save_camera_space_depth_table",
    "save_settings_config_file"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS_t {
    uint offsetsAndSizes[50];
    char stringdata0[25];
    char stringdata1[21];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[17];
    char stringdata5[13];
    char stringdata6[5];
    char stringdata7[30];
    char stringdata8[6];
    char stringdata9[16];
    char stringdata10[18];
    char stringdata11[10];
    char stringdata12[12];
    char stringdata13[20];
    char stringdata14[5];
    char stringdata15[19];
    char stringdata16[25];
    char stringdata17[16];
    char stringdata18[11];
    char stringdata19[15];
    char stringdata20[13];
    char stringdata21[22];
    char stringdata22[5];
    char stringdata23[30];
    char stringdata24[26];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 24),  // "tool::scan::CameraWorker"
        QT_MOC_LITERAL(25, 20),  // "camera_opened_signal"
        QT_MOC_LITERAL(46, 0),  // ""
        QT_MOC_LITERAL(47, 5),  // "state"
        QT_MOC_LITERAL(53, 16),  // "new_frame_signal"
        QT_MOC_LITERAL(70, 12),  // "std::int64_t"
        QT_MOC_LITERAL(83, 4),  // "time"
        QT_MOC_LITERAL(88, 29),  // "std::shared_ptr<cam::K2Frame>"
        QT_MOC_LITERAL(118, 5),  // "frame"
        QT_MOC_LITERAL(124, 15),  // "no_frame_signal"
        QT_MOC_LITERAL(140, 17),  // "loop_ended_signal"
        QT_MOC_LITERAL(158, 9),  // "kill_loop"
        QT_MOC_LITERAL(168, 11),  // "open_camera"
        QT_MOC_LITERAL(180, 19),  // "cam::K2FrameRequest"
        QT_MOC_LITERAL(200, 4),  // "mode"
        QT_MOC_LITERAL(205, 18),  // "manager_read_frame"
        QT_MOC_LITERAL(224, 24),  // "update_camera_parameters"
        QT_MOC_LITERAL(249, 15),  // "cam::K2Settings"
        QT_MOC_LITERAL(265, 10),  // "parameters"
        QT_MOC_LITERAL(276, 14),  // "set_loop_state"
        QT_MOC_LITERAL(291, 12),  // "close_camera"
        QT_MOC_LITERAL(304, 21),  // "save_camera_intrisics"
        QT_MOC_LITERAL(326, 4),  // "path"
        QT_MOC_LITERAL(331, 29),  // "save_camera_space_depth_table"
        QT_MOC_LITERAL(361, 25)   // "save_settings_config_file"
    },
    "tool::scan::CameraWorker",
    "camera_opened_signal",
    "",
    "state",
    "new_frame_signal",
    "std::int64_t",
    "time",
    "std::shared_ptr<cam::K2Frame>",
    "frame",
    "no_frame_signal",
    "loop_ended_signal",
    "kill_loop",
    "open_camera",
    "cam::K2FrameRequest",
    "mode",
    "manager_read_frame",
    "update_camera_parameters",
    "cam::K2Settings",
    "parameters",
    "set_loop_state",
    "close_camera",
    "save_camera_intrisics",
    "path",
    "save_camera_space_depth_table",
    "save_settings_config_file"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   92,    2, 0x06,    1 /* Public */,
       4,    2,   95,    2, 0x06,    3 /* Public */,
       9,    0,  100,    2, 0x06,    6 /* Public */,
      10,    0,  101,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,  102,    2, 0x0a,    8 /* Public */,
      12,    1,  103,    2, 0x0a,    9 /* Public */,
      15,    0,  106,    2, 0x0a,   11 /* Public */,
      16,    1,  107,    2, 0x0a,   12 /* Public */,
      19,    1,  110,    2, 0x0a,   14 /* Public */,
      20,    0,  113,    2, 0x0a,   16 /* Public */,
      21,    1,  114,    2, 0x0a,   17 /* Public */,
      23,    1,  117,    2, 0x0a,   19 /* Public */,
      24,    1,  120,    2, 0x0a,   21 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 7,    6,    8,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   22,
    QMetaType::Void, QMetaType::QString,   22,
    QMetaType::Void, QMetaType::QString,   22,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::scan::CameraWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CameraWorker, std::true_type>,
        // method 'camera_opened_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'new_frame_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::int64_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>,
        // method 'no_frame_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'loop_ended_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'kill_loop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'open_camera'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2FrameRequest, std::false_type>,
        // method 'manager_read_frame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'update_camera_parameters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2Settings, std::false_type>,
        // method 'set_loop_state'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'close_camera'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'save_camera_intrisics'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'save_camera_space_depth_table'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'save_settings_config_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>
    >,
    nullptr
} };

void tool::scan::CameraWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CameraWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->camera_opened_signal((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->new_frame_signal((*reinterpret_cast< std::add_pointer_t<std::int64_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[2]))); break;
        case 2: _t->no_frame_signal(); break;
        case 3: _t->loop_ended_signal(); break;
        case 4: _t->kill_loop(); break;
        case 5: _t->open_camera((*reinterpret_cast< std::add_pointer_t<cam::K2FrameRequest>>(_a[1]))); break;
        case 6: _t->manager_read_frame(); break;
        case 7: _t->update_camera_parameters((*reinterpret_cast< std::add_pointer_t<cam::K2Settings>>(_a[1]))); break;
        case 8: _t->set_loop_state((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->close_camera(); break;
        case 10: _t->save_camera_intrisics((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->save_camera_space_depth_table((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->save_settings_config_file((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CameraWorker::*)(bool );
            if (_t _q_method = &CameraWorker::camera_opened_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CameraWorker::*)(std::int64_t , std::shared_ptr<cam::K2Frame> );
            if (_t _q_method = &CameraWorker::new_frame_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CameraWorker::*)();
            if (_t _q_method = &CameraWorker::no_frame_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CameraWorker::*)();
            if (_t _q_method = &CameraWorker::loop_ended_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *tool::scan::CameraWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::scan::CameraWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEscanSCOPECameraWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::scan::CameraWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void tool::scan::CameraWorker::camera_opened_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void tool::scan::CameraWorker::new_frame_signal(std::int64_t _t1, std::shared_ptr<cam::K2Frame> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void tool::scan::CameraWorker::no_frame_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void tool::scan::CameraWorker::loop_ended_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
