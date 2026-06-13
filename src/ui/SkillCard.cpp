#include "ui/SkillCard.h"
#include "database/DatabaseManager.h"
#include <QMouseEvent>
#include <QPainter>
#include <QFont>
#include <QPushButton>
#include <QDir>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QInputDialog>

// ============================================================================
// 构造/析构
// ============================================================================

SkillCard::SkillCard(const SkillInfo &skill, QWidget *parent)
    : QWidget(parent)
    , m_skillId(skill.id)
    , m_name(skill.name)
    , m_description(skill.description)
    , m_frequency(skill.frequency)
    , m_tags(skill.tags)
    , m_enabledAgentIds(skill.enabledAgentIds)
    , m_largeMode(true)
{
    // 必须设置此属性，自定义 QWidget 的样式表（border/background）才能生效
    setAttribute(Qt::WA_StyledBackground, true);

    // 所有控件一次性创建，之后只更新数据不复用不删除
    setupUi();
    applyStyle();
    updateData();
    arrangeLayout();
}

SkillCard::~SkillCard()
{
}

// ============================================================================
// 控件创建（只调用一次）
// ============================================================================

void SkillCard::setupUi()
{
    // --- 成员控件（生命周期 = 卡片生命周期）---
    m_checkBox = new QCheckBox(this);
    connect(m_checkBox, &QCheckBox::toggled, this, [this](bool checked) {
        emit checkBoxToggled(m_skillId, checked);
    });

    m_nameLabel = new QLabel(this);
    m_nameLabel->setTextFormat(Qt::PlainText);

    m_descLabel = new QLabel(this);
    m_descLabel->setTextFormat(Qt::PlainText);
    m_descLabel->setWordWrap(true);

    m_freqButton = new QPushButton(this);
    m_freqButton->setFixedSize(20, 20);
    m_freqButton->setCursor(Qt::PointingHandCursor);
    m_freqButton->setToolTip("点击选择使用频率");
    connect(m_freqButton, &QPushButton::clicked, this, &SkillCard::showFrequencyMenu);

    m_freqLabel = new QLabel(this);
    m_freqLabel->setCursor(Qt::PointingHandCursor);
    m_freqLabel->setToolTip("点击选择使用频率");
    connect(m_freqLabel, &QLabel::linkActivated, this, [this]() { showFrequencyMenu(); });

    m_tagsWidget = new QWidget(this);
    m_tagsLayout = new QHBoxLayout(m_tagsWidget);
    m_tagsLayout->setContentsMargins(0, 0, 0, 0);
    m_tagsLayout->setSpacing(3);

    m_agentsWidget = new QWidget(this);
    m_agentsLayout = new QHBoxLayout(m_agentsWidget);
    m_agentsLayout->setContentsMargins(0, 0, 0, 0);
    m_agentsLayout->setSpacing(3);

    m_smallIconLabel = new QLabel(this);
    m_smallIconLabel->setAlignment(Qt::AlignCenter);
}

// ============================================================================
// 样式（只调用一次 + 切换模式时）
// ============================================================================

void SkillCard::applyStyle()
{
    setStyleSheet(R"(
        SkillCard {
            background-color: #ffffff;
            border: 1px solid #d0d0d0;
            border-radius: 8px;
        }
        SkillCard:hover {
            border: 1px solid #4a90d9;
            background-color: #f8f9ff;
        }
    )");
}

// ============================================================================
// 数据更新（刷新内容时不重建控件）
// ============================================================================

