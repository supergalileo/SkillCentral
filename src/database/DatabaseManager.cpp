#include "database/DatabaseManager.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>
#include <QUuid>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), m_initialized(false)
{
    // 生成唯一的连接名
    m_connectionName = "SkillCentral_" + QUuid::createUuid().toString(QUuid::WithoutBraces);
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

bool DatabaseManager::openDatabase(const QString &dbPath)
{
    closeDatabase();

    QString nativePath = QDir::toNativeSeparators(dbPath);
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_db.setDatabaseName(nativePath);

    if (!m_db.open()) {
        emit databaseError("Failed to open database: " + m_db.lastError().text());
        return false;
    }

    return true;
}

void DatabaseManager::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    // 总是尝试移除连接
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase::removeDatabase(m_connectionName);
    }
    m_initialized = false;
}

bool DatabaseManager::initializeDatabase()
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    if (!createTables()) {
        return false;
    }

    // 插入预置 Agent 数据（path 留空，由用户手动配置）
    QVector<QPair<QString, QString>> presetAgents = {
        {"WorkBuddy", ""},
        {"Claude Code", ""},
        {"Codex", ""},
        {"MimoCode", ""},
        {"Trae", ""},
        {"OpenCode", ""}
    };

    for (const auto &agent : presetAgents) {
        QSqlQuery query(m_db);
        query.prepare("SELECT id FROM agents WHERE name = ?");
        query.addBindValue(agent.first);

        if (!query.exec()) {
            emit databaseError("Failed to check agent: " + query.lastError().text());
            continue;
        }

        if (!query.next()) {
            // 预设 agent 默认禁用，避免自动扫描系统内置 skills
            query.prepare("INSERT INTO agents (name, path, enabled) VALUES (?, ?, 0)");
            query.addBindValue(agent.first);
            query.addBindValue(agent.second);

            if (!query.exec()) {
                emit databaseError("Failed to insert preset agent: " + query.lastError().text());
            }
        } else {
            // 已存在的预设 agent：强制禁用，防止扫描系统内置 skills
            int agentId = query.value(0).toInt();
            QSqlQuery updateQuery(m_db);
            updateQuery.prepare("UPDATE agents SET enabled = 0 WHERE id = ?");
            updateQuery.addBindValue(agentId);
            if (!updateQuery.exec()) {
                emit databaseError("Failed to disable preset agent: " + updateQuery.lastError().text());
            }
        }
    }

    m_initialized = true;
    return true;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_db);

    // 创建 skills 表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS skills (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE,
            path TEXT,
            description TEXT,
            frequency INTEGER DEFAULT 1,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            last_used DATETIME
        )
    )")) {
        emit databaseError("Failed to create skills table: " + query.lastError().text());
        return false;
    }

    // 创建 tags 表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS tags (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE
        )
    )")) {
        emit databaseError("Failed to create tags table: " + query.lastError().text());
        return false;
    }

    // 创建 skill_tags 表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS skill_tags (
            skill_id INTEGER,
            tag_id INTEGER,
            PRIMARY KEY (skill_id, tag_id),
            FOREIGN KEY (skill_id) REFERENCES skills(id) ON DELETE CASCADE,
            FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE
        )
    )")) {
        emit databaseError("Failed to create skill_tags table: " + query.lastError().text());
        return false;
    }

    // 创建 agents 表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS agents (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE,
            path TEXT,
            enabled BOOLEAN DEFAULT 1,
            icon TEXT
        )
    )")) {
        emit databaseError("Failed to create agents table: " + query.lastError().text());
        return false;
    }

    // 创建 skill_agents 表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS skill_agents (
            skill_id INTEGER,
            agent_id INTEGER,
            enabled BOOLEAN DEFAULT 0,
            PRIMARY KEY (skill_id, agent_id),
            FOREIGN KEY (skill_id) REFERENCES skills(id) ON DELETE CASCADE,
            FOREIGN KEY (agent_id) REFERENCES agents(id) ON DELETE CASCADE
        )
    )")) {
        emit databaseError("Failed to create skill_agents table: " + query.lastError().text());
        return false;
    }

    // 创建 settings 表
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS settings (
            key TEXT PRIMARY KEY,
            value TEXT
        )
    )")) {
        emit databaseError("Failed to create settings table: " + query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseManager::checkTableExists(const QString &tableName)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=?");
    query.addBindValue(tableName);

    if (!query.exec()) {
        emit databaseError("Failed to check table existence: " + query.lastError().text());
        return false;
    }

    return query.next();
}

