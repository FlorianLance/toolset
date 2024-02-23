/****************************************************************************
** Meta object code from reading C++ file 'frame_reader_worker.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../workers/frame_reader_worker.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'frame_reader_worker.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS = QtMocHelpers::stringData(
    "tool::net::FrameReaderW",
    "frame_received_signal",
    "",
    "cam::K2UdpHeader",
    "header",
    "std::shared_ptr<cam::K2Frame>",
    "frame",
    "process_packets",
    "std::vector<QByteArray*>",
    "data"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS_t {
    uint offsetsAndSizes[20];
    char stringdata0[24];
    char stringdata1[22];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[7];
    char stringdata5[30];
    char stringdata6[6];
    char stringdata7[16];
    char stringdata8[25];
    char stringdata9[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS = {
    {
        QT_MOC_LITERAL(0, 23),  // "tool::net::FrameReaderW"
        QT_MOC_LITERAL(24, 21),  // "frame_received_signal"
        QT_MOC_LITERAL(46, 0),  // ""
        QT_MOC_LITERAL(47, 16),  // "cam::K2UdpHeader"
        QT_MOC_LITERAL(64, 6),  // "header"
        QT_MOC_LITERAL(71, 29),  // "std::shared_ptr<cam::K2Frame>"
        QT_MOC_LITERAL(101, 5),  // "frame"
        QT_MOC_LITERAL(107, 15),  // "process_packets"
        QT_MOC_LITERAL(123, 24),  // "std::vector<QByteArray*>"
        QT_MOC_LITERAL(148, 4)   // "data"
    },
    "tool::net::FrameReaderW",
    "frame_received_signal",
    "",
    "cam::K2UdpHeader",
    "header",
    "std::shared_ptr<cam::K2Frame>",
    "frame",
    "process_packets",
    "std::vector<QByteArray*>",
    "data"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   26,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    1,   31,    2, 0x0a,    4 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::net::FrameReaderW::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FrameReaderW, std::true_type>,
        // method 'frame_received_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2UdpHeader, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>,
        // method 'process_packets'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<QByteArray*>, std::false_type>
    >,
    nullptr
} };

void tool::net::FrameReaderW::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FrameReaderW *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->frame_received_signal((*reinterpret_cast< std::add_pointer_t<cam::K2UdpHeader>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[2]))); break;
        case 1: _t->process_packets((*reinterpret_cast< std::add_pointer_t<std::vector<QByteArray*>>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FrameReaderW::*)(cam::K2UdpHeader , std::shared_ptr<cam::K2Frame> );
            if (_t _q_method = &FrameReaderW::frame_received_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *tool::net::FrameReaderW::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::net::FrameReaderW::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEnetSCOPEFrameReaderWENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::net::FrameReaderW::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void tool::net::FrameReaderW::frame_received_signal(cam::K2UdpHeader _t1, std::shared_ptr<cam::K2Frame> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
