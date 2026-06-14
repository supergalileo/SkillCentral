#include "ui/MainWindow.h"
#include "ui/CardWidget.h"
#include "ui/SkillCard.h"
#include "ui/SkillSidebar.h"
#include "ui/SettingsDialog.h"
#include "ui/TagManagerDialog.h"
#include "models/AgentManager.h"
#include "models/SkillManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QScrollBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QDateTime>
#include <QDir>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_dbManager(nullptr)
    , m_agentManager(nullptr)
    , m_skillManager(nullptr)
    , m_largeMode(true)
{
    setupUi();
    setWindowTitle("SkillCentral");
    resize(1000, 700);
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setDatabaseManager(DatabaseManager *dbManager)
{
    m_dbManager = dbManager;

    // 创建 SkillManager
    if (!m_skillManager) {
        m_skillManager = new SkillManager(m_dbManager, this);
    }

    // 创建 AgentManager（需要 SkillManager）
    if (!m_agentManager) {
        m_agentManager = new AgentManager(m_dbManager, m_skillManager, this);
    }

    // 传给侧边栏
    if (m_skillSidebar) {
        m_skillSidebar->setDatabaseManager(m_dbManager);
        m_skillSidebar->setAgentManager(m_agentManager);
    }

    // 如果数据库为空，创建测试数据
    QVector<SkillInfo> existing = m_dbManager->getAllSkills();
    if (existing.isEmpty()) {
        createTestData();
    }

    // 扫描 agent 路径（在 show() 之前完成，避免画面闪烁）
    if (m_agentManager) {
        bool autoScan = m_dbManager->getSetting("autoScan", "true") == "true";
        if (autoScan) {
            qInfo() << "Startup scan starting...";
            m_agentManager->scanAllAgents();
            qInfo() << "Startup scan done";
        }
    }

    // 清理旧的 theme 设置
    if (m_dbManager) {
        m_dbManager->setSetting("theme", "浅色");
    }

    // 在 show() 之后通过 timer 加载 UI，确保布局已就绪
    QTimer::singleShot(0, this, [this]() {
        loadSkills();
        loadTags();
        loadAgents();
    });
}

void MainWindow::setupUi()
{
    // 中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    // ===== 顶部搜索框 =====
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索 skill 名称或标签");
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setStyleSheet(
        "QLineEdit { padding: 6px 10px; border: 1px solid #ccc; "
        "border-radius: 6px; font-size: 13px; }"
        "QLineEdit:focus { border: 1px solid #4a90d9; }"
    );
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    mainLayout->addWidget(m_searchEdit);

    // ===== 标签云 =====
    m_tagScrollArea = new QScrollArea(this);
    m_tagScrollArea->setWidgetResizable(true);
    m_tagScrollArea->setFixedHeight(40);
    m_tagScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tagScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tagScrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: transparent; }"
    );

    m_tagContainer = new QWidget(m_tagScrollArea);
    m_tagLayout = new QHBoxLayout(m_tagContainer);
    m_tagLayout->setContentsMargins(4, 4, 4, 4);
    m_tagLayout->setSpacing(6);
    m_tagLayout->addStretch();
    m_tagScrollArea->setWidget(m_tagContainer);
    mainLayout->addWidget(m_tagScrollArea);

    // ===== 工具栏 =====
    QWidget *toolbarWidget = new QWidget(this);
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbarWidget);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    toolbarLayout->setSpacing(6);

    m_selectAllCheck = new QCheckBox("全选", this);
    connect(m_selectAllCheck, &QCheckBox::toggled, this, &MainWindow::onSelectAllClicked);
    toolbarLayout->addWidget(m_selectAllCheck);

    m_batchEnableBtn = new QPushButton("启用▼", this);
    m_batchDisableBtn = new QPushButton("禁用", this);
    m_batchDeleteBtn = new QPushButton("删除", this);
    m_batchTagBtn = new QPushButton("打标签", this);

    connect(m_batchEnableBtn, &QPushButton::clicked, this, &MainWindow::onBatchEnableClicked);
    connect(m_batchDisableBtn, &QPushButton::clicked, this, &MainWindow::onBatchDisableClicked);
    connect(m_batchDeleteBtn, &QPushButton::clicked, this, &MainWindow::onBatchDeleteClicked);
    connect(m_batchTagBtn, &QPushButton::clicked, this, &MainWindow::onBatchTagClicked);

    toolbarLayout->addWidget(m_batchEnableBtn);
    toolbarLayout->addWidget(m_batchDisableBtn);
    toolbarLayout->addWidget(m_batchDeleteBtn);
    toolbarLayout->addWidget(m_batchTagBtn);

    toolbarLayout->addStretch();

    m_toggleSizeBtn = new QPushButton("切换到小卡片", this);
    connect(m_toggleSizeBtn, &QPushButton::clicked, this, &MainWindow::onToggleCardSize);
    toolbarLayout->addWidget(m_toggleSizeBtn);

    m_sortCombo = new QComboBox(this);
    m_sortCombo->addItem("按频率▼");
    m_sortCombo->addItem("按名称");
    m_sortCombo->addItem("按导入时间");
    m_sortCombo->addItem("按最后使用");
    connect(m_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSortChanged);
    toolbarLayout->addWidget(m_sortCombo);

    toolbarLayout->addStretch();

    // 刷新按钮
    m_refreshBtn = new QPushButton("🔄 刷新", this);
    m_refreshBtn->setStyleSheet(
        "QPushButton { padding: 4px 12px; border: 1px solid #ccc; "
        "border-radius: 4px; font-size: 12px; }"
        "QPushButton:hover { background-color: #f0f0f0; }"
    );
    m_refreshBtn->setToolTip("重新扫描 agent 路径，同步 skill 到中央库");
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    toolbarLayout->addWidget(m_refreshBtn);

    mainLayout->addWidget(toolbarWidget);

    // ===== 频率筛选栏 =====
    QWidget *freqWidget = new QWidget(this);
    freqWidget->setObjectName("freqBar");
    freqWidget->setStyleSheet("#freqBar { background-color: #f8f9fa; border-bottom: 1px solid #e0e0e0; padding: 6px; }");
    QHBoxLayout *freqLayout = new QHBoxLayout(freqWidget);
    freqLayout->setContentsMargins(8, 4, 8, 4);
    freqLayout->setSpacing(6);

    QLabel *freqLabel = new QLabel("频率筛选：", freqWidget);
    freqLabel->setStyleSheet("font-size: 12px;");
    freqLayout->addWidget(freqLabel);

    QStringList freqNames = {"常用", "一般", "很少", "废弃"};
    for (int i = 0; i < 4; ++i) {
        QPushButton *btn = new QPushButton("■ " + freqNames[i], freqWidget);
        btn->setCheckable(true);
        btn->setProperty("freqValue", i + 1);
        QString color = (i == 0 ? "#4caf50" : (i == 1 ? "#ff9800" : (i == 2 ? "#9e9e9e" : "#f44336")));
        btn->setStyleSheet(
            QString("QPushButton { border: 1px solid %1; border-radius: 12px; "
                    "padding: 3px 10px; color: %1; background-color: transparent; "
                    "font-size: 11px; }"
                    "QPushButton:checked { background-color: %1; color: white; }")
            .arg(color)
        );
        connect(btn, &QPushButton::clicked, this, [this, btn]() {
            onFrequencyFilterClicked(btn);
        });
        m_freqButtons.append(btn);
        freqLayout->addWidget(btn);
    }

    freqLayout->addStretch();
    mainLayout->addWidget(freqWidget);

    // ===== 卡片区域 =====
    m_cardScrollArea = new CardScrollArea(this);
    m_cardScrollArea->setWidgetResizable(true);
    m_cardScrollArea->setStyleSheet(
        "QScrollArea { border: 1px solid #e0e0e0; border-radius: 6px; "
        "background-color: #f5f5f5; }"
    );

    m_cardWidget = new CardWidget(m_cardScrollArea);
    m_cardScrollArea->setWidget(m_cardWidget);

    connect(m_cardScrollArea, &CardScrollArea::viewportClicked, m_cardWidget, &CardWidget::onViewportClicked);

    // 额外保障：在 viewport 上安装事件过滤器
    m_cardScrollArea->viewport()->installEventFilter(m_cardWidget);

    connect(m_cardWidget, &CardWidget::cardClicked, this, &MainWindow::onCardClicked);
    connect(m_cardWidget, &CardWidget::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(m_cardWidget, &CardWidget::frequencyChanged, this, &MainWindow::onFrequencyChanged);
    connect(m_cardWidget, &CardWidget::agentToggled, this, &MainWindow::onAgentToggled);
    connect(m_cardWidget, &CardWidget::tagAddRequested, this, &MainWindow::onCardTagAddRequested);
    connect(m_cardWidget, &CardWidget::tagRemoveRequested, this, &MainWindow::onCardTagRemoveRequested);
    connect(m_cardWidget, &CardWidget::deleteRequested, this, &MainWindow::onCardDeleteRequested);
    connect(m_cardWidget, &CardWidget::openFolderRequested, this, [this](int skillId) {
        if (!m_dbManager) return;
        SkillInfo skill = m_dbManager->getSkill(skillId);
        if (skill.id == -1 || skill.path.isEmpty()) return;
        QDesktopServices::openUrl(QUrl::fromLocalFile(skill.path));
    });
    connect(m_cardWidget, &CardWidget::agentFolderRequested, this, [this](int skillId, int agentId) {
        if (!m_dbManager) return;
        SkillInfo skill = m_dbManager->getSkill(skillId);
        AgentInfo agent = m_dbManager->getAgent(agentId);
        if (skill.id == -1 || agent.id == -1 || agent.path.isEmpty()) return;
        QString folderName = QFileInfo(skill.path).fileName();
        if (folderName.isEmpty()) folderName = skill.name;
        QString agentSkillPath = QDir::cleanPath(agent.path + "/" + folderName);
        QDesktopServices::openUrl(QUrl::fromLocalFile(agentSkillPath));
    });

    mainLayout->addWidget(m_cardScrollArea, 1);

    // ===== 状态栏 =====
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("QLabel { color: #666; font-size: 12px; padding: 4px; }");
    mainLayout->addWidget(m_statusLabel);

    // 创建菜单栏
    QMenuBar *menuBarWidget = new QMenuBar(this);
    QMenu *fileMenu = menuBarWidget->addMenu("文件(&F)");
    QAction *addSkillAction = fileMenu->addAction("添加 Skill...");
    connect(addSkillAction, &QAction::triggered, this, &MainWindow::onAddSkillClicked);
    QAction *backupAction = fileMenu->addAction("备份数据库...");
    connect(backupAction, &QAction::triggered, this, &MainWindow::onBackupClicked);

    QMenu *settingsMenu = menuBarWidget->addMenu("设置(&S)");
    QAction *settingsAction = settingsMenu->addAction("偏好设置...");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onRequestSettings);

    setMenuBar(menuBarWidget);

    // 创建侧边栏
    m_skillSidebar = new SkillSidebar(this);
    m_skillSidebar->setDatabaseManager(m_dbManager);
    m_skillSidebar->hide();
    
    // 连接侧边栏信号
    connect(m_skillSidebar, &SkillSidebar::editRequested, this, &MainWindow::onEditSkillRequested);
    connect(m_skillSidebar, &SkillSidebar::deleteRequested, this, &MainWindow::onDeleteSkillRequested);
    connect(m_skillSidebar, &SkillSidebar::exportRequested, this, &MainWindow::onExportSkillRequested);
    connect(m_skillSidebar, &SkillSidebar::closed, this, &MainWindow::onSidebarClosed);
}

