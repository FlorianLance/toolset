/****************************************************************************
** Meta object code from reading C++ file 'tcp_reader_worker.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../workers/tcp_reader_worker.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcp_reader_worker.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS = QtMocHelpers::stringData(
    "tool::net::TcpReaderWorker",
    "tcp_packet_received_signal",
    "",
    "cam::K2TcpPacket",
    "tcpPacket",
    "connected_state_signal",
    "readingAddress",
    "readingPort",
    "state",
    "new_connection_signal",
    "address",
    "port",
    "connection_ended_signal",
    "enable_reading",
    "disable_reading",
    "new_connection",
    "disconnected",
    "ready_read"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS_t {
    uint offsetsAndSizes[36];
    char stringdata0[27];
    char stringdata1[27];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[10];
    char stringdata5[23];
    char stringdata6[15];
    char stringdata7[12];
    char stringdata8[6];
    char stringdata9[22];
    char stringdata10[8];
    char stringdata11[5];
    char stringdata12[24];
    char stringdata13[15];
    char stringdata14[16];
    char stringdata15[15];
    char stringdata16[13];
    char stringdata17[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 26),  // "tool::net::TcpReaderWorker"
        QT_MOC_LITERAL(27, 26),  // "tcp_packet_received_signal"
        QT_MOC_LITERAL(54, 0),  // ""
        QT_MOC_LITERAL(55, 16),  // "cam::K2TcpPacket"
        QT_MOC_LITERAL(72, 9),  // "tcpPacket"
        QT_MOC_LITERAL(82, 22),  // "connected_state_signal"
        QT_MOC_LITERAL(105, 14),  // "readingAddress"
        QT_MOC_LITERAL(120, 11),  // "readingPort"
        QT_MOC_LITERAL(132, 5),  // "state"
        QT_MOC_LITERAL(138, 21),  // "new_connection_signal"
        QT_MOC_LITERAL(160, 7),  // "address"
        QT_MOC_LITERAL(168, 4),  // "port"
        QT_MOC_LITERAL(173, 23),  // "connection_ended_signal"
        QT_MOC_LITERAL(197, 14),  // "enable_reading"
        QT_MOC_LITERAL(212, 15),  // "disable_reading"
        QT_MOC_LITERAL(228, 14),  // "new_connection"
        QT_MOC_LITERAL(243, 12),  // "disconnected"
        QT_MOC_LITERAL(256, 10)   // "ready_read"
    },
    "tool::net::TcpReaderWorker",
    "tcp_packet_received_signal",
    "",
    "cam::K2TcpPacket",
    "tcpPacket",
    "connected_state_signal",
    "readingAddress",
    "readingPort",
    "state",
    "new_connection_signal",
    "address",
    "port",
    "connection_ended_signal",
    "enable_reading",
    "disable_reading",
    "new_connection",
    "disconnected",
    "ready_read"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x06,    1 /* Public */,
       5,    3,   71,    2, 0x06,    3 /* Public */,
       9,    2,   78,    2, 0x06,    7 /* Public */,
      12,    2,   83,    2, 0x06,   10 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      13,    1,   88,    2, 0x0a,   13 /* Public */,
      14,    0,   91,    2, 0x0a,   15 /* Public */,
      15,    0,   92,    2, 0x08,   16 /* Private */,
      16,    0,   93,    2, 0x08,   17 /* Private */,
      17,    0,   94,    2, 0x08,   18 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Bool,    6,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   10,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   10,   11,

 // slots: parameters
    QMetaType::Bool, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::net::TcpReaderWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TcpReaderWorker, std::true_type>,
        // method 'tcp_packet_received_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2TcpPacket, std::false_type>,
        // method 'connected_state_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'new_connection_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'connection_ended_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'enable_reading'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'disable_reading'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'new_connection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ready_read'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void tool::net::TcpReaderWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TcpReaderWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->tcp_packet_received_signal((*reinterpret_cast< std::add_pointer_t<cam::K2TcpPacket>>(_a[1]))); break;
        case 1: _t->connected_state_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 2: _t->new_connection_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->connection_ended_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: { bool _r = _t->enable_reading((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->disable_reading(); break;
        case 6: _t->new_connection(); break;
        case 7: _t->disconnected(); break;
        case 8: _t->ready_read(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TcpReaderWorker::*)(cam::K2TcpPacket );
            if (_t _q_method = &TcpReaderWorker::tcp_packet_received_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TcpReaderWorker::*)(QString , int , bool );
            if (_t _q_method = &TcpReaderWorker::connected_state_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TcpReaderWorker::*)(QString , QString );
            if (_t _q_method = &TcpReaderWorker::new_connection_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TcpReaderWorker::*)(QString , QString );
            if (_t _q_method = &TcpReaderWorker::connection_ended_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *tool::net::TcpReaderWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::net::TcpReaderWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEnetSCOPETcpReaderWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::net::TcpReaderWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void tool::net::TcpReaderWorker::tcp_packet_received_signal(cam::K2TcpPacket _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void tool::net::TcpReaderWorker::connected_state_signal(QString _t1, int _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void tool::net::TcpReaderWorker::new_connection_signal(QString _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void tool::net::TcpReaderWorker::connection_ended_signal(QString _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