int DatabaseManager::addSkill(const QString &name, const QString &path, const QString &description)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return -1;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO skills (name, path, description, created_at, updated_at)
        VALUES (?, ?, ?, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
    )");
    query.addBindValue(name);
    query.addBindValue(path);  // 不转换为本地分隔符，保持原始路径格式
    query.addBindValue(description);

    if (!query.exec()) {
        emit databaseError("Failed to add skill: " + query.lastError().text());
        return -1;
    }

    int id = query.lastInsertId().toInt();
    emit skillChanged(id);
    return id;
}

bool DatabaseManager::updateSkill(int id, const QVariantMap &fields)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QStringList setClauses;
    QVariantList values;

    for (auto it = fields.begin(); it != fields.end(); ++it) {
        setClauses << it.key() + " = ?";
        values << it.value();
    }

    if (setClauses.isEmpty()) {
        return true;
    }

    setClauses << "updated_at = CURRENT_TIMESTAMP";

    QString sql = "UPDATE skills SET " + setClauses.join(", ") + " WHERE id = ?";
    values << id;

    QSqlQuery query(m_db);
    query.prepare(sql);

    for (const QVariant &value : values) {
        query.addBindValue(value);
    }

    if (!query.exec()) {
        emit databaseError("Failed to update skill: " + query.lastError().text());
        return false;
    }

    emit skillChanged(id);
    return true;
}

bool DatabaseManager::deleteSkill(int id)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM skills WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        emit databaseError("Failed to delete skill: " + query.lastError().text());
        return false;
    }

    emit skillChanged(id);
    return true;
}

SkillInfo DatabaseManager::getSkill(int id)
{
    SkillInfo info;
    info.id = -1;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return info;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM skills WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        emit databaseError("Failed to get skill: " + query.lastError().text());
        return info;
    }

    if (query.next()) {
        info.id = query.value("id").toInt();
        info.name = query.value("name").toString();
        info.path = query.value("path").toString();
        info.description = query.value("description").toString();
        info.frequency = query.value("frequency").toInt();
        info.createdAt = query.value("created_at").toDateTime();
        info.updatedAt = query.value("updated_at").toDateTime();
        info.lastUsed = query.value("last_used").toDateTime();

        // 获取 tags
        QSqlQuery tagQuery(m_db);
        tagQuery.prepare(R"(
            SELECT t.name FROM tags t
            JOIN skill_tags st ON t.id = st.tag_id
            WHERE st.skill_id = ?
        )");
        tagQuery.addBindValue(id);

        if (tagQuery.exec()) {
            while (tagQuery.next()) {
                info.tags << tagQuery.value(0).toString();
            }
        }

        // 获取 enabled agent IDs
        QSqlQuery agentQuery(m_db);
        agentQuery.prepare("SELECT agent_id FROM skill_agents WHERE skill_id = ? AND enabled = 1");
        agentQuery.addBindValue(id);

        if (agentQuery.exec()) {
            while (agentQuery.next()) {
                info.enabledAgentIds << agentQuery.value(0).toInt();
            }
        }
    }

    return info;
}

QVector<SkillInfo> DatabaseManager::getAllSkills(const QString &orderBy)
{
    QVector<SkillInfo> skills;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return skills;
    }

    QString validOrderBy = "frequency";
    QStringList validColumns = {"frequency", "name", "created_at", "updated_at", "last_used"};
    if (validColumns.contains(orderBy)) {
        validOrderBy = orderBy;
    }

    QSqlQuery query(m_db);
    QString sql = QString("SELECT id FROM skills ORDER BY %1").arg(validOrderBy);
    if (!query.exec(sql)) {
        emit databaseError("Failed to get all skills: " + query.lastError().text());
        return skills;
    }

    while (query.next()) {
        skills << getSkill(query.value(0).toInt());
    }

    return skills;
}

QVector<SkillInfo> DatabaseManager::searchSkills(const QString &keyword)
{
    QVector<SkillInfo> skills;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return skills;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT id FROM skills
        WHERE name LIKE ? OR description LIKE ? OR path LIKE ?
        ORDER BY frequency
    )");
    QString pattern = "%" + keyword + "%";
    query.addBindValue(pattern);
    query.addBindValue(pattern);
    query.addBindValue(pattern);

    if (!query.exec()) {
        emit databaseError("Failed to search skills: " + query.lastError().text());
        return skills;
    }

    while (query.next()) {
        skills << getSkill(query.value(0).toInt());
    }

    return skills;
}

