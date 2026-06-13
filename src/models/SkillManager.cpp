#include "models/SkillManager.h"
#include "models/Skill.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QTemporaryDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegularExpression>
#include <QDebug>
#include <QCoreApplication>

#ifdef QT_VERSION_MAJOR_6
#include <QtCore/QZipWriter>
#include <QtCore/QZipReader>
#endif

SkillManager::SkillManager(DatabaseManager *dbManager, QObject *parent)
    : QObject(parent)
    , m_dbManager(dbManager)
    , m_libraryPath("D:/skillLibrary/skills")
{
    // Ensure library directory exists
    QDir libDir(m_libraryPath);
    if (!libDir.exists()) {
        libDir.mkpath(".");
    }
}

SkillManager::~SkillManager()
{
}

int SkillManager::importFromFolder(const QString &folderPath)
{
    emit importProgress(0, 1, "Starting import from folder...");

    // Check if folder is valid
    QFileInfo folderInfo(folderPath);
    if (!folderInfo.exists() || !folderInfo.isDir()) {
        emit errorOccurred("Invalid folder path: " + folderPath);
        return -1;
    }

    // Check if SKILL.md exists
    QString skillMdPath = QDir::cleanPath(folderPath + "/SKILL.md");
    QFileInfo skillMdInfo(skillMdPath);
    if (!skillMdInfo.exists()) {
        emit errorOccurred("SKILL.md not found in: " + folderPath);
        return -1;
    }

    // Parse skill name from SKILL.md or use folder name
    QString skillName = SkillManager::parseName(skillMdPath);
    if (skillName.isEmpty()) {
        skillName = folderInfo.fileName();
    }

    // Check for duplicate
    if (checkDuplicate(skillName)) {
        int action = handleDuplicate(skillName);
        if (action == -1) { // Cancel
            emit errorOccurred("Import cancelled by user");
            return -1;
        } else if (action == 0) { // Overwrite
            // TODO: Delete existing skill and overwrite
            skillName = generateUniqueName(skillName);
        } else { // Rename
            skillName = generateUniqueName(skillName);
        }
    }

    // Copy folder to library
    if (!copySkillFolder(folderPath, skillName)) {
        emit errorOccurred("Failed to copy skill folder");
        return -1;
    }

    // Parse description
    QString description = SkillManager::parseDescription(skillMdPath);

    // Add to database
    QString skillPath = QDir::cleanPath(m_libraryPath + "/" + skillName);
    int skillId = m_dbManager->addSkill(skillName, skillPath, description);

    if (skillId == -1) {
        emit errorOccurred("Failed to add skill to database");
        return -1;
    }

    emit importProgress(1, 1, "Import completed");
    emit skillImported(skillId);

    return skillId;
}

int SkillManager::importFromWorkBuddy(const QString &workbuddyPath)
{
    QString skillsDir = workbuddyPath + "/skills";
    QDir dir(skillsDir);

    if (!dir.exists()) {
        emit errorOccurred("WorkBuddy skills directory not found: " + skillsDir);
        return -1;
    }

    QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    int total = subDirs.size();
    int imported = 0;

    for (int i = 0; i < total; ++i) {
        QString subDir = subDirs[i];
        QString fullPath = QDir::cleanPath(skillsDir + "/" + subDir);

        emit importProgress(i, total, "Importing " + subDir + "...");

        int skillId = importFromFolder(fullPath);
        if (skillId != -1) {
            imported++;
        }
    }

    emit importProgress(total, total, QString("Completed: %1/%2 skills imported").arg(imported).arg(total));

    return imported;
}

