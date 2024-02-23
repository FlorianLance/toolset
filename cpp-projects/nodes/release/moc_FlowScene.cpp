/****************************************************************************
** Meta object code from reading C++ file 'FlowScene.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../nodes/FlowScene.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FlowScene.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSQtNodesSCOPEFlowSceneENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSQtNodesSCOPEFlowSceneENDCLASS = QtMocHelpers::stringData(
    "QtNodes::FlowScene",
    "nodeCreated",
    "",
    "QtNodes::Node&",
    "n",
    "nodePlaced",
    "nodeDeleted",
    "connectionCreated",
    "QtNodes::Connection",
    "c",
    "connectionDeleted",
    "nodeMoved",
    "newLocation",
    "nodeDoubleClicked",
    "connectionHovered",
    "QtNodes::Connection&",
    "screenPos",
    "nodeHovered",
    "connectionHoverLeft",
    "nodeHoverLeft",
    "nodeContextMenu",
    "pos",
    "setupConnectionSignals",
    "sendConnectionCreatedToNodes",
    "sendConnectionDeletedToNodes"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSQtNodesSCOPEFlowSceneENDCLASS_t {
    uint offsetsAndSizes[50];
    char stringdata0[19];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[2];
    char stringdata5[11];
    char stringdata6[12];
    char stringdata7[18];
    char stringdata8[20];
    char stringdata9[2];
    char stringdata10[18];
    char stringdata11[10];
    char stringdata12[12];
    char stringdata13[18];
    char stringdata14[18];
    char stringdata15[21];
    char stringdata16[10];
    char stringdata17[12];
    char stringdata18[20];
    char stringdata19[14];
    char stringdata20[16];
    char stringdata21[4];
    char stringdata22[23];
    char stringdata23[29];
    char stringdata24[29];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSQtNodesSCOPEFlowSceneENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSQtNodesSCOPEFlowSceneENDCLASS_t qt_meta_stringdata_CLASSQtNodesSCOPEFlowSceneENDCLASS = {
    {
        QT_MOC_LITERAL(0, 18),  // "QtNodes::FlowScene"
        QT_MOC_LITERAL(19, 11),  // "nodeCreated"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 14),  // "QtNodes::Node&"
        QT_MOC_LITERAL(47, 1),  // "n"
        QT_MOC_LITERAL(49, 10),  // "nodePlaced"
        QT_MOC_LITERAL(60, 11),  // "nodeDeleted"
        QT_MOC_LITERAL(72, 17),  // "connectionCreated"
        QT_MOC_LITERAL(90, 19),  // "QtNodes::Connection"
        QT_MOC_LITERAL(110, 1),  // "c"
        QT_MOC_LITERAL(112, 17),  // "connectionDeleted"
        QT_MOC_LITERAL(130, 9),  // "nodeMoved"
        QT_MOC_LITERAL(140, 11),  // "newLocation"
        QT_MOC_LITERAL(152, 17),  // "nodeDoubleClicked"
        QT_MOC_LITERAL(170, 17),  // "connectionHovered"
        QT_MOC_LITERAL(188, 20),  // "QtNodes::Connection&"
        QT_MOC_LITERAL(209, 9),  // "screenPos"
        QT_MOC_LITERAL(219, 11),  // "nodeHovered"
        QT_MOC_LITERAL(231, 19),  // "connectionHoverLeft"
        QT_MOC_LITERAL(251, 13),  // "nodeHoverLeft"
        QT_MOC_LITERAL(265, 15),  // "nodeContextMenu"
        QT_MOC_LITERAL(281, 3),  // "pos"
        QT_MOC_LITERAL(285, 22),  // "setupConnectionSignals"
        QT_MOC_LITERAL(308, 28),  // "sendConnectionCreatedToNodes"
        QT_MOC_LITERAL(337, 28)   // "sendConnectionDeletedToNodes"
    },
    "QtNodes::FlowScene",
    "nodeCreated",
    "",
    "QtNodes::Node&",
    "n",
    "nodePlaced",
    "nodeDeleted",
    "connectionCreated",
    "QtNodes::Connection",
    "c",
    "connectionDeleted",
    "nodeMoved",
    "newLocation",
    "nodeDoubleClicked",
    "connectionHovered",
    "QtNodes::Connection&",
    "screenPos",
    "nodeHovered",
    "connectionHoverLeft",
    "nodeHoverLeft",
    "nodeContextMenu",
    "pos",
    "setupConnectionSignals",
    "sendConnectionCreatedToNodes",
    "sendConnectionDeletedToNodes"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSQtNodesSCOPEFlowSceneENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  104,    2, 0x06,    1 /* Public */,
       5,    1,  107,    2, 0x06,    3 /* Public */,
       6,    1,  110,    2, 0x06,    5 /* Public */,
       7,    1,  113,    2, 0x06,    7 /* Public */,
      10,    1,  116,    2, 0x06,    9 /* Public */,
      11,    2,  119,    2, 0x06,   11 /* Public */,
      13,    1,  124,    2, 0x06,   14 /* Public */,
      14,    2,  127,    2, 0x06,   16 /* Public */,
      17,    2,  132,    2, 0x06,   19 /* Public */,
      18,    1,  137,    2, 0x06,   22 /* Public */,
      19,    1,  140,    2, 0x06,   24 /* Public */,
      20,    2,  143,    2, 0x06,   26 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      22,    1,  148,    2, 0x08,   29 /* Private */,
      23,    1,  151,    2, 0x08,   31 /* Private */,
      24,    1,  154,    2, 0x08,   33 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPointF,    4,   12,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 15, QMetaType::QPoint,    9,   16,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPoint,    4,   16,
    QMetaType::Void, 0x80000000 | 15,    9,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QPointF,    4,   21,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

