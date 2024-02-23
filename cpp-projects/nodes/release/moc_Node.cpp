/****************************************************************************
** Meta object code from reading C++ file 'Node.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../nodes/Node.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Node.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSQtNodesSCOPENodeENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSQtNodesSCOPENodeENDCLASS = QtMocHelpers::stringData(
    "QtNodes::Node",
    "propagateData",
    "",
    "std::shared_ptr<QtNodes::NodeData>",
    "nodeData",
    "QtNodes::PortIndex",
    "inPortIndex",
    "onDataUpdated",
    "index",
    "onNodeSizeUpdated"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSQtNodesSCOPENodeENDCLASS_t {
    uint offsetsAndSizes[20];
    char stringdata0[14];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[35];
    char stringdata4[9];
    char stringdata5[19];
    char stringdata6[12];
    char stringdata7[14];
    char stringdata8[6];
    char stringdata9[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSQtNodesSCOPENodeENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSQtNodesSCOPENodeENDCLASS_t qt_meta_stringdata_CLASSQtNodesSCOPENodeENDCLASS = {
    {
        QT_MOC_LITERAL(0, 13),  // "QtNodes::Node"
        QT_MOC_LITERAL(14, 13),  // "propagateData"
        QT_MOC_LITERAL(28, 0),  // ""
        QT_MOC_LITERAL(29, 34),  // "std::shared_ptr<QtNodes::Node..."
        QT_MOC_LITERAL(64, 8),  // "nodeData"
        QT_MOC_LITERAL(73, 18),  // "QtNodes::PortIndex"
        QT_MOC_LITERAL(92, 11),  // "inPortIndex"
        QT_MOC_LITERAL(104, 13),  // "onDataUpdated"
        QT_MOC_LITERAL(118, 5),  // "index"
        QT_MOC_LITERAL(124, 17)   // "onNodeSizeUpdated"
    },
    "QtNodes::Node",
    "propagateData",
    "",
    "std::shared_ptr<QtNodes::NodeData>",
    "nodeData",
    "QtNodes::PortIndex",
    "inPortIndex",
    "onDataUpdated",
    "index",
    "onNodeSizeUpdated"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSQtNodesSCOPENodeENDCLASS[] = {

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
       1,    2,   32,    2, 0x10a,    1 /* Public | MethodIsConst  */,
       7,    1,   37,    2, 0x0a,    4 /* Public */,
       9,    0,   40,    2, 0x0a,    6 /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 5,    8,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject QtNodes::Node::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSQtNodesSCOPENodeENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSQtNodesSCOPENodeENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSQtNodesSCOPENodeENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Node, std::true_type>,
        // method 'propagateData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::shared_ptr<QtNodes::NodeData>, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::PortIndex, std::false_type>,
        // method 'onDataUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::PortIndex, std::false_type>,
        // method 'onNodeSizeUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void QtNodes::Node::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Node *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->propagateData((*reinterpret_cast< std::add_pointer_t<std::shared_ptr<QtNodes::NodeData>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QtNodes::PortIndex>>(_a[2]))); break;
        case 1: _t->onDataUpdated((*reinterpret_cast< std::add_pointer_t<QtNodes::PortIndex>>(_a[1]))); break;
        case 2: _t->onNodeSizeUpdated(); break;
        default: ;
        }
    }
}

const QMetaObject *QtNodes::Node::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::Node::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSQtNodesSCOPENodeENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QtNodes::Node::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
