#include <QTest>
#include <QObject>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTemporaryDir>
#include <QSignalSpy>
#include "models/Skill.h"
#include "models/SkillManager.h"
#include "database/DatabaseManager.h"

class TestSkillManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Skill class tests
    void testSkillConstructor();
    void testSkillGettersSetters();
    void testSkillIsValid();
    void testSkillGetSkillMdPath();

    // SkillManager tests
    void testImportFromFolder();
    void testImportFromFolderInvalidPath();
    void testImportFromFolderNoSkillMd();
    void testExportToZip();
    void testValidateSkill();
    void testValidateAllSkills();
    void testAutoDetectTags();
    void testParseDescription();
    void testParseTriggers();
    void testParseName();
    void testEnableDisableSkillForAgent();

private:
    DatabaseManager *m_dbManager;
    SkillManager *m_skillManager;
    QString m_testDbPath;
    QString m_libraryPath;

    void createTestSkillFolder(const QString &path, const QString &name, const QString &content);
};

void TestSkillManager::initTestCase()
{
    qDebug() << "=== initTestCase started ===";

    // Set up test database
    m_testDbPath = QDir::temp().absoluteFilePath("test_skillcentral.db");
    m_libraryPath = QDir::temp().absoluteFilePath("test_skilllibrary");

    qDebug() << "Test DB path:" << m_testDbPath;
    qDebug() << "Library path:" << m_libraryPath;

    // Clean up any previous test data
    QFile::remove(m_testDbPath);
    QDir libDir(m_libraryPath);
    if (libDir.exists()) {
        libDir.removeRecursively();
    }
    libDir.mkpath(".");

    // Initialize database
    m_dbManager = new DatabaseManager();
    bool dbOpened = m_dbManager->openDatabase(m_testDbPath);
    qDebug() << "Database opened:" << dbOpened;

    bool dbInitialized = m_dbManager->initializeDatabase();
    qDebug() << "Database initialized:" << dbInitialized;

    // Initialize skill manager
    m_skillManager = new SkillManager(m_dbManager);

    qDebug() << "=== initTestCase completed ===";
}

void TestSkillManager::cleanupTestCase()
{
    delete m_skillManager;
    delete m_dbManager;

    // Clean up test data
    QFile::remove(m_testDbPath);
    QDir libDir(m_libraryPath);
    if (libDir.exists()) {
        libDir.removeRecursively();
    }
}

void TestSkillManager::init()
{
    // Clean up skills table before each test
    QSqlQuery query;
    query.exec("DELETE FROM skills");
    query.exec("DELETE FROM tags");
    query.exec("DELETE FROM skill_tags");
    query.exec("DELETE FROM agents");
    query.exec("DELETE FROM skill_agents");
}

void TestSkillManager::cleanup()
{
    // Clean up after each test
}

void TestSkillManager::createTestSkillFolder(const QString &path, const QString &name, const QString &content)
{
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString skillMdPath = QDir::cleanPath(path + "/SKILL.md");
    QFile file(skillMdPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << content;
        file.close();
    }
}

// Skill class tests
void TestSkillManager::testSkillConstructor()
{
    // Create a file to prove the test is running
    QFile proofFile("D:/workbuddy/SkillCentral/build/test_proof.txt");
    if (proofFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&proofFile);
        out << "testSkillConstructor is running at " << QDateTime::currentDateTime().toString() << "\n";
        proofFile.close();
    }

    QWARN("testSkillConstructor is running");

    Skill skill;
    QCOMPARE(skill.getId(), -1);
    QCOMPARE(skill.getName(), QString());
    QCOMPARE(skill.getPath(), QString());
    QCOMPARE(skill.getFrequency(), 2);
    QVERIFY(skill.getTags().isEmpty());
    QVERIFY(skill.getEnabledAgentIds().isEmpty());
}

