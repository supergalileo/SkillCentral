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
    int importFromFolder(const QString &folderPath);
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
    bool checkDuplicate(const QString &name);
    int handleDuplicate(const QString &name);
};

#endif // SKILLMANAGER_H
