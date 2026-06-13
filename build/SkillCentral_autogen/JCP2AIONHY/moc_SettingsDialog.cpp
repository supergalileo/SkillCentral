/****************************************************************************
** Meta object code from reading C++ file 'SettingsDialog.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/ui/SettingsDialog.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SettingsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
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
struct qt_meta_tag_ZN14SettingsDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto SettingsDialog::qt_create_metaobjectdata<qt_meta_tag_ZN14SettingsDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SettingsDialog",
        "settingsChanged",
        "",
        "agentsChanged",
        "libraryPathChanged",
        "newPath",
        "themeChanged",
        "theme",
        "onBrowseLibraryPath",
        "onResetLibraryPath",
        "onThemeChanged",
        "index",
        "onAddAgent",
        "onEditAgent",
        "onToggleAgent",
        "onDeleteAgent",
        "onAgentSelectionChanged",
        "onAgentContextMenu",
        "QPoint",
        "pos",
        "onBackupNow",
        "onRestoreBackup",
        "onBackupSelectionChanged",
        "onCheckUpdate"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'settingsChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'agentsChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'libraryPathChanged'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'themeChanged'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onBrowseLibraryPath'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onResetLibraryPath'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onThemeChanged'
        QtMocHelpers::SlotData<void(int)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 11 },
        }}),
        // Slot 'onAddAgent'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEditAgent'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleAgent'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteAgent'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAgentSelectionChanged'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAgentContextMenu'
        QtMocHelpers::SlotData<void(const QPoint &)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Slot 'onBackupNow'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRestoreBackup'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBackupSelectionChanged'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCheckUpdate'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SettingsDialog, qt_meta_tag_ZN14SettingsDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SettingsDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14SettingsDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14SettingsDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14SettingsDialogE_t>.metaTypes,
    nullptr
} };

void SettingsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SettingsDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->settingsChanged(); break;
        case 1: _t->agentsChanged(); break;
        case 2: _t->libraryPathChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->themeChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onBrowseLibraryPath(); break;
        case 5: _t->onResetLibraryPath(); break;
        case 6: _t->onThemeChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->onAddAgent(); break;
        case 8: _t->onEditAgent(); break;
        case 9: _t->onToggleAgent(); break;
        case 10: _t->onDeleteAgent(); break;
        case 11: _t->onAgentSelectionChanged(); break;
        case 12: _t->onAgentContextMenu((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 13: _t->onBackupNow(); break;
        case 14: _t->onRestoreBackup(); break;
        case 15: _t->onBackupSelectionChanged(); break;
        case 16: _t->onCheckUpdate(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SettingsDialog::*)()>(_a, &SettingsDialog::settingsChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsDialog::*)()>(_a, &SettingsDialog::agentsChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsDialog::*)(const QString & )>(_a, &SettingsDialog::libraryPathChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SettingsDialog::*)(const QString & )>(_a, &SettingsDialog::themeChanged, 3))
            return;
    }
}

const QMetaObject *SettingsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14SettingsDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int SettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void SettingsDialog::settingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SettingsDialog::agentsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SettingsDialog::libraryPathChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void SettingsDialog::themeChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
