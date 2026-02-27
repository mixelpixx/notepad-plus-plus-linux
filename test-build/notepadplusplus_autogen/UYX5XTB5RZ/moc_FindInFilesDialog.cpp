/****************************************************************************
** Meta object code from reading C++ file 'FindInFilesDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../ui/FindInFilesDialog.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FindInFilesDialog.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_NotepadPlusPlus__FindInFilesWorker_t {
    uint offsetsAndSizes[32];
    char stringdata0[35];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[11];
    char stringdata5[13];
    char stringdata6[9];
    char stringdata7[12];
    char stringdata8[15];
    char stringdata9[12];
    char stringdata10[14];
    char stringdata11[11];
    char stringdata12[15];
    char stringdata13[13];
    char stringdata14[12];
    char stringdata15[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_NotepadPlusPlus__FindInFilesWorker_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_NotepadPlusPlus__FindInFilesWorker_t qt_meta_stringdata_NotepadPlusPlus__FindInFilesWorker = {
    {
        QT_MOC_LITERAL(0, 34),  // "NotepadPlusPlus::FindInFilesW..."
        QT_MOC_LITERAL(35, 9),  // "fileFound"
        QT_MOC_LITERAL(45, 0),  // ""
        QT_MOC_LITERAL(46, 8),  // "filePath"
        QT_MOC_LITERAL(55, 10),  // "lineNumber"
        QT_MOC_LITERAL(66, 12),  // "columnNumber"
        QT_MOC_LITERAL(79, 8),  // "lineText"
        QT_MOC_LITERAL(88, 11),  // "matchedText"
        QT_MOC_LITERAL(100, 14),  // "searchProgress"
        QT_MOC_LITERAL(115, 11),  // "currentFile"
        QT_MOC_LITERAL(127, 13),  // "filesSearched"
        QT_MOC_LITERAL(141, 10),  // "totalFiles"
        QT_MOC_LITERAL(152, 14),  // "searchFinished"
        QT_MOC_LITERAL(167, 12),  // "totalMatches"
        QT_MOC_LITERAL(180, 11),  // "searchError"
        QT_MOC_LITERAL(192, 5)   // "error"
    },
    "NotepadPlusPlus::FindInFilesWorker",
    "fileFound",
    "",
    "filePath",
    "lineNumber",
    "columnNumber",
    "lineText",
    "matchedText",
    "searchProgress",
    "currentFile",
    "filesSearched",
    "totalFiles",
    "searchFinished",
    "totalMatches",
    "searchError",
    "error"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_NotepadPlusPlus__FindInFilesWorker[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    5,   38,    2, 0x06,    1 /* Public */,
       8,    3,   49,    2, 0x06,    7 /* Public */,
      12,    2,   56,    2, 0x06,   11 /* Public */,
      14,    1,   61,    2, 0x06,   14 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::QString,    3,    4,    5,    6,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,    9,   10,   11,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   13,   11,
    QMetaType::Void, QMetaType::QString,   15,

       0        // eod
};

