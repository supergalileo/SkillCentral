/****************************************************************************
** Meta object code from reading C++ file 'SkillManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/models/SkillManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SkillManager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12SkillManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto SkillManager::qt_create_metaobjectdata<qt_meta_tag_ZN12SkillManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SkillManager",
        "importProgress",
        "",
        "current",
        "total",
        "message",
        "exportProgress",
        "skillImported",
        "skillId",
        "skillDeleted",
        "skillValidated",
        "valid",
        "errorOccurred"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'importProgress'
        QtMocHelpers::SignalData<void(int, int, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'exportProgress'
        QtMocHelpers::SignalData<void(int, int, const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'skillImported'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'skillDeleted'
        QtMocHelpers::SignalData<void(int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'skillValidated'
        QtMocHelpers::SignalData<void(int, bool)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::Bool, 11 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SkillManager, qt_meta_tag_ZN12SkillManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SkillManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12SkillManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12SkillManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12SkillManagerE_t>.metaTypes,
    nullptr
} };

void SkillManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SkillManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->importProgress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->exportProgress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 2: _t->skillImported((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->skillDeleted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->skillValidated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 5: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SkillManager::*)(int , int , const QString & )>(_a, &SkillManager::importProgress, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SkillManager::*)(int , int , const QString & )>(_a, &SkillManager::exportProgress, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SkillManager::*)(int )>(_a, &SkillManager::skillImported, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SkillManager::*)(int )>(_a, &SkillManager::skillDeleted, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SkillManager::*)(int , bool )>(_a, &SkillManager::skillValidated, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SkillManager::*)(const QString & )>(_a, &SkillManager::errorOccurred, 5))
            return;
    }
}

const QMetaObject *SkillManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SkillManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12SkillManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SkillManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void SkillManager::importProgress(int _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void SkillManager::exportProgress(int _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3);
}

// SIGNAL 2
void SkillManager::skillImported(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void SkillManager::skillDeleted(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void SkillManager::skillValidated(int _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void SkillManager::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
