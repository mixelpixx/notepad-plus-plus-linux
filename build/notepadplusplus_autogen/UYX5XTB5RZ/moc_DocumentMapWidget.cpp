/****************************************************************************
** Meta object code from reading C++ file 'DocumentMapWidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../ui/DocumentMapWidget.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DocumentMapWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_NotepadPlusPlus__DocumentMapWidget_t {
    uint offsetsAndSizes[20];
    char stringdata0[35];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[5];
    char stringdata4[16];
    char stringdata5[7];
    char stringdata6[20];
    char stringdata7[30];
    char stringdata8[7];
    char stringdata9[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_NotepadPlusPlus__DocumentMapWidget_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_NotepadPlusPlus__DocumentMapWidget_t qt_meta_stringdata_NotepadPlusPlus__DocumentMapWidget = {
    {
        QT_MOC_LITERAL(0, 34),  // "NotepadPlusPlus::DocumentMapW..."
        QT_MOC_LITERAL(35, 15),  // "positionClicked"
        QT_MOC_LITERAL(51, 0),  // ""
        QT_MOC_LITERAL(52, 4),  // "line"
        QT_MOC_LITERAL(57, 15),  // "scrollRequested"
        QT_MOC_LITERAL(73, 6),  // "deltaY"
        QT_MOC_LITERAL(80, 19),  // "onEditorTextChanged"
        QT_MOC_LITERAL(100, 29),  // "onEditorCursorPositionChanged"
        QT_MOC_LITERAL(130, 6),  // "column"
        QT_MOC_LITERAL(137, 16)   // "onEditorScrolled"
    },
    "NotepadPlusPlus::DocumentMapWidget",
    "positionClicked",
    "",
    "line",
    "scrollRequested",
    "deltaY",
    "onEditorTextChanged",
    "onEditorCursorPositionChanged",
    "column",
    "onEditorScrolled"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_NotepadPlusPlus__DocumentMapWidget[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,
       4,    1,   47,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    0,   50,    2, 0x08,    5 /* Private */,
       7,    2,   51,    2, 0x08,    6 /* Private */,
       9,    0,   56,    2, 0x08,    9 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    8,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject NotepadPlusPlus::DocumentMapWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_NotepadPlusPlus__DocumentMapWidget.offsetsAndSizes,
    qt_meta_data_NotepadPlusPlus__DocumentMapWidget,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_NotepadPlusPlus__DocumentMapWidget_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DocumentMapWidget, std::true_type>,
        // method 'positionClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'scrollRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onEditorTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditorCursorPositionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onEditorScrolled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void NotepadPlusPlus::DocumentMapWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DocumentMapWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->positionClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->scrollRequested((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onEditorTextChanged(); break;
        case 3: _t->onEditorCursorPositionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->onEditorScrolled(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DocumentMapWidget::*)(int );
            if (_t _q_method = &DocumentMapWidget::positionClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DocumentMapWidget::*)(int );
            if (_t _q_method = &DocumentMapWidget::scrollRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *NotepadPlusPlus::DocumentMapWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotepadPlusPlus::DocumentMapWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NotepadPlusPlus__DocumentMapWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int NotepadPlusPlus::DocumentMapWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void NotepadPlusPlus::DocumentMapWidget::positionClicked(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NotepadPlusPlus::DocumentMapWidget::scrollRequested(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
