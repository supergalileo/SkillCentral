#ifndef AGENTMANAGER_H
#define AGENTMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVector>

class DatabaseManager;

struct AgentInfo;

class AgentManager : public QObject
{
    Q_OBJECT
public:
    explicit AgentManager(DatabaseManager *dbManager, QObject *parent = nullptr);

    // Agent CRUD（委托给 DatabaseManager，加日志）
    int addAgent(const QString &name, const QString &path, bool enabled = true, const QString &icon = "");
    bool updateAgent(int id, const QVariantMap &fields);
    bool deleteAgent(int id);
    AgentInfo getAgent(int id);
    QVector<AgentInfo> getAllAgents();
    bool setAgentEnabled(int id, bool enabled);

    // Skill 启用/禁用（核心功能）
    bool enableSkillForAgent(int skillId, int agentId);
    bool disableSkillForAgent(int skillId, int agentId);

    // 文件夹操作
    bool copySkillToAgent(int skillId, int agentId);
    bool removeSkillFromAgent(int skillId, int agentId);

    // 启动时扫描
    bool scanAgentPath(int agentId);
    QVector<int> scanAllAgents();

    // 查询
    QVector<int> getAgentEnabledSkills(int agentId);
    QVector<int> getSkillEnabledAgents(int skillId);

    // 验证方法（供测试使用）
    bool validateAgentPath(int agentId);
    QString getSkillTargetPath(int skillId, int agentId);

signals:
    void agentChanged(int agentId);
    void skillEnabled(int skillId, int agentId);
    void skillDisabled(int skillId, int agentId);
    void scanProgress(int current, int total, const QString &message);
    void errorOccurred(const QString &message);

private:
    DatabaseManager *m_dbManager;
    QString m_libraryPath;

    bool copyDirectory(const QString &sourcePath, const QString &targetPath);
};

#endif // AGENTMANAGER_H
