/****************************************************************************
** Meta object code from reading C++ file 'multi_depth_camera_gl_widget.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/multi_depth_camera_gl_widget.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'multi_depth_camera_gl_widget.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS = QtMocHelpers::stringData(
    "tool::ui::MultiDepthCameraGlWidget",
    "update_grabber_cloud_display_parameters",
    "",
    "size_t",
    "id",
    "tool::ui::DisplayOptions",
    "p",
    "update_cloud",
    "tool::cam::K2CloudDisplayData*",
    "cloudData",
    "update_mesh",
    "tool::cam::K2MeshDisplayData*",
    "meshData",
    "update_bodies",
    "std::array<tool::cam::K2BodyInfos,6>*",
    "bodies",
    "update_raw_cloud",
    "std::vector<geo::Pt3f>*",
    "points",
    "colors"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS_t {
    uint offsetsAndSizes[40];
    char stringdata0[35];
    char stringdata1[40];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[3];
    char stringdata5[25];
    char stringdata6[2];
    char stringdata7[13];
    char stringdata8[31];
    char stringdata9[10];
    char stringdata10[12];
    char stringdata11[30];
    char stringdata12[9];
    char stringdata13[14];
    char stringdata14[38];
    char stringdata15[7];
    char stringdata16[17];
    char stringdata17[24];
    char stringdata18[7];
    char stringdata19[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS = {
    {
        QT_MOC_LITERAL(0, 34),  // "tool::ui::MultiDepthCameraGlW..."
        QT_MOC_LITERAL(35, 39),  // "update_grabber_cloud_display_..."
        QT_MOC_LITERAL(75, 0),  // ""
        QT_MOC_LITERAL(76, 6),  // "size_t"
        QT_MOC_LITERAL(83, 2),  // "id"
        QT_MOC_LITERAL(86, 24),  // "tool::ui::DisplayOptions"
        QT_MOC_LITERAL(111, 1),  // "p"
        QT_MOC_LITERAL(113, 12),  // "update_cloud"
        QT_MOC_LITERAL(126, 30),  // "tool::cam::K2CloudDisplayData*"
        QT_MOC_LITERAL(157, 9),  // "cloudData"
        QT_MOC_LITERAL(167, 11),  // "update_mesh"
        QT_MOC_LITERAL(179, 29),  // "tool::cam::K2MeshDisplayData*"
        QT_MOC_LITERAL(209, 8),  // "meshData"
        QT_MOC_LITERAL(218, 13),  // "update_bodies"
        QT_MOC_LITERAL(232, 37),  // "std::array<tool::cam::K2BodyI..."
        QT_MOC_LITERAL(270, 6),  // "bodies"
        QT_MOC_LITERAL(277, 16),  // "update_raw_cloud"
        QT_MOC_LITERAL(294, 23),  // "std::vector<geo::Pt3f>*"
        QT_MOC_LITERAL(318, 6),  // "points"
        QT_MOC_LITERAL(325, 6)   // "colors"
    },
    "tool::ui::MultiDepthCameraGlWidget",
    "update_grabber_cloud_display_parameters",
    "",
    "size_t",
    "id",
    "tool::ui::DisplayOptions",
    "p",
    "update_cloud",
    "tool::cam::K2CloudDisplayData*",
    "cloudData",
    "update_mesh",
    "tool::cam::K2MeshDisplayData*",
    "meshData",
    "update_bodies",
    "std::array<tool::cam::K2BodyInfos,6>*",
    "bodies",
    "update_raw_cloud",
    "std::vector<geo::Pt3f>*",
    "points",
    "colors"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   44,    2, 0x0a,    1 /* Public */,
       7,    2,   49,    2, 0x0a,    4 /* Public */,
      10,    2,   54,    2, 0x0a,    7 /* Public */,
      13,    2,   59,    2, 0x0a,   10 /* Public */,
      16,    3,   64,    2, 0x0a,   13 /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 8,    4,    9,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 11,    4,   12,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 14,    4,   15,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 17, 0x80000000 | 17,    4,   18,   19,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ui::MultiDepthCameraGlWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<SfmlQtGlWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MultiDepthCameraGlWidget, std::true_type>,
        // method 'update_grabber_cloud_display_parameters'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<tool::ui::DisplayOptions, std::false_type>,
        // method 'update_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<tool::cam::K2CloudDisplayData *, std::false_type>,
        // method 'update_mesh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<tool::cam::K2MeshDisplayData *, std::false_type>,
        // method 'update_bodies'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::array<tool::cam::K2BodyInfos,6> *, std::false_type>,
        // method 'update_raw_cloud'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<geo::Pt3f> *, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<geo::Pt3f> *, std::false_type>
    >,
    nullptr
} };

void tool::ui::MultiDepthCameraGlWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MultiDepthCameraGlWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->update_grabber_cloud_display_parameters((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<tool::ui::DisplayOptions>>(_a[2]))); break;
        case 1: _t->update_cloud((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<tool::cam::K2CloudDisplayData*>>(_a[2]))); break;
        case 2: _t->update_mesh((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<tool::cam::K2MeshDisplayData*>>(_a[2]))); break;
        case 3: _t->update_bodies((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::array<tool::cam::K2BodyInfos,6>*>>(_a[2]))); break;
        case 4: _t->update_raw_cloud((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::vector<geo::Pt3f>*>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<std::vector<geo::Pt3f>*>>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject *tool::ui::MultiDepthCameraGlWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ui::MultiDepthCameraGlWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEuiSCOPEMultiDepthCameraGlWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return SfmlQtGlWidget::qt_metacast(_clname);
}

int tool::ui::MultiDepthCameraGlWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SfmlQtGlWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
