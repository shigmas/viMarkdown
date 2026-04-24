/****************************************************************************
** Meta object code from reading C++ file 'MarkdownPreview.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/MarkdownPreview.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MarkdownPreview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.0. It"
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
struct qt_meta_tag_ZN15MarkdownPreviewE_t {};
} // unnamed namespace

template <> constexpr inline auto MarkdownPreview::qt_create_metaobjectdata<qt_meta_tag_ZN15MarkdownPreviewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MarkdownPreview",
        "lineClicked",
        "",
        "blockNumber",
        "checkboxLineClicked",
        "checked",
        "anchorClicked",
        "title",
        "anchor",
        "name",
        "readOnly",
        "textInserted",
        "textRemoved",
        "Enter_pressed",
        "Tab_pressed",
        "BS_pressed",
        "Del_pressed",
        "cut_triggered",
        "undo_triggered",
        "redo_triggered",
        "posContextChanged",
        "PosContext",
        "context",
        "acontext",
        "changeFontSize",
        "delta"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'lineClicked'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'checkboxLineClicked'
        QtMocHelpers::SignalData<void(bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'anchorClicked'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString, bool)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 8 }, { QMetaType::QString, 9 }, { QMetaType::Bool, 10 },
        }}),
        // Signal 'anchorClicked'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString)>(6, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 8 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'textInserted'
        QtMocHelpers::SignalData<void(QString)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 2 },
        }}),
        // Signal 'textRemoved'
        QtMocHelpers::SignalData<void(int)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 2 },
        }}),
        // Signal 'Enter_pressed'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'Tab_pressed'
        QtMocHelpers::SignalData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'BS_pressed'
        QtMocHelpers::SignalData<void(bool)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 2 },
        }}),
        // Signal 'Del_pressed'
        QtMocHelpers::SignalData<void(bool)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 2 },
        }}),
        // Signal 'cut_triggered'
        QtMocHelpers::SignalData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'undo_triggered'
        QtMocHelpers::SignalData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'redo_triggered'
        QtMocHelpers::SignalData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'posContextChanged'
        QtMocHelpers::SignalData<void(const PosContext &, const PosContext &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 21, 22 }, { 0x80000000 | 21, 23 },
        }}),
        // Signal 'changeFontSize'
        QtMocHelpers::SignalData<void(int)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 25 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MarkdownPreview, qt_meta_tag_ZN15MarkdownPreviewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MarkdownPreview::staticMetaObject = { {
    QMetaObject::SuperData::link<QTextEdit::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15MarkdownPreviewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15MarkdownPreviewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15MarkdownPreviewE_t>.metaTypes,
    nullptr
} };

void MarkdownPreview::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MarkdownPreview *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->lineClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->checkboxLineClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->anchorClicked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[4]))); break;
        case 3: _t->anchorClicked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 4: _t->textInserted((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->textRemoved((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->Enter_pressed(); break;
        case 7: _t->Tab_pressed(); break;
        case 8: _t->BS_pressed((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->Del_pressed((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->cut_triggered(); break;
        case 11: _t->undo_triggered(); break;
        case 12: _t->redo_triggered(); break;
        case 13: _t->posContextChanged((*reinterpret_cast<std::add_pointer_t<PosContext>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<PosContext>>(_a[2]))); break;
        case 14: _t->changeFontSize((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)(int )>(_a, &MarkdownPreview::lineClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)(bool )>(_a, &MarkdownPreview::checkboxLineClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)(const QString & , const QString & , const QString , bool )>(_a, &MarkdownPreview::anchorClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)(QString )>(_a, &MarkdownPreview::textInserted, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)(int )>(_a, &MarkdownPreview::textRemoved, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)()>(_a, &MarkdownPreview::Enter_pressed, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)()>(_a, &MarkdownPreview::Tab_pressed, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)(bool )>(_a, &MarkdownPreview::BS_pressed, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)(bool )>(_a, &MarkdownPreview::Del_pressed, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)()>(_a, &MarkdownPreview::cut_triggered, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)()>(_a, &MarkdownPreview::undo_triggered, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)()>(_a, &MarkdownPreview::redo_triggered, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)(const PosContext & , const PosContext & )>(_a, &MarkdownPreview::posContextChanged, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownPreview::*)(int )>(_a, &MarkdownPreview::changeFontSize, 14))
            return;
    }
}

const QMetaObject *MarkdownPreview::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MarkdownPreview::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15MarkdownPreviewE_t>.strings))
        return static_cast<void*>(this);
    return QTextEdit::qt_metacast(_clname);
}

int MarkdownPreview::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void MarkdownPreview::lineClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void MarkdownPreview::checkboxLineClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void MarkdownPreview::anchorClicked(const QString & _t1, const QString & _t2, const QString _t3, bool _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 4
void MarkdownPreview::textInserted(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void MarkdownPreview::textRemoved(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void MarkdownPreview::Enter_pressed()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void MarkdownPreview::Tab_pressed()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void MarkdownPreview::BS_pressed(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void MarkdownPreview::Del_pressed(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void MarkdownPreview::cut_triggered()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void MarkdownPreview::undo_triggered()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void MarkdownPreview::redo_triggered()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void MarkdownPreview::posContextChanged(const PosContext & _t1, const PosContext & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1, _t2);
}

// SIGNAL 14
void MarkdownPreview::changeFontSize(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1);
}
QT_WARNING_POP
