/****************************************************************************
** Meta object code from reading C++ file 'tcp_sender_worker.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../workers/tcp_sender_worker.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcp_sender_worker.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS = QtMocHelpers::stringData(
    "tool::net::TcpSenderWorker",
    "connection_state_signal",
    "",
    "message_signal",
    "message",
    "enable_writing",
    "targetName",
    "writingPort",
    "readingInterface",
    "readingPort",
    "disable_writing",
    "open_camera_command",
    "cam::K2FrameRequest",
    "mode",
    "close_camera_command",
    "update_settings_command",
    "cam::K2Settings",
    "p",
    "ask_new_frame_command",
    "ask_to_save_settings_file_command",
    "set_size_udp_packets",
    "size",
    "shutdown_computer",
    "restart_computer"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS_t {
    uint offsetsAndSizes[48];
    char stringdata0[27];
    char stringdata1[24];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[8];
    char stringdata5[15];
    char stringdata6[11];
    char stringdata7[12];
    char stringdata8[17];
    char stringdata9[12];
    char stringdata10[16];
    char stringdata11[20];
    char stringdata12[20];
    char stringdata13[5];
    char stringdata14[21];
    char stringdata15[24];
    char stringdata16[16];
    char stringdata17[2];
    char stringdata18[22];
    char stringdata19[34];
    char stringdata20[21];
    char stringdata21[5];
    char stringdata22[18];
    char stringdata23[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 26),  // "tool::net::TcpSenderWorker"
        QT_MOC_LITERAL(27, 23),  // "connection_state_signal"
        QT_MOC_LITERAL(51, 0),  // ""
        QT_MOC_LITERAL(52, 14),  // "message_signal"
        QT_MOC_LITERAL(67, 7),  // "message"
        QT_MOC_LITERAL(75, 14),  // "enable_writing"
        QT_MOC_LITERAL(90, 10),  // "targetName"
        QT_MOC_LITERAL(101, 11),  // "writingPort"
        QT_MOC_LITERAL(113, 16),  // "readingInterface"
        QT_MOC_LITERAL(130, 11),  // "readingPort"
        QT_MOC_LITERAL(142, 15),  // "disable_writing"
        QT_MOC_LITERAL(158, 19),  // "open_camera_command"
        QT_MOC_LITERAL(178, 19),  // "cam::K2FrameRequest"
        QT_MOC_LITERAL(198, 4),  // "mode"
        QT_MOC_LITERAL(203, 20),  // "close_camera_command"
        QT_MOC_LITERAL(224, 23),  // "update_settings_command"
        QT_MOC_LITERAL(248, 15),  // "cam::K2Settings"
        QT_MOC_LITERAL(264, 1),  // "p"
        QT_MOC_LITERAL(266, 21),  // "ask_new_frame_command"
        QT_MOC_LITERAL(288, 33),  // "ask_to_save_settings_file_com..."
        QT_MOC_LITERAL(322, 20),  // "set_size_udp_packets"
        QT_MOC_LITERAL(343, 4),  // "size"
        QT_MOC_LITERAL(348, 17),  // "shutdown_computer"
        QT_MOC_LITERAL(366, 16)   // "restart_computer"
    },
    "tool::net::TcpSenderWorker",
    "connection_state_signal",
    "",
    "message_signal",
    "message",
    "enable_writing",
    "targetName",
    "writingPort",
    "readingInterface",
    "readingPort",
    "disable_writing",
    "open_camera_command",
    "cam::K2FrameRequest",
    "mode",
    "close_camera_command",
    "update_settings_command",
    "cam::K2Settings",
    "p",
    "ask_new_frame_command",
    "ask_to_save_settings_file_command",
    "set_size_udp_packets",
    "size",
    "shutdown_computer",
    "restart_computer"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,
       3,    1,   89,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    4,   92,    2, 0x0a,    5 /* Public */,
      10,    0,  101,    2, 0x0a,   10 /* Public */,
      11,    1,  102,    2, 0x0a,   11 /* Public */,
      14,    0,  105,    2, 0x0a,   13 /* Public */,
      15,    1,  106,    2, 0x0a,   14 /* Public */,
      18,    0,  109,    2, 0x0a,   16 /* Public */,
      19,    0,  110,    2, 0x0a,   17 /* Public */,
      20,    1,  111,    2, 0x0a,   18 /* Public */,
      22,    0,  114,    2, 0x0a,   20 /* Public */,
      23,    0,  115,    2, 0x0a,   21 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int,    6,    7,    8,    9,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::net::TcpSenderWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TcpSenderWorker, std::true_type>,
        // method 'connection_state_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'message_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'enable_writing'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'disable_writing'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'open_camera_command'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2FrameRequest, std::false_type>,
        // method 'close_camera_command'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'update_settings_command'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2Settings, std::false_type>,
        // method 'ask_new_frame_command'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ask_to_save_settings_file_command'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_size_udp_packets'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'shutdown_computer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'restart_computer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void tool::net::TcpSenderWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TcpSenderWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connection_state_signal((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->message_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->enable_writing((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4]))); break;
        case 3: _t->disable_writing(); break;
        case 4: _t->open_camera_command((*reinterpret_cast< std::add_pointer_t<cam::K2FrameRequest>>(_a[1]))); break;
        case 5: _t->close_camera_command(); break;
        case 6: _t->update_settings_command((*reinterpret_cast< std::add_pointer_t<cam::K2Settings>>(_a[1]))); break;
        case 7: _t->ask_new_frame_command(); break;
        case 8: _t->ask_to_save_settings_file_command(); break;
        case 9: _t->set_size_udp_packets((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->shutdown_computer(); break;
        case 11: _t->restart_computer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TcpSenderWorker::*)(bool );
            if (_t _q_method = &TcpSenderWorker::connection_state_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TcpSenderWorker::*)(QString );
            if (_t _q_method = &TcpSenderWorker::message_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *tool::net::TcpSenderWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::net::TcpSenderWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpSenderWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::net::TcpSenderWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void tool::net::TcpSenderWorker::connection_state_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void tool::net::TcpSenderWorker::message_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