QVector<SkillInfo> DatabaseManager::filterByTag(const QString &tag)
{
    QVector<SkillInfo> skills;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return skills;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT s.id FROM skills s
        JOIN skill_tags st ON s.id = st.skill_id
        JOIN tags t ON st.tag_id = t.id
        WHERE t.name = ?
        ORDER BY s.frequency
    )");
    query.addBindValue(tag);

    if (!query.exec()) {
        emit databaseError("Failed to filter by tag: " + query.lastError().text());
        return skills;
    }

    while (query.next()) {
        skills << getSkill(query.value(0).toInt());
    }

    return skills;
}

int DatabaseManager::addTag(const QString &name)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return -1;
    }

    // 先检查是否已存在
    int existingId = getOrCreateTag(name);
    return existingId;
}

bool DatabaseManager::deleteTag(int id)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM tags WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        emit databaseError("Failed to delete tag: " + query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseManager::renameTag(int id, const QString &newName)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("UPDATE tags SET name = ? WHERE id = ?");
    query.addBindValue(newName);
    query.addBindValue(id);

    if (!query.exec()) {
        emit databaseError("Failed to rename tag: " + query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseManager::mergeTags(int sourceId, int targetId)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    // 更新 skill_tags 表中的引用
    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE OR IGNORE skill_tags SET tag_id = ? WHERE tag_id = ?
    )");
    query.addBindValue(targetId);
    query.addBindValue(sourceId);

    if (!query.exec()) {
        emit databaseError("Failed to merge tags: " + query.lastError().text());
        return false;
    }

    // 删除源标签
    return deleteTag(sourceId);
}

QStringList DatabaseManager::getAllTags()
{
    QStringList tags;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return tags;
    }

    QSqlQuery query(m_db);
    if (!query.exec("SELECT name FROM tags ORDER BY name")) {
        emit databaseError("Failed to get all tags: " + query.lastError().text());
        return tags;
    }

    while (query.next()) {
        tags << query.value(0).toString();
    }

    return tags;
}

QVector<TagInfo> DatabaseManager::getTags()
{
    QVector<TagInfo> tags;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return tags;
    }

    QSqlQuery query(m_db);
    if (!query.exec("SELECT id, name FROM tags ORDER BY name")) {
        emit databaseError("Failed to get tags: " + query.lastError().text());
        return tags;
    }

    while (query.next()) {
        TagInfo info;
        info.id = query.value(0).toInt();
        info.name = query.value(1).toString();
        tags << info;
    }

    return tags;
}

bool DatabaseManager::addSkillTag(int skillId, int tagId)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT OR IGNORE INTO skill_tags (skill_id, tag_id) VALUES (?, ?)");
    query.addBindValue(skillId);
    query.addBindValue(tagId);

    if (!query.exec()) {
        emit databaseError("Failed to add skill tag: " + query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseManager::removeSkillTag(int skillId, int tagId)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM skill_tags WHERE skill_id = ? AND tag_id = ?");
    query.addBindValue(skillId);
    query.addBindValue(tagId);

    if (!query.exec()) {
        emit databaseError("Failed to remove skill tag: " + query.lastError().text());
        return false;
    }

    return true;
}

bool DatabaseManager::setSkillTags(int skillId, const QVector<int> &tagIds)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    // 开始事务
    if (!m_db.transaction()) {
        emit databaseError("Failed to begin transaction");
        return false;
    }

    // 删除现有的标签关联
    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("DELETE FROM skill_tags WHERE skill_id = ?");
    deleteQuery.addBindValue(skillId);

    if (!deleteQuery.exec()) {
        m_db.rollback();
        emit databaseError("Failed to clear skill tags: " + deleteQuery.lastError().text());
        return false;
    }

    // 插入新的标签关联
    for (int tagId : tagIds) {
        QSqlQuery insertQuery(m_db);
        insertQuery.prepare("INSERT INTO skill_tags (skill_id, tag_id) VALUES (?, ?)");
        insertQuery.addBindValue(skillId);
        insertQuery.addBindValue(tagId);

        if (!insertQuery.exec()) {
            m_db.rollback();
            emit databaseError("Failed to set skill tag: " + insertQuery.lastError().text());
            return false;
        }
    }

    if (!m_db.commit()) {
        emit databaseError("Failed to commit transaction");
        return false;
    }

    return true;
}

int DatabaseManager::addAgent(const QString &name, const QString &path, bool enabled, const QString &icon)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return -1;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO agents (name, path, enabled, icon)
        VALUES (?, ?, ?, ?)
    )");
    query.addBindValue(name);
    query.addBindValue(path);  // 不转换为本地分隔符
    query.addBindValue(enabled ? 1 : 0);
    query.addBindValue(icon);

    if (!query.exec()) {
        emit databaseError("Failed to add agent: " + query.lastError().text());
        return -1;
    }

    int id = query.lastInsertId().toInt();
    emit agentChanged(id);
    return id;
}