void TestSkillManager::testSkillGettersSetters()
{
    Skill skill;

    skill.setId(1);
    QCOMPARE(skill.getId(), 1);

    skill.setName("Test Skill");
    QCOMPARE(skill.getName(), QString("Test Skill"));

    skill.setPath("C:/test/path");
    QCOMPARE(skill.getPath(), QString("C:/test/path"));

    skill.setDescription("Test description");
    QCOMPARE(skill.getDescription(), QString("Test description"));

    skill.setFrequency(1);
    QCOMPARE(skill.getFrequency(), 1);

    QDateTime now = QDateTime::currentDateTime();
    skill.setCreatedAt(now);
    QCOMPARE(skill.getCreatedAt(), now);

    skill.setUpdatedAt(now);
    QCOMPARE(skill.getUpdatedAt(), now);

    skill.setLastUsed(now);
    QCOMPARE(skill.getLastUsed(), now);

    QStringList tags = {"tag1", "tag2"};
    skill.setTags(tags);
    QCOMPARE(skill.getTags(), tags);

    QVector<int> agentIds = {1, 2, 3};
    skill.setEnabledAgentIds(agentIds);
    QCOMPARE(skill.getEnabledAgentIds(), agentIds);
}

void TestSkillManager::testSkillIsValid()
{
    Skill skill;

    // Invalid: empty name and path
    QVERIFY(!skill.isValid());

    // Invalid: name set but path doesn't exist
    skill.setName("Test");
    skill.setPath("C:/nonexistent/path");
    QVERIFY(!skill.isValid());

    // Valid: name set and path exists
    QTemporaryDir tempDir;
    skill.setPath(tempDir.path());
    QVERIFY(skill.isValid());

    // Invalid: path is a file, not directory
    QString tempFilePath = tempDir.filePath("test.txt");
    QFile testFile(tempFilePath);
    Q_UNUSED(testFile.open(QIODevice::WriteOnly));
    testFile.close();
    skill.setPath(tempFilePath);
    QVERIFY(!skill.isValid());
}

void TestSkillManager::testSkillGetSkillMdPath()
{
    Skill skill;
    skill.setPath("C:/skills/my_skill");
    QCOMPARE(skill.getSkillMdPath(), QString("C:/skills/my_skill/SKILL.md"));

    skill.setPath("C:/skills/my_skill/");
    QCOMPARE(skill.getSkillMdPath(), QString("C:/skills/my_skill/SKILL.md"));
}

// SkillManager tests
void TestSkillManager::testImportFromFolder()
{
    // Create a test skill folder
    QTemporaryDir tempDir;
    QString skillContent = "# Test Skill\n\nThis is a test skill.\n\n## Triggers\n\n- trigger1\n- trigger2\n";
    createTestSkillFolder(tempDir.path(), "Test Skill", skillContent);

    // Import the skill
    int skillId = m_skillManager->importFromFolder(tempDir.path());

    QVERIFY(skillId != -1);
    QVERIFY(skillId > 0);
}

void TestSkillManager::testImportFromFolderInvalidPath()
{
    // Test with non-existent path
    int skillId = m_skillManager->importFromFolder("C:/nonexistent/path");

    QCOMPARE(skillId, -1);
}

void TestSkillManager::testImportFromFolderNoSkillMd()
{
    // Create a folder without SKILL.md
    QTemporaryDir tempDir;
    QDir dir(tempDir.path());
    QFile file(dir.filePath("test.txt"));
    Q_UNUSED(file.open(QIODevice::WriteOnly));
    file.close();

    // Try to import
    int skillId = m_skillManager->importFromFolder(tempDir.path());

    QCOMPARE(skillId, -1);
}

void TestSkillManager::testExportToZip()
{
    // TODO: Implement when QZipWriter is available
    QSKIP("ZIP export not implemented yet");
}

void TestSkillManager::testValidateSkill()
{
    // Create a test skill
    QTemporaryDir tempDir;
    QString skillContent = "# Test Skill\n\nThis is a test skill.\n";
    createTestSkillFolder(tempDir.path(), "Test Skill", skillContent);

    int skillId = m_skillManager->importFromFolder(tempDir.path());
    QVERIFY(skillId != -1);

    // Validate the skill
    bool valid = m_skillManager->validateSkill(skillId);

    QVERIFY(valid);
}

