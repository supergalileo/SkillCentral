#include "ui/MainWindow.h"
#include "ui/CardWidget.h"
#include "ui/SkillCard.h"
#include "ui/SkillSidebar.h"
#include "ui/SettingsDialog.h"
#include "models/AgentManager.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_dbManager(nullptr)
    , m_agentManager(nullptr)
    , m_largeMode(true)
    , m_agentFilterLayout(nullptr)
{
    setupUi();
    setWindowTitle("SkillCentral");
    resize(1000, 700);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setDatabaseManager(DatabaseManager *dbManager)
{
    m_dbManager = dbManager;

    // 创建 AgentManager
    if (!m_agentManager) {
        m_agentManager = new AgentManager(m_dbManager, this);
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
    } else {
        loadSkills();
        loadTags();
        loadAgents();
    }

    // 启动时自动扫描 agent 路径（安全调用，失败不崩溃）
    if (m_agentManager) {
        bool autoScan = true;
        if (m_dbManager) {
            autoScan = m_dbManager->getSetting("autoScan", "true") == "true";
        }
        if (autoScan) {
            QTimer::singleShot(500, this, [this]() {
                if (m_agentManager) {
                    m_agentManager->scanAllAgents();
                }
            });
        }
    }
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

    m_batchEnableBtn = new QPushButton("批量启用▼", this);
    m_batchDisableBtn = new QPushButton("批量禁用", this);
    m_batchDeleteBtn = new QPushButton("批量删除", this);
    m_batchTagBtn = new QPushButton("批量打标签", this);

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

    mainLayout->addWidget(toolbarWidget);

    // ===== 频率筛选栏 =====
    QWidget *freqWidget = new QWidget(this);
    freqWidget->setStyleSheet("background-color: #f8f9fa; border-bottom: 1px solid #e0e0e0; padding: 6px;");
    QHBoxLayout *freqLayout = new QHBoxLayout(freqWidget);
    freqLayout->setContentsMargins(8, 4, 8, 4);
    freqLayout->setSpacing(6);

    QLabel *freqLabel = new QLabel("频率筛选：", freqWidget);
    freqLabel->setStyleSheet("color: #666; font-size: 12px;");
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

    // ===== Agent 筛选栏 =====
    QWidget *agentFilterWidget = new QWidget(this);
    agentFilterWidget->setStyleSheet("background-color: #f0f4ff; border-bottom: 1px solid #d0e0ff; padding: 6px;");
    QHBoxLayout *agentFilterLayout = new QHBoxLayout(agentFilterWidget);
    agentFilterLayout->setContentsMargins(8, 4, 8, 4);
    agentFilterLayout->setSpacing(6);

    QLabel *agentFilterLabel = new QLabel("Agent筛选：", agentFilterWidget);
    agentFilterLabel->setStyleSheet("color: #666; font-size: 12px;");
    agentFilterLayout->addWidget(agentFilterLabel);

    agentFilterLayout->addStretch();
    mainLayout->addWidget(agentFilterWidget);

    // 存储布局指针，供 loadAgents() 使用
    m_agentFilterLayout = agentFilterLayout;

    // ===== 卡片区域 =====
    m_cardScrollArea = new QScrollArea(this);
    m_cardScrollArea->setWidgetResizable(true);
    m_cardScrollArea->setStyleSheet(
        "QScrollArea { border: 1px solid #e0e0e0; border-radius: 6px; "
        "background-color: #f5f5f5; }"
    );

    m_cardWidget = new CardWidget(m_cardScrollArea);
    m_cardScrollArea->setWidget(m_cardWidget);

    connect(m_cardWidget, &CardWidget::cardClicked, this, &MainWindow::onCardClicked);
    connect(m_cardWidget, &CardWidget::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(m_cardWidget, &CardWidget::frequencyChanged, this, &MainWindow::onFrequencyChanged);
    connect(m_cardWidget, &CardWidget::agentToggled, this, &MainWindow::onAgentToggled);

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

    // 使用测试数据
    createTestData();
    
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
    // 实现全选/取消全选
    bool checked = m_selectAllCheck->isChecked();
    // 这里需要 CardWidget 提供全选接口
    // 暂时通过遍历卡片实现
    updateStatus();
}

void MainWindow::onBatchEnableClicked()
{
    QVector<int> ids = m_cardWidget->getSelectedSkillIds();
    if (ids.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择 skill");
        return;
    }
    // TODO: 实现批量启用
    QMessageBox::information(this, "批量启用", QString("将启用 %1 个 skill").arg(ids.size()));
}

void MainWindow::onBatchDisableClicked()
{
    QVector<int> ids = m_cardWidget->getSelectedSkillIds();
    if (ids.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择 skill");
        return;
    }
    // TODO: 实现批量禁用
    QMessageBox::information(this, "批量禁用", QString("将禁用 %1 个 skill").arg(ids.size()));
}

void MainWindow::onBatchDeleteClicked()
{
    QVector<int> ids = m_cardWidget->getSelectedSkillIds();
    if (ids.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择 skill");
        return;
    }
    auto reply = QMessageBox::question(this, "确认删除",
        QString("确定要删除 %1 个 skill 吗？").arg(ids.size()),
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // TODO: 实现批量删除
        QMessageBox::information(this, "批量删除", "删除功能待实现");
    }
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
        "输入标签名称：", QLineEdit::Normal, "", &ok);
    if (ok && !tag.isEmpty()) {
        // TODO: 实现批量打标签
        QMessageBox::information(this, "批量打标签",
            QString("将给 %1 个 skill 添加标签: %2").arg(ids.size()).arg(tag));
    }
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
    Q_UNUSED(skillId);
    QMessageBox::information(this, "导出 Skill", "导出功能待实现");
    // TODO: 实现导出功能
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

    // 连接 agentsChanged 信号以刷新 agent 列表
    connect(&dialog, &SettingsDialog::agentsChanged, this, [this]() {
        loadAgents();
        m_cardWidget->setAgentList(m_allAgents);
    });

    dialog.exec();
}

void MainWindow::onManageTagsClicked()
{
    if (!m_dbManager) {
        QMessageBox::warning(this, "警告", "数据库未初始化");
        return;
    }
    // TODO: 创建 TagManagerDialog 并打开
    QMessageBox::information(this, "管理标签", "标签管理功能待实现");
}

void MainWindow::onAddSkillClicked()
{
    if (!m_dbManager) {
        QMessageBox::warning(this, "警告", "数据库未初始化");
        return;
    }

    QString folderPath = QFileDialog::getExistingDirectory(this,
        "选择 Skill 文件夹（包含 SKILL.md）",
        QDir::homePath(),
        QFileDialog::ShowDirsOnly);

    if (folderPath.isEmpty()) {
        return;
    }

    // 检查 SKILL.md 是否存在
    if (!QFile::exists(folderPath + "/SKILL.md")) {
        QMessageBox::warning(this, "错误",
            "所选文件夹中没有 SKILL.md 文件。\n"
            "请选择一个包含 SKILL.md 的 skill 文件夹。");
        return;
    }

    // 获取 skill 名称
    QFileInfo fi(folderPath);
    QString skillName = fi.fileName();

    // 检查是否已存在
    QVector<SkillInfo> existing = m_dbManager->searchSkills(skillName);
    for (const SkillInfo &s : existing) {
        if (s.name == skillName) {
            auto reply = QMessageBox::question(this, "重复检测",
                QString("Skill '%1' 已存在，是否覆盖？").arg(skillName),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if (reply == QMessageBox::Yes) {
                m_dbManager->updateSkill(s.id, {{"path", folderPath}});
            }
            loadSkills();
            return;
        }
    }

    // 添加新 skill
    int id = m_dbManager->addSkill(skillName, folderPath, "");
    if (id > 0) {
        QMessageBox::information(this, "成功",
            QString("Skill '%1' 已成功导入！ID: %2").arg(skillName).arg(id));
        loadSkills();
    } else {
        QMessageBox::warning(this, "错误", "导入失败：无法添加到数据库");
    }
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

void MainWindow::onAgentFilterClicked(QPushButton *btn)
{
    int agentId = btn->property("agentId").toInt();

    if (btn->isChecked()) {
        if (!m_activeFilterAgentIds.contains(agentId)) {
            m_activeFilterAgentIds.append(agentId);
        }
    } else {
        m_activeFilterAgentIds.removeAll(agentId);
    }

    m_cardWidget->filterByAgents(m_activeFilterAgentIds);
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

    // 刷新卡片显示
    m_cardWidget->setSkills(m_allSkills);

    updateStatus();
}

void MainWindow::onAgentToggled(int skillId, int agentId, bool enabled)
{
    if (!m_agentManager) {
        return;
    }

    if (enabled) {
        // 点亮 → 复制 skill 到 agent 目录
        if (!m_agentManager->enableSkillForAgent(skillId, agentId)) {
            QMessageBox::warning(this, "操作失败",
                QString("无法将 Skill 复制到 Agent 目录"));
            return;
        }
    } else {
        // 点灭 → 从 agent 目录删除 skill
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

    // 刷新卡片显示
    m_cardWidget->setSkills(m_allSkills);
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

    // 用存储的布局指针直接操作，无需遍历
    if (!m_agentFilterLayout) {
        return;
    }

    // 清除旧按钮（保留 label 和 stretch）
    while (m_agentFilterLayout->count() > 1) {
        QLayoutItem *item = m_agentFilterLayout->takeAt(1);
        if (item->widget()) delete item->widget();
        delete item;
    }
    m_agentFilterButtons.clear();

    // 为每个 agent 创建筛选按钮
    for (const AgentInfo &agent : m_allAgents) {
        QPushButton *btn = new QPushButton(agent.name);
        btn->setCheckable(true);
        btn->setProperty("agentId", agent.id);
        btn->setStyleSheet(
            QString("QPushButton { border: 1px solid #4a90d9; border-radius: 12px; "
                    "padding: 3px 10px; color: #4a90d9; background-color: transparent; "
                    "font-size: 11px; }"
                    "QPushButton:checked { background-color: #4a90d9; color: white; }")
        );
        connect(btn, &QPushButton::clicked, this, [this, btn]() {
            onAgentFilterClicked(btn);
        });
        m_agentFilterLayout->insertWidget(m_agentFilterLayout->count() - 1, btn);
        m_agentFilterButtons.append(btn);
    }
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
