/****************************************************************************
** Meta object code from reading C++ file 'ex_base_w.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../gui/ex_widgets/ex_base_w.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ex_base_w.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEexSCOPEExBaseWENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEexSCOPEExBaseWENDCLASS = QtMocHelpers::stringData(
    "tool::ex::ExBaseW",
    "ui_change_signal",
    "",
    "QStringView",
    "name",
    "action_signal",
    "update_from_components_signal",
    "update_from_resources_signal"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEexSCOPEExBaseWENDCLASS_t {
    uint offsetsAndSizes[16];
    char stringdata0[18];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[5];
    char stringdata5[14];
    char stringdata6[30];
    char stringdata7[29];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEexSCOPEExBaseWENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEexSCOPEExBaseWENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEexSCOPEExBaseWENDCLASS = {
    {
        QT_MOC_LITERAL(0, 17),  // "tool::ex::ExBaseW"
        QT_MOC_LITERAL(18, 16),  // "ui_change_signal"
        QT_MOC_LITERAL(35, 0),  // ""
        QT_MOC_LITERAL(36, 11),  // "QStringView"
        QT_MOC_LITERAL(48, 4),  // "name"
        QT_MOC_LITERAL(53, 13),  // "action_signal"
        QT_MOC_LITERAL(67, 29),  // "update_from_components_signal"
        QT_MOC_LITERAL(97, 28)   // "update_from_resources_signal"
    },
    "tool::ex::ExBaseW",
    "ui_change_signal",
    "",
    "QStringView",
    "name",
    "action_signal",
    "update_from_components_signal",
    "update_from_resources_signal"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEexSCOPEExBaseWENDCLASS[] = {

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
       1,    1,   38,    2, 0x06,    1 /* Public */,
       5,    1,   41,    2, 0x06,    3 /* Public */,
       6,    0,   44,    2, 0x06,    5 /* Public */,
       7,    0,   45,    2, 0x06,    6 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::ex::ExBaseW::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEexSCOPEExBaseWENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEexSCOPEExBaseWENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEexSCOPEExBaseWENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ExBaseW, std::true_type>,
        // method 'ui_change_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QStringView, std::false_type>,
        // method 'action_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QStringView, std::false_type>,
        // method 'update_from_components_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'update_from_resources_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void tool::ex::ExBaseW::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ExBaseW *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ui_change_signal((*reinterpret_cast< std::add_pointer_t<QStringView>>(_a[1]))); break;
        case 1: _t->action_signal((*reinterpret_cast< std::add_pointer_t<QStringView>>(_a[1]))); break;
        case 2: _t->update_from_components_signal(); break;
        case 3: _t->update_from_resources_signal(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ExBaseW::*)(QStringView );
            if (_t _q_method = &ExBaseW::ui_change_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ExBaseW::*)(QStringView );
            if (_t _q_method = &ExBaseW::action_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ExBaseW::*)();
            if (_t _q_method = &ExBaseW::update_from_components_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ExBaseW::*)();
            if (_t _q_method = &ExBaseW::update_from_resources_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *tool::ex::ExBaseW::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::ex::ExBaseW::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEexSCOPEExBaseWENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::ex::ExBaseW::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void tool::ex::ExBaseW::ui_change_signal(QStringView _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void tool::ex::ExBaseW::action_signal(QStringView _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void tool::ex::ExBaseW::update_from_components_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void tool::ex::ExBaseW::update_from_resources_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
