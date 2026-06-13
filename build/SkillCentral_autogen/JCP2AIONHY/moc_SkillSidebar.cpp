/****************************************************************************
** Meta object code from reading C++ file 'SkillSidebar.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/ui/SkillSidebar.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SkillSidebar.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12SkillSidebarE_t {};
} // unnamed namespace

template <> constexpr inline auto SkillSidebar::qt_create_metaobjectdata<qt_meta_tag_ZN12SkillSidebarE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SkillSidebar",
        "editRequested",
        "",
        "skillId",
        "deleteRequested",
        "exportRequested",
        "closed",
        "onAnimationFinished",
        "onEditClicked",
        "onDeleteClicked",
        "onExportClicked",
        "onCloseClicked",
        "sidebarWidth"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'editRequested'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'deleteRequested'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'exportRequested'
        QtMocHelpers::SignalData<void(int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'closed'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onAnimationFinished'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEditClicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteClicked'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportClicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCloseClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'sidebarWidth'
        QtMocHelpers::PropertyData<int>(12, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SkillSidebar, qt_meta_tag_ZN12SkillSidebarE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SkillSidebar::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12SkillSidebarE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12SkillSidebarE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12SkillSidebarE_t>.metaTypes,
    nullptr
} };

void SkillSidebar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SkillSidebar *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->editRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->deleteRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->exportRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->closed(); break;
        case 4: _t->onAnimationFinished(); break;
        case 5: _t->onEditClicked(); break;
        case 6: _t->onDeleteClicked(); break;
        case 7: _t->onExportClicked(); break;
        case 8: _t->onCloseClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SkillSidebar::*)(int )>(_a, &SkillSidebar::editRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SkillSidebar::*)(int )>(_a, &SkillSidebar::deleteRequested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SkillSidebar::*)(int )>(_a, &SkillSidebar::exportRequested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SkillSidebar::*)()>(_a, &SkillSidebar::closed, 3))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->sidebarWidth(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setSidebarWidth(*reinterpret_cast<int*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *SkillSidebar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SkillSidebar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12SkillSidebarE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SkillSidebar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void SkillSidebar::editRequested(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void SkillSidebar::deleteRequested(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void SkillSidebar::exportRequested(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void SkillSidebar::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