Q_CONSTINIT const QMetaObject QtNodes::FlowScene::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsScene::staticMetaObject>(),
    qt_meta_stringdata_CLASSQtNodesSCOPEFlowSceneENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSQtNodesSCOPEFlowSceneENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSQtNodesSCOPEFlowSceneENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FlowScene, std::true_type>,
        // method 'nodeCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Node &, std::false_type>,
        // method 'nodePlaced'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Node &, std::false_type>,
        // method 'nodeDeleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Node &, std::false_type>,
        // method 'connectionCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Connection const &, std::false_type>,
        // method 'connectionDeleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Connection const &, std::false_type>,
        // method 'nodeMoved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Node &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPointF &, std::false_type>,
        // method 'nodeDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Node &, std::false_type>,
        // method 'connectionHovered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Connection &, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPoint, std::false_type>,
        // method 'nodeHovered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Node &, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPoint, std::false_type>,
        // method 'connectionHoverLeft'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Connection &, std::false_type>,
        // method 'nodeHoverLeft'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Node &, std::false_type>,
        // method 'nodeContextMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Node &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPointF &, std::false_type>,
        // method 'setupConnectionSignals'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Connection const &, std::false_type>,
        // method 'sendConnectionCreatedToNodes'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Connection const &, std::false_type>,
        // method 'sendConnectionDeletedToNodes'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QtNodes::Connection const &, std::false_type>
    >,
    nullptr
} };

