#include "ui/SettingsDialog.h"
#include "database/DatabaseManager.h"
#include "models/AgentManager.h"
#include "ui/AgentDialog.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QProgressBar>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QProgressDialog>
#include <QApplication>
#include <QStyle>

SettingsDialog::SettingsDialog(DatabaseManager *dbManager, AgentManager *agentManager, QWidget *parent)
    : QDialog(parent)
    , m_dbManager(dbManager)
    , m_agentManager(agentManager)
    , m_tabWidget(nullptr)
    , m_libraryPathEdit(nullptr)
    , m_autoScanCheck(nullptr)
    , m_autoBackupCheck(nullptr)
    , m_backupFrequencyCombo(nullptr)
    , m_themeCombo(nullptr)
    , m_themeWarningLabel(nullptr)
    , m_agentTable(nullptr)
    , m_addAgentButton(nullptr)
    , m_editAgentButton(nullptr)
    , m_toggleAgentButton(nullptr)
    , m_deleteAgentButton(nullptr)
    , m_backupNowButton(nullptr)
    , m_backupList(nullptr)
    , m_restoreButton(nullptr)
    , m_nextBackupLabel(nullptr)
    , m_versionLabel(nullptr)
    , m_copyrightLabel(nullptr)
    , m_checkUpdateButton(nullptr)
    , m_aboutQtButton(nullptr)
{
    setupUi();
    loadSettings();
    refreshAgents();
    refreshBackups();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::onBrowseLibraryPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择中央库路径",
                                                     m_libraryPathEdit->text(),
                                                     QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()) {
        m_libraryPathEdit->setText(dir);
    }
}

void SettingsDialog::onResetLibraryPath()
{
    m_libraryPathEdit->setText(getDefaultLibraryPath());
}

void SettingsDialog::onThemeChanged(int index)
{
    QString theme = m_themeCombo->itemText(index);
    if (theme == "深色") {
        m_themeWarningLabel->setText("（主题切换需要重启生效）");
    } else {
        m_themeWarningLabel->setText("（主题切换需要重启生效）");
    }
}

void SettingsDialog::onAddAgent()
{
    AgentDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName();
        QString path = dialog.getPath();
        QString icon = dialog.getIcon();
        bool enabled = dialog.isEnabled();

        // 检查名称是否重复
        QVector<AgentInfo> agents = m_dbManager->getAllAgents();
        for (const AgentInfo &agent : agents) {
            if (agent.name == name) {
                QMessageBox::warning(this, "警告", "Agent 名称已存在");
                return;
            }
        }

        // 有路径才允许启用
        if (enabled && (path.isEmpty() || !QDir(path).exists())) {
            QMessageBox::warning(this, "警告", "路径无效或未设置，无法启用。请先设置有效路径。");
            enabled = false;
        }

        int id = m_dbManager->addAgent(name, path, enabled, icon);
        if (id > 0) {
            refreshAgents();
            emit agentsChanged();
        } else {
            QMessageBox::warning(this, "错误", "添加 Agent 失败");
        }
    }
}

void SettingsDialog::onEditAgent()
{
    QList<QTableWidgetItem *> selected = m_agentTable->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first()->row();
    int agentId = m_agentTable->item(row, 0)->data(Qt::UserRole).toInt();

    AgentInfo agent = m_dbManager->getAgent(agentId);
    if (agent.id == -1) {
        QMessageBox::warning(this, "错误", "获取 Agent 信息失败");
        return;
    }

    AgentDialog dialog(this, &agent);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName();
        QString path = dialog.getPath();
        QString icon = dialog.getIcon();
        bool enabled = dialog.isEnabled();

        // 检查名称是否重复（排除自己）
        QVector<AgentInfo> agents = m_dbManager->getAllAgents();
        for (const AgentInfo &a : agents) {
            if (a.id != agentId && a.name == name) {
                QMessageBox::warning(this, "警告", "Agent 名称已存在");
                return;
            }
        }

        // 有路径才允许启用
        if (enabled && (path.isEmpty() || !QDir(path).exists())) {
            QMessageBox::warning(this, "警告", "路径无效或未设置，无法启用。请先设置有效路径。");
            enabled = false;
        }

        QVariantMap fields;
        fields["name"] = name;
        fields["path"] = path;
        fields["icon"] = icon;
        fields["enabled"] = enabled;

        bool ok = m_dbManager->updateAgent(agentId, fields);

        if (ok) {
            refreshAgents();
            emit agentsChanged();
        } else {
            QMessageBox::warning(this, "错误", "更新 Agent 失败");
        }
    }
}

