/****************************************************************************
** Meta object code from reading C++ file 'curve_widget.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../gui/widgets/curve_widget.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'curve_widget.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPECurveWENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEuiSCOPECurveWENDCLASS = QtMocHelpers::stringData(
    "tool::ui::CurveW",
    "data_updated_signal",
    "",
    "settings_updated_signal",
    "reset",
    "size_t",
    "nbCurves",
    "add_point_to_end",
    "xOffset",
    "y",
    "idCurve",
    "add_point",
    "x",
    "set_points",
    "std::vector<double>",
    "set_first_y",
    "value",
    "set_last_y",
    "set_type",
    "Curve::Type",
    "type",
    "set_fitted_state",
    "state",
    "remove_symbol",
    "set_title",
    "title",
    "set_x_title",
    "set_y_title",
    "set_x_range",
    "min",
    "max",
    "set_y_range",
    "set_pen",
    "pen"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEuiSCOPECurveWENDCLASS_t {
    uint offsetsAndSizes[68];
    char stringdata0[17];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[24];
    char stringdata4[6];
    char stringdata5[7];
    char stringdata6[9];
    char stringdata7[17];
    char stringdata8[8];
    char stringdata9[2];
    char stringdata10[8];
    char stringdata11[10];
    char stringdata12[2];
    char stringdata13[11];
    char stringdata14[20];
    char stringdata15[12];
    char stringdata16[6];
    char stringdata17[11];
    char stringdata18[9];
    char stringdata19[12];
    char stringdata20[5];
    char stringdata21[17];
    char stringdata22[6];
    char stringdata23[14];
    char stringdata24[10];
    char stringdata25[6];
    char stringdata26[12];
    char stringdata27[12];
    char stringdata28[12];
    char stringdata29[4];
    char stringdata30[4];
    char stringdata31[12];
    char stringdata32[8];
    char stringdata33[4];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEuiSCOPECurveWENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEuiSCOPECurveWENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEuiSCOPECurveWENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "tool::ui::CurveW"
        QT_MOC_LITERAL(17, 19),  // "data_updated_signal"
        QT_MOC_LITERAL(37, 0),  // ""
        QT_MOC_LITERAL(38, 23),  // "settings_updated_signal"
        QT_MOC_LITERAL(62, 5),  // "reset"
        QT_MOC_LITERAL(68, 6),  // "size_t"
        QT_MOC_LITERAL(75, 8),  // "nbCurves"
        QT_MOC_LITERAL(84, 16),  // "add_point_to_end"
        QT_MOC_LITERAL(101, 7),  // "xOffset"
        QT_MOC_LITERAL(109, 1),  // "y"
        QT_MOC_LITERAL(111, 7),  // "idCurve"
        QT_MOC_LITERAL(119, 9),  // "add_point"
        QT_MOC_LITERAL(129, 1),  // "x"
        QT_MOC_LITERAL(131, 10),  // "set_points"
        QT_MOC_LITERAL(142, 19),  // "std::vector<double>"
        QT_MOC_LITERAL(162, 11),  // "set_first_y"
        QT_MOC_LITERAL(174, 5),  // "value"
        QT_MOC_LITERAL(180, 10),  // "set_last_y"
        QT_MOC_LITERAL(191, 8),  // "set_type"
        QT_MOC_LITERAL(200, 11),  // "Curve::Type"
        QT_MOC_LITERAL(212, 4),  // "type"
        QT_MOC_LITERAL(217, 16),  // "set_fitted_state"
        QT_MOC_LITERAL(234, 5),  // "state"
        QT_MOC_LITERAL(240, 13),  // "remove_symbol"
        QT_MOC_LITERAL(254, 9),  // "set_title"
        QT_MOC_LITERAL(264, 5),  // "title"
        QT_MOC_LITERAL(270, 11),  // "set_x_title"
        QT_MOC_LITERAL(282, 11),  // "set_y_title"
        QT_MOC_LITERAL(294, 11),  // "set_x_range"
        QT_MOC_LITERAL(306, 3),  // "min"
        QT_MOC_LITERAL(310, 3),  // "max"
        QT_MOC_LITERAL(314, 11),  // "set_y_range"
        QT_MOC_LITERAL(326, 7),  // "set_pen"
        QT_MOC_LITERAL(334, 3)   // "pen"
    },
    "tool::ui::CurveW",
    "data_updated_signal",
    "",
    "settings_updated_signal",
    "reset",
    "size_t",
    "nbCurves",
    "add_point_to_end",
    "xOffset",
    "y",
    "idCurve",
    "add_point",
    "x",
    "set_points",
    "std::vector<double>",
    "set_first_y",
    "value",
    "set_last_y",
    "set_type",
    "Curve::Type",
    "type",
    "set_fitted_state",
    "state",
    "remove_symbol",
    "set_title",
    "title",
    "set_x_title",
    "set_y_title",
    "set_x_range",
    "min",
    "max",
    "set_y_range",
    "set_pen",
    "pen"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEuiSCOPECurveWENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  122,    2, 0x06,    1 /* Public */,
       3,    0,  123,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    1,  124,    2, 0x0a,    3 /* Public */,
       4,    0,  127,    2, 0x2a,    5 /* Public | MethodCloned */,
       7,    3,  128,    2, 0x0a,    6 /* Public */,
      11,    3,  135,    2, 0x0a,   10 /* Public */,
      13,    3,  142,    2, 0x0a,   14 /* Public */,
      15,    2,  149,    2, 0x0a,   18 /* Public */,
      17,    2,  154,    2, 0x0a,   21 /* Public */,
      18,    2,  159,    2, 0x0a,   24 /* Public */,
      21,    1,  164,    2, 0x0a,   27 /* Public */,
      23,    1,  167,    2, 0x0a,   29 /* Public */,
      24,    1,  170,    2, 0x0a,   31 /* Public */,
      26,    1,  173,    2, 0x0a,   33 /* Public */,
      27,    1,  176,    2, 0x0a,   35 /* Public */,
      28,    2,  179,    2, 0x0a,   37 /* Public */,
      31,    2,  184,    2, 0x0a,   40 /* Public */,
      32,    2,  189,    2, 0x0a,   43 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, 0x80000000 | 5,    8,    9,   10,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, 0x80000000 | 5,   12,    9,   10,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 14, 0x80000000 | 5,   12,    9,   10,
    QMetaType::Void, QMetaType::Double, 0x80000000 | 5,   16,   10,
    QMetaType::Void, QMetaType::Double, 0x80000000 | 5,   16,   10,
    QMetaType::Void, 0x80000000 | 19, 0x80000000 | 5,   20,   10,
    QMetaType::Void, QMetaType::Bool,   22,
    QMetaType::Void, 0x80000000 | 5,   10,
    QMetaType::Void, QMetaType::QString,   25,
    QMetaType::Void, QMetaType::QString,   25,
    QMetaType::Void, QMetaType::QString,   25,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal,   29,   30,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal,   29,   30,
    QMetaType::Void, QMetaType::QPen, 0x80000000 | 5,   33,   10,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ui::CurveW::staticMetaObject = { {
    QMetaObject::SuperData::link<QwtPlot::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEuiSCOPECurveWENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEuiSCOPECurveWENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEuiSCOPECurveWENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CurveW, std::true_type>,
        // method 'data_updated_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'settings_updated_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'reset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'add_point_to_end'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'add_point'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'set_points'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<double>, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<double>, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'set_first_y'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'set_last_y'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'set_type'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Curve::Type, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'set_fitted_state'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'remove_symbol'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>,
        // method 'set_title'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'set_x_title'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'set_y_title'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'set_x_range'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        // method 'set_y_range'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        QtPrivate::TypeAndForceComplete<qreal, std::false_type>,
        // method 'set_pen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPen &, std::false_type>,
        QtPrivate::TypeAndForceComplete<size_t, std::false_type>
    >,
    nullptr
} };