int SkillManager::importFromGitHub(const QString &repoUrl)
{
    emit importProgress(0, 1, "Cloning from GitHub...");

    // Create temporary directory
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        emit errorOccurred("Failed to create temporary directory");
        return -1;
    }

    // Clone repository
    QProcess git;
    QStringList args;
    args << "clone" << repoUrl << tempDir.path();

    git.setWorkingDirectory(tempDir.path());
    git.start("git", args);

    if (!git.waitForFinished(60000)) { // 60 second timeout
        emit errorOccurred("Git clone timeout");
        return -1;
    }

    if (git.exitCode() != 0) {
        emit errorOccurred("Git clone failed: " + git.readAllStandardError());
        return -1;
    }

    // Find skill folder in cloned repo
    QDir clonedDir(tempDir.path());
    QStringList subDirs = clonedDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (subDirs.isEmpty()) {
        emit errorOccurred("No skill found in cloned repository");
        return -1;
    }

    // Import the first directory found
    QString skillFolder = QDir::cleanPath(tempDir.path() + "/" + subDirs[0]);
    return importFromFolder(skillFolder);
}

int SkillManager::importFromZip(const QString &zipPath)
{
    emit importProgress(0, 1, "Extracting ZIP file...");

    // Check if ZIP file exists
    QFileInfo zipInfo(zipPath);
    if (!zipInfo.exists() || !zipInfo.isFile()) {
        emit errorOccurred("Invalid ZIP file: " + zipPath);
        return -1;
    }

    // Create temporary directory
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        emit errorOccurred("Failed to create temporary directory");
        return -1;
    }

    // Extract ZIP
#ifndef QT_VERSION_MAJOR_6
    // Use QuaZip or simple extraction
    emit errorOccurred("ZIP extraction not implemented - Qt6 QZipReader required");
    return -1;
#else
    QZipReader zipReader(zipPath);
    if (!zipReader.exists()) {
        emit errorOccurred("Failed to open ZIP file");
        return -1;
    }

    zipReader.extractAll(tempDir.path());
    zipReader.close();
#endif

    // Find skill folder in extracted content
    QDir extractedDir(tempDir.path());
    QStringList subDirs = extractedDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QString skillFolder;
    if (subDirs.contains("SKILL.md")) {
        skillFolder = tempDir.path();
    } else if (!subDirs.isEmpty()) {
        skillFolder = QDir::cleanPath(tempDir.path() + "/" + subDirs[0]);
    } else {
        emit errorOccurred("No skill found in ZIP file");
        return -1;
    }

    return importFromFolder(skillFolder);
}

int SkillManager::importByDragDrop(const QStringList &paths)
{
    int total = paths.size();
    int imported = 0;

    for (int i = 0; i < total; ++i) {
        QString path = paths[i];
        emit importProgress(i, total, "Processing " + path + "...");

        QFileInfo info(path);
        int skillId = -1;

        if (info.isDir()) {
            skillId = importFromFolder(path);
        } else if (info.suffix().toLower() == "zip") {
            skillId = importFromZip(path);
        } else {
            emit errorOccurred("Unsupported file type: " + path);
            continue;
        }

        if (skillId != -1) {
            imported++;
        }
    }

    emit importProgress(total, total, QString("Completed: %1/%2 items imported").arg(imported).arg(total));

    return imported;
}

bool SkillManager::exportToZip(int skillId, const QString &outputPath)
{
    emit exportProgress(0, 1, "Exporting skill to ZIP...");

    // Get skill info from database
    SkillInfo skillInfo = m_dbManager->getSkill(skillId);
    if (skillInfo.id == -1) {
        emit errorOccurred("Skill not found: " + QString::number(skillId));
        return false;
    }

    // Check if skill path exists
    QDir skillDir(skillInfo.path);
    if (!skillDir.exists()) {
        emit errorOccurred("Skill path not found: " + skillInfo.path);
        return false;
    }

#ifndef QT_VERSION_MAJOR_6
    emit errorOccurred("ZIP export not implemented - Qt6 QZipWriter required");
    return false;
#else
    // Create ZIP file
    QZipWriter zipWriter(outputPath);

    // Add all files in skill folder
    QStringList files = skillDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &file : files) {
        QString filePath = QDir::cleanPath(skillInfo.path + "/" + file);
        QFileInfo fileInfo(filePath);

        if (fileInfo.isDir()) {
            // Recursively add directory
            QDir subDir(filePath);
            QStringList subFiles = subDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
            for (const QString &subFile : subFiles) {
                zipWriter.addFile(file + "/" + subFile, QByteArray());
            }
        } else {
            // Add file
            QFile f(filePath);
            if (f.open(QIODevice::ReadOnly)) {
                zipWriter.addFile(file, f.readAll());
                f.close();
            }
        }
    }

    zipWriter.close();
