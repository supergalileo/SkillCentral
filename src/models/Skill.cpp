#include "models/Skill.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>

Skill::Skill(QObject *parent)
    : QObject(parent)
    , m_id(-1)
    , m_frequency(2)
{
}

Skill::Skill(int id, const QString &name, const QString &path, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_name(name)
    , m_path(path)
    , m_frequency(2)
{
}

bool Skill::isValid() const
{
    // Check if name is not empty
    if (m_name.isEmpty()) {
        return false;
    }

    // Check if path exists
    QFileInfo pathInfo(m_path);
    if (!pathInfo.exists()) {
        return false;
    }

    // Check if it's a directory
    if (!pathInfo.isDir()) {
        return false;
    }

    return true;
}

QString Skill::getSkillMdPath() const
{
    // Return the full path to SKILL.md
    return QDir::cleanPath(m_path + "/SKILL.md");
}