void SettingsDialog::onToggleAgent()
{
    QList<QTableWidgetItem *> selected = m_agentTable->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first()->row();
    int agentId = m_agentTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString status = m_agentTable->item(row, 2)->text();

    bool newEnabled = (status == "禁用");

    // 启用时检查路径是否有效
    if (newEnabled) {
        AgentInfo agent = m_dbManager->getAgent(agentId);
        if (agent.path.isEmpty() || !QDir(agent.path).exists()) {
            QMessageBox::warning(this, "警告", "该 Agent 路径未设置或无效，无法启用。请先编辑设置有效路径。");
            return;
        }
    }

    if (m_dbManager->setAgentEnabled(agentId, newEnabled)) {
        refreshAgents();
        emit agentsChanged();
    }
}

void SettingsDialog::onDeleteAgent()
{
    QList<QTableWidgetItem *> selected = m_agentTable->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first()->row();
    int agentId = m_agentTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString name = m_agentTable->item(row, 0)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                  QString("确定要删除 Agent '%1' 吗？").arg(name),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_dbManager->deleteAgent(agentId)) {
            refreshAgents();
            emit agentsChanged();
        } else {
            QMessageBox::warning(this, "错误", "删除 Agent 失败");
        }
    }
}

void SettingsDialog::onAgentSelectionChanged()
{
    QList<QTableWidgetItem *> selected = m_agentTable->selectedItems();
    bool hasSelection = !selected.isEmpty();

    m_editAgentButton->setEnabled(hasSelection);
    m_toggleAgentButton->setEnabled(hasSelection);
    m_deleteAgentButton->setEnabled(hasSelection);
}

void SettingsDialog::onAgentContextMenu(const QPoint &pos)
{
    QList<QTableWidgetItem *> selected = m_agentTable->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    QMenu contextMenu(this);

    QAction *editAction = contextMenu.addAction("编辑");
    connect(editAction, &QAction::triggered, this, &SettingsDialog::onEditAgent);

    QAction *toggleAction = contextMenu.addAction("启用/禁用");
    connect(toggleAction, &QAction::triggered, this, &SettingsDialog::onToggleAgent);

    QAction *deleteAction = contextMenu.addAction("删除");
    connect(deleteAction, &QAction::triggered, this, &SettingsDialog::onDeleteAgent);

    contextMenu.exec(m_agentTable->mapToGlobal(pos));
}

void SettingsDialog::onBackupNow()
{
    QString backupDir = getBackupDir();
    QDir dir(backupDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString backupPath = backupDir + "/backup_" + QDateTime::currentDateTime().toString("yyyyMMdd") + ".db";

    // 检查文件是否已存在
    if (QFile::exists(backupPath)) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "文件已存在",
                                      QString("备份文件 %1 已存在，是否覆盖？").arg(backupPath),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    // 显示进度对话框
    QProgressDialog progress("正在备份...", "取消", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    // 执行备份
    if (m_dbManager->exportBackup(backupPath)) {
        progress.setValue(100);
        QMessageBox::information(this, "备份成功", QString("备份已保存到：%1").arg(backupPath));
        refreshBackups();
    } else {
        QMessageBox::warning(this, "备份失败", "备份过程中出现错误");
    }
}

void SettingsDialog::onRestoreBackup()
{
    QList<QListWidgetItem *> selected = m_backupList->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    QString backupPath = selected.first()->data(Qt::UserRole).toString();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认恢复",
                                  QString("确定要从 %1 恢复备份吗？\n当前数据将被覆盖。").arg(backupPath),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_dbManager->importBackup(backupPath)) {
            QMessageBox::information(this, "恢复成功", "备份已成功恢复");
            refreshAgents();
        } else {
            QMessageBox::warning(this, "恢复失败", "恢复过程中出现错误");
        }
    }
}