// createTestData / loadSkills / loadTags / loadAgents
// 已移至文件末尾，此处不再重复定义

void MainWindow::onSearchTextChanged(const QString &text)
{
    m_cardWidget->filter(text, m_activeFilterTags);
    updateStatus();
}

void MainWindow::onTagClicked(const QString &tag)
{
    if (m_activeFilterTags.contains(tag)) {
        m_activeFilterTags.removeAll(tag);
    } else {
        m_activeFilterTags.append(tag);
    }
    updateTagButtons();
    m_cardWidget->filter(m_searchEdit->text(), m_activeFilterTags);
    updateStatus();
}

void MainWindow::onSelectAllClicked()
{
    bool checked = m_selectAllCheck->isChecked();
    m_cardWidget->selectAll(checked);
    updateStatus();
}

void MainWindow::onBatchEnableClicked()
{
    QVector<int> ids = m_cardWidget->getSelectedSkillIds();
    if (ids.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择 skill");
        return;
    }

    // 选择目标 Agent
    QStringList agentNames;
    QVector<int> agentIds;
    for (const AgentInfo &agent : m_allAgents) {
        if (agent.enabled && !agent.path.isEmpty() && QDir(agent.path).exists()) {
            agentNames << agent.name;
            agentIds << agent.id;
        }
    }

    if (agentNames.isEmpty()) {
        QMessageBox::warning(this, "警告", "没有可用的 Agent（请先在设置中配置并启用 Agent）");
        return;
    }

    bool ok;
    QString agentName = QInputDialog::getItem(this, "批量启用",
        QString("将 %1 个 skill 启用到哪个 Agent？").arg(ids.size()),
        agentNames, 0, false, &ok);
    if (!ok) {
        return;
    }

    int agentIndex = agentNames.indexOf(agentName);
    int agentId = agentIds[agentIndex];

    int success = 0;
    for (int skillId : ids) {
        if (m_agentManager->enableSkillForAgent(skillId, agentId)) {
            success++;
        }
    }

    QMessageBox::information(this, "批量启用",
        QString("成功启用 %1/%2 个 skill 到 %3").arg(success).arg(ids.size()).arg(agentName));
    loadSkills();
}