void QtNodes::FlowScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FlowScene *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->nodeCreated((*reinterpret_cast< std::add_pointer_t<QtNodes::Node&>>(_a[1]))); break;
        case 1: _t->nodePlaced((*reinterpret_cast< std::add_pointer_t<QtNodes::Node&>>(_a[1]))); break;
        case 2: _t->nodeDeleted((*reinterpret_cast< std::add_pointer_t<QtNodes::Node&>>(_a[1]))); break;
        case 3: _t->connectionCreated((*reinterpret_cast< std::add_pointer_t<QtNodes::Connection>>(_a[1]))); break;
        case 4: _t->connectionDeleted((*reinterpret_cast< std::add_pointer_t<QtNodes::Connection>>(_a[1]))); break;
        case 5: _t->nodeMoved((*reinterpret_cast< std::add_pointer_t<QtNodes::Node&>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[2]))); break;
        case 6: _t->nodeDoubleClicked((*reinterpret_cast< std::add_pointer_t<QtNodes::Node&>>(_a[1]))); break;
        case 7: _t->connectionHovered((*reinterpret_cast< std::add_pointer_t<QtNodes::Connection&>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[2]))); break;
        case 8: _t->nodeHovered((*reinterpret_cast< std::add_pointer_t<QtNodes::Node&>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[2]))); break;
        case 9: _t->connectionHoverLeft((*reinterpret_cast< std::add_pointer_t<QtNodes::Connection&>>(_a[1]))); break;
        case 10: _t->nodeHoverLeft((*reinterpret_cast< std::add_pointer_t<QtNodes::Node&>>(_a[1]))); break;
        case 11: _t->nodeContextMenu((*reinterpret_cast< std::add_pointer_t<QtNodes::Node&>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[2]))); break;
        case 12: _t->setupConnectionSignals((*reinterpret_cast< std::add_pointer_t<QtNodes::Connection>>(_a[1]))); break;
        case 13: _t->sendConnectionCreatedToNodes((*reinterpret_cast< std::add_pointer_t<QtNodes::Connection>>(_a[1]))); break;
        case 14: _t->sendConnectionDeletedToNodes((*reinterpret_cast< std::add_pointer_t<QtNodes::Connection>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FlowScene::*)(QtNodes::Node & );
            if (_t _q_method = &FlowScene::nodeCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Node & );
            if (_t _q_method = &FlowScene::nodePlaced; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Node & );
            if (_t _q_method = &FlowScene::nodeDeleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Connection const & );
            if (_t _q_method = &FlowScene::connectionCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Connection const & );
            if (_t _q_method = &FlowScene::connectionDeleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Node & , const QPointF & );
            if (_t _q_method = &FlowScene::nodeMoved; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Node & );
            if (_t _q_method = &FlowScene::nodeDoubleClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Connection & , QPoint );
            if (_t _q_method = &FlowScene::connectionHovered; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Node & , QPoint );
            if (_t _q_method = &FlowScene::nodeHovered; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Connection & );
            if (_t _q_method = &FlowScene::connectionHoverLeft; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Node & );
            if (_t _q_method = &FlowScene::nodeHoverLeft; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (FlowScene::*)(QtNodes::Node & , const QPointF & );
            if (_t _q_method = &FlowScene::nodeContextMenu; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
    }
}

const QMetaObject *QtNodes::FlowScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtNodes::FlowScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSQtNodesSCOPEFlowSceneENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsScene::qt_metacast(_clname);
}

int QtNodes::FlowScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void QtNodes::FlowScene::nodeCreated(QtNodes::Node & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtNodes::FlowScene::nodePlaced(QtNodes::Node & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtNodes::FlowScene::nodeDeleted(QtNodes::Node & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QtNodes::FlowScene::connectionCreated(QtNodes::Connection const & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QtNodes::FlowScene::connectionDeleted(QtNodes::Connection const & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QtNodes::FlowScene::nodeMoved(QtNodes::Node & _t1, const QPointF & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QtNodes::FlowScene::nodeDoubleClicked(QtNodes::Node & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QtNodes::FlowScene::connectionHovered(QtNodes::Connection & _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QtNodes::FlowScene::nodeHovered(QtNodes::Node & _t1, QPoint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QtNodes::FlowScene::connectionHoverLeft(QtNodes::Connection & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void QtNodes::FlowScene::nodeHoverLeft(QtNodes::Node & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void QtNodes::FlowScene::nodeContextMenu(QtNodes::Node & _t1, const QPointF & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_WARNING_POP
