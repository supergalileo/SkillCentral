#include "models/AgentManager.h"
#include "database/DatabaseManager.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

AgentManager::AgentManager(DatabaseManager *dbManager, QObject *parent)
    : QObject(parent), m_dbManager(dbManager)
{
    if (m_dbManager) {
        m_libraryPath = m_dbManager->getSetting("library_path", "D:/skillLibrary");
    }
}

int AgentManager::addAgent(const QString &name, const QString &path, bool enabled, const QString &icon)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        emit errorOccurred("DatabaseManager is null");
        return -1;
    }

    qInfo() << "Adding agent:" << name << "path:" << path;
    int id = m_dbManager->addAgent(name, path, enabled, icon);
    if (id > 0) {
        emit agentChanged(id);
    }
    return id;
}

bool AgentManager::updateAgent(int id, const QVariantMap &fields)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        emit errorOccurred("DatabaseManager is null");
        return false;
    }

    qInfo() << "Updating agent:" << id;
    bool result = m_dbManager->updateAgent(id, fields);
    if (result) {
        emit agentChanged(id);
    }
    return result;
}

bool AgentManager::deleteAgent(int id)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        emit errorOccurred("DatabaseManager is null");
        return false;
    }

    qInfo() << "Deleting agent:" << id;
    bool result = m_dbManager->deleteAgent(id);
    if (result) {
        emit agentChanged(id);
    }
    return result;
}

AgentInfo AgentManager::getAgent(int id)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        AgentInfo info;
        info.id = -1;
        return info;
    }

    return m_dbManager->getAgent(id);
}

QVector<AgentInfo> AgentManager::getAllAgents()
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        return QVector<AgentInfo>();
    }

    return m_dbManager->getAllAgents();
}

bool AgentManager::setAgentEnabled(int id, bool enabled)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        emit errorOccurred("DatabaseManager is null");
        return false;
    }

    qInfo() << "Setting agent" << id << "enabled:" << enabled;
    bool result = m_dbManager->setAgentEnabled(id, enabled);
    if (result) {
        emit agentChanged(id);
    }
    return result;
}

bool AgentManager::enableSkillForAgent(int skillId, int agentId)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        emit errorOccurred("DatabaseManager is null");
        return false;
    }

    qInfo() << "Enabling skill" << skillId << "for agent" << agentId;

    // 复制 skill 文件夹到 agent 路径
    if (!copySkillToAgent(skillId, agentId)) {
        emit errorOccurred("Failed to copy skill to agent");
        return false;
    }

    // 更新数据库
    if (!m_dbManager->enableSkillForAgent(skillId, agentId)) {
        emit errorOccurred("Failed to update database");
        return false;
    }

    emit skillEnabled(skillId, agentId);
    return true;
}

bool AgentManager::disableSkillForAgent(int skillId, int agentId)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        emit errorOccurred("DatabaseManager is null");
        return false;
    }

    qInfo() << "Disabling skill" << skillId << "for agent" << agentId;

    // 从 agent 路径删除 skill 文件夹
    if (!removeSkillFromAgent(skillId, agentId)) {
        emit errorOccurred("Failed to remove skill from agent");
        return false;
    }

    // 更新数据库
    if (!m_dbManager->disableSkillForAgent(skillId, agentId)) {
        emit errorOccurred("Failed to update database");
        return false;
    }

    emit skillDisabled(skillId, agentId);
    return true;
}

bool AgentManager::copySkillToAgent(int skillId, int agentId)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        return false;
    }

    // 获取 skill 信息
    SkillInfo skillInfo = m_dbManager->getSkill(skillId);
    if (skillInfo.id == -1) {
        qWarning() << "Skill not found:" << skillId;
        return false;
    }

    // 获取 agent 信息
    AgentInfo agentInfo = m_dbManager->getAgent(agentId);
    if (agentInfo.id == -1) {
        qWarning() << "Agent not found:" << agentId;
        return false;
    }

    if (agentInfo.path.isEmpty() || !QDir(agentInfo.path).exists()) {
        qWarning() << "Agent path is invalid:" << agentInfo.path;
        return false;
    }

    // 构建源路径和目标路径
    QString sourcePath = skillInfo.path;
    QString targetPath = getSkillTargetPath(skillId, agentId);

    qInfo() << "Copying skill from" << sourcePath << "to" << targetPath;

    // 如果目标已存在，先删除
    QDir targetDir(targetPath);
    if (targetDir.exists()) {
        qInfo() << "Target path already exists, removing:" << targetPath;
        if (!targetDir.removeRecursively()) {
            qWarning() << "Failed to remove existing target path";
            return false;
        }
    }

    // 复制文件夹
    if (!copyDirectory(sourcePath, targetPath)) {
        qWarning() << "Failed to copy directory";
        return false;
    }

    qInfo() << "Successfully copied skill to agent";
    return true;
}

bool AgentManager::removeSkillFromAgent(int skillId, int agentId)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        return false;
    }

    QString targetPath = getSkillTargetPath(skillId, agentId);

    if (targetPath.isEmpty()) {
        qWarning() << "Failed to get target path";
        return false;
    }

    QDir targetDir(targetPath);
    if (!targetDir.exists()) {
        qWarning() << "Target path does not exist:" << targetPath;
        return true;  // 已经不存在，视为成功
    }

    qInfo() << "Removing skill from agent:" << targetPath;

    if (!targetDir.removeRecursively()) {
        qWarning() << "Failed to remove directory:" << targetPath;
        return false;
    }

    qInfo() << "Successfully removed skill from agent";
    return true;
}