void SettingsDialog::onBackupSelectionChanged()
{
    QList<QListWidgetItem *> selected = m_backupList->selectedItems();
    m_restoreButton->setEnabled(!selected.isEmpty());
}

void SettingsDialog::onCheckUpdate()
{
    QMessageBox::information(this, "检查更新", "已是最新版本");
}

void SettingsDialog::setupUi()
{
    setWindowTitle("设置");
    setMinimumSize(700, 500);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(setupGeneralTab(), "常规");
    m_tabWidget->addTab(setupAgentsTab(), "Agent 管理");
    m_tabWidget->addTab(setupBackupTab(), "备份");
    m_tabWidget->addTab(setupAboutTab(), "关于");

    // 按钮
    QPushButton *okButton = new QPushButton("确定", this);
    QPushButton *cancelButton = new QPushButton("取消", this);

    connect(okButton, &QPushButton::clicked, this, &SettingsDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

QWidget *SettingsDialog::setupGeneralTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);

    // 中央库路径
    QGroupBox *pathGroup = new QGroupBox("中央库路径");
    QHBoxLayout *pathLayout = new QHBoxLayout();

    m_libraryPathEdit = new QLineEdit(pathGroup);
    QPushButton *browseButton = new QPushButton("浏览...", pathGroup);
    QPushButton *resetButton = new QPushButton("重置为默认", pathGroup);

    connect(browseButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseLibraryPath);
    connect(resetButton, &QPushButton::clicked, this, &SettingsDialog::onResetLibraryPath);

    pathLayout->addWidget(m_libraryPathEdit);
    pathLayout->addWidget(browseButton);
    pathLayout->addWidget(resetButton);
    pathGroup->setLayout(pathLayout);

    // 启动选项
    QGroupBox *startupGroup = new QGroupBox("启动选项");
    QVBoxLayout *startupLayout = new QVBoxLayout();

    m_autoScanCheck = new QCheckBox("启动时自动扫描 agent 路径", startupGroup);
    m_autoBackupCheck = new QCheckBox("关闭时自动备份数据库", startupGroup);

    startupLayout->addWidget(m_autoScanCheck);
    startupLayout->addWidget(m_autoBackupCheck);
    startupGroup->setLayout(startupLayout);

    // 自动备份频率
    QGroupBox *backupGroup = new QGroupBox("自动备份");
    QFormLayout *backupLayout = new QFormLayout();

    m_backupFrequencyCombo = new QComboBox(backupGroup);
    m_backupFrequencyCombo->addItems({"每周", "每天", "每月"});

    backupLayout->addRow("自动备份频率:", m_backupFrequencyCombo);
    backupGroup->setLayout(backupLayout);

    // 主题
    QGroupBox *themeGroup = new QGroupBox("主题");
    QFormLayout *themeLayout = new QFormLayout();

    m_themeCombo = new QComboBox(themeGroup);
    m_themeCombo->addItems({"浅色", "深色"});
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onThemeChanged);

    m_themeWarningLabel = new QLabel("（主题切换需要重启生效）", themeGroup);
    m_themeWarningLabel->setStyleSheet("color: gray;");

    themeLayout->addRow("主题:", m_themeCombo);
    themeLayout->addRow("", m_themeWarningLabel);
    themeGroup->setLayout(themeLayout);

    mainLayout->addWidget(pathGroup);
    mainLayout->addWidget(startupGroup);
    mainLayout->addWidget(backupGroup);
    mainLayout->addWidget(themeGroup);
    mainLayout->addStretch();

    return tab;
}

