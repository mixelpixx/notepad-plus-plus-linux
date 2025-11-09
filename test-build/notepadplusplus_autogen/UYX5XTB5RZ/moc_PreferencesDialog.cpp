/****************************************************************************
** Meta object code from reading C++ file 'PreferencesDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../ui/PreferencesDialog.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PreferencesDialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_NotepadPlusPlus__PreferencesDialog_t {
    uint offsetsAndSizes[22];
    char stringdata0[35];
    char stringdata1[7];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[18];
    char stringdata5[14];
    char stringdata6[15];
    char stringdata7[15];
    char stringdata8[22];
    char stringdata9[17];
    char stringdata10[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_NotepadPlusPlus__PreferencesDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_NotepadPlusPlus__PreferencesDialog_t qt_meta_stringdata_NotepadPlusPlus__PreferencesDialog = {
    {
        QT_MOC_LITERAL(0, 34),  // "NotepadPlusPlus::PreferencesD..."
        QT_MOC_LITERAL(35, 6),  // "accept"
        QT_MOC_LITERAL(42, 0),  // ""
        QT_MOC_LITERAL(43, 6),  // "reject"
        QT_MOC_LITERAL(50, 17),  // "onResetToDefaults"
        QT_MOC_LITERAL(68, 13),  // "onFontChanged"
        QT_MOC_LITERAL(82, 14),  // "onColorChanged"
        QT_MOC_LITERAL(97, 14),  // "onThemeChanged"
        QT_MOC_LITERAL(112, 21),  // "onLanguageAssociation"
        QT_MOC_LITERAL(134, 16),  // "onImportSettings"
        QT_MOC_LITERAL(151, 16)   // "onExportSettings"
    },
    "NotepadPlusPlus::PreferencesDialog",
    "accept",
    "",
    "reject",
    "onResetToDefaults",
    "onFontChanged",
    "onColorChanged",
    "onThemeChanged",
    "onLanguageAssociation",
    "onImportSettings",
    "onExportSettings"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_NotepadPlusPlus__PreferencesDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   68,    2, 0x0a,    1 /* Public */,
       3,    0,   69,    2, 0x0a,    2 /* Public */,
       4,    0,   70,    2, 0x08,    3 /* Private */,
       5,    0,   71,    2, 0x08,    4 /* Private */,
       6,    0,   72,    2, 0x08,    5 /* Private */,
       7,    0,   73,    2, 0x08,    6 /* Private */,
       8,    0,   74,    2, 0x08,    7 /* Private */,
       9,    0,   75,    2, 0x08,    8 /* Private */,
      10,    0,   76,    2, 0x08,    9 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject NotepadPlusPlus::PreferencesDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_NotepadPlusPlus__PreferencesDialog.offsetsAndSizes,
    qt_meta_data_NotepadPlusPlus__PreferencesDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_NotepadPlusPlus__PreferencesDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PreferencesDialog, std::true_type>,
        // method 'accept'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reject'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onResetToDefaults'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFontChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onColorChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onThemeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLanguageAssociation'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onImportSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void NotepadPlusPlus::PreferencesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PreferencesDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->accept(); break;
        case 1: _t->reject(); break;
        case 2: _t->onResetToDefaults(); break;
        case 3: _t->onFontChanged(); break;
        case 4: _t->onColorChanged(); break;
        case 5: _t->onThemeChanged(); break;
        case 6: _t->onLanguageAssociation(); break;
        case 7: _t->onImportSettings(); break;
        case 8: _t->onExportSettings(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *NotepadPlusPlus::PreferencesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotepadPlusPlus::PreferencesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NotepadPlusPlus__PreferencesDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int NotepadPlusPlus::PreferencesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