void SkillCard::updateData()
{
    // 频率按钮颜色
    m_freqButton->setStyleSheet(QString(
        "QPushButton { border-radius: 10px; background-color: %1; }"
        "QPushButton:hover { border: 1px solid #333; }"
    ).arg(frequencyToColor(m_frequency)));

    // 频率文字标签
    if (m_freqLabel) {
        m_freqLabel->setText(frequencyToText(m_frequency));
        m_freqLabel->setStyleSheet(QString("color: %1; font-size: 10px; font-weight: bold;").arg(frequencyToColor(m_frequency)));
    }

    // 名称
    m_nameLabel->setText(m_name);
    QFont nameFont = m_nameLabel->font();
    nameFont.setBold(true);
    if (m_largeMode) {
        nameFont.setPointSize(10);
    } else {
        nameFont.setPointSize(8);
    }
    m_nameLabel->setFont(nameFont);

    // 描述 — 小字号以在有限空间内显示约8行
    if (!m_description.isEmpty()) {
        QString desc = m_description.length() > 400 ? m_description.left(400) + "..." : m_description;
        m_descLabel->setText(desc);
        m_descLabel->setStyleSheet("color: #666666; font-size: 9px; line-height: 130%;");
    }

    // 标签 — 始终显示标签区域
    clearLayoutWidgets(m_tagsLayout);
    if (!m_tags.isEmpty()) {
        int tagCount = qMin(3, m_tags.size());
        for (int i = 0; i < tagCount; ++i) {
            QLabel *tagLabel = new QLabel("#" + m_tags[i], m_tagsWidget);
            tagLabel->setStyleSheet(
                "QLabel { color: #4a90d9; font-size: 11px; "
                "background-color: #e8f0fe; border-radius: 3px; padding: 2px 6px; }");
            m_tagsLayout->addWidget(tagLabel);
        }
        if (m_tags.size() > 3) {
            QLabel *moreLabel = new QLabel(QString("+%1").arg(m_tags.size() - 3), m_tagsWidget);
            moreLabel->setStyleSheet("color: #999; font-size: 11px;");
            m_tagsLayout->addWidget(moreLabel);
        }
    } else {
        QLabel *noTagLabel = new QLabel("无标签", m_tagsWidget);
        noTagLabel->setStyleSheet("color: #bbb; font-size: 11px; font-style: italic;");
        m_tagsLayout->addWidget(noTagLabel);
    }
    m_tagsLayout->addStretch();

    // Agent 按钮 — 显示所有已配置的 agent
    clearLayoutWidgets(m_agentsLayout);
    for (const AgentInfo &agent : m_agentList) {
        QPushButton *btn = new QPushButton(m_agentsWidget);
        btn->setFixedSize(22, 22);
        btn->setCheckable(true);
        btn->setChecked(m_enabledAgentIds.contains(agent.id));
        btn->setText(agent.name.left(1).toUpper());
        QString tip;
        if (agent.path.isEmpty()) {
            tip = QString("%1（路径未配置）").arg(agent.name);
        } else if (!QDir(agent.path).exists()) {
            tip = QString("%1（路径无效: %2）").arg(agent.name, agent.path);
        } else {
            tip = QString("%1 (%2)").arg(agent.name, agent.path);
        }
        btn->setToolTip(tip);
        btn->setStyleSheet(
            "QPushButton { border: 1px solid #ccc; border-radius: 11px; "
            "background-color: #e0e0e0; font-size: 9px; }"
            "QPushButton:checked { background-color: #4a90d9; color: white; }"
            "QPushButton:disabled { background-color: #f0f0f0; color: #aaa; }");
        connect(btn, &QPushButton::toggled, this, [this, agent](bool checked) {
            emit agentToggled(m_skillId, agent.id, checked);
        });
        m_agentsLayout->addWidget(btn);
    }
}

// ============================================================================
// 布局安排（切换模式或首次显示时调用）
// ============================================================================

void SkillCard::arrangeLayout()
{
    // 清理旧布局（只删 layout，不删子控件）
    QLayout *oldLayout = layout();
    if (oldLayout) {
        while (oldLayout->count() > 0) {
            QLayoutItem *item = oldLayout->takeAt(0);
            // 控件保留在 this 上，不 delete
            if (item->widget()) {
                item->widget()->setParent(this);
            }
            delete item;
        }
        delete oldLayout;
    }

    if (m_largeMode) {
        setFixedSize(260, 210);
        arrangeLargeMode();
    } else {
        setFixedSize(150, 110);
        arrangeSmallMode();
    }
}

void SkillCard::arrangeLargeMode()
{
    m_smallIconLabel->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 6, 8, 6);
    mainLayout->setSpacing(4);

    // === 第1行：频率圆点 + 频率文字(左) + 名称(中) + 多选框(右) ===
    QHBoxLayout *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);
    topRow->setSpacing(4);
    topRow->addWidget(m_freqButton);       // 频率圆点
    topRow->addWidget(m_freqLabel);        // 频率文字

    m_nameLabel->setWordWrap(false);
    m_nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_nameLabel->setStyleSheet("color: #333;");
    topRow->addWidget(m_nameLabel, 1);    // 名称（占满剩余空间）

    topRow->addWidget(m_checkBox);         // 多选框
    mainLayout->addLayout(topRow);

    // === 第2行：描述 ===
    if (!m_description.isEmpty()) {
        m_descLabel->setWordWrap(true);
        m_descLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        mainLayout->addWidget(m_descLabel);
    }

    // === 第3行：标签 ===
    mainLayout->addWidget(m_tagsWidget);

    // 弹性空间推到底部
    mainLayout->addStretch(1);

    // === 底部行：Agent 图标(左) ===
    QHBoxLayout *bottomRow = new QHBoxLayout();
    bottomRow->setContentsMargins(0, 0, 0, 0);
    bottomRow->setSpacing(4);

    bottomRow->addWidget(m_agentsWidget, 0, Qt::AlignLeft);
    bottomRow->addStretch(1);

    mainLayout->addLayout(bottomRow);
}