Q_CONSTINIT const QMetaObject NotepadPlusPlus::FindInFilesWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_NotepadPlusPlus__FindInFilesWorker.offsetsAndSizes,
    qt_meta_data_NotepadPlusPlus__FindInFilesWorker,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_NotepadPlusPlus__FindInFilesWorker_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FindInFilesWorker, std::true_type>,
        // method 'fileFound'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'searchProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'searchFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'searchError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void NotepadPlusPlus::FindInFilesWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FindInFilesWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->fileFound((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5]))); break;
        case 1: _t->searchProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 2: _t->searchFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->searchError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FindInFilesWorker::*)(const QString & , int , int , const QString & , const QString & );
            if (_t _q_method = &FindInFilesWorker::fileFound; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FindInFilesWorker::*)(const QString & , int , int );
            if (_t _q_method = &FindInFilesWorker::searchProgress; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (FindInFilesWorker::*)(int , int );
            if (_t _q_method = &FindInFilesWorker::searchFinished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (FindInFilesWorker::*)(const QString & );
            if (_t _q_method = &FindInFilesWorker::searchError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *NotepadPlusPlus::FindInFilesWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotepadPlusPlus::FindInFilesWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NotepadPlusPlus__FindInFilesWorker.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int NotepadPlusPlus::FindInFilesWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
void NotepadPlusPlus::FindInFilesWorker::fileFound(const QString & _t1, int _t2, int _t3, const QString & _t4, const QString & _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NotepadPlusPlus::FindInFilesWorker::searchProgress(const QString & _t1, int _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NotepadPlusPlus::FindInFilesWorker::searchFinished(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NotepadPlusPlus::FindInFilesWorker::searchError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
namespace {
struct qt_meta_stringdata_NotepadPlusPlus__FindInFilesDialog_t {
    uint offsetsAndSizes[60];
    char stringdata0[35];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[11];
    char stringdata5[5];
    char stringdata6[19];
    char stringdata7[10];
    char stringdata8[14];
    char stringdata9[14];
    char stringdata10[16];
    char stringdata11[22];
    char stringdata12[17];
    char stringdata13[5];
    char stringdata14[7];
    char stringdata15[12];
    char stringdata16[13];
    char stringdata17[9];
    char stringdata18[12];
    char stringdata19[17];
    char stringdata20[12];
    char stringdata21[14];
    char stringdata22[11];
    char stringdata23[17];
    char stringdata24[13];
    char stringdata25[14];
    char stringdata26[6];
    char stringdata27[15];
    char stringdata28[12];
    char stringdata29[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_NotepadPlusPlus__FindInFilesDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_NotepadPlusPlus__FindInFilesDialog_t qt_meta_stringdata_NotepadPlusPlus__FindInFilesDialog = {
    {
        QT_MOC_LITERAL(0, 34),  // "NotepadPlusPlus::FindInFilesD..."
        QT_MOC_LITERAL(35, 17),  // "openFileRequested"
        QT_MOC_LITERAL(53, 0),  // ""
        QT_MOC_LITERAL(54, 8),  // "filePath"
        QT_MOC_LITERAL(63, 10),  // "lineNumber"
        QT_MOC_LITERAL(74, 4),  // "show"
        QT_MOC_LITERAL(79, 18),  // "setSearchDirectory"
        QT_MOC_LITERAL(98, 9),  // "directory"
        QT_MOC_LITERAL(108, 13),  // "onFindClicked"
        QT_MOC_LITERAL(122, 13),  // "onStopClicked"
        QT_MOC_LITERAL(136, 15),  // "onBrowseClicked"
        QT_MOC_LITERAL(152, 21),  // "onResultDoubleClicked"
        QT_MOC_LITERAL(174, 16),  // "QTreeWidgetItem*"
        QT_MOC_LITERAL(191, 4),  // "item"
        QT_MOC_LITERAL(196, 6),  // "column"
        QT_MOC_LITERAL(203, 11),  // "onFileFound"
        QT_MOC_LITERAL(215, 12),  // "columnNumber"
        QT_MOC_LITERAL(228, 8),  // "lineText"
        QT_MOC_LITERAL(237, 11),  // "matchedText"
        QT_MOC_LITERAL(249, 16),  // "onSearchProgress"
        QT_MOC_LITERAL(266, 11),  // "currentFile"
        QT_MOC_LITERAL(278, 13),  // "filesSearched"
        QT_MOC_LITERAL(292, 10),  // "totalFiles"
        QT_MOC_LITERAL(303, 16),  // "onSearchFinished"
        QT_MOC_LITERAL(320, 12),  // "totalMatches"
        QT_MOC_LITERAL(333, 13),  // "onSearchError"
        QT_MOC_LITERAL(347, 5),  // "error"
        QT_MOC_LITERAL(353, 14),  // "onClearResults"
        QT_MOC_LITERAL(368, 11),  // "onExpandAll"
        QT_MOC_LITERAL(380, 13)   // "onCollapseAll"
    },
    "NotepadPlusPlus::FindInFilesDialog",
    "openFileRequested",
    "",
    "filePath",
    "lineNumber",
    "show",
    "setSearchDirectory",
    "directory",
    "onFindClicked",
    "onStopClicked",
    "onBrowseClicked",
    "onResultDoubleClicked",
    "QTreeWidgetItem*",
    "item",
    "column",
    "onFileFound",
    "columnNumber",
    "lineText",
    "matchedText",
    "onSearchProgress",
    "currentFile",
    "filesSearched",
    "totalFiles",
    "onSearchFinished",
    "totalMatches",
    "onSearchError",
    "error",
    "onClearResults",
    "onExpandAll",
    "onCollapseAll"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_NotepadPlusPlus__FindInFilesDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   98,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    0,  103,    2, 0x0a,    4 /* Public */,
       6,    1,  104,    2, 0x0a,    5 /* Public */,
       8,    0,  107,    2, 0x08,    7 /* Private */,
       9,    0,  108,    2, 0x08,    8 /* Private */,
      10,    0,  109,    2, 0x08,    9 /* Private */,
      11,    2,  110,    2, 0x08,   10 /* Private */,
      15,    5,  115,    2, 0x08,   13 /* Private */,
      19,    3,  126,    2, 0x08,   19 /* Private */,
      23,    2,  133,    2, 0x08,   23 /* Private */,
      25,    1,  138,    2, 0x08,   26 /* Private */,
      27,    0,  141,    2, 0x08,   28 /* Private */,
      28,    0,  142,    2, 0x08,   29 /* Private */,
      29,    0,  143,    2, 0x08,   30 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12, QMetaType::Int,   13,   14,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::QString,    3,    4,   16,   17,   18,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,   20,   21,   22,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   24,   22,
    QMetaType::Void, QMetaType::QString,   26,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject NotepadPlusPlus::FindInFilesDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_NotepadPlusPlus__FindInFilesDialog.offsetsAndSizes,
    qt_meta_data_NotepadPlusPlus__FindInFilesDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_NotepadPlusPlus__FindInFilesDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<FindInFilesDialog, std::true_type>,
        // method 'openFileRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setSearchDirectory'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onFindClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStopClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onBrowseClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onResultDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onFileFound'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onSearchProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onSearchFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onSearchError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onClearResults'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExpandAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCollapseAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void NotepadPlusPlus::FindInFilesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FindInFilesDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->openFileRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->show(); break;
        case 2: _t->setSearchDirectory((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->onFindClicked(); break;
        case 4: _t->onStopClicked(); break;
        case 5: _t->onBrowseClicked(); break;
        case 6: _t->onResultDoubleClicked((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->onFileFound((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5]))); break;
        case 8: _t->onSearchProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 9: _t->onSearchFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 10: _t->onSearchError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onClearResults(); break;
        case 12: _t->onExpandAll(); break;
        case 13: _t->onCollapseAll(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FindInFilesDialog::*)(const QString & , int );
            if (_t _q_method = &FindInFilesDialog::openFileRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *NotepadPlusPlus::FindInFilesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NotepadPlusPlus::FindInFilesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NotepadPlusPlus__FindInFilesDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int NotepadPlusPlus::FindInFilesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void NotepadPlusPlus::FindInFilesDialog::openFileRequested(const QString & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