#endif

    emit exportProgress(1, 1, "Export completed");

    return true;
}

bool SkillManager::exportMultipleToZip(const QVector<int> &skillIds, const QString &outputPath)
{
    int total = skillIds.size();

    emit exportProgress(0, total, "Exporting multiple skills...");

#ifndef QT_VERSION_MAJOR_6
    emit errorOccurred("ZIP export not implemented - Qt6 QZipWriter required");
    return false;
#else
    QZipWriter zipWriter(outputPath);

    for (int i = 0; i < total; ++i) {
        int skillId = skillIds[i];
        emit exportProgress(i, total, "Exporting skill " + QString::number(skillId) + "...");

        // Get skill info
        SkillInfo skillInfo = m_dbManager->getSkill(skillId);
        if (skillInfo.id == -1) {
            continue;
        }

        // Add skill folder to ZIP
        QDir skillDir(skillInfo.path);
        QStringList files = skillDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        for (const QString &file : files) {
            QString filePath = QDir::cleanPath(skillInfo.path + "/" + file);
            QFileInfo fileInfo(filePath);

            QString zipPath = skillInfo.name + "/" + file;

            if (fileInfo.isDir()) {
                QDir subDir(filePath);
                QStringList subFiles = subDir.entryList(QDir::Files);
                for (const QString &subFile : subFiles) {
                    QFile f(QDir::cleanPath(filePath + "/" + subFile));
                    if (f.open(QIODevice::ReadOnly)) {
                        zipWriter.addFile(zipPath + "/" + subFile, f.readAll());
                        f.close();
                    }
                }
            } else {
                QFile f(filePath);
                if (f.open(QIODevice::ReadOnly)) {
                    zipWriter.addFile(zipPath, f.readAll());
                    f.close();
                }
            }
        }
    }

    zipWriter.close();
#endif

    emit exportProgress(total, total, "Export completed");

    return true;
}

bool SkillManager::validateSkill(int skillId)
{
    // Get skill info from database
    SkillInfo skillInfo = m_dbManager->getSkill(skillId);
    if (skillInfo.id == -1) {
        emit skillValidated(skillId, false);
        return false;
    }

    // Check if skill path exists
    QDir skillDir(skillInfo.path);
    if (!skillDir.exists()) {
        emit skillValidated(skillId, false);
        return false;
    }

    // Check if SKILL.md exists
    QString skillMdPath = QDir::cleanPath(skillInfo.path + "/SKILL.md");
    QFileInfo skillMdInfo(skillMdPath);
    if (!skillMdInfo.exists()) {
        emit skillValidated(skillId, false);
        return false;
    }

    emit skillValidated(skillId, true);
    return true;
}

QVector<int> SkillManager::validateAllSkills()
{
    QVector<int> invalidSkills;

    // Get all skills
    QVector<SkillInfo> allSkills = m_dbManager->getAllSkills();

    for (const SkillInfo &skill : allSkills) {
        if (!validateSkill(skill.id)) {
            invalidSkills.append(skill.id);
        }
    }

    return invalidSkills;
}

bool SkillManager::fixSkill(int skillId, const QString &newSkillMdPath)
{
    // Check if new path is valid
    QFileInfo newPathInfo(newSkillMdPath);
    if (!newPathInfo.exists() || !newPathInfo.isFile()) {
        emit errorOccurred("Invalid SKILL.md path: " + newSkillMdPath);
        return false;
    }

    // Get skill info
    SkillInfo skillInfo = m_dbManager->getSkill(skillId);
    if (skillInfo.id == -1) {
        emit errorOccurred("Skill not found: " + QString::number(skillId));
        return false;
    }

    // Update path in database
    QVariantMap fields;
    fields["path"] = newPathInfo.absolutePath();
    fields["updated_at"] = QDateTime::currentDateTime();

    bool success = m_dbManager->updateSkill(skillId, fields);

    if (success) {
        emit skillValidated(skillId, true);
    }

    return success;
}