QWidget *SettingsDialog::setupAgentsTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);

    // Agent 列表
    m_agentTable = new QTableWidget(tab);
    m_agentTable->setColumnCount(3);
    m_agentTable->setHorizontalHeaderLabels({"名称", "路径", "状态"});
    m_agentTable->setSelectionBehavior(QTableWidget::SelectRows);
    m_agentTable->setSelectionMode(QTableWidget::SingleSelection);
    m_agentTable->setAlternatingRowColors(true);
    m_agentTable->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_agentTable, &QTableWidget::itemSelectionChanged,
            this, &SettingsDialog::onAgentSelectionChanged);
    connect(m_agentTable, &QTableWidget::customContextMenuRequested,
            this, &SettingsDialog::onAgentContextMenu);

    // 按钮
    m_addAgentButton = new QPushButton("添加 Agent...", tab);
    m_editAgentButton = new QPushButton("编辑...", tab);
    m_toggleAgentButton = new QPushButton("启用/禁用", tab);
    m_deleteAgentButton = new QPushButton("删除", tab);

    m_editAgentButton->setEnabled(false);
    m_toggleAgentButton->setEnabled(false);
    m_deleteAgentButton->setEnabled(false);

    connect(m_addAgentButton, &QPushButton::clicked, this, &SettingsDialog::onAddAgent);
    connect(m_editAgentButton, &QPushButton::clicked, this, &SettingsDialog::onEditAgent);
    connect(m_toggleAgentButton, &QPushButton::clicked, this, &SettingsDialog::onToggleAgent);
    connect(m_deleteAgentButton, &QPushButton::clicked, this, &SettingsDialog::onDeleteAgent);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_addAgentButton);
    buttonLayout->addWidget(m_editAgentButton);
    buttonLayout->addWidget(m_toggleAgentButton);
    buttonLayout->addWidget(m_deleteAgentButton);
    buttonLayout->addStretch();

    mainLayout->addWidget(m_agentTable);
    mainLayout->addLayout(buttonLayout);

    return tab;
}

QWidget *SettingsDialog::setupBackupTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);

    // 手动备份
    QGroupBox *manualGroup = new QGroupBox("手动备份");
    QHBoxLayout *manualLayout = new QHBoxLayout();

    m_backupNowButton = new QPushButton("立即备份", manualGroup);
    connect(m_backupNowButton, &QPushButton::clicked, this, &SettingsDialog::onBackupNow);

    manualLayout->addWidget(m_backupNowButton);
    manualLayout->addStretch();
    manualGroup->setLayout(manualLayout);

    // 恢复备份
    QGroupBox *restoreGroup = new QGroupBox("恢复备份");
    QVBoxLayout *restoreLayout = new QVBoxLayout();

    m_backupList = new QListWidget(restoreGroup);
    m_backupList->setSelectionMode(QListWidget::SingleSelection);
    connect(m_backupList, &QListWidget::itemSelectionChanged,
            this, &SettingsDialog::onBackupSelectionChanged);

    m_restoreButton = new QPushButton("恢复", restoreGroup);
    m_restoreButton->setEnabled(false);
    connect(m_restoreButton, &QPushButton::clicked, this, &SettingsDialog::onRestoreBackup);

    restoreLayout->addWidget(m_backupList);
    restoreLayout->addWidget(m_restoreButton);
    restoreGroup->setLayout(restoreLayout);

    // 自动备份
    QGroupBox *autoGroup = new QGroupBox("自动备份");
    QVBoxLayout *autoLayout = new QVBoxLayout();

    m_nextBackupLabel = new QLabel("下次自动备份时间: 未知", autoGroup);
    autoLayout->addWidget(m_nextBackupLabel);
    autoGroup->setLayout(autoLayout);

    mainLayout->addWidget(manualGroup);
    mainLayout->addWidget(restoreGroup);
    mainLayout->addWidget(autoGroup);
    mainLayout->addStretch();

    return tab;
}

