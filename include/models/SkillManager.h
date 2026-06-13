#ifndef SKILLMANAGER_H
#define SKILLMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include "Skill.h"
#include "../database/DatabaseManager.h"

class SkillManager : public QObject
{
    Q_OBJECT
public:
    explicit SkillManager(DatabaseManager *dbManager, QObject *parent = nullptr);
    ~SkillManager();

    // Import skills
    int importFromFolder(const QString &folderPath, bool allowOverwrite = false);
    int importFromWorkBuddy(const QString &workbuddyPath);
    int importFromGitHub(const QString &repoUrl);
    int importFromZip(const QString &zipPath);
    int importByDragDrop(const QStringList &paths);

    // Export skills
    bool exportToZip(int skillId, const QString &outputPath);
    bool exportMultipleToZip(const QVector<int> &skillIds, const QString &outputPath);

    // Skill operations
    bool validateSkill(int skillId);
    QVector<int> validateAllSkills();
    bool fixSkill(int skillId, const QString &newSkillMdPath);
    bool deleteSkill(int skillId, bool deleteFromAgents = true);

    // Auto-tagging
    QStringList autoDetectTags(int skillId);
    bool autoTagSkill(int skillId);

    // Enable/Disable for agents
    bool enableSkillForAgent(int skillId, int agentId);
    bool disableSkillForAgent(int skillId, int agentId);

    // Parsing SKILL.md
    static QString parseDescription(const QString &skillMdPath);
    static QStringList parseTriggers(const QString &skillMdPath);
    static QString parseName(const QString &skillMdPath);
    static QStringList parseTags(const QString &skillMdPath);  // 新增：从 SKILL.md 解析标签

    // 文件名安全化（去除 Windows 非法字符）
    static QString sanitizeFolderName(const QString &name);

    // 重复检测（供 UI 层在导入前调用）
    bool checkDuplicate(const QString &name);

signals:
    void importProgress(int current, int total, const QString &message);
    void exportProgress(int current, int total, const QString &message);
    void skillImported(int skillId);
    void skillDeleted(int skillId);
    void skillValidated(int skillId, bool valid);
    void errorOccurred(const QString &message);

private:
    DatabaseManager *m_dbManager;
    QString m_libraryPath;

    bool copySkillFolder(const QString &sourcePath, const QString &skillName);
    QString generateUniqueName(const QString &baseName);
    int handleDuplicate(const QString &name);
};

#endif // SKILLMANAGER_H
