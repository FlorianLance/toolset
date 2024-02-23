/****************************************************************************
** Meta object code from reading C++ file 'ex_curve_x_w.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../gui/ex_widgets/ex_curve_x_w.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ex_curve_x_w.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEexSCOPECurveXWENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEexSCOPECurveXWENDCLASS = QtMocHelpers::stringData(
    "tool::ex::CurveXW",
    "set_y_range",
    "",
    "min",
    "max",
    "autoMinMax",
    "set_line_color",
    "color",
    "set_max_size",
    "maxSize",
    "add_value",
    "y",
    "add_values",
    "std::vector<double>",
    "yValues",
    "clear",
    "update_y_scale_from_min_max"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEexSCOPECurveXWENDCLASS_t {
    uint offsetsAndSizes[34];
    char stringdata0[18];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[4];
    char stringdata4[4];
    char stringdata5[11];
    char stringdata6[15];
    char stringdata7[6];
    char stringdata8[13];
    char stringdata9[8];
    char stringdata10[10];
    char stringdata11[2];
    char stringdata12[11];
    char stringdata13[20];
    char stringdata14[8];
    char stringdata15[6];
    char stringdata16[28];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEexSCOPECurveXWENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEexSCOPECurveXWENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEexSCOPECurveXWENDCLASS = {
    {
        QT_MOC_LITERAL(0, 17),  // "tool::ex::CurveXW"
        QT_MOC_LITERAL(18, 11),  // "set_y_range"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 3),  // "min"
        QT_MOC_LITERAL(35, 3),  // "max"
        QT_MOC_LITERAL(39, 10),  // "autoMinMax"
        QT_MOC_LITERAL(50, 14),  // "set_line_color"
        QT_MOC_LITERAL(65, 5),  // "color"
        QT_MOC_LITERAL(71, 12),  // "set_max_size"
        QT_MOC_LITERAL(84, 7),  // "maxSize"
        QT_MOC_LITERAL(92, 9),  // "add_value"
        QT_MOC_LITERAL(102, 1),  // "y"
        QT_MOC_LITERAL(104, 10),  // "add_values"
        QT_MOC_LITERAL(115, 19),  // "std::vector<double>"
        QT_MOC_LITERAL(135, 7),  // "yValues"
        QT_MOC_LITERAL(143, 5),  // "clear"
        QT_MOC_LITERAL(149, 27)   // "update_y_scale_from_min_max"
    },
    "tool::ex::CurveXW",
    "set_y_range",
    "",
    "min",
    "max",
    "autoMinMax",
    "set_line_color",
    "color",
    "set_max_size",
    "maxSize",
    "add_value",
    "y",
    "add_values",
    "std::vector<double>",
    "yValues",
    "clear",
    "update_y_scale_from_min_max"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEexSCOPECurveXWENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   56,    2, 0x0a,    1 /* Public */,
       6,    1,   63,    2, 0x0a,    5 /* Public */,
       8,    1,   66,    2, 0x0a,    7 /* Public */,
      10,    1,   69,    2, 0x0a,    9 /* Public */,
      12,    1,   72,    2, 0x0a,   11 /* Public */,
      15,    0,   75,    2, 0x0a,   13 /* Public */,
      16,    0,   76,    2, 0x0a,   14 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::QColor,    7,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::QReal,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ex::CurveXW::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtPlot::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEexSCOPECurveXWENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEexSCOPECurveXWENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEexSCOPECurveXWENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CurveXW, std::true_type>,
        // method 'set_y_range'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'set_line_color'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QColor, std::false_type>,
        // method 'set_max_size'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'add_value'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        // method 'add_values'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::vector<double> &, std::false_type>,
        // method 'clear'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'update_y_scale_from_min_max'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void tool::ex::CurveXW::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CurveXW *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->set_y_range((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qreal>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 1: _t->set_line_color((*reinterpret_cast< std::add_pointer_t<QColor>>(_a[1]))); break;
        case 2: _t->set_max_size((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->add_value((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1]))); break;
        case 4: _t->add_values((*reinterpret_cast< std::add_pointer_t<std::vector<double>>>(_a[1]))); break;
        case 5: _t->clear(); break;
        case 6: _t->update_y_scale_from_min_max(); break;
        default: ;
        }
    }
}

const QMetaObject *tool::ex::CurveXW::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ex::CurveXW::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEexSCOPECurveXWENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QwtPlot::qt_metacast(_clname);
}

int tool::ex::CurveXW::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
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
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS = QtMocHelpers::stringData(
    "tool::ex::ExCurveXW",
    "name_changed_signal",
    "",
    "name"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS_t {
    uint offsetsAndSizes[8];
    char stringdata0[20];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS = {
    {
        QT_MOC_LITERAL(0, 19),  // "tool::ex::ExCurveXW"
        QT_MOC_LITERAL(20, 19),  // "name_changed_signal"
        QT_MOC_LITERAL(40, 0),  // ""
        QT_MOC_LITERAL(41, 4)   // "name"
    },
    "tool::ex::ExCurveXW",
    "name_changed_signal",
    "",
    "name"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x06,    1 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ex::ExCurveXW::staticMetaObject = { {
    QMetaObject::SuperData::link<ExItemW<CurveXManagerW>::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ExCurveXW, std::true_type>,
        // method 'name_changed_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>
    >,
    nullptr
} };

void tool::ex::ExCurveXW::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ExCurveXW *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->name_changed_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ExCurveXW::*)(QString );
            if (_t _q_method = &ExCurveXW::name_changed_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *tool::ex::ExCurveXW::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ex::ExCurveXW::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEexSCOPEExCurveXWENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return ExItemW<CurveXManagerW>::qt_metacast(_clname);
}

int tool::ex::ExCurveXW::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ExItemW<CurveXManagerW>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void tool::ex::ExCurveXW::name_changed_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
