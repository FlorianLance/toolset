/****************************************************************************
** Meta object code from reading C++ file 'depth_camera_gl_widget.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/depth_camera_gl_widget.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'depth_camera_gl_widget.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS = QtMocHelpers::stringData(
    "tool::ui::DepthCameraGlWidget",
    "update_cloud",
    "",
    "cam::K2CloudDisplayData*",
    "cloudData",
    "update_mesh",
    "cam::K2MeshDisplayData*",
    "meshData",
    "update_joints_state",
    "std::array<cam::K2BodyInfos,6>*",
    "bodies"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS_t {
    uint offsetsAndSizes[22];
    char stringdata0[30];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[25];
    char stringdata4[10];
    char stringdata5[12];
    char stringdata6[24];
    char stringdata7[9];
    char stringdata8[20];
    char stringdata9[32];
    char stringdata10[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS = {
    {
        QT_MOC_LITERAL(0, 29),  // "tool::ui::DepthCameraGlWidget"
        QT_MOC_LITERAL(30, 12),  // "update_cloud"
        QT_MOC_LITERAL(43, 0),  // ""
        QT_MOC_LITERAL(44, 24),  // "cam::K2CloudDisplayData*"
        QT_MOC_LITERAL(69, 9),  // "cloudData"
        QT_MOC_LITERAL(79, 11),  // "update_mesh"
        QT_MOC_LITERAL(91, 23),  // "cam::K2MeshDisplayData*"
        QT_MOC_LITERAL(115, 8),  // "meshData"
        QT_MOC_LITERAL(124, 19),  // "update_joints_state"
        QT_MOC_LITERAL(144, 31),  // "std::array<cam::K2BodyInfos,6>*"
        QT_MOC_LITERAL(176, 6)   // "bodies"
    },
    "tool::ui::DepthCameraGlWidget",
    "update_cloud",
    "",
    "cam::K2CloudDisplayData*",
    "cloudData",
    "update_mesh",
    "cam::K2MeshDisplayData*",
    "meshData",
    "update_joints_state",
    "std::array<cam::K2BodyInfos,6>*",
    "bodies"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   32,    2, 0x0a,    1 /* Public */,
       5,    1,   35,    2, 0x0a,    3 /* Public */,
       8,    1,   38,    2, 0x0a,    5 /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ui::DepthCameraGlWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<SfmlQtGlWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DepthCameraGlWidget, std::true_type>,
        // method 'update_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2CloudDisplayData *, std::false_type>,
        // method 'update_mesh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<cam::K2MeshDisplayData *, std::false_type>,
        // method 'update_joints_state'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::array<cam::K2BodyInfos,6> *, std::false_type>
    >,
    nullptr
} };

void tool::ui::DepthCameraGlWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DepthCameraGlWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->update_cloud((*reinterpret_cast< std::add_pointer_t<cam::K2CloudDisplayData*>>(_a[1]))); break;
        case 1: _t->update_mesh((*reinterpret_cast< std::add_pointer_t<cam::K2MeshDisplayData*>>(_a[1]))); break;
        case 2: _t->update_joints_state((*reinterpret_cast< std::add_pointer_t<std::array<cam::K2BodyInfos,6>*>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *tool::ui::DepthCameraGlWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ui::DepthCameraGlWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEuiSCOPEDepthCameraGlWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return SfmlQtGlWidget::qt_metacast(_clname);
}

int tool::ui::DepthCameraGlWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SfmlQtGlWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
