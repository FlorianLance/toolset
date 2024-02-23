/****************************************************************************
** Meta object code from reading C++ file 'manager_process_data_worker.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../workers/manager_process_data_worker.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'manager_process_data_worker.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS = QtMocHelpers::stringData(
    "tool::ui::ProcessDataWorker",
    "end_update_image_signal",
    "",
    "std::int64_t",
    "update_color_signal",
    "update_depth_signal",
    "update_infra_signal",
    "update_cloud_data_signal",
    "cam::K2CloudDisplayData*",
    "coloredCloudData",
    "update_mesh_data_signal",
    "cam::K2MeshDisplayData*",
    "coloredMeshData",
    "update_bodies_data_signal",
    "std::array<cam::K2BodyInfos,6>*",
    "bodiesData",
    "new_frame_to_save_signal",
    "std::shared_ptr<cam::K2Frame>",
    "frame",
    "process_data_from_udp_frame",
    "cam::K2UdpHeader",
    "header"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS_t {
    uint offsetsAndSizes[44];
    char stringdata0[28];
    char stringdata1[24];
    char stringdata2[1];
    char stringdata3[13];
    char stringdata4[20];
    char stringdata5[20];
    char stringdata6[20];
    char stringdata7[25];
    char stringdata8[25];
    char stringdata9[17];
    char stringdata10[24];
    char stringdata11[24];
    char stringdata12[16];
    char stringdata13[26];
    char stringdata14[32];
    char stringdata15[11];
    char stringdata16[25];
    char stringdata17[30];
    char stringdata18[6];
    char stringdata19[28];
    char stringdata20[17];
    char stringdata21[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 27),  // "tool::ui::ProcessDataWorker"
        QT_MOC_LITERAL(28, 23),  // "end_update_image_signal"
        QT_MOC_LITERAL(52, 0),  // ""
        QT_MOC_LITERAL(53, 12),  // "std::int64_t"
        QT_MOC_LITERAL(66, 19),  // "update_color_signal"
        QT_MOC_LITERAL(86, 19),  // "update_depth_signal"
        QT_MOC_LITERAL(106, 19),  // "update_infra_signal"
        QT_MOC_LITERAL(126, 24),  // "update_cloud_data_signal"
        QT_MOC_LITERAL(151, 24),  // "cam::K2CloudDisplayData*"
        QT_MOC_LITERAL(176, 16),  // "coloredCloudData"
        QT_MOC_LITERAL(193, 23),  // "update_mesh_data_signal"
        QT_MOC_LITERAL(217, 23),  // "cam::K2MeshDisplayData*"
        QT_MOC_LITERAL(241, 15),  // "coloredMeshData"
        QT_MOC_LITERAL(257, 25),  // "update_bodies_data_signal"
        QT_MOC_LITERAL(283, 31),  // "std::array<cam::K2BodyInfos,6>*"
        QT_MOC_LITERAL(315, 10),  // "bodiesData"
        QT_MOC_LITERAL(326, 24),  // "new_frame_to_save_signal"
        QT_MOC_LITERAL(351, 29),  // "std::shared_ptr<cam::K2Frame>"
        QT_MOC_LITERAL(381, 5),  // "frame"
        QT_MOC_LITERAL(387, 27),  // "process_data_from_udp_frame"
        QT_MOC_LITERAL(415, 16),  // "cam::K2UdpHeader"
        QT_MOC_LITERAL(432, 6)   // "header"
    },
    "tool::ui::ProcessDataWorker",
    "end_update_image_signal",
    "",
    "std::int64_t",
    "update_color_signal",
    "update_depth_signal",
    "update_infra_signal",
    "update_cloud_data_signal",
    "cam::K2CloudDisplayData*",
    "coloredCloudData",
    "update_mesh_data_signal",
    "cam::K2MeshDisplayData*",
    "coloredMeshData",
    "update_bodies_data_signal",
    "std::array<cam::K2BodyInfos,6>*",
    "bodiesData",
    "new_frame_to_save_signal",
    "std::shared_ptr<cam::K2Frame>",
    "frame",
    "process_data_from_udp_frame",
    "cam::K2UdpHeader",
    "header"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x06,    1 /* Public */,
       4,    1,   71,    2, 0x06,    3 /* Public */,
       5,    1,   74,    2, 0x06,    5 /* Public */,
       6,    1,   77,    2, 0x06,    7 /* Public */,
       7,    1,   80,    2, 0x06,    9 /* Public */,
      10,    1,   83,    2, 0x06,   11 /* Public */,
      13,    1,   86,    2, 0x06,   13 /* Public */,
      16,    1,   89,    2, 0x06,   15 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      19,    2,   92,    2, 0x0a,   17 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::QImage,    2,
    QMetaType::Void, QMetaType::QImage,    2,
    QMetaType::Void, QMetaType::QImage,    2,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 17,   18,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 20, 0x80000000 | 17,   21,   18,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ui::ProcessDataWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ProcessDataWorker, std::true_type>,
        // method 'end_update_image_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::int64_t, std::false_type>,
        // method 'update_color_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QImage, std::false_type>,
        // method 'update_depth_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QImage, std::false_type>,
        // method 'update_infra_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QImage, std::false_type>,
        // method 'update_cloud_data_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2CloudDisplayData *, std::false_type>,
        // method 'update_mesh_data_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2MeshDisplayData *, std::false_type>,
        // method 'update_bodies_data_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::array<cam::K2BodyInfos,6> *, std::false_type>,
        // method 'new_frame_to_save_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>,
        // method 'process_data_from_udp_frame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2UdpHeader, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<cam::K2Frame>, std::false_type>
    >,
    nullptr
} };