void SkillCard::arrangeSmallMode()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(2);

    // 顶部：频率 + 多选框
    QHBoxLayout *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);
    m_freqButton->setFixedSize(14, 14);
    topRow->addWidget(m_freqButton);
    topRow->addStretch();
    topRow->addWidget(m_checkBox);
    mainLayout->addLayout(topRow);

    // 图标
    m_smallIconLabel->setText("📄");
    m_smallIconLabel->setStyleSheet("font-size: 22px;");
    mainLayout->addWidget(m_smallIconLabel, 0, Qt::AlignCenter);

    // 名称（截断）
    QString shortName = m_name.length() > 18 ? m_name.left(18) + "..." : m_name;
    m_nameLabel->setText(shortName);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setStyleSheet("font-size: 10px; color: #333;");
    mainLayout->addWidget(m_nameLabel, 0, Qt::AlignCenter);

    // 小模式下隐藏描述/标签/agent
    m_descLabel->hide();
    m_tagsWidget->hide();
    m_agentsWidget->hide();
}

// ============================================================================
// 公共接口
// ============================================================================

void SkillCard::setAgentList(const QVector<AgentInfo> &agents)
{
    m_agentList = agents;
    updateData();
}

void SkillCard::updateSkillData(const SkillInfo &skill)
{
    m_name = skill.name;
    m_description = skill.description;
    m_frequency = skill.frequency;
    m_tags = skill.tags;
    m_enabledAgentIds = skill.enabledAgentIds;
    updateData();
}

void SkillCard::setLargeMode(bool large)
{
    if (m_largeMode != large) {
        m_largeMode = large;
        applyStyle();
        updateData();
        arrangeLayout();

        if (!m_largeMode) {
            m_descLabel->hide();
            m_tagsWidget->hide();
            m_agentsWidget->hide();
            m_smallIconLabel->show();
        } else {
            m_descLabel->show();
            m_tagsWidget->show();
            m_agentsWidget->show();
            m_smallIconLabel->hide();
        }
    }
}

bool SkillCard::isChecked() const
{
    return m_checkBox->isChecked();
}

void SkillCard::setChecked(bool checked)
{
    m_checkBox->setChecked(checked);
}

// ============================================================================
// 事件处理
// ============================================================================

void SkillCard::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QWidget *child = childAt(event->pos());
    if (child && (dynamic_cast<QAbstractButton *>(child) ||
                  child == m_smallIconLabel)) {
        return;
    }

    // 点击频率标签区域 → 打开频率菜单
    if (child == m_freqLabel) {
        showFrequencyMenu();
        return;
    }

    emit cardClicked(m_skillId);
}

void SkillCard::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    // 添加标签
    QAction *addTagAction = menu.addAction("添加标签...");
    connect(addTagAction, &QAction::triggered, this, [this]() {
        emit tagAddRequested(m_skillId);
    });

    // 删除标签（子菜单）
    if (!m_tags.isEmpty()) {
        QMenu *tagMenu = menu.addMenu("删除标签");
        for (const QString &tag : m_tags) {
            QAction *action = tagMenu->addAction("#" + tag);
            connect(action, &QAction::triggered, this, [this, tag]() {
                emit tagRemoveRequested(m_skillId, tag);
            });
        }
    }

    menu.addSeparator();

    // 删除 Skill
    QAction *deleteAction = menu.addAction("删除 Skill");
    connect(deleteAction, &QAction::triggered, this, [this]() {
        emit deleteRequested(m_skillId);
    });

    menu.exec(event->globalPos());
}

// ============================================================================
// 频率相关
// ============================================================================

QString SkillCard::frequencyToColor(int freq) const
{
    switch (freq) {
        case 1: return "#4caf50";
        case 2: return "#ff9800";
        case 3: return "#9e9e9e";
        case 4: return "#f44336";
        default: return "#9e9e9e";
    }
}

QString SkillCard::frequencyToText(int freq) const
{
    switch (freq) {
        case 1: return "常用";
        case 2: return "一般";
        case 3: return "很少";
        case 4: return "废弃";
        default: return "未知";
    }
}

void SkillCard::showFrequencyMenu()
{
    QMenu menu(this);
    QStringList freqNames = {"常用", "一般", "很少", "废弃"};
    QStringList freqColors = {"#4caf50", "#ff9800", "#9e9e9e", "#f44336"};

    for (int i = 0; i < 4; ++i) {
        int freqValue = i + 1;
        QPixmap pix(12, 12);
        pix.fill(QColor(freqColors[i]));

        QAction *action = menu.addAction(QIcon(pix), freqNames[i]);
        if (m_frequency == freqValue) {
            action->setCheckable(true);
            action->setChecked(true);
        }

        connect(action, &QAction::triggered, this, [this, freqValue]() {
            if (freqValue != m_frequency) {
                emit frequencyClicked(m_skillId, freqValue);
            }
        });
    }

    menu.exec(m_freqButton->mapToGlobal(QPoint(0, m_freqButton->height())));
}

// ============================================================================
// 辅助方法
// ============================================================================

void SkillCard::clearLayoutWidgets(QLayout *layout)
{
    if (!layout) return;
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}
