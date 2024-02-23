/****************************************************************************
** Meta object code from reading C++ file 'qt_logger.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt_logger.hpp"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qt_logger.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASStoolSCOPEQtLoggerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASStoolSCOPEQtLoggerENDCLASS = QtMocHelpers::stringData(
    "tool::QtLogger",
    "message_signal",
    "",
    "message",
    "error_signal",
    "error",
    "warning_signal",
    "warning",
    "log_signal",
    "log",
    "status_signal",
    "status",
    "ms",
    "unity_message_signal",
    "QStringView",
    "unity_error_signal",
    "unity_warning_signal",
    "unity_unknow_signal",
    "unknow",
    "progress_signal",
    "state"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASStoolSCOPEQtLoggerENDCLASS_t {
    uint offsetsAndSizes[42];
    char stringdata0[15];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[13];
    char stringdata5[6];
    char stringdata6[15];
    char stringdata7[8];
    char stringdata8[11];
    char stringdata9[4];
    char stringdata10[14];
    char stringdata11[7];
    char stringdata12[3];
    char stringdata13[21];
    char stringdata14[12];
    char stringdata15[19];
    char stringdata16[21];
    char stringdata17[20];
    char stringdata18[7];
    char stringdata19[16];
    char stringdata20[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASStoolSCOPEQtLoggerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASStoolSCOPEQtLoggerENDCLASS_t qt_meta_stringdata_CLASStoolSCOPEQtLoggerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14),  // "tool::QtLogger"
        QT_MOC_LITERAL(15, 14),  // "message_signal"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 7),  // "message"
        QT_MOC_LITERAL(39, 12),  // "error_signal"
        QT_MOC_LITERAL(52, 5),  // "error"
        QT_MOC_LITERAL(58, 14),  // "warning_signal"
        QT_MOC_LITERAL(73, 7),  // "warning"
        QT_MOC_LITERAL(81, 10),  // "log_signal"
        QT_MOC_LITERAL(92, 3),  // "log"
        QT_MOC_LITERAL(96, 13),  // "status_signal"
        QT_MOC_LITERAL(110, 6),  // "status"
        QT_MOC_LITERAL(117, 2),  // "ms"
        QT_MOC_LITERAL(120, 20),  // "unity_message_signal"
        QT_MOC_LITERAL(141, 11),  // "QStringView"
        QT_MOC_LITERAL(153, 18),  // "unity_error_signal"
        QT_MOC_LITERAL(172, 20),  // "unity_warning_signal"
        QT_MOC_LITERAL(193, 19),  // "unity_unknow_signal"
        QT_MOC_LITERAL(213, 6),  // "unknow"
        QT_MOC_LITERAL(220, 15),  // "progress_signal"
        QT_MOC_LITERAL(236, 5)   // "state"
    },
    "tool::QtLogger",
    "message_signal",
    "",
    "message",
    "error_signal",
    "error",
    "warning_signal",
    "warning",
    "log_signal",
    "log",
    "status_signal",
    "status",
    "ms",
    "unity_message_signal",
    "QStringView",
    "unity_error_signal",
    "unity_warning_signal",
    "unity_unknow_signal",
    "unknow",
    "progress_signal",
    "state"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASStoolSCOPEQtLoggerENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   74,    2, 0x06,    1 /* Public */,
       4,    1,   77,    2, 0x06,    3 /* Public */,
       6,    1,   80,    2, 0x06,    5 /* Public */,
       8,    1,   83,    2, 0x06,    7 /* Public */,
      10,    2,   86,    2, 0x06,    9 /* Public */,
      13,    1,   91,    2, 0x06,   12 /* Public */,
      15,    1,   94,    2, 0x06,   14 /* Public */,
      16,    1,   97,    2, 0x06,   16 /* Public */,
      17,    1,  100,    2, 0x06,   18 /* Public */,
      19,    1,  103,    2, 0x06,   20 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   11,   12,
    QMetaType::Void, 0x80000000 | 14,    3,
    QMetaType::Void, 0x80000000 | 14,    5,
    QMetaType::Void, 0x80000000 | 14,    7,
    QMetaType::Void, 0x80000000 | 14,   18,
    QMetaType::Void, QMetaType::Int,   20,

       0        // eod
};

Q_CONSTINIT const QMetaObject tool::QtLogger::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASStoolSCOPEQtLoggerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASStoolSCOPEQtLoggerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASStoolSCOPEQtLoggerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<QtLogger, std::true_type>,
        // method 'message_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'error_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'warning_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'log_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'status_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'unity_message_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QStringView, std::false_type>,
        // method 'unity_error_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QStringView, std::false_type>,
        // method 'unity_warning_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QStringView, std::false_type>,
        // method 'unity_unknow_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QStringView, std::false_type>,
        // method 'progress_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void tool::QtLogger::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QtLogger *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->message_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->error_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->warning_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->log_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->status_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 5: _t->unity_message_signal((*reinterpret_cast< std::add_pointer_t<QStringView>>(_a[1]))); break;
        case 6: _t->unity_error_signal((*reinterpret_cast< std::add_pointer_t<QStringView>>(_a[1]))); break;
        case 7: _t->unity_warning_signal((*reinterpret_cast< std::add_pointer_t<QStringView>>(_a[1]))); break;
        case 8: _t->unity_unknow_signal((*reinterpret_cast< std::add_pointer_t<QStringView>>(_a[1]))); break;
        case 9: _t->progress_signal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QtLogger::*)(QString );
            if (_t _q_method = &QtLogger::message_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QtLogger::*)(QString );
            if (_t _q_method = &QtLogger::error_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QtLogger::*)(QString );
            if (_t _q_method = &QtLogger::warning_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (QtLogger::*)(QString );
            if (_t _q_method = &QtLogger::log_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (QtLogger::*)(QString , int );
            if (_t _q_method = &QtLogger::status_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (QtLogger::*)(QStringView );
            if (_t _q_method = &QtLogger::unity_message_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (QtLogger::*)(QStringView );
            if (_t _q_method = &QtLogger::unity_error_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (QtLogger::*)(QStringView );
            if (_t _q_method = &QtLogger::unity_warning_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (QtLogger::*)(QStringView );
            if (_t _q_method = &QtLogger::unity_unknow_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (QtLogger::*)(int );
            if (_t _q_method = &QtLogger::progress_signal; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject *tool::QtLogger::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tool::QtLogger::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASStoolSCOPEQtLoggerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int tool::QtLogger::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void tool::QtLogger::message_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void tool::QtLogger::error_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void tool::QtLogger::warning_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void tool::QtLogger::log_signal(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void tool::QtLogger::status_signal(QString _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void tool::QtLogger::unity_message_signal(QStringView _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void tool::QtLogger::unity_error_signal(QStringView _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void tool::QtLogger::unity_warning_signal(QStringView _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void tool::QtLogger::unity_unknow_signal(QStringView _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void tool::QtLogger::progress_signal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