bool SkillManager::deleteSkill(int skillId, bool deleteFromAgents)
{
    // Get skill info
    SkillInfo skillInfo = m_dbManager->getSkill(skillId);
    if (skillInfo.id == -1) {
        emit errorOccurred("Skill not found: " + QString::number(skillId));
        return false;
    }

    // Delete skill folder
    QDir skillDir(skillInfo.path);
    if (skillDir.exists()) {
        if (!skillDir.removeRecursively()) {
            emit errorOccurred("Failed to delete skill folder: " + skillInfo.path);
            return false;
        }
    }

    // Delete from database
    bool success = m_dbManager->deleteSkill(skillId);

    if (success) {
        emit skillDeleted(skillId);
    }

    return success;
}

QStringList SkillManager::autoDetectTags(int skillId)
{
    QStringList detectedTags;

    // Get skill info
    SkillInfo skillInfo = m_dbManager->getSkill(skillId);
    if (skillInfo.id == -1) {
        return detectedTags;
    }

    // Read SKILL.md content
    QString skillMdPath = QDir::cleanPath(skillInfo.path + "/SKILL.md");
    QFile file(skillMdPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return detectedTags;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    file.close();

    // Extract headings (h1, h2)
    QStringList lines = content.split("\n");
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.startsWith("# ")) {
            detectedTags.append(trimmed.mid(2).trimmed());
        } else if (trimmed.startsWith("## ")) {
            detectedTags.append(trimmed.mid(3).trimmed());
        }
    }

    // Extract trigger words
    QStringList triggers = SkillManager::parseTriggers(skillMdPath);
    detectedTags.append(triggers);

    // Extract high-frequency words (simple implementation)
    // Remove markdown syntax and count words
    QString plainText = content;
    plainText.remove(QRegularExpression("#+ ")); // Remove headings
    plainText.remove(QRegularExpression("`[^`]*`")); // Remove inline code
    plainText.remove(QRegularExpression("\\[.*\\]\\(.*\\)")); // Remove links

    // Simple word frequency analysis
    QStringList words = plainText.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    QMap<QString, int> wordCount;
    for (const QString &word : words) {
        QString cleaned = word.toLower().remove(QRegularExpression("[^\\w]"));
        if (cleaned.length() > 3) { // Only consider words longer than 3 chars
            wordCount[cleaned]++;
        }
    }

    // Add top 5 high-frequency words as tags
    // Sort by frequency (simple approach: just add all words sorted by key)
    QStringList frequentWords = wordCount.keys();
    frequentWords.sort();
    for (int i = 0; i < qMin(5, frequentWords.size()); ++i) {
        detectedTags.append(frequentWords[i]);
    }

    // Remove duplicates
    detectedTags.removeDuplicates();

    return detectedTags;
}

bool SkillManager::autoTagSkill(int skillId)
{
    // Auto-detect tags
    QStringList tags = autoDetectTags(skillId);

    if (tags.isEmpty()) {
        return false;
    }

    // Add tags to database
    for (const QString &tagName : tags) {
        int tagId = m_dbManager->getOrCreateTag(tagName);
        if (tagId != -1) {
            m_dbManager->addSkillTag(skillId, tagId);
        }
    }

    return true;
}

bool SkillManager::enableSkillForAgent(int skillId, int agentId)
{
    return m_dbManager->enableSkillForAgent(skillId, agentId);
}

bool SkillManager::disableSkillForAgent(int skillId, int agentId)
{
    return m_dbManager->disableSkillForAgent(skillId, agentId);
}

