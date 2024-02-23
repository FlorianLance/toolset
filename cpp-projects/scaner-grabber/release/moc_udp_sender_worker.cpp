/****************************************************************************
** Meta object code from reading C++ file 'udp_sender_worker.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../workers/udp_sender_worker.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'udp_sender_worker.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS = QtMocHelpers::stringData(
    "tool::net::UdpSenderWorker",
    "connected_state_signal",
    "",
    "writingAddress",
    "writingPort",
    "state",
    "nb_bytes_sent_signal",
    "timeStamp",
    "size_t",
    "nbBytes",
    "packets_failure_signal",
    "count",
    "frame_sent_signal",
    "std::int64_t",
    "time",
    "enable_writing",
    "disable_writing",
    "send_frame",
    "cam::K2TcpPacket",
    "command",
    "std::shared_ptr<cam::K2Frame>",
    "frame"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS_t {
    uint offsetsAndSizes[44];
    char stringdata0[27];
    char stringdata1[23];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[12];
    char stringdata5[6];
    char stringdata6[21];
    char stringdata7[10];
    char stringdata8[7];
    char stringdata9[8];
    char stringdata10[23];
    char stringdata11[6];
    char stringdata12[18];
    char stringdata13[13];
    char stringdata14[5];
    char stringdata15[15];
    char stringdata16[16];
    char stringdata17[11];
    char stringdata18[17];
    char stringdata19[8];
    char stringdata20[30];
    char stringdata21[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 26),  // "tool::net::UdpSenderWorker"
        QT_MOC_LITERAL(27, 22),  // "connected_state_signal"
        QT_MOC_LITERAL(50, 0),  // ""
        QT_MOC_LITERAL(51, 14),  // "writingAddress"
        QT_MOC_LITERAL(66, 11),  // "writingPort"
        QT_MOC_LITERAL(78, 5),  // "state"
        QT_MOC_LITERAL(84, 20),  // "nb_bytes_sent_signal"
        QT_MOC_LITERAL(105, 9),  // "timeStamp"
        QT_MOC_LITERAL(115, 6),  // "size_t"
        QT_MOC_LITERAL(122, 7),  // "nbBytes"
        QT_MOC_LITERAL(130, 22),  // "packets_failure_signal"
        QT_MOC_LITERAL(153, 5),  // "count"
        QT_MOC_LITERAL(159, 17),  // "frame_sent_signal"
        QT_MOC_LITERAL(177, 12),  // "std::int64_t"
        QT_MOC_LITERAL(190, 4),  // "time"
        QT_MOC_LITERAL(195, 14),  // "enable_writing"
        QT_MOC_LITERAL(210, 15),  // "disable_writing"
        QT_MOC_LITERAL(226, 10),  // "send_frame"
        QT_MOC_LITERAL(237, 16),  // "cam::K2TcpPacket"
        QT_MOC_LITERAL(254, 7),  // "command"
        QT_MOC_LITERAL(262, 29),  // "std::shared_ptr<cam::K2Frame>"
        QT_MOC_LITERAL(292, 5)   // "frame"
    },
    "tool::net::UdpSenderWorker",
    "connected_state_signal",
    "",
    "writingAddress",
    "writingPort",
    "state",
    "nb_bytes_sent_signal",
    "timeStamp",
    "size_t",
    "nbBytes",
    "packets_failure_signal",
    "count",
    "frame_sent_signal",
    "std::int64_t",
    "time",
    "enable_writing",
    "disable_writing",
    "send_frame",
    "cam::K2TcpPacket",
    "command",
    "std::shared_ptr<cam::K2Frame>",
    "frame"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   56,    2, 0x06,    1 /* Public */,
       6,    2,   63,    2, 0x06,    5 /* Public */,
      10,    1,   68,    2, 0x06,    8 /* Public */,
      12,    1,   71,    2, 0x06,   10 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      15,    2,   74,    2, 0x0a,   12 /* Public */,
      16,    0,   79,    2, 0x0a,   15 /* Public */,
      17,    2,   80,    2, 0x0a,   16 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::LongLong, 0x80000000 | 8,    7,    9,
    QMetaType::Void, 0x80000000 | 8,   11,
    QMetaType::Void, 0x80000000 | 13,   14,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    3,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18, 0x80000000 | 20,   19,   21,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::net::UdpSenderWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UdpSenderWorker, std::true_type>,
        // method 'connected_state_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'nb_bytes_sent_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'packets_failure_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'frame_sent_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::int64_t, std::false_type>,
        // method 'enable_writing'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'disable_writing'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'send_frame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2TcpPacket, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>
    >,
    nullptr
} };

void tool::net::UdpSenderWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UdpSenderWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connected_state_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 1: _t->nb_bytes_sent_signal((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[2]))); break;
        case 2: _t->packets_failure_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1]))); break;
        case 3: _t->frame_sent_signal((*reinterpret_cast< std::add_pointer_t<std::int64_t>>(_a[1]))); break;
        case 4: _t->enable_writing((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 5: _t->disable_writing(); break;
        case 6: _t->send_frame((*reinterpret_cast< std::add_pointer_t<cam::K2TcpPacket>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UdpSenderWorker::*)(QString , int , bool );
            if (_t _q_method = &UdpSenderWorker::connected_state_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UdpSenderWorker::*)(qint64 , size_t );
            if (_t _q_method = &UdpSenderWorker::nb_bytes_sent_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (UdpSenderWorker::*)(size_t );
            if (_t _q_method = &UdpSenderWorker::packets_failure_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (UdpSenderWorker::*)(std::int64_t );
            if (_t _q_method = &UdpSenderWorker::frame_sent_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *tool::net::UdpSenderWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::net::UdpSenderWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEnetSCOPEUdpSenderWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::net::UdpSenderWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void tool::net::UdpSenderWorker::connected_state_signal(QString _t1, int _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void tool::net::UdpSenderWorker::nb_bytes_sent_signal(qint64 _t1, size_t _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void tool::net::UdpSenderWorker::packets_failure_signal(size_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void tool::net::UdpSenderWorker::frame_sent_signal(std::int64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