bool DatabaseManager::updateAgent(int id, const QVariantMap &fields)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QStringList setClauses;
    QVariantList values;

    for (auto it = fields.begin(); it != fields.end(); ++it) {
        setClauses << it.key() + " = ?";
        values << it.value();
    }

    if (setClauses.isEmpty()) {
        return true;
    }

    QString sql = "UPDATE agents SET " + setClauses.join(", ") + " WHERE id = ?";
    values << id;

    QSqlQuery query(m_db);
    query.prepare(sql);

    for (const QVariant &value : values) {
        query.addBindValue(value);
    }

    if (!query.exec()) {
        emit databaseError("Failed to update agent: " + query.lastError().text());
        return false;
    }

    emit agentChanged(id);
    return true;
}

bool DatabaseManager::deleteAgent(int id)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM agents WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        emit databaseError("Failed to delete agent: " + query.lastError().text());
        return false;
    }

    emit agentChanged(id);
    return true;
}

bool DatabaseManager::setAgentEnabled(int id, bool enabled)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("UPDATE agents SET enabled = ? WHERE id = ?");
    query.addBindValue(enabled ? 1 : 0);
    query.addBindValue(id);

    if (!query.exec()) {
        emit databaseError("Failed to set agent enabled: " + query.lastError().text());
        return false;
    }

    emit agentChanged(id);
    return true;
}

AgentInfo DatabaseManager::getAgent(int id)
{
    AgentInfo info;
    info.id = -1;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return info;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM agents WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        emit databaseError("Failed to get agent: " + query.lastError().text());
        return info;
    }

    if (query.next()) {
        info.id = query.value("id").toInt();
        info.name = query.value("name").toString();
        info.path = query.value("path").toString();
        info.enabled = query.value("enabled").toBool();
        info.icon = query.value("icon").toString();
    }

    return info;
}

QVector<AgentInfo> DatabaseManager::getAllAgents()
{
    QVector<AgentInfo> agents;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return agents;
    }

    QSqlQuery query(m_db);
    if (!query.exec("SELECT * FROM agents ORDER BY name")) {
        emit databaseError("Failed to get all agents: " + query.lastError().text());
        return agents;
    }

    while (query.next()) {
        AgentInfo info;
        info.id = query.value("id").toInt();
        info.name = query.value("name").toString();
        info.path = query.value("path").toString();
        info.enabled = query.value("enabled").toBool();
        info.icon = query.value("icon").toString();
        agents << info;
    }

    return agents;
}

bool DatabaseManager::enableSkillForAgent(int skillId, int agentId)
{
    return setSkillAgentEnabled(skillId, agentId, true);
}

bool DatabaseManager::disableSkillForAgent(int skillId, int agentId)
{
    return setSkillAgentEnabled(skillId, agentId, false);
}

bool DatabaseManager::setSkillAgentEnabled(int skillId, int agentId, bool enabled)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QSqlQuery query(m_db);

    // 先尝试更新
    query.prepare(R"(
        UPDATE skill_agents SET enabled = ?
        WHERE skill_id = ? AND agent_id = ?
    )");
    query.addBindValue(enabled ? 1 : 0);
    query.addBindValue(skillId);
    query.addBindValue(agentId);

    if (!query.exec()) {
        emit databaseError("Failed to update skill agent: " + query.lastError().text());
        return false;
    }

    // 如果没有更新任何行，则插入新行
    if (query.numRowsAffected() == 0) {
        query.prepare(R"(
            INSERT INTO skill_agents (skill_id, agent_id, enabled)
            VALUES (?, ?, ?)
        )");
        query.addBindValue(skillId);
        query.addBindValue(agentId);
        query.addBindValue(enabled ? 1 : 0);

        if (!query.exec()) {
            emit databaseError("Failed to insert skill agent: " + query.lastError().text());
            return false;
        }
    }

    return true;
}

QVector<int> DatabaseManager::getSkillEnabledAgents(int skillId)
{
    QVector<int> agentIds;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return agentIds;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT agent_id FROM skill_agents WHERE skill_id = ? AND enabled = 1");
    query.addBindValue(skillId);

    if (!query.exec()) {
        emit databaseError("Failed to get skill enabled agents: " + query.lastError().text());
        return agentIds;
    }

    while (query.next()) {
        agentIds << query.value(0).toInt();
    }

    return agentIds;
}