void TestSkillManager::testValidateAllSkills()
{
    // Create test skills
    QTemporaryDir tempDir1;
    QTemporaryDir tempDir2;

    QString skillContent1 = "# Skill 1\n\nThis is skill 1.\n";
    QString skillContent2 = "# Skill 2\n\nThis is skill 2.\n";

    createTestSkillFolder(tempDir1.path(), "Skill 1", skillContent1);
    createTestSkillFolder(tempDir2.path(), "Skill 2", skillContent2);

    int skillId1 = m_skillManager->importFromFolder(tempDir1.path());
    int skillId2 = m_skillManager->importFromFolder(tempDir2.path());

    QVERIFY(skillId1 != -1);
    QVERIFY(skillId2 != -1);

    // Validate all skills
    QVector<int> invalidSkills = m_skillManager->validateAllSkills();

    QVERIFY(invalidSkills.isEmpty());
}

void TestSkillManager::testAutoDetectTags()
{
    // Create a test skill with rich content
    QTemporaryDir tempDir;
    QString skillContent = "# Test Skill\n\nThis is a test skill for testing auto-detect tags.\n\n## Triggers\n\n- test trigger\n- auto detect\n\n## Features\n\nThis skill provides testing functionality.\n";
    createTestSkillFolder(tempDir.path(), "Test Skill", skillContent);

    int skillId = m_skillManager->importFromFolder(tempDir.path());
    QVERIFY(skillId != -1);

    // Auto-detect tags
    QStringList tags = m_skillManager->autoDetectTags(skillId);

    // Should detect headings and triggers
    QVERIFY(tags.size() > 0);
}

void TestSkillManager::testParseDescription()
{
    // Create a test SKILL.md
    QTemporaryDir tempDir;
    QString skillContent = "# Test Skill\n\nThis is the description of the test skill. It provides testing functionality.\n\n## Usage\n\nUse this skill for testing.\n";
    createTestSkillFolder(tempDir.path(), "Test Skill", skillContent);

    QString skillMdPath = QDir::cleanPath(tempDir.path() + "/SKILL.md");

    // Parse description
    QString description = SkillManager::parseDescription(skillMdPath);

    QVERIFY(!description.isEmpty());
    QVERIFY(description.contains("description"));
    QVERIFY(description.length() <= 200);
}

void TestSkillManager::testParseTriggers()
{
    // Create a test SKILL.md with triggers
    QTemporaryDir tempDir;
    QString skillContent = "# Test Skill\n\n## 触发词\n\n- test skill\n- testing\n- auto trigger\n\n## Description\n\nThis is a test.\n";
    createTestSkillFolder(tempDir.path(), "Test Skill", skillContent);

    QString skillMdPath = QDir::cleanPath(tempDir.path() + "/SKILL.md");

    // Parse triggers
    QStringList triggers = SkillManager::parseTriggers(skillMdPath);

    QVERIFY(triggers.size() > 0);
}

void TestSkillManager::testParseName()
{
    // Create a test SKILL.md with h1 heading
    QTemporaryDir tempDir;
    QString skillContent = "# My Test Skill\n\nThis is a test skill.\n";
    createTestSkillFolder(tempDir.path(), "My Test Skill", skillContent);

    QString skillMdPath = QDir::cleanPath(tempDir.path() + "/SKILL.md");

    // Parse name
    QString name = SkillManager::parseName(skillMdPath);

    QCOMPARE(name, QString("My Test Skill"));
}

void TestSkillManager::testEnableDisableSkillForAgent()
{
    // Create a test skill
    QTemporaryDir tempDir;
    QString skillContent = "# Test Skill\n\nThis is a test skill.\n";
    createTestSkillFolder(tempDir.path(), "Test Skill", skillContent);

    int skillId = m_skillManager->importFromFolder(tempDir.path());
    QVERIFY(skillId != -1);

    // Create a test agent
    int agentId = m_dbManager->addAgent("Test Agent", "C:/test/agent", true, "");
    QVERIFY(agentId != -1);

    // Enable skill for agent
    bool success = m_skillManager->enableSkillForAgent(skillId, agentId);
    QVERIFY(success);

    // Disable skill for agent
    success = m_skillManager->disableSkillForAgent(skillId, agentId);
    QVERIFY(success);
}

QTEST_MAIN(TestSkillManager)
#include "test_SkillManager.moc"