void MainWindow::onBatchDisableClicked()
{
    QVector<int> ids = m_cardWidget->getSelectedSkillIds();
    if (ids.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择 skill");
        return;
    }

    // 选择目标 Agent
    QStringList agentNames;
    QVector<int> agentIds;
    for (const AgentInfo &agent : m_allAgents) {
        if (agent.enabled && !agent.path.isEmpty() && QDir(agent.path).exists()) {
            agentNames << agent.name;
            agentIds << agent.id;
        }
    }

    if (agentNames.isEmpty()) {
        QMessageBox::warning(this, "警告", "没有可用的 Agent");
        return;
    }

    bool ok;
    QString agentName = QInputDialog::getItem(this, "批量禁用",
        QString("将 %1 个 skill 从哪个 Agent 禁用？").arg(ids.size()),
        agentNames, 0, false, &ok);
    if (!ok) {
        return;
    }

    int agentIndex = agentNames.indexOf(agentName);
    int agentId = agentIds[agentIndex];

    int success = 0;
    for (int skillId : ids) {
        if (m_agentManager->disableSkillForAgent(skillId, agentId)) {
            success++;
        }
    }

    QMessageBox::information(this, "批量禁用",
        QString("成功禁用 %1/%2 个 skill 从 %3").arg(success).arg(ids.size()).arg(agentName));
    loadSkills();
}

