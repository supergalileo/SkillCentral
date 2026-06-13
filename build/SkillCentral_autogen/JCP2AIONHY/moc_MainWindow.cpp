/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/ui/MainWindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "skillCardClicked",
        "",
        "skillId",
        "skillSelectionChanged",
        "count",
        "requestSettings",
        "requestAddSkill",
        "requestBackup",
        "openSkillSidebar",
        "onSearchTextChanged",
        "text",
        "onTagClicked",
        "tag",
        "onSelectAllClicked",
        "onBatchEnableClicked",
        "onBatchDisableClicked",
        "onBatchDeleteClicked",
        "onBatchTagClicked",
        "onToggleCardSize",
        "onSortChanged",
        "index",
        "onCardClicked",
        "onSelectionChanged",
        "onFrequencyChanged",
        "newFrequency",
        "onAgentToggled",
        "agentId",
        "enabled",
        "onRequestSettings",
        "onManageTagsClicked",
        "onAddSkillClicked",
        "onBackupClicked",
        "onFrequencyFilterClicked",
        "QPushButton*",
        "btn",
        "onAgentFilterClicked",
        "onEditSkillRequested",
        "onDeleteSkillRequested",
        "onExportSkillRequested",
        "onSidebarClosed"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'skillCardClicked'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'skillSelectionChanged'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Signal 'requestSettings'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestAddSkill'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestBackup'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'openSkillSidebar'
        QtMocHelpers::SlotData<void(int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'onSearchTextChanged'
        QtMocHelpers::SlotData<void(const QString &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'onTagClicked'
        QtMocHelpers::SlotData<void(const QString &)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'onSelectAllClicked'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBatchEnableClicked'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBatchDisableClicked'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBatchDeleteClicked'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBatchTagClicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleCardSize'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSortChanged'
        QtMocHelpers::SlotData<void(int)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 21 },
        }}),
        // Slot 'onCardClicked'
        QtMocHelpers::SlotData<void(int)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'onSelectionChanged'
        QtMocHelpers::SlotData<void(int)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Slot 'onFrequencyChanged'
        QtMocHelpers::SlotData<void(int, int)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 25 },
        }}),
        // Slot 'onAgentToggled'
        QtMocHelpers::SlotData<void(int, int, bool)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 27 }, { QMetaType::Bool, 28 },
        }}),
        // Slot 'onRequestSettings'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onManageTagsClicked'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddSkillClicked'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBackupClicked'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFrequencyFilterClicked'
        QtMocHelpers::SlotData<void(QPushButton *)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 34, 35 },
        }}),
        // Slot 'onAgentFilterClicked'
        QtMocHelpers::SlotData<void(QPushButton *)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 34, 35 },
        }}),
        // Slot 'onEditSkillRequested'
        QtMocHelpers::SlotData<void(int)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'onDeleteSkillRequested'
        QtMocHelpers::SlotData<void(int)>(38, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'onExportSkillRequested'
        QtMocHelpers::SlotData<void(int)>(39, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'onSidebarClosed'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->skillCardClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->skillSelectionChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->requestSettings(); break;
        case 3: _t->requestAddSkill(); break;
        case 4: _t->requestBackup(); break;
        case 5: _t->openSkillSidebar((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onSearchTextChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onTagClicked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onSelectAllClicked(); break;
        case 9: _t->onBatchEnableClicked(); break;
        case 10: _t->onBatchDisableClicked(); break;
        case 11: _t->onBatchDeleteClicked(); break;
        case 12: _t->onBatchTagClicked(); break;
        case 13: _t->onToggleCardSize(); break;
        case 14: _t->onSortChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->onCardClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->onSelectionChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->onFrequencyChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 18: _t->onAgentToggled((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        case 19: _t->onRequestSettings(); break;
        case 20: _t->onManageTagsClicked(); break;
        case 21: _t->onAddSkillClicked(); break;
        case 22: _t->onBackupClicked(); break;
        case 23: _t->onFrequencyFilterClicked((*reinterpret_cast<std::add_pointer_t<QPushButton*>>(_a[1]))); break;
        case 24: _t->onAgentFilterClicked((*reinterpret_cast<std::add_pointer_t<QPushButton*>>(_a[1]))); break;
        case 25: _t->onEditSkillRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 26: _t->onDeleteSkillRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 27: _t->onExportSkillRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 28: _t->onSidebarClosed(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 23:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QPushButton* >(); break;
            }
            break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QPushButton* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(int )>(_a, &MainWindow::skillCardClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(int )>(_a, &MainWindow::skillSelectionChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::requestSettings, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::requestAddSkill, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::requestBackup, 4))
            return;
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::skillCardClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void MainWindow::skillSelectionChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void MainWindow::requestSettings()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void MainWindow::requestAddSkill()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MainWindow::requestBackup()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
