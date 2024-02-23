/****************************************************************************
** Meta object code from reading C++ file 'udp_reader_manager.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../managers/udp_reader_manager.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'udp_reader_manager.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS = QtMocHelpers::stringData(
    "tool::net::K2UdpReaderManager",
    "kill_workers_signal",
    "",
    "packet_received_signal",
    "size_t",
    "idInterface",
    "idGrabber",
    "idFrame",
    "frame_dropped_signal",
    "packetsDropped",
    "all_packets_received_signal",
    "std::vector<QByteArray*>",
    "buffers"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS_t {
    uint offsetsAndSizes[26];
    char stringdata0[30];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[23];
    char stringdata4[7];
    char stringdata5[12];
    char stringdata6[10];
    char stringdata7[8];
    char stringdata8[21];
    char stringdata9[15];
    char stringdata10[28];
    char stringdata11[25];
    char stringdata12[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 29),  // "tool::net::K2UdpReaderManager"
        QT_MOC_LITERAL(30, 19),  // "kill_workers_signal"
        QT_MOC_LITERAL(50, 0),  // ""
        QT_MOC_LITERAL(51, 22),  // "packet_received_signal"
        QT_MOC_LITERAL(74, 6),  // "size_t"
        QT_MOC_LITERAL(81, 11),  // "idInterface"
        QT_MOC_LITERAL(93, 9),  // "idGrabber"
        QT_MOC_LITERAL(103, 7),  // "idFrame"
        QT_MOC_LITERAL(111, 20),  // "frame_dropped_signal"
        QT_MOC_LITERAL(132, 14),  // "packetsDropped"
        QT_MOC_LITERAL(147, 27),  // "all_packets_received_signal"
        QT_MOC_LITERAL(175, 24),  // "std::vector<QByteArray*>"
        QT_MOC_LITERAL(200, 7)   // "buffers"
    },
    "tool::net::K2UdpReaderManager",
    "kill_workers_signal",
    "",
    "packet_received_signal",
    "size_t",
    "idInterface",
    "idGrabber",
    "idFrame",
    "frame_dropped_signal",
    "packetsDropped",
    "all_packets_received_signal",
    "std::vector<QByteArray*>",
    "buffers"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x06,    1 /* Public */,
       3,    3,   39,    2, 0x06,    2 /* Public */,
       8,    4,   46,    2, 0x06,    6 /* Public */,
      10,    3,   55,    2, 0x06,   11 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int, 0x80000000 | 4,    5,    6,    7,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int, 0x80000000 | 4, 0x80000000 | 4,    5,    6,    7,    9,
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int, 0x80000000 | 11,    5,    6,   12,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::net::K2UdpReaderManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<K2UdpReaderManager, std::true_type>,
        // method 'kill_workers_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
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
        QtPrivate::TypeAndForceComplete<std::vector<QByteArray*>, std::false_type>
    >,
    nullptr
} };

void tool::net::K2UdpReaderManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<K2UdpReaderManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->kill_workers_signal(); break;
        case 1: _t->packet_received_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[3]))); break;
        case 2: _t->frame_dropped_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[4]))); break;
        case 3: _t->all_packets_received_signal((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<std::vector<QByteArray*>>>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (K2UdpReaderManager::*)();
            if (_t _q_method = &K2UdpReaderManager::kill_workers_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (K2UdpReaderManager::*)(size_t , int , size_t );
            if (_t _q_method = &K2UdpReaderManager::packet_received_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (K2UdpReaderManager::*)(size_t , int , size_t , size_t );
            if (_t _q_method = &K2UdpReaderManager::frame_dropped_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (K2UdpReaderManager::*)(size_t , int , std::vector<QByteArray*> );
            if (_t _q_method = &K2UdpReaderManager::all_packets_received_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *tool::net::K2UdpReaderManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::net::K2UdpReaderManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEnetSCOPEK2UdpReaderManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::net::K2UdpReaderManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void tool::net::K2UdpReaderManager::kill_workers_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void tool::net::K2UdpReaderManager::packet_received_signal(size_t _t1, int _t2, size_t _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void tool::net::K2UdpReaderManager::frame_dropped_signal(size_t _t1, int _t2, size_t _t3, size_t _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void tool::net::K2UdpReaderManager::all_packets_received_signal(size_t _t1, int _t2, std::vector<QByteArray*> _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
