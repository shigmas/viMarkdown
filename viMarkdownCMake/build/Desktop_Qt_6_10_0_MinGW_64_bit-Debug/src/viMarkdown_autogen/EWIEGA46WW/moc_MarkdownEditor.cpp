/****************************************************************************
** Meta object code from reading C++ file 'MarkdownEditor.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/MarkdownEditor.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MarkdownEditor.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14MarkdownEditorE_t {};
} // unnamed namespace

template <> constexpr inline auto MarkdownEditor::qt_create_metaobjectdata<qt_meta_tag_ZN14MarkdownEditorE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MarkdownEditor",
        "tab_pressed",
        "",
        "esc_pressed",
        "link_clicked",
        "title",
        "readOnly",
        "changeFontSize",
        "delta",
        "posContextChanged",
        "PosContext",
        "context",
        "acontext"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'tab_pressed'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'esc_pressed'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'link_clicked'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &, bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::QString, 2 }, { QMetaType::QString, 2 }, { QMetaType::Bool, 6 },
        }}),
        // Signal 'link_clicked'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &)>(4, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::QString, 2 }, { QMetaType::QString, 2 },
        }}),
        // Signal 'changeFontSize'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'posContextChanged'
        QtMocHelpers::SignalData<void(const PosContext &, const PosContext &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { 0x80000000 | 10, 12 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MarkdownEditor, qt_meta_tag_ZN14MarkdownEditorE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MarkdownEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<QPlainTextEdit::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14MarkdownEditorE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14MarkdownEditorE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14MarkdownEditorE_t>.metaTypes,
    nullptr
} };

void MarkdownEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MarkdownEditor *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->tab_pressed(); break;
        case 1: _t->esc_pressed(); break;
        case 2: _t->link_clicked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[4]))); break;
        case 3: _t->link_clicked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 4: _t->changeFontSize((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->posContextChanged((*reinterpret_cast<std::add_pointer_t<PosContext>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<PosContext>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MarkdownEditor::*)()>(_a, &MarkdownEditor::tab_pressed, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownEditor::*)()>(_a, &MarkdownEditor::esc_pressed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownEditor::*)(const QString & , const QString & , const QString & , bool )>(_a, &MarkdownEditor::link_clicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownEditor::*)(int )>(_a, &MarkdownEditor::changeFontSize, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MarkdownEditor::*)(const PosContext & , const PosContext & )>(_a, &MarkdownEditor::posContextChanged, 5))
            return;
    }
}

const QMetaObject *MarkdownEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MarkdownEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14MarkdownEditorE_t>.strings))
        return static_cast<void*>(this);
    return QPlainTextEdit::qt_metacast(_clname);
}

int MarkdownEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void MarkdownEditor::tab_pressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MarkdownEditor::esc_pressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MarkdownEditor::link_clicked(const QString & _t1, const QString & _t2, const QString & _t3, bool _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 4
void MarkdownEditor::changeFontSize(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void MarkdownEditor::posContextChanged(const PosContext & _t1, const PosContext & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}
QT_WARNING_POP
