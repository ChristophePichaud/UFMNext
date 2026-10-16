/****************************************************************************
** Meta object code from reading C++ file 'ShapeToolboxWidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/ufmnext/gui/ShapeToolboxWidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ShapeToolboxWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
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
struct qt_meta_stringdata_CLASSufmnextSCOPEguiSCOPEShapeToolboxWidgetENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSufmnextSCOPEguiSCOPEShapeToolboxWidgetENDCLASS = QtMocHelpers::stringData(
    "ufmnext::gui::ShapeToolboxWidget",
    "toolChanged",
    "",
    "ToolMode",
    "mode",
    "core::ShapeType",
    "type",
    "core::LineCapStyle",
    "startCap",
    "endCap",
    "shapeClass",
    "onItemSelectionChanged",
    "onLoadPluginClicked"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSufmnextSCOPEguiSCOPEShapeToolboxWidgetENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    5,   32,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,   43,    2, 0x08,    7 /* Private */,
      12,    0,   44,    2, 0x08,    8 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5, 0x80000000 | 7, 0x80000000 | 7, QMetaType::QString,    4,    6,    8,    9,   10,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ufmnext::gui::ShapeToolboxWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSufmnextSCOPEguiSCOPEShapeToolboxWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSufmnextSCOPEguiSCOPEShapeToolboxWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSufmnextSCOPEguiSCOPEShapeToolboxWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ShapeToolboxWidget, std::true_type>,
        // method 'toolChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ToolMode, std::false_type>,
        QtPrivate::TypeAndForceComplete<core::ShapeType, std::false_type>,
        QtPrivate::TypeAndForceComplete<core::LineCapStyle, std::false_type>,
        QtPrivate::TypeAndForceComplete<core::LineCapStyle, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onItemSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLoadPluginClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ufmnext::gui::ShapeToolboxWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ShapeToolboxWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->toolChanged((*reinterpret_cast< std::add_pointer_t<ToolMode>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<core::ShapeType>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<core::LineCapStyle>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<core::LineCapStyle>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5]))); break;
        case 1: _t->onItemSelectionChanged(); break;
        case 2: _t->onLoadPluginClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ShapeToolboxWidget::*)(ToolMode , core::ShapeType , core::LineCapStyle , core::LineCapStyle , const QString & );
            if (_t _q_method = &ShapeToolboxWidget::toolChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *ufmnext::gui::ShapeToolboxWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ufmnext::gui::ShapeToolboxWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSufmnextSCOPEguiSCOPEShapeToolboxWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ufmnext::gui::ShapeToolboxWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void ufmnext::gui::ShapeToolboxWidget::toolChanged(ToolMode _t1, core::ShapeType _t2, core::LineCapStyle _t3, core::LineCapStyle _t4, const QString & _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