void MainWindow::onBatchDeleteClicked()
{
    QVector<int> ids = m_cardWidget->getSelectedSkillIds();
    if (ids.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择 skill");
        return;
    }

    auto reply = QMessageBox::question(this, "确认删除",
        QString("确定要删除 %1 个 skill 吗？\n（将同时删除中央库中的文件夹）").arg(ids.size()),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    int success = 0;
    for (int skillId : ids) {
        if (m_skillManager->deleteSkill(skillId, true)) {
            success++;
        }
    }

    QMessageBox::information(this, "批量删除",
        QString("成功删除 %1/%2 个 skill").arg(success).arg(ids.size()));
    loadSkills();
    loadTags();
}

void MainWindow::onBatchTagClicked()
{
    QVector<int> ids = m_cardWidget->getSelectedSkillIds();
    if (ids.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择 skill");
        return;
    }

    bool ok;
    QString tag = QInputDialog::getText(this, "批量打标签",
        QString("输入标签名称（将添加到 %1 个 skill）：").arg(ids.size()),
        QLineEdit::Normal, "", &ok);
    if (!ok || tag.trimmed().isEmpty()) {
        return;
    }

    QString trimmed = tag.trimmed();
    int tagId = m_dbManager->getOrCreateTag(trimmed);

    int success = 0;
    for (int skillId : ids) {
        if (m_dbManager->addSkillTag(skillId, tagId)) {
            success++;
        }
    }

    QMessageBox::information(this, "批量打标签",
        QString("成功给 %1/%2 个 skill 添加标签「%3」").arg(success).arg(ids.size()).arg(trimmed));
    loadSkills();
    loadTags();
}

void MainWindow::onToggleCardSize()
{
    m_largeMode = !m_largeMode;
    m_cardWidget->setCardSize(m_largeMode);
    m_toggleSizeBtn->setText(m_largeMode ? "切换到小卡片" : "切换到大卡片");
}

void MainWindow::onSortChanged(int index)
{
    QStringList criteria = {"frequency", "name", "createdAt", "lastUsed"};
    if (index >= 0 && index < criteria.size()) {
        m_cardWidget->sortBy(criteria[index]);
    }
}

void MainWindow::onCardClicked(int skillId)
{
    emit skillCardClicked(skillId);
    // 打开侧边栏显示详情
    openSkillSidebar(skillId);
}

void MainWindow::openSkillSidebar(int skillId)
{
    if (m_skillSidebar) {
        m_skillSidebar->loadSkill(skillId);
    }
}

void MainWindow::onEditSkillRequested(int skillId)
{
    Q_UNUSED(skillId);
    QMessageBox::information(this, "编辑 Skill", "编辑功能待实现");
    // TODO: 打开编辑对话框
}

void MainWindow::onDeleteSkillRequested(int skillId)
{
    auto reply = QMessageBox::question(this, "确认删除",
        QString("确定要删除 Skill ID: %1 吗？").arg(skillId),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_dbManager) {
            m_dbManager->deleteSkill(skillId);
            loadSkills();  // 重新加载
        }
        if (m_skillSidebar) {
            m_skillSidebar->close();
        }
    }
}

