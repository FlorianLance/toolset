/****************************************************************************
** Meta object code from reading C++ file 'interface_udp_receiver_worker.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../workers/interface_udp_receiver_worker.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'interface_udp_receiver_worker.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS = QtMocHelpers::stringData(
    "tool::net::InterfaceUdpReceiverWorker",
    "start_reading_signal",
    "",
    "timeout_packet_signal",
    "connected_state_signal",
    "message_signal",
    "packet_received_signal",
    "size_t",
    "idInterface",
    "idGrabber",
    "idFrame",
    "frame_dropped_signal",
    "packetsDropped",
    "all_packets_received_signal",
    "std::vector<QByteArray*>",
    "buffers",
    "enable_reading",
    "readingPort",
    "disable_reading",
    "read_data"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS_t {
    uint offsetsAndSizes[40];
    char stringdata0[38];
    char stringdata1[21];
    char stringdata2[1];
    char stringdata3[22];
    char stringdata4[23];
    char stringdata5[15];
    char stringdata6[23];
    char stringdata7[7];
    char stringdata8[12];
    char stringdata9[10];
    char stringdata10[8];
    char stringdata11[21];
    char stringdata12[15];
    char stringdata13[28];
    char stringdata14[25];
    char stringdata15[8];
    char stringdata16[15];
    char stringdata17[12];
    char stringdata18[16];
    char stringdata19[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 37),  // "tool::net::InterfaceUdpReceiv..."
        QT_MOC_LITERAL(38, 20),  // "start_reading_signal"
        QT_MOC_LITERAL(59, 0),  // ""
        QT_MOC_LITERAL(60, 21),  // "timeout_packet_signal"
        QT_MOC_LITERAL(82, 22),  // "connected_state_signal"
        QT_MOC_LITERAL(105, 14),  // "message_signal"
        QT_MOC_LITERAL(120, 22),  // "packet_received_signal"
        QT_MOC_LITERAL(143, 6),  // "size_t"
        QT_MOC_LITERAL(150, 11),  // "idInterface"
        QT_MOC_LITERAL(162, 9),  // "idGrabber"
        QT_MOC_LITERAL(172, 7),  // "idFrame"
        QT_MOC_LITERAL(180, 20),  // "frame_dropped_signal"
        QT_MOC_LITERAL(201, 14),  // "packetsDropped"
        QT_MOC_LITERAL(216, 27),  // "all_packets_received_signal"
        QT_MOC_LITERAL(244, 24),  // "std::vector<QByteArray*>"
        QT_MOC_LITERAL(269, 7),  // "buffers"
        QT_MOC_LITERAL(277, 14),  // "enable_reading"
        QT_MOC_LITERAL(292, 11),  // "readingPort"
        QT_MOC_LITERAL(304, 15),  // "disable_reading"
        QT_MOC_LITERAL(320, 9)   // "read_data"
    },
    "tool::net::InterfaceUdpReceiverWorker",
    "start_reading_signal",
    "",
    "timeout_packet_signal",
    "connected_state_signal",
    "message_signal",
    "packet_received_signal",
    "size_t",
    "idInterface",
    "idGrabber",
    "idFrame",
    "frame_dropped_signal",
    "packetsDropped",
    "all_packets_received_signal",
    "std::vector<QByteArray*>",
    "buffers",
    "enable_reading",
    "readingPort",
    "disable_reading",
    "read_data"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    1 /* Public */,
       3,    0,   75,    2, 0x06,    2 /* Public */,
       4,    1,   76,    2, 0x06,    3 /* Public */,
       5,    1,   79,    2, 0x06,    5 /* Public */,
       6,    3,   82,    2, 0x06,    7 /* Public */,
      11,    4,   89,    2, 0x06,   11 /* Public */,
      13,    3,   98,    2, 0x06,   16 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      16,    1,  105,    2, 0x0a,   20 /* Public */,
      18,    0,  108,    2, 0x0a,   22 /* Public */,
      19,    0,  109,    2, 0x08,   23 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int, 0x80000000 | 7,    8,    9,   10,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int, 0x80000000 | 7, 0x80000000 | 7,    8,    9,   10,   12,
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int, 0x80000000 | 14,    8,    9,   15,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::net::InterfaceUdpReceiverWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<InterfaceUdpReceiverWorker, std::true_type>,
        // method 'start_reading_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'timeout_packet_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connected_state_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'message_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'packet_received_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'frame_dropped_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'all_packets_received_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<QByteArray*>, std::false_type>,
        // method 'enable_reading'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'disable_reading'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'read_data'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void tool::net::InterfaceUdpReceiverWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<InterfaceUdpReceiverWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->start_reading_signal(); break;
        case 1: _t->timeout_packet_signal(); break;
        case 2: _t->connected_state_signal((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->message_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->packet_received_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[3]))); break;
        case 5: _t->frame_dropped_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[4]))); break;
        case 6: _t->all_packets_received_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<std::vector<QByteArray*>>>(_a[3]))); break;
        case 7: _t->enable_reading((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->disable_reading(); break;
        case 9: _t->read_data(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (InterfaceUdpReceiverWorker::*)();
            if (_t _q_method = &InterfaceUdpReceiverWorker::start_reading_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (InterfaceUdpReceiverWorker::*)();
            if (_t _q_method = &InterfaceUdpReceiverWorker::timeout_packet_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (InterfaceUdpReceiverWorker::*)(bool );
            if (_t _q_method = &InterfaceUdpReceiverWorker::connected_state_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (InterfaceUdpReceiverWorker::*)(QString );
            if (_t _q_method = &InterfaceUdpReceiverWorker::message_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (InterfaceUdpReceiverWorker::*)(size_t , int , size_t );
            if (_t _q_method = &InterfaceUdpReceiverWorker::packet_received_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (InterfaceUdpReceiverWorker::*)(size_t , int , size_t , size_t );
            if (_t _q_method = &InterfaceUdpReceiverWorker::frame_dropped_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (InterfaceUdpReceiverWorker::*)(size_t , int , std::vector<QByteArray*> );
            if (_t _q_method = &InterfaceUdpReceiverWorker::all_packets_received_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject *tool::net::InterfaceUdpReceiverWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::net::InterfaceUdpReceiverWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEnetSCOPEInterfaceUdpReceiverWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::net::InterfaceUdpReceiverWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void tool::net::InterfaceUdpReceiverWorker::start_reading_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void tool::net::InterfaceUdpReceiverWorker::timeout_packet_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void tool::net::InterfaceUdpReceiverWorker::connected_state_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void tool::net::InterfaceUdpReceiverWorker::message_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void tool::net::InterfaceUdpReceiverWorker::packet_received_signal(size_t _t1, int _t2, size_t _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void tool::net::InterfaceUdpReceiverWorker::frame_dropped_signal(size_t _t1, int _t2, size_t _t3, size_t _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void tool::net::InterfaceUdpReceiverWorker::all_packets_received_signal(size_t _t1, int _t2, std::vector<QByteArray*> _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