bool AgentManager::scanAgentPath(int agentId)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        return false;
    }

    qInfo() << "Scanning agent path for agent:" << agentId;

    // 获取 agent 信息
    AgentInfo agentInfo = m_dbManager->getAgent(agentId);
    if (agentInfo.id == -1) {
        qWarning() << "Agent not found:" << agentId;
        emit errorOccurred("Agent not found");
        return false;
    }

    if (!validateAgentPath(agentId)) {
        emit errorOccurred("Agent path is invalid");
        return false;
    }

    QDir agentDir(agentInfo.path);
    QStringList subDirs = agentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    int current = 0;
    int total = subDirs.size();

    emit scanProgress(current, total, "Starting scan...");

    for (const QString &subDir : subDirs) {
        current++;
        QString subDirPath = agentDir.filePath(subDir);

        emit scanProgress(current, total, QString("Scanning: %1").arg(subDir));

        qInfo() << "Found directory:" << subDir;

        // TODO: 检查是否在中 央库中存在
        // TODO: 如果不存在，调用 SkillManager::importFromFolder 导入
        // TODO: 如果在中央库存在但 skill_agents 表中没有记录，添加记录
    }

    emit scanProgress(total, total, "Scan completed");
    qInfo() << "Scan completed for agent:" << agentId;
    return true;
}

QVector<int> AgentManager::scanAllAgents()
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        return QVector<int>();
    }

    qInfo() << "Scanning all agents";

    QVector<AgentInfo> agents = m_dbManager->getAllAgents();
    QVector<int> newSkillIds;

    for (const AgentInfo &agent : agents) {
        if (!agent.enabled) {
            qInfo() << "Skipping disabled agent:" << agent.name;
            continue;
        }

        qInfo() << "Scanning agent:" << agent.name;
        if (scanAgentPath(agent.id)) {
            // TODO: 收集新导入的 skill ID
        }
    }

    qInfo() << "Total new skills imported:" << newSkillIds.size();
    return newSkillIds;
}

QVector<int> AgentManager::getAgentEnabledSkills(int agentId)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        return QVector<int>();
    }

    return m_dbManager->getAgentEnabledSkills(agentId);
}

QVector<int> AgentManager::getSkillEnabledAgents(int skillId)
{
    if (!m_dbManager) {
        qWarning() << "DatabaseManager is null";
        return QVector<int>();
    }

    return m_dbManager->getSkillEnabledAgents(skillId);
}

bool AgentManager::validateAgentPath(int agentId)
{
    if (!m_dbManager) {
        return false;
    }

    AgentInfo agentInfo = m_dbManager->getAgent(agentId);
    if (agentInfo.id == -1) {
        return false;
    }

    QDir agentDir(agentInfo.path);
    bool exists = agentDir.exists();

    if (!exists) {
        qWarning() << "Agent path does not exist:" << agentInfo.path;
    }

    return exists;
}

QString AgentManager::getSkillTargetPath(int skillId, int agentId)
{
    if (!m_dbManager) {
        return QString();
    }

    // 获取 skill 信息
    SkillInfo skillInfo = m_dbManager->getSkill(skillId);
    if (skillInfo.id == -1) {
        qWarning() << "Skill not found:" << skillId;
        return QString();
    }

    // 获取 agent 信息
    AgentInfo agentInfo = m_dbManager->getAgent(agentId);
    if (agentInfo.id == -1) {
        qWarning() << "Agent not found:" << agentId;
        return QString();
    }

    if (agentInfo.path.isEmpty()) {
        qWarning() << "Agent path is empty";
        return QString();
    }

    // 构建目标路径：agentPath + "/" + skillName
    QString targetPath = QDir(agentInfo.path).filePath(skillInfo.name);

    qInfo() << "Target path for skill" << skillInfo.name << ":" << targetPath;
    return targetPath;
}

bool AgentManager::copyDirectory(const QString &sourcePath, const QString &targetPath)
{
    QDir sourceDir(sourcePath);
    if (!sourceDir.exists()) {
        qWarning() << "Source directory does not exist:" << sourcePath;
        return false;
    }

    QDir targetDir(targetPath);
    if (!targetDir.exists()) {
        if (!targetDir.mkpath(".")) {
            qWarning() << "Failed to create target directory:" << targetPath;
            return false;
        }
    }

    // 复制所有文件
    QStringList files = sourceDir.entryList(QDir::Files);
    for (const QString &file : files) {
        QString sourceFile = sourceDir.filePath(file);
        QString targetFile = targetDir.filePath(file);

        if (QFile::exists(targetFile)) {
            if (!QFile::remove(targetFile)) {
                qWarning() << "Failed to remove existing file:" << targetFile;
                return false;
            }
        }

        if (!QFile::copy(sourceFile, targetFile)) {
            qWarning() << "Failed to copy file:" << sourceFile << "to" << targetFile;
            return false;
        }
    }

    // 递归复制子目录
    QStringList subDirs = sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &subDir : subDirs) {
        QString newSourcePath = sourceDir.filePath(subDir);
        QString newTargetPath = targetDir.filePath(subDir);

        if (!copyDirectory(newSourcePath, newTargetPath)) {
            qWarning() << "Failed to copy subdirectory:" << subDir;
            return false;
        }
    }

    return true;
}