void MainWindow::onExportSkillRequested(int skillId)
{
    if (!m_skillManager) {
        QMessageBox::warning(this, "警告", "SkillManager 未初始化");
        return;
    }

    SkillInfo skill = m_dbManager->getSkill(skillId);
    if (skill.id == -1) {
        QMessageBox::warning(this, "错误", "Skill 不存在");
        return;
    }

    QString defaultName = skill.name + ".zip";
    QString filePath = QFileDialog::getSaveFileName(this,
        "导出 Skill", defaultName, "ZIP 文件 (*.zip)");

    if (filePath.isEmpty()) {
        return;
    }

    if (m_skillManager->exportToZip(skillId, filePath)) {
        QMessageBox::information(this, "导出成功",
            QString("Skill 已导出到：\n%1").arg(filePath));
    } else {
        QMessageBox::warning(this, "导出失败", "导出过程中出现错误");
    }
}

void MainWindow::onSidebarClosed()
{
    // 侧边栏已关闭
    // 可以在这里添加额外的清理逻辑
}

void MainWindow::onRequestSettings()
{
    if (!m_dbManager) {
        QMessageBox::warning(this, "警告", "数据库未初始化");
        return;
    }
    SettingsDialog dialog(m_dbManager, m_agentManager, this);

    // 连接 agentsChanged 信号以刷新 agent 列表和卡片
    connect(&dialog, &SettingsDialog::agentsChanged, this, [this]() {
        loadAgents();
        loadSkills();
    });

    dialog.exec();
}

void MainWindow::onManageTagsClicked()
{
    if (!m_dbManager) {
        QMessageBox::warning(this, "警告", "数据库未初始化");
        return;
    }
    TagManagerDialog dialog(m_dbManager, this);
    dialog.exec();
    // 标签变化后刷新标签云
    loadTags();
}

void MainWindow::onAddSkillClicked()
{
    if (!m_dbManager || !m_skillManager) {
        QMessageBox::warning(this, "警告", "系统未就绪");
        return;
    }

    QStringList methods = {"从文件夹导入", "从 GitHub 链接导入"};
    bool ok;
    QString method = QInputDialog::getItem(this, "添加 Skill", "选择导入方式：",
        methods, 0, false, &ok);
    if (!ok) return;

    if (method == "从文件夹导入") {
        // ===== 从文件夹导入 =====
        QString folderPath = QFileDialog::getExistingDirectory(this,
            "选择 Skill 文件夹（包含 SKILL.md）",
            QDir::homePath(),
            QFileDialog::ShowDirsOnly);

        if (folderPath.isEmpty()) {
            return;
        }

        // 检查 SKILL.md 是否存在
        QString skillMdPath = QDir::cleanPath(folderPath + "/SKILL.md");
        if (!QFile::exists(skillMdPath)) {
            QMessageBox::warning(this, "导入失败",
                "导入失败，请确认文件夹中包含 SKILL.md 文件");
            return;
        }

        // 获取文件夹名作为 skill 名称
        QFileInfo folderInfo(folderPath);
        QString skillName = folderInfo.fileName();

        // 检查重复
        if (m_skillManager->checkDuplicate(skillName)) {
            QStringList options;
            options << "覆盖" << "重命名" << "取消";
            bool ok2;
            QString choice = QInputDialog::getItem(this, "Skill 已存在",
                QString("名为「%1」的 skill 已存在，请选择操作：").arg(skillName),
                options, 0, false, &ok2);

            if (!ok2 || choice == "取消") {
                return;
            }
            if (choice == "覆盖") {
                int skillId = m_skillManager->importFromFolder(folderPath, true);
                if (skillId == -1) {
                    QMessageBox::warning(this, "导入失败", "导入失败，请确认文件夹中包含 SKILL.md 文件");
                    return;
                }
                QMessageBox::information(this, "成功", QString("Skill 已覆盖导入！ID: %1").arg(skillId));
                loadSkills();
                loadTags();
                return;
            }
        }

        // 正常导入
        int skillId = m_skillManager->importFromFolder(folderPath);

        if (skillId == -1) {
            QMessageBox::warning(this, "导入失败",
                "导入失败，请确认文件夹中包含 SKILL.md 文件");
            return;
        }

        QMessageBox::information(this, "成功",
            QString("Skill 已成功导入！ID: %1").arg(skillId));
    } else {
        // ===== 从 GitHub 链接导入（异步，防止卡 UI）=====
        QString url = QInputDialog::getText(this, "从 GitHub 导入",
            "输入 GitHub 仓库 URL：", QLineEdit::Normal,
            "https://github.com/", &ok);
        if (!ok || url.trimmed().isEmpty()) return;

        QString trimmedUrl = url.trimmed();

        // 禁用添加菜单项，避免重复操作
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_statusLabel->setText("正在从 GitHub 导入，请稍候...");

        QFutureWatcher<int> *watcher = new QFutureWatcher<int>(this);
        connect(watcher, &QFutureWatcher<int>::finished, this, [this, watcher]() {
            QApplication::restoreOverrideCursor();
            int skillId = watcher->result();
            watcher->deleteLater();

            if (skillId == -1) {
                QMessageBox::warning(this, "导入失败",
                    "从 GitHub 导入失败，请检查 URL 是否正确，"
                    "以及网络连接是否正常。");
            } else {
                QMessageBox::information(this, "成功",
                    QString("Skill 已成功从 GitHub 导入！ID: %1").arg(skillId));
            }

            loadSkills();
            loadTags();
            m_statusLabel->setText(QString("共 %1 个 skill").arg(m_allSkills.size()));
        });

        SkillManager *skillManager = m_skillManager; // 捕获指针
        watcher->setFuture(QtConcurrent::run([skillManager, trimmedUrl]() {
            return skillManager->importFromGitHub(trimmedUrl);
        }));
        return; // 异步导入，watcher 完成时会自动 reload
    }

    loadSkills();
    loadTags();
}