void tool::ui::CurveW::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CurveW *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->data_updated_signal(); break;
        case 1: _t->settings_updated_signal(); break;
        case 2: _t->reset((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1]))); break;
        case 3: _t->reset(); break;
        case 4: _t->add_point_to_end((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[3]))); break;
        case 5: _t->add_point((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[3]))); break;
        case 6: _t->set_points((*reinterpret_cast< std::add_pointer_t<std::vector<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::vector<double>>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[3]))); break;
        case 7: _t->set_first_y((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[2]))); break;
        case 8: _t->set_last_y((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[2]))); break;
        case 9: _t->set_type((*reinterpret_cast< std::add_pointer_t<Curve::Type>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[2]))); break;
        case 10: _t->set_fitted_state((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->remove_symbol((*reinterpret_cast< std::add_pointer_t<size_t>>(_a[1]))); break;
        case 12: _t->set_title((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->set_x_title((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->set_y_title((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->set_x_range((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qreal>>(_a[2]))); break;
        case 16: _t->set_y_range((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qreal>>(_a[2]))); break;
        case 17: _t->set_pen((*reinterpret_cast< std::add_pointer_t<QPen>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<size_t>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CurveW::*)();
            if (_t _q_method = &CurveW::data_updated_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CurveW::*)();
            if (_t _q_method = &CurveW::settings_updated_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *tool::ui::CurveW::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ui::CurveW::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEuiSCOPECurveWENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QwtPlot::qt_metacast(_clname);
}

int tool::ui::CurveW::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void tool::ui::CurveW::data_updated_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void tool::ui::CurveW::settings_updated_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
