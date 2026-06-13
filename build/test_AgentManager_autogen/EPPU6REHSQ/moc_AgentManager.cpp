/****************************************************************************
** Meta object code from reading C++ file 'AgentManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/models/AgentManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AgentManager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12AgentManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto AgentManager::qt_create_metaobjectdata<qt_meta_tag_ZN12AgentManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AgentManager",
        "agentChanged",
        "",
        "agentId",
        "skillEnabled",
        "skillId",
        "skillDisabled",
        "scanProgress",
        "current",
        "total",
        "message",
        "errorOccurred"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'agentChanged'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'skillEnabled'
        QtMocHelpers::SignalData<void(int, int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::Int, 3 },
        }}),
        // Signal 'skillDisabled'
        QtMocHelpers::SignalData<void(int, int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::Int, 3 },
        }}),
        // Signal 'scanProgress'
        QtMocHelpers::SignalData<void(int, int, const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::Int, 9 }, { QMetaType::QString, 10 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AgentManager, qt_meta_tag_ZN12AgentManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AgentManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AgentManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AgentManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12AgentManagerE_t>.metaTypes,
    nullptr
} };

void AgentManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AgentManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->agentChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->skillEnabled((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->skillDisabled((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->scanProgress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 4: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AgentManager::*)(int )>(_a, &AgentManager::agentChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AgentManager::*)(int , int )>(_a, &AgentManager::skillEnabled, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AgentManager::*)(int , int )>(_a, &AgentManager::skillDisabled, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AgentManager::*)(int , int , const QString & )>(_a, &AgentManager::scanProgress, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AgentManager::*)(const QString & )>(_a, &AgentManager::errorOccurred, 4))
            return;
    }
}

const QMetaObject *AgentManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AgentManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AgentManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AgentManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void AgentManager::agentChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void AgentManager::skillEnabled(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void AgentManager::skillDisabled(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void AgentManager::scanProgress(int _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3);
}

// SIGNAL 4
void AgentManager::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
