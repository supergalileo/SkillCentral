#ifndef SKILL_H
#define SKILL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVector>

class Skill : public QObject
{
    Q_OBJECT
public:
    explicit Skill(QObject *parent = nullptr);
    Skill(int id, const QString &name, const QString &path, QObject *parent = nullptr);

    int getId() const { return m_id; }
    void setId(int id) { m_id = id; }

    QString getName() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString getPath() const { return m_path; }
    void setPath(const QString &path) { m_path = path; }

    QString getDescription() const { return m_description; }
    void setDescription(const QString &description) { m_description = description; }

    int getFrequency() const { return m_frequency; }
    void setFrequency(int frequency) { m_frequency = frequency; }

    QDateTime getCreatedAt() const { return m_createdAt; }
    void setCreatedAt(const QDateTime &dt) { m_createdAt = dt; }

    QDateTime getUpdatedAt() const { return m_updatedAt; }
    void setUpdatedAt(const QDateTime &dt) { m_updatedAt = dt; }

    QDateTime getLastUsed() const { return m_lastUsed; }
    void setLastUsed(const QDateTime &dt) { m_lastUsed = dt; }

    QStringList getTags() const { return m_tags; }
    void setTags(const QStringList &tags) { m_tags = tags; }

    QVector<int> getEnabledAgentIds() const { return m_enabledAgentIds; }
    void setEnabledAgentIds(const QVector<int> &ids) { m_enabledAgentIds = ids; }

    bool isValid() const;
    QString getSkillMdPath() const;

private:
    int m_id;
    QString m_name;
    QString m_path;
    QString m_description;
    int m_frequency; // 1=常用, 2=一般, 3=很少, 4=废弃
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    QDateTime m_lastUsed;
    QStringList m_tags;
    QVector<int> m_enabledAgentIds;
};

#endif // SKILL_H