void MainWindow::onBackupClicked()
{
    if (!m_dbManager) {
        QMessageBox::warning(this, "警告", "数据库未初始化");
        return;
    }

    QString backupDir = "D:/skillLibrary/backups";
    QDir dir(backupDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString backupPath = backupDir + "/backup_" + timestamp + ".db";

    if (m_dbManager->exportBackup(backupPath)) {
        QMessageBox::information(this, "备份成功",
            QString("数据库已备份到：\n%1").arg(backupPath));
    } else {
        QMessageBox::warning(this, "备份失败", "无法创建备份文件");
    }
}

void MainWindow::onFrequencyFilterClicked(QPushButton *btn)
{
    int freqValue = btn->property("freqValue").toInt();

    if (btn->isChecked()) {
        if (!m_activeFilterFrequencies.contains(freqValue)) {
            m_activeFilterFrequencies.append(freqValue);
        }
    } else {
        m_activeFilterFrequencies.removeAll(freqValue);
    }

    m_cardWidget->filterByFrequency(m_activeFilterFrequencies);
    updateStatus();
}

void MainWindow::onSelectionChanged(int count)
{
    Q_UNUSED(count);
    updateStatus();
}

void MainWindow::onFrequencyChanged(int skillId, int newFrequency)
{
    // 写入数据库
    if (m_dbManager) {
        QVariantMap fields;
        fields["frequency"] = newFrequency;
        m_dbManager->updateSkill(skillId, fields);
    }

    // 更新内存数据
    for (SkillInfo &skill : m_allSkills) {
        if (skill.id == skillId) {
            skill.frequency = newFrequency;
            break;
        }
    }

    // 原地更新卡片（不跳动）
    m_cardWidget->updateSkill(skillId, m_dbManager->getSkill(skillId));

    updateStatus();
}

void MainWindow::onAgentToggled(int skillId, int agentId, bool enabled)
{
    if (!m_agentManager) {
        return;
    }

    if (enabled) {
        if (!m_agentManager->enableSkillForAgent(skillId, agentId)) {
            QMessageBox::warning(this, "操作失败",
                QString("无法将 Skill 复制到 Agent 目录"));
            return;
        }
    } else {
        if (!m_agentManager->disableSkillForAgent(skillId, agentId)) {
            QMessageBox::warning(this, "操作失败",
                QString("无法从 Agent 目录移除 Skill"));
            return;
        }
    }

    // 更新内存中的 enabledAgentIds
    for (SkillInfo &skill : m_allSkills) {
        if (skill.id == skillId) {
            if (enabled) {
                if (!skill.enabledAgentIds.contains(agentId)) {
                    skill.enabledAgentIds.append(agentId);
                }
            } else {
                skill.enabledAgentIds.removeAll(agentId);
            }
            break;
        }
    }

    // 原地更新卡片（不跳动）
    m_cardWidget->updateSkill(skillId, m_dbManager->getSkill(skillId));
}

void MainWindow::onCardTagAddRequested(int skillId)
{
    if (!m_dbManager) return;

    SkillInfo skill = m_dbManager->getSkill(skillId);
    if (skill.id == -1) return;

    // 获取所有已有标签
    QVector<TagInfo> allTags = m_dbManager->getTags();
    QStringList tagNames;
    for (const TagInfo &t : allTags) {
        if (!skill.tags.contains(t.name)) {
            tagNames << t.name;
        }
    }

    bool ok;
    QString tag;
    if (!tagNames.isEmpty()) {
        // 有已有标签时，提供选择 + 自定义输入
        tagNames.prepend("(输入新标签)");
        tag = QInputDialog::getItem(this, "添加标签",
            QString("为「%1」添加标签：").arg(skill.name),
            tagNames, 0, false, &ok);
        if (!ok) return;
        if (tag == "(输入新标签)") {
            tag = QInputDialog::getText(this, "添加标签",
                "输入新标签名称：", QLineEdit::Normal, "", &ok);
            if (!ok || tag.trimmed().isEmpty()) return;
            tag = tag.trimmed();
        }
    } else {
        tag = QInputDialog::getText(this, "添加标签",
            QString("为「%1」输入标签名称：").arg(skill.name),
            QLineEdit::Normal, "", &ok);
        if (!ok || tag.trimmed().isEmpty()) return;
        tag = tag.trimmed();
    }

    int tagId = m_dbManager->getOrCreateTag(tag);
    if (tagId > 0) {
        m_dbManager->addSkillTag(skillId, tagId);
    }

    m_cardWidget->updateSkill(skillId, m_dbManager->getSkill(skillId));
    loadTags();
}

void MainWindow::onCardTagRemoveRequested(int skillId, const QString &tag)
{
    if (!m_dbManager) return;

    SkillInfo skill = m_dbManager->getSkill(skillId);
    if (skill.id == -1) return;

    auto reply = QMessageBox::question(this, "确认删除标签",
        QString("确定要从「%1」移除标签「%2」吗？").arg(skill.name, tag),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    QVector<TagInfo> allTags = m_dbManager->getTags();
    for (const TagInfo &t : allTags) {
        if (t.name == tag) {
            m_dbManager->removeSkillTag(skillId, t.id);
            break;
        }
    }

    m_cardWidget->updateSkill(skillId, m_dbManager->getSkill(skillId));
    loadTags();
}

void MainWindow::onCardDeleteRequested(int skillId)
{
    if (!m_dbManager || !m_skillManager) return;

    SkillInfo skill = m_dbManager->getSkill(skillId);
    if (skill.id == -1) return;

    auto reply = QMessageBox::question(this, "确认删除",
        QString("确定要删除 Skill「%1」吗？\n将同时删除中央库中的文件夹和所有 Agent 中的副本。").arg(skill.name),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    m_skillManager->deleteSkill(skillId, true);
    loadSkills();
    loadTags();
}

void MainWindow::updateStatus()
{
    int total = m_allSkills.size();
    int selected = m_cardWidget->getSelectedCount();

    QString filterInfo = "";
    if (!m_activeFilterTags.isEmpty()) {
        filterInfo = QString(" | 当前筛选：%1").arg(m_activeFilterTags.join(", "));
    }
    if (!m_searchEdit->text().isEmpty()) {
        filterInfo += QString(" 关键词：%1").arg(m_searchEdit->text());
    }

    m_statusLabel->setText(QString("共 %1 个 skill | 已选中 %2 个%3")
        .arg(total).arg(selected).arg(filterInfo));
}

void MainWindow::updateTagButtons()
{
    // 清除旧按钮
    QLayoutItem *item;
    while ((item = m_tagLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->hide();
            item->widget()->setParent(nullptr);
            delete item->widget();
        }
        delete item;
    }
    m_tagButtons.clear();

    // 创建标签按钮
    for (const QString &tag : m_allTags) {
        QPushButton *btn = new QPushButton("#" + tag, m_tagContainer);
        btn->setCheckable(true);
        btn->setChecked(m_activeFilterTags.contains(tag));
        btn->setStyleSheet(
            "QPushButton { border: 1px solid #4a90d9; border-radius: 12px; "
            "padding: 3px 10px; color: #4a90d9; background-color: transparent; "
            "font-size: 11px; }"
            "QPushButton:checked { background-color: #4a90d9; color: white; }"
            "QPushButton:hover { background-color: #e8f0fe; }"
        );
        QString tagCopy = tag;
        connect(btn, &QPushButton::clicked, this, [this, tagCopy]() {
            onTagClicked(tagCopy);
        });
        m_tagLayout->addWidget(btn);
        m_tagButtons.append(btn);
    }

    // 管理标签按钮
    m_manageTagsButton = new QPushButton("管理标签", m_tagContainer);
    m_manageTagsButton->setStyleSheet(
        "QPushButton { border: 1px dashed #999; border-radius: 12px; "
        "padding: 3px 10px; color: #999; font-size: 11px; }"
        "QPushButton:hover { border: 1px solid #4a90d9; color: #4a90d9; }"
    );
    connect(m_manageTagsButton, &QPushButton::clicked, this, &MainWindow::onManageTagsClicked);
    m_tagLayout->addWidget(m_manageTagsButton);
    m_tagLayout->addStretch();
}

void MainWindow::createTestData()
{
    if (!m_dbManager) {
        return;
    }

    // 创建测试 skill 目录（确保路径存在，避免 validateSkills 误删）
    QStringList testSkills = {"web-access", "academic-paper", "huashu-design", "deep-research", "pdf"};
    for (const QString &name : testSkills) {
        QDir().mkpath("D:/skillLibrary/skills/" + name);
    }

    // 创建测试 skill
    int id1 = m_dbManager->addSkill("web-access", "D:/skillLibrary/skills/web-access", "联网搜索与网页抓取");
    int id2 = m_dbManager->addSkill("academic-paper", "D:/skillLibrary/skills/academic-paper", "学术论文写作");
    int id3 = m_dbManager->addSkill("huashu-design", "D:/skillLibrary/skills/huashu-design", "高保真原型设计");
    int id4 = m_dbManager->addSkill("deep-research", "D:/skillLibrary/skills/deep-research", "深度研究");
    int id5 = m_dbManager->addSkill("pdf", "D:/skillLibrary/skills/pdf", "PDF 处理");

    // 添加标签
    int tagId1 = m_dbManager->addTag("联网");
    int tagId2 = m_dbManager->addTag("学术");
    int tagId3 = m_dbManager->addTag("设计");
    int tagId4 = m_dbManager->addTag("研究");
    int tagId5 = m_dbManager->addTag("PDF");

    // 关联标签（简单方式：直接执行 SQL）
    if (id1 > 0) m_dbManager->addSkillTag(id1, tagId1);
    if (id2 > 0) m_dbManager->addSkillTag(id2, tagId2);
    if (id3 > 0) m_dbManager->addSkillTag(id3, tagId3);
    if (id4 > 0) m_dbManager->addSkillTag(id4, tagId4);
    if (id5 > 0) {
        m_dbManager->addSkillTag(id5, tagId5);
        m_dbManager->addSkillTag(id5, tagId1);
    }
}

void MainWindow::loadSkills()
{
    if (!m_dbManager) {
        return;
    }

    // 暂不清理孤儿记录（避免误删测试数据）
    // 后续通过扫描 agent 目录自动修复
    m_allSkills = m_dbManager->getAllSkills("frequency");

    m_cardWidget->setSkills(m_allSkills);

    // 传递 agent 列表给卡片
    m_allAgents = m_dbManager->getAllAgents();
    m_cardWidget->setAgentList(m_allAgents);

    updateStatus();
}

void MainWindow::loadTags()
{
    if (!m_dbManager) {
        return;
    }

    m_allTags = m_dbManager->getAllTags();
    updateTagButtons();
}

void MainWindow::loadAgents()
{
    if (!m_dbManager) {
        return;
    }

    m_allAgents = m_dbManager->getAllAgents();
    // Agent 筛选栏已移除，无需动态创建按钮
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭时自动备份数据库
    if (m_dbManager) {
        bool autoBackup = m_dbManager->getSetting("autoBackup", "true") == "true";
        if (autoBackup) {
            QString backupDir = "D:/skillLibrary/backups";
            QDir dir(backupDir);
            if (!dir.exists()) {
                dir.mkpath(".");
            }
            QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
            QString backupPath = backupDir + "/auto_backup_" + timestamp + ".db";
            m_dbManager->exportBackup(backupPath);
        }
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::onRefreshClicked()
{
    if (!m_agentManager || !m_dbManager) {
        QMessageBox::warning(this, "警告", "系统未就绪，请稍后再试");
        return;
    }

    qInfo() << "Manual refresh triggered";

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_statusLabel->setText("正在扫描 agent 路径...");
    QApplication::processEvents();  // 让 UI 更新

    QVector<int> scannedAgents = m_agentManager->scanAllAgents();

    loadSkills();

    m_statusLabel->setText(QString("扫描完成，已处理 %1 个 agent，共 %2 个 skill")
                          .arg(scannedAgents.size()).arg(m_allSkills.size()));

    QApplication::restoreOverrideCursor();

    qInfo() << "Manual refresh done, scanned" << scannedAgents.size() << "agents";
}

// ============ 拖拽事件 ============

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (!m_skillManager) {
        QMessageBox::warning(this, "警告", "SkillManager 未初始化");
        return;
    }

    QVector<QString> paths;
    QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl &url : urls) {
        paths.append(url.toLocalFile());
    }

    if (paths.isEmpty()) {
        return;
    }

    qInfo() << "Drag-drop: got" << paths.size() << "items";
    m_statusLabel->setText(QString("正在导入 %1 个文件/文件夹...").arg(paths.size()));
    QApplication::processEvents();

    int imported = m_skillManager->importByDragDrop(paths);

    qInfo() << "Drag-drop import done:" << imported << "skills imported";
    loadSkills();
    loadTags();
    m_statusLabel->setText(QString("拖拽导入完成，共导入 %1 个 skill").arg(imported));
}

// 主题功能已移除，仅保留浅色模式
