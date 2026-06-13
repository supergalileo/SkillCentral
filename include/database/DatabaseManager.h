#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <QVector>

struct AgentInfo {
    int id;
    QString name;
    QString path;
    bool enabled;
    QString icon;
};

struct TagInfo {
    int id;
    QString name;
};

struct SkillInfo {
    int id;
    QString name;
    QString path;
    QString description;
    int frequency; // 1=常用, 2=一般, 3=很少, 4=废弃
    QDateTime createdAt;
    QDateTime updatedAt;
    QDateTime lastUsed;
    QStringList tags;
    QVector<int> enabledAgentIds;
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool openDatabase(const QString &dbPath);
    void closeDatabase();
    bool initializeDatabase();

    // Skills CRUD
    int addSkill(const QString &name, const QString &path, const QString &description);
    bool updateSkill(int id, const QVariantMap &fields);
    bool deleteSkill(int id);
    SkillInfo getSkill(int id);
    QVector<SkillInfo> getAllSkills(const QString &orderBy = "frequency");
    QVector<SkillInfo> searchSkills(const QString &keyword);
    QVector<SkillInfo> filterByTag(const QString &tag);

    // Tags CRUD
    int addTag(const QString &name);
    bool deleteTag(int id);
    bool renameTag(int id, const QString &newName);
    bool mergeTags(int sourceId, int targetId);
    QStringList getAllTags();
    QVector<TagInfo> getTags(); // 返回标签ID和名称

    // Skill-Tag relations
    bool addSkillTag(int skillId, int tagId);
    bool removeSkillTag(int skillId, int tagId);
    bool setSkillTags(int skillId, const QVector<int> &tagIds);

    // Agents CRUD
    int addAgent(const QString &name, const QString &path, bool enabled = true, const QString &icon = "");
    bool updateAgent(int id, const QVariantMap &fields);
    bool deleteAgent(int id);
    bool setAgentEnabled(int id, bool enabled);
    AgentInfo getAgent(int id);
    QVector<AgentInfo> getAllAgents();

    // Skill-Agent relations
    bool enableSkillForAgent(int skillId, int agentId);
    bool disableSkillForAgent(int skillId, int agentId);
    bool setSkillAgentEnabled(int skillId, int agentId, bool enabled);
    QVector<int> getSkillEnabledAgents(int skillId);
    QVector<int> getAgentEnabledSkills(int agentId);

    // Settings
    bool setSetting(const QString &key, const QString &value);
    QString getSetting(const QString &key, const QString &defaultValue = "");

    // Backup & Restore
    bool exportBackup(const QString &backupPath);
    bool importBackup(const QString &backupPath);

    // Validation
    QVector<int> validateSkills();

    // 清空所有标签
    bool clearAllTags();

    // Helper
    int getOrCreateTag(const QString &name);

signals:
    void databaseError(const QString &message);
    void skillChanged(int skillId);
    void agentChanged(int agentId);

private:
    QSqlDatabase m_db;
    bool m_initialized;
    QString m_connectionName;

    bool createTables();
    bool checkTableExists(const QString &tableName);
};

#endif // DATABASEMANAGER_H