QVector<int> DatabaseManager::getAgentEnabledSkills(int agentId)
{
    QVector<int> skillIds;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return skillIds;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT skill_id FROM skill_agents WHERE agent_id = ? AND enabled = 1");
    query.addBindValue(agentId);

    if (!query.exec()) {
        emit databaseError("Failed to get agent enabled skills: " + query.lastError().text());
        return skillIds;
    }

    while (query.next()) {
        skillIds << query.value(0).toInt();
    }

    return skillIds;
}

bool DatabaseManager::setSetting(const QString &key, const QString &value)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?)
    )");
    query.addBindValue(key);
    query.addBindValue(value);

    if (!query.exec()) {
        emit databaseError("Failed to set setting: " + query.lastError().text());
        return false;
    }

    return true;
}

QString DatabaseManager::getSetting(const QString &key, const QString &defaultValue)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return defaultValue;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT value FROM settings WHERE key = ?");
    query.addBindValue(key);

    if (!query.exec()) {
        emit databaseError("Failed to get setting: " + query.lastError().text());
        return defaultValue;
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    return defaultValue;
}

bool DatabaseManager::exportBackup(const QString &backupPath)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QFile file(backupPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit databaseError("Failed to open backup file: " + file.errorString());
        return false;
    }

    QTextStream out(&file);

    // 导出 schema
    QSqlQuery query(m_db);
    if (query.exec("SELECT sql FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%'")) {
        while (query.next()) {
            out << query.value(0).toString() << ";\n";
        }
    }

    out << "\n";

    // 导出数据
    QStringList tables = {"skills", "tags", "skill_tags", "agents", "skill_agents", "settings"};
    for (const QString &table : tables) {
        if (query.exec(QString("SELECT * FROM %1").arg(table))) {
            QSqlRecord rec = query.record();
            while (query.next()) {
                out << "INSERT INTO " << table << " VALUES (";
                QStringList values;
                for (int i = 0; i < rec.count(); ++i) {
                    QVariant val = query.value(i);
                    if (val.isNull()) {
                        values << "NULL";
                    } else if (val.typeId() == QMetaType::QString) {
                        values << "'" + val.toString().replace("'", "''") + "'";
                    } else {
                        values << val.toString();
                    }
                }
                out << values.join(", ") << ");\n";
            }
        }
    }

    file.close();
    return true;
}

bool DatabaseManager::importBackup(const QString &backupPath)
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    QFile file(backupPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit databaseError("Failed to open backup file: " + file.errorString());
        return false;
    }

    QTextStream in(&file);
    QString sql = in.readAll();
    file.close();

    // 执行 SQL 语句
    QStringList statements = sql.split(";", Qt::SkipEmptyParts);
    for (const QString &statement : statements) {
        QString trimmed = statement.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }

        QSqlQuery query(m_db);
        if (!query.exec(trimmed)) {
            emit databaseError("Failed to import backup: " + query.lastError().text());
            return false;
        }
    }

    return true;
}

QVector<int> DatabaseManager::validateSkills()
{
    QVector<int> invalidIds;

    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return invalidIds;
    }

    QSqlQuery query(m_db);
    if (!query.exec("SELECT id, path FROM skills")) {
        emit databaseError("Failed to validate skills: " + query.lastError().text());
        return invalidIds;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString path = query.value(1).toString();

        if (!QFile::exists(path)) {
            invalidIds << id;
        }
    }

    return invalidIds;
}

int DatabaseManager::getOrCreateTag(const QString &name)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM tags WHERE name = ?");
    query.addBindValue(name);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    query.prepare("INSERT INTO tags (name) VALUES (?)");
    query.addBindValue(name);

    if (!query.exec()) {
        emit databaseError("Failed to create tag: " + query.lastError().text());
        return -1;
    }

    return query.lastInsertId().toInt();
}

bool DatabaseManager::clearAllTags()
{
    if (!m_db.isOpen()) {
        emit databaseError("Database not open");
        return false;
    }

    if (!m_db.transaction()) {
        emit databaseError("Failed to begin transaction");
        return false;
    }

    QSqlQuery q1(m_db);
    if (!q1.exec("DELETE FROM skill_tags")) {
        m_db.rollback();
        emit databaseError("Failed to clear skill_tags: " + q1.lastError().text());
        return false;
    }

    QSqlQuery q2(m_db);
    if (!q2.exec("DELETE FROM tags")) {
        m_db.rollback();
        emit databaseError("Failed to clear tags: " + q2.lastError().text());
        return false;
    }

    if (!m_db.commit()) {
        emit databaseError("Failed to commit transaction");
        return false;
    }

    return true;
}