QWidget *SettingsDialog::setupAboutTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);
    mainLayout->setAlignment(Qt::AlignCenter);

    // 图标（暂用 Qt 默认图标）
    QLabel *iconLabel = new QLabel(tab);
    iconLabel->setPixmap(QApplication::style()->standardPixmap(QStyle::SP_ComputerIcon).scaled(64, 64));
    iconLabel->setAlignment(Qt::AlignCenter);

    // 应用名称
    QLabel *nameLabel = new QLabel("SkillCentral", tab);
    nameLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    nameLabel->setAlignment(Qt::AlignCenter);

    // 版本号
    m_versionLabel = new QLabel("版本: v1.0.0", tab);
    m_versionLabel->setAlignment(Qt::AlignCenter);

    // 作者
    QLabel *authorLabel = new QLabel("作者: Dongmiao", tab);
    authorLabel->setAlignment(Qt::AlignCenter);

    // 操作按钮
    m_checkUpdateButton = new QPushButton("检查更新...", tab);
    m_aboutQtButton = new QPushButton("关于 Qt...", tab);

    connect(m_checkUpdateButton, &QPushButton::clicked, this, &SettingsDialog::onCheckUpdate);
    connect(m_aboutQtButton, &QPushButton::clicked, []() {
        QMessageBox::aboutQt(nullptr, "关于 Qt");
    });

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_checkUpdateButton);
    buttonLayout->addWidget(m_aboutQtButton);
    buttonLayout->addStretch();

    // 版权声明
    m_copyrightLabel = new QLabel("Copyright © 2024 Dongmiao. All rights reserved.", tab);
    m_copyrightLabel->setAlignment(Qt::AlignCenter);
    m_copyrightLabel->setStyleSheet("color: gray;");

    mainLayout->addWidget(iconLabel);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(m_versionLabel);
    mainLayout->addWidget(authorLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(m_copyrightLabel);

    return tab;
}

void SettingsDialog::loadSettings()
{
    // 中央库路径
    QString libraryPath = m_dbManager->getSetting("libraryPath", getDefaultLibraryPath());
    m_libraryPathEdit->setText(libraryPath);

    // 启动选项
    bool autoScan = m_dbManager->getSetting("autoScan", "true") == "true";
    bool autoBackup = m_dbManager->getSetting("autoBackup", "true") == "true";
    m_autoScanCheck->setChecked(autoScan);
    m_autoBackupCheck->setChecked(autoBackup);

    // 自动备份频率
    QString frequency = m_dbManager->getSetting("backupFrequency", "每周");
    int index = m_backupFrequencyCombo->findText(frequency);
    if (index >= 0) {
        m_backupFrequencyCombo->setCurrentIndex(index);
    }

    // 主题
    QString theme = m_dbManager->getSetting("theme", "浅色");
    index = m_themeCombo->findText(theme);
    if (index >= 0) {
        m_themeCombo->setCurrentIndex(index);
    }
}

void SettingsDialog::saveSettings()
{
    // 中央库路径
    if (!m_dbManager->setSetting("libraryPath", m_libraryPathEdit->text())) {
        QMessageBox::warning(this, "保存失败", "无法保存库路径到数据库");
    }

    // 启动选项
    m_dbManager->setSetting("autoScan", m_autoScanCheck->isChecked() ? "true" : "false");
    m_dbManager->setSetting("autoBackup", m_autoBackupCheck->isChecked() ? "true" : "false");

    // 自动备份频率
    m_dbManager->setSetting("backupFrequency", m_backupFrequencyCombo->currentText());

    // 主题
    m_dbManager->setSetting("theme", m_themeCombo->currentText());
}

