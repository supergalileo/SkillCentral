#include "ui/SkillCard.h"
#include "database/DatabaseManager.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QFont>
#include <QPushButton>
#include <QToolButton>
#include <QAbstractButton>
#include <QPainterPath>
#include <QMenu>
#include <QAction>

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
    setupUi();
    updateUi();
}

SkillCard::~SkillCard()
{
}

void SkillCard::setupUi()
{
    m_checkBox = new QCheckBox(this);
    m_checkBox->setChecked(false);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setWordWrap(true);
    m_nameLabel->setTextFormat(Qt::PlainText);

    m_descLabel = new QLabel(this);
    m_descLabel->setWordWrap(true);
    m_descLabel->setTextFormat(Qt::PlainText);

    m_freqButton = new QPushButton(this);
    m_freqButton->setFixedSize(24, 24);
    m_freqButton->setCursor(Qt::PointingHandCursor);
    m_freqButton->setToolTip("点击选择使用频率");
    connect(m_freqButton, &QPushButton::clicked, this, &SkillCard::showFrequencyMenu);

    m_tagsWidget = new QWidget(this);
    m_tagsLayout = new QHBoxLayout(m_tagsWidget);
    m_tagsLayout->setContentsMargins(0, 0, 0, 0);
    m_tagsLayout->setSpacing(4);

    m_agentsWidget = new QWidget(this);
    m_agentsLayout = new QHBoxLayout(m_agentsWidget);
    m_agentsLayout->setContentsMargins(0, 0, 0, 0);
    m_agentsLayout->setSpacing(2);

    m_smallIconLabel = new QLabel(this);
    m_smallIconLabel->setAlignment(Qt::AlignCenter);
}

void SkillCard::updateUi()
{
    QLayout *oldLayout = layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    updateFrequencyColor();

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

    if (m_largeMode) {
        setFixedSize(250, 200);
        setupLargeMode();
    } else {
        setFixedSize(120, 80);
        setupSmallMode();
    }
}

void SkillCard::setupLargeMode()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(6);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addWidget(m_checkBox);
    QFont nameFont = m_nameLabel->font();
    nameFont.setBold(true);
    nameFont.setPointSize(10);
    m_nameLabel->setFont(nameFont);
    m_nameLabel->setText(m_name.length() > 20 ? m_name.left(20) + "..." : m_name);
    m_nameLabel->setFixedWidth(180);
    topLayout->addWidget(m_nameLabel, 1);
    topLayout->addWidget(m_freqButton);
    mainLayout->addLayout(topLayout);

    if (!m_description.isEmpty()) {
        QString desc = m_description.length() > 50
                      ? m_description.left(50) + "..."
                      : m_description;
        m_descLabel->setText(desc);
        m_descLabel->setStyleSheet("color: #666666; font-size: 11px;");
        m_descLabel->setFixedHeight(40);
        mainLayout->addWidget(m_descLabel);
    }

    if (!m_tags.isEmpty()) {
        QLayoutItem *item;
        while ((item = m_tagsLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        int count = qMin(3, m_tags.size());
        for (int i = 0; i < count; ++i) {
            QLabel *tagLabel = new QLabel("#" + m_tags[i], m_tagsWidget);
            tagLabel->setStyleSheet(
                "QLabel { color: #4a90d9; font-size: 10px; "
                "background-color: #e8f0fe; border-radius: 3px; "
                "padding: 1px 4px; }"
            );
            m_tagsLayout->addWidget(tagLabel);
        }
        m_tagsLayout->addStretch();
        mainLayout->addWidget(m_tagsWidget);
    }

    mainLayout->addStretch();

    // Agent 按钮 - 用简单占位
    if (!m_agentsWidget->parent()) {
        m_agentsWidget->setParent(this);
    }
    QLayoutItem *item;
    while ((item = m_agentsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    m_agentButtons.clear();

    // 简单显示3个 agent 按钮（占位）
    QStringList agentNames = {"W", "C", "K"};
    for (int i = 0; i < 3; ++i) {
        QPushButton *btn = new QPushButton(m_agentsWidget);
        btn->setFixedSize(22, 22);
        btn->setCheckable(true);
        btn->setChecked(i < m_enabledAgentIds.size());
        btn->setText(agentNames[i]);
        btn->setToolTip(QString("Agent %1").arg(i + 1));
        btn->setStyleSheet(
            "QPushButton { border: 1px solid #ccc; border-radius: 11px; "
            "background-color: #e0e0e0; font-size: 9px; }"
            "QPushButton:checked { background-color: #4a90d9; color: white; }"
        );
        int agentId = i + 1;
        connect(btn, &QPushButton::toggled, this, [this, agentId](bool checked) {
            emit agentToggled(m_skillId, agentId, checked);
        });
        m_agentsLayout->addWidget(btn);
        m_agentButtons.append(btn);
    }
    m_agentsLayout->addStretch();
    mainLayout->addWidget(m_agentsWidget);
}

void SkillCard::setupSmallMode()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(4);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addWidget(m_checkBox);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    m_smallIconLabel->setText("📄");
    m_smallIconLabel->setStyleSheet("font-size: 24px;");
    mainLayout->addWidget(m_smallIconLabel, 0, Qt::AlignCenter);

    QFont nameFont = m_nameLabel->font();
    nameFont.setBold(true);
    nameFont.setPointSize(8);
    m_nameLabel->setFont(nameFont);
    QString shortName = m_name.length() > 8 ? m_name.left(8) + "..." : m_name;
    m_nameLabel->setText(shortName);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setStyleSheet("font-size: 10px;");
    mainLayout->addWidget(m_nameLabel);
}

void SkillCard::setLargeMode(bool large)
{
    if (m_largeMode != large) {
        m_largeMode = large;
        updateUi();
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

void SkillCard::mousePressEvent(QMouseEvent *event)
{
    QWidget *child = childAt(event->pos());
    if (child && (dynamic_cast<QAbstractButton *>(child) ||
                  child == m_smallIconLabel)) {
        QWidget::mousePressEvent(event);
        return;
    }
    emit cardClicked(m_skillId);
}

void SkillCard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(0, 0, -1, -1), 8, 8);
    painter.fillPath(path, palette().window());
    painter.strokePath(path, QPen(QColor("#d0d0d0"), 1));
    if (underMouse()) {
        painter.strokePath(path, QPen(QColor("#4a90d9"), 1));
    }
}

void SkillCard::updateFrequencyColor()
{
    QString color = frequencyToColor(m_frequency);
    QString text = frequencyToText(m_frequency);
    m_freqButton->setStyleSheet(QString(
        "QPushButton { background-color: %1; border: none; "
        "border-radius: 4px; color: white; font-size: 9px; font-weight: bold; }"
        "QPushButton:hover { border: 2px solid #333; }"
    ).arg(color));
    m_freqButton->setToolTip(text);
}

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
