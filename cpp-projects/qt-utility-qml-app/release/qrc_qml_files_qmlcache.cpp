/****************************************************************************
** Resource object code
**
** Created by: The Resource Compiler for Qt version 6.5.0
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

static const unsigned char qt_resource_data[] = {
  // D:/DEV/Perso/toolset/cpp-projects/qt-utility-qml-app/qml/QtUtilityMain.qml
  0x0,0x0,0x0,0x0,
  
  
};

static const unsigned char qt_resource_name[] = {
  // qml
  0x0,0x3,
  0x0,0x0,0x78,0x3c,
  0x0,0x71,
  0x0,0x6d,0x0,0x6c,
    // QtUtilityMain.qml
  0x0,0x11,
  0x7,0x2,0x5a,0x9c,
  0x0,0x51,
  0x0,0x74,0x0,0x55,0x0,0x74,0x0,0x69,0x0,0x6c,0x0,0x69,0x0,0x74,0x0,0x79,0x0,0x4d,0x0,0x61,0x0,0x69,0x0,0x6e,0x0,0x2e,0x0,0x71,0x0,0x6d,0x0,0x6c,
  
  
};

static const unsigned char qt_resource_struct[] = {
  // :
  0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x1,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  // :/qml
  0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x2,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  // :/qml/QtUtilityMain.qml
  0x0,0x0,0x0,0xc,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,
0x0,0x0,0x1,0x95,0x8e,0xed,0x70,0x6f,

};

#ifdef QT_NAMESPACE
#  define QT_RCC_PREPEND_NAMESPACE(name) ::QT_NAMESPACE::name
#  define QT_RCC_MANGLE_NAMESPACE0(x) x
#  define QT_RCC_MANGLE_NAMESPACE1(a, b) a##_##b
#  define QT_RCC_MANGLE_NAMESPACE2(a, b) QT_RCC_MANGLE_NAMESPACE1(a,b)
#  define QT_RCC_MANGLE_NAMESPACE(name) QT_RCC_MANGLE_NAMESPACE2( \
        QT_RCC_MANGLE_NAMESPACE0(name), QT_RCC_MANGLE_NAMESPACE0(QT_NAMESPACE))
#else
#   define QT_RCC_PREPEND_NAMESPACE(name) name
#   define QT_RCC_MANGLE_NAMESPACE(name) name
#endif

#ifdef QT_NAMESPACE
namespace QT_NAMESPACE {
#endif

bool qRegisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);
bool qUnregisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);

#ifdef QT_NAMESPACE
}
#endif

int QT_RCC_MANGLE_NAMESPACE(qInitResources_qml_files_qmlcache)();
int QT_RCC_MANGLE_NAMESPACE(qInitResources_qml_files_qmlcache)()
{
    int version = 3;
    QT_RCC_PREPEND_NAMESPACE(qRegisterResourceData)
        (version, qt_resource_struct, qt_resource_name, qt_resource_data);
    return 1;
}

int QT_RCC_MANGLE_NAMESPACE(qCleanupResources_qml_files_qmlcache)();
int QT_RCC_MANGLE_NAMESPACE(qCleanupResources_qml_files_qmlcache)()
{
    int version = 3;
    QT_RCC_PREPEND_NAMESPACE(qUnregisterResourceData)
       (version, qt_resource_struct, qt_resource_name, qt_resource_data);
    return 1;
}

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif

namespace {
   struct initializer {
       initializer() { QT_RCC_MANGLE_NAMESPACE(qInitResources_qml_files_qmlcache)(); }
       ~initializer() { QT_RCC_MANGLE_NAMESPACE(qCleanupResources_qml_files_qmlcache)(); }
   } dummy;
}

#ifdef __clang__
#   pragma clang diagnostic pop
#endif
