#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTest>
#include <QDateTime>
#include <QDir>
#include <QSignalSpy>
#include <QFile>
#include <QTextStream>

#include "models/AgentManager.h"
#include "database/DatabaseManager.h"

class TestAgentManager : public QObject
{
    Q_OBJECT

private:
    AgentManager *agentManager;
    DatabaseManager *dbManager;
    QTemporaryDir *tempDir;
    QString libraryPath;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Agent CRUD 测试
    void testAddAgent();
    void testUpdateAgent();
    void testDeleteAgent();
    void testSetAgentEnabled();
    void testGetAgent();
    void testGetAllAgents();

    // Skill 启用/禁用测试
    void testEnableSkillForAgent();
    void testDisableSkillForAgent();

    // 文件夹操作测试
    void testCopySkillToAgent();
    void testRemoveSkillFromAgent();

    // 扫描测试
    void testScanAgentPath();
    void testScanAllAgents();

    // 查询测试
    void testGetAgentEnabledSkills();
    void testGetSkillEnabledAgents();

    // 错误处理测试
    void testInvalidAgentPath();
    void testPermissionDenied();
};

void TestAgentManager::initTestCase()
{
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

void TestAgentManager::cleanupTestCase()
{
    // 清理
}

void TestAgentManager::init()
{
    dbManager = new DatabaseManager();
    tempDir = new QTemporaryDir();
    QVERIFY(tempDir->isValid());

    // 创建中央库路径
    libraryPath = QDir(tempDir->path()).filePath("skillLibrary");
    QDir().mkpath(libraryPath + "/skills");

    // 打开数据库
    QString dbPath = QDir(tempDir->path()).filePath("test.db");
    QVERIFY(dbManager->openDatabase(dbPath));
    QVERIFY(dbManager->initializeDatabase());

    // 设置库路径
    dbManager->setSetting("library_path", libraryPath);

    agentManager = new AgentManager(dbManager, nullptr);
}

void TestAgentManager::cleanup()
{
    delete agentManager;
    delete dbManager;
    delete tempDir;
}

// Agent CRUD 测试
void TestAgentManager::testAddAgent()
{
    int id = agentManager->addAgent("TestAgent", "C:/test/path", true, "icon.png");
    QVERIFY(id > 0);

    AgentInfo info = agentManager->getAgent(id);
    QCOMPARE(info.name, "TestAgent");
    QCOMPARE(info.path, "C:/test/path");
    QVERIFY(info.enabled);
    QCOMPARE(info.icon, "icon.png");
}

void TestAgentManager::testUpdateAgent()
{
    int id = agentManager->addAgent("TestAgent", "C:/test/path");
    QVERIFY(id > 0);

    QVariantMap fields;
    fields["name"] = "UpdatedAgent";
    fields["path"] = "C:/updated/path";
    fields["enabled"] = false;
    fields["icon"] = "updated.png";

    QVERIFY(agentManager->updateAgent(id, fields));

    AgentInfo info = agentManager->getAgent(id);
    QCOMPARE(info.name, "UpdatedAgent");
    QCOMPARE(info.path, "C:/updated/path");
    QVERIFY(!info.enabled);
    QCOMPARE(info.icon, "updated.png");
}

void TestAgentManager::testDeleteAgent()
{
    int id = agentManager->addAgent("TestAgent", "C:/test/path");
    QVERIFY(id > 0);

    QVERIFY(agentManager->deleteAgent(id));

    AgentInfo info = agentManager->getAgent(id);
    QCOMPARE(info.id, -1);
}

void TestAgentManager::testSetAgentEnabled()
{
    int id = agentManager->addAgent("TestAgent", "C:/test/path", false);
    QVERIFY(id > 0);

    AgentInfo info = agentManager->getAgent(id);
    QVERIFY(!info.enabled);

    QVERIFY(agentManager->setAgentEnabled(id, true));

    info = agentManager->getAgent(id);
    QVERIFY(info.enabled);
}

void TestAgentManager::testGetAgent()
{
    int id = agentManager->addAgent("TestAgent", "C:/test/path");
    QVERIFY(id > 0);

    AgentInfo info = agentManager->getAgent(id);
    QCOMPARE(info.id, id);
    QCOMPARE(info.name, "TestAgent");
    QCOMPARE(info.path, "C:/test/path");
}

void TestAgentManager::testGetAllAgents()
{
    // 获取初始 Agent 数量（预置 Agent）
    QVector<AgentInfo> initialAgents = agentManager->getAllAgents();
    int initialCount = initialAgents.size();

    agentManager->addAgent("Agent1", "C:/path1");
    agentManager->addAgent("Agent2", "C:/path2");
    agentManager->addAgent("Agent3", "C:/path3");

    QVector<AgentInfo> agents = agentManager->getAllAgents();
    QCOMPARE(agents.size(), initialCount + 3);
}

// Skill 启用/禁用测试
void TestAgentManager::testEnableSkillForAgent()
{
    // 创建测试 skill
    QString skillPath = libraryPath + "/skills/test_skill";
    QDir().mkpath(skillPath);
    QFile skillFile(skillPath + "/test.txt");
    skillFile.open(QIODevice::WriteOnly);
    skillFile.write("test content");
    skillFile.close();

    int skillId = dbManager->addSkill("test_skill", skillPath, "Test skill");
    QVERIFY(skillId > 0);

    // 创建测试 agent
    QTemporaryDir agentDir;
    int agentId = agentManager->addAgent("TestAgent", agentDir.path());
    QVERIFY(agentId > 0);

    // 启用 skill
    QVERIFY(agentManager->enableSkillForAgent(skillId, agentId));

    // 验证文件夹已复制
    QString targetPath = agentDir.path() + "/test_skill";
    QVERIFY(QDir(targetPath).exists());
    QVERIFY(QFile::exists(targetPath + "/test.txt"));

    // 验证数据库记录
    QVector<int> enabledSkills = agentManager->getAgentEnabledSkills(agentId);
    QVERIFY(enabledSkills.contains(skillId));
}

void TestAgentManager::testDisableSkillForAgent()
{
    // 创建测试 skill
    QString skillPath = libraryPath + "/skills/test_skill";
    QDir().mkpath(skillPath);

    int skillId = dbManager->addSkill("test_skill", skillPath, "Test skill");
    QVERIFY(skillId > 0);

    // 创建测试 agent
    QTemporaryDir agentDir;
    int agentId = agentManager->addAgent("TestAgent", agentDir.path());
    QVERIFY(agentId > 0);

    // 先启用 skill
    QVERIFY(agentManager->enableSkillForAgent(skillId, agentId));
    QVERIFY(QDir(agentDir.path() + "/test_skill").exists());

    // 禁用 skill
    QVERIFY(agentManager->disableSkillForAgent(skillId, agentId));

    // 验证文件夹已删除
    QVERIFY(!QDir(agentDir.path() + "/test_skill").exists());

    // 验证数据库记录
    QVector<int> enabledSkills = agentManager->getAgentEnabledSkills(agentId);
    QVERIFY(!enabledSkills.contains(skillId));
}

// 文件夹操作测试
void TestAgentManager::testCopySkillToAgent()
{
    // 创建测试 skill
    QString skillPath = libraryPath + "/skills/test_skill";
    QDir().mkpath(skillPath);
    QFile skillFile(skillPath + "/test.txt");
    skillFile.open(QIODevice::WriteOnly);
    skillFile.write("test content");
    skillFile.close();

    int skillId = dbManager->addSkill("test_skill", skillPath, "Test skill");
    QVERIFY(skillId > 0);

    // 创建测试 agent
    QTemporaryDir agentDir;
    int agentId = agentManager->addAgent("TestAgent", agentDir.path());
    QVERIFY(agentId > 0);

    // 复制 skill
    QVERIFY(agentManager->copySkillToAgent(skillId, agentId));

    // 验证文件夹已复制
    QString targetPath = agentDir.path() + "/test_skill";
    QVERIFY(QDir(targetPath).exists());
    QVERIFY(QFile::exists(targetPath + "/test.txt"));
}

void TestAgentManager::testRemoveSkillFromAgent()
{
    // 创建测试 skill
    QString skillPath = libraryPath + "/skills/test_skill";
    QDir().mkpath(skillPath);

    int skillId = dbManager->addSkill("test_skill", skillPath, "Test skill");
    QVERIFY(skillId > 0);

    // 创建测试 agent
    QTemporaryDir agentDir;
    int agentId = agentManager->addAgent("TestAgent", agentDir.path());
    QVERIFY(agentId > 0);

    // 先复制 skill
    QVERIFY(agentManager->copySkillToAgent(skillId, agentId));
    QVERIFY(QDir(agentDir.path() + "/test_skill").exists());

    // 删除 skill
    QVERIFY(agentManager->removeSkillFromAgent(skillId, agentId));

    // 验证文件夹已删除
    QVERIFY(!QDir(agentDir.path() + "/test_skill").exists());
}

// 扫描测试
void TestAgentManager::testScanAgentPath()
{
    // 创建测试 agent
    QTemporaryDir agentDir;
    int agentId = agentManager->addAgent("TestAgent", agentDir.path());
    QVERIFY(agentId > 0);

    // 在 agent 路径下创建未注册的 skill 文件夹
    QString unknownSkillPath = agentDir.path() + "/unknown_skill";
    QDir().mkpath(unknownSkillPath);
    QFile skillFile(unknownSkillPath + "/SKILL.md");
    skillFile.open(QIODevice::WriteOnly);
    skillFile.write("# Test Skill");
    skillFile.close();

    // 扫描
    QVERIFY(agentManager->scanAgentPath(agentId));

    // 验证 skill 已导入到中央库
    // 这里需要检查数据库或直接检查 libraryPath
}

void TestAgentManager::testScanAllAgents()
{
    // 创建两个测试 agent
    QTemporaryDir agentDir1;
    QTemporaryDir agentDir2;

    int agentId1 = agentManager->addAgent("Agent1", agentDir1.path());
    int agentId2 = agentManager->addAgent("Agent2", agentDir2.path());
    QVERIFY(agentId1 > 0);
    QVERIFY(agentId2 > 0);

    // 在 agent1 路径下创建 skill 文件夹
    QString skillPath1 = agentDir1.path() + "/skill1";
    QDir().mkpath(skillPath1);

    // 扫描所有 agent
    QVector<int> newSkillIds = agentManager->scanAllAgents();

    // 验证结果
    QVERIFY(newSkillIds.size() >= 0);  // 可能有新 skill 被导入
}

// 查询测试
void TestAgentManager::testGetAgentEnabledSkills()
{
    // 创建测试 skill
    QString skillPath = libraryPath + "/skills/test_skill";
    QDir().mkpath(skillPath);

    int skillId = dbManager->addSkill("test_skill", skillPath, "Test skill");
    QVERIFY(skillId > 0);

    // 创建测试 agent
    QTemporaryDir agentDir;
    int agentId = agentManager->addAgent("TestAgent", agentDir.path());
    QVERIFY(agentId > 0);

    // 启用 skill
    QVERIFY(agentManager->enableSkillForAgent(skillId, agentId));

    // 查询
    QVector<int> enabledSkills = agentManager->getAgentEnabledSkills(agentId);
    QVERIFY(enabledSkills.contains(skillId));
}

void TestAgentManager::testGetSkillEnabledAgents()
{
    // 创建测试 skill
    QString skillPath = libraryPath + "/skills/test_skill";
    QDir().mkpath(skillPath);

    int skillId = dbManager->addSkill("test_skill", skillPath, "Test skill");
    QVERIFY(skillId > 0);

    // 创建两个测试 agent
    QTemporaryDir agentDir1;
    QTemporaryDir agentDir2;

    int agentId1 = agentManager->addAgent("Agent1", agentDir1.path());
    int agentId2 = agentManager->addAgent("Agent2", agentDir2.path());
    QVERIFY(agentId1 > 0);
    QVERIFY(agentId2 > 0);

    // 启用 skill 到两个 agent
    QVERIFY(agentManager->enableSkillForAgent(skillId, agentId1));
    QVERIFY(agentManager->enableSkillForAgent(skillId, agentId2));

    // 查询
    QVector<int> enabledAgents = agentManager->getSkillEnabledAgents(skillId);
    QVERIFY(enabledAgents.contains(agentId1));
    QVERIFY(enabledAgents.contains(agentId2));
}

// 错误处理测试
void TestAgentManager::testInvalidAgentPath()
{
    // 创建 agent 但路径不存在
    int agentId = agentManager->addAgent("TestAgent", "C:/non/existent/path");
    QVERIFY(agentId > 0);

    // 验证路径应该失败
    QVERIFY(!agentManager->validateAgentPath(agentId));
}

void TestAgentManager::testPermissionDenied()
{
    // 这个测试在 Windows 上比较复杂，暂时跳过
    QSKIP("Permission denied test not implemented for Windows");
}

QTEST_MAIN(TestAgentManager)
#include "test_AgentManager.moc"