void tool::ui::ProcessDataWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProcessDataWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->end_update_image_signal((*reinterpret_cast< std::add_pointer_t<std::int64_t>>(_a[1]))); break;
        case 1: _t->update_color_signal((*reinterpret_cast< std::add_pointer_t<QImage>>(_a[1]))); break;
        case 2: _t->update_depth_signal((*reinterpret_cast< std::add_pointer_t<QImage>>(_a[1]))); break;
        case 3: _t->update_infra_signal((*reinterpret_cast< std::add_pointer_t<QImage>>(_a[1]))); break;
        case 4: _t->update_cloud_data_signal((*reinterpret_cast< std::add_pointer_t<cam::K2CloudDisplayData*>>(_a[1]))); break;
        case 5: _t->update_mesh_data_signal((*reinterpret_cast< std::add_pointer_t<cam::K2MeshDisplayData*>>(_a[1]))); break;
        case 6: _t->update_bodies_data_signal((*reinterpret_cast< std::add_pointer_t<std::array<cam::K2BodyInfos,6>*>>(_a[1]))); break;
        case 7: _t->new_frame_to_save_signal((*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[1]))); break;
        case 8: _t->process_data_from_udp_frame((*reinterpret_cast< std::add_pointer_t<cam::K2UdpHeader>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::shared_ptr<cam::K2Frame>>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProcessDataWorker::*)(std::int64_t );
            if (_t _q_method = &ProcessDataWorker::end_update_image_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProcessDataWorker::*)(QImage );
            if (_t _q_method = &ProcessDataWorker::update_color_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ProcessDataWorker::*)(QImage );
            if (_t _q_method = &ProcessDataWorker::update_depth_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ProcessDataWorker::*)(QImage );
            if (_t _q_method = &ProcessDataWorker::update_infra_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ProcessDataWorker::*)(cam::K2CloudDisplayData * );
            if (_t _q_method = &ProcessDataWorker::update_cloud_data_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ProcessDataWorker::*)(cam::K2MeshDisplayData * );
            if (_t _q_method = &ProcessDataWorker::update_mesh_data_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ProcessDataWorker::*)(std::array<cam::K2BodyInfos,6> * );
            if (_t _q_method = &ProcessDataWorker::update_bodies_data_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ProcessDataWorker::*)(std::shared_ptr<cam::K2Frame> );
            if (_t _q_method = &ProcessDataWorker::new_frame_to_save_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject *tool::ui::ProcessDataWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ui::ProcessDataWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEuiSCOPEProcessDataWorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::ui::ProcessDataWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void tool::ui::ProcessDataWorker::end_update_image_signal(std::int64_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void tool::ui::ProcessDataWorker::update_color_signal(QImage _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void tool::ui::ProcessDataWorker::update_depth_signal(QImage _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void tool::ui::ProcessDataWorker::update_infra_signal(QImage _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void tool::ui::ProcessDataWorker::update_cloud_data_signal(cam::K2CloudDisplayData * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void tool::ui::ProcessDataWorker::update_mesh_data_signal(cam::K2MeshDisplayData * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void tool::ui::ProcessDataWorker::update_bodies_data_signal(std::array<cam::K2BodyInfos,6> * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void tool::ui::ProcessDataWorker::new_frame_to_save_signal(std::shared_ptr<cam::K2Frame> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