void SettingsDialog::refreshAgents()
{
    m_agentTable->setRowCount(0);
    QVector<AgentInfo> agents = m_dbManager->getAllAgents();

    // 预置 Agent 名称列表
    QStringList presetNames = {"WorkBuddy", "Claude Code", "Codex", "MimoCode", "Trae", "OpenCode"};

    for (const AgentInfo &agent : agents) {
        int row = m_agentTable->rowCount();
        m_agentTable->insertRow(row);

        QTableWidgetItem *nameItem = new QTableWidgetItem(agent.name);
        nameItem->setData(Qt::UserRole, agent.id);

        QTableWidgetItem *pathItem = new QTableWidgetItem(agent.path);
        QTableWidgetItem *statusItem = new QTableWidgetItem(agent.enabled ? "启用" : "禁用");

        // 预置 Agent 用灰色背景
        if (presetNames.contains(agent.name)) {
            nameItem->setBackground(QColor(240, 240, 240));
            pathItem->setBackground(QColor(240, 240, 240));
            statusItem->setBackground(QColor(240, 240, 240));
        }

        m_agentTable->setItem(row, 0, nameItem);
        m_agentTable->setItem(row, 1, pathItem);
        m_agentTable->setItem(row, 2, statusItem);
    }

    m_agentTable->resizeColumnsToContents();
}

void SettingsDialog::refreshBackups()
{
    m_backupList->clear();
    QString backupDir = getBackupDir();
    QDir dir(backupDir);

    if (dir.exists()) {
        QStringList filters;
        filters << "backup_*.db";
        QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time);

        for (const QFileInfo &file : files) {
            QListWidgetItem *item = new QListWidgetItem(file.fileName(), m_backupList);
            item->setData(Qt::UserRole, file.absoluteFilePath());
        }
    }

    // 更新下次备份时间
    m_nextBackupLabel->setText("下次自动备份时间: " + getNextBackupTime());
}

QString SettingsDialog::getDefaultLibraryPath() const
{
    return "D:/skillLibrary";
}

QString SettingsDialog::getBackupDir() const
{
    return m_libraryPathEdit->text() + "/backups";
}

QString SettingsDialog::getNextBackupTime() const
{
    QString frequency = m_backupFrequencyCombo->currentText();
    QDateTime now = QDateTime::currentDateTime();

    if (frequency == "每天") {
        return now.addDays(1).toString("yyyy-MM-dd") + " 00:00";
    } else if (frequency == "每周") {
        int daysToMonday = (8 - now.date().dayOfWeek()) % 7;
        if (daysToMonday == 0) daysToMonday = 7;
        return now.addDays(daysToMonday).toString("yyyy-MM-dd") + " 00:00";
    } else if (frequency == "每月") {
        QDate nextMonth = now.date().addMonths(1);
        nextMonth = QDate(nextMonth.year(), nextMonth.month(), 1);
        return nextMonth.toString("yyyy-MM-dd") + " 00:00";
    }

    return "未知";
}

void SettingsDialog::accept()
{
    // 保存设置
    saveSettings();

    // 验证 libraryPath 是否真的写入了数据库
    QString savedPath = m_dbManager->getSetting("libraryPath", "");
    if (savedPath != m_libraryPathEdit->text()) {
        QMessageBox::warning(this, "保存失败",
            QString("库路径未正确保存！\n期望: %1\n实际: %2")
                .arg(m_libraryPathEdit->text()).arg(savedPath));
    }

    // 发射信号
    emit settingsChanged();

    QString newPath = m_libraryPathEdit->text();
    emit libraryPathChanged(newPath);

    QString theme = m_themeCombo->currentText();
    emit themeChanged(theme == "深色" ? "dark" : "light");

    QDialog::accept();
}

void SettingsDialog::reject()
{
    QDialog::reject();
}