QString SkillManager::parseDescription(const QString &skillMdPath)
{
    QFile file(skillMdPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "";
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    QString description;
    bool inCodeBlock = false;
    int charCount = 0;

    while (!in.atEnd() && charCount < 200) {
        QString line = in.readLine();

        // Skip code blocks
        if (line.trimmed().startsWith("```")) {
            inCodeBlock = !inCodeBlock;
            continue;
        }

        if (inCodeBlock) {
            continue;
        }

        // Skip headings
        if (line.trimmed().startsWith("#")) {
            continue;
        }

        // Skip empty lines
        if (line.trimmed().isEmpty()) {
            continue;
        }

        // Found a valid description line
        if (!description.isEmpty()) {
            description += " ";
        }
        description += line.trimmed();
        charCount += line.trimmed().length();

        // Stop if we have enough characters
        if (charCount >= 200) {
            description = description.left(200);
            break;
        }
    }

    file.close();

    return description;
}

QStringList SkillManager::parseTriggers(const QString &skillMdPath)
{
    QStringList triggers;

    QFile file(skillMdPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return triggers;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    bool inTriggerSection = false;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QString trimmed = line.trimmed();

        // Check if we're entering a trigger section
        if (trimmed.contains("触发词") || trimmed.contains("触发") || trimmed.contains("Trigger") || trimmed.contains("trigger")) {
            inTriggerSection = true;
            continue;
        }

        // If in trigger section, collect list items
        if (inTriggerSection) {
            // Check if it's a list item
            if (trimmed.startsWith("- ") || trimmed.startsWith("* ")) {
                QString trigger = trimmed.mid(2).trimmed();
                if (!trigger.isEmpty()) {
                    triggers.append(trigger);
                }
            } else if (trimmed.startsWith("1. ") || trimmed.startsWith("2. ") || trimmed.startsWith("3. ")) {
                QString trigger = trimmed.mid(trimmed.indexOf(" ") + 1).trimmed();
                if (!trigger.isEmpty()) {
                    triggers.append(trigger);
                }
            } else if (!trimmed.isEmpty() && !trimmed.startsWith("#")) {
                // Possibly end of trigger section
                // Continue collecting non-empty lines
                triggers.append(trimmed);
            } else if (trimmed.startsWith("#")) {
                // New section started
                inTriggerSection = false;
            }
        }
    }

    file.close();

    return triggers;
}

QString SkillManager::parseName(const QString &skillMdPath)
{
    QFile file(skillMdPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "";
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QString trimmed = line.trimmed();

        // Extract h1 heading
        if (trimmed.startsWith("# ")) {
            file.close();
            return trimmed.mid(2).trimmed();
        }
    }

    file.close();

    return "";
}

bool SkillManager::copySkillFolder(const QString &sourcePath, const QString &skillName)
{
    QString destPath = QDir::cleanPath(m_libraryPath + "/" + skillName);

    // Check if destination already exists
    QDir destDir(destPath);
    if (destDir.exists()) {
        // Remove existing folder
        if (!destDir.removeRecursively()) {
            return false;
        }
    }

    // Create destination directory
    if (!destDir.mkpath(".")) {
        return false;
    }

    // Copy all files
    QDir sourceDir(sourcePath);
    QStringList files = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &file : files) {
        QString src = QDir::cleanPath(sourcePath + "/" + file);
        QString dst = QDir::cleanPath(destPath + "/" + file);

        QFileInfo info(src);
        if (info.isDir()) {
            // Recursively copy subdirectory
            if (!copySkillFolder(src, file)) {
                return false;
            }
        } else {
            // Copy file
            if (!QFile::copy(src, dst)) {
                return false;
            }
        }
    }

    return true;
}

QString SkillManager::generateUniqueName(const QString &baseName)
{
    QString uniqueName = baseName;
    int counter = 1;

    while (checkDuplicate(uniqueName)) {
        uniqueName = baseName + " (" + QString::number(counter) + ")";
        counter++;
    }

    return uniqueName;
}

bool SkillManager::checkDuplicate(const QString &name)
{
    QVector<SkillInfo> allSkills = m_dbManager->getAllSkills();

    for (const SkillInfo &skill : allSkills) {
        if (skill.name == name) {
            return true;
        }
    }

    return false;
}

int SkillManager::handleDuplicate(const QString &name)
{
    // TODO: In a real GUI application, this would show a dialog
    // For now, return 1 (rename) as default action
    Q_UNUSED(name);

    // Return values:
    // -1 = Cancel
    //  0 = Overwrite
    //  1 = Rename

    // This should be implemented with a proper dialog in the UI
    // For testing purposes, we'll emit a signal and return rename
    emit errorOccurred("Duplicate skill found: " + name + ". Auto-renaming.");

    return 1; // Rename
}
