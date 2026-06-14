#include "ui/SkillSidebar.h"
#include "ui/MarkdownRenderer.h"
#include "models/AgentManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QPainter>
#include <QEvent>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCheckBox>
#include <QFrame>

SkillSidebar::SkillSidebar(QWidget *parent)
    : QWidget(parent)
    , m_dbManager(nullptr)
    , m_agentManager(nullptr)
    , m_currentSkillId(-1)
    , m_isOpen(false)
    , m_animation(nullptr)
    , m_overlay(nullptr)
    , m_sidebarWidth(0)
    , m_topBar(nullptr)
    , m_closeButton(nullptr)
    , m_titleLabel(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_descLabel(nullptr)
    , m_contentBrowser(nullptr)
    , m_tagsWidget(nullptr)
    , m_agentsWidget(nullptr)
    , m_editButton(nullptr)
    , m_deleteButton(nullptr)
    , m_exportButton(nullptr)
{
    setupUi();
    if (parentWidget()) {
        parentWidget()->installEventFilter(this);
    }
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("SkillSidebar { background-color: white; border-left: 1px solid #e0e0e0; }");
    hide();
}

SkillSidebar::~SkillSidebar()
{
}

void SkillSidebar::setDatabaseManager(DatabaseManager *dbManager)
{
    m_dbManager = dbManager;
}

void SkillSidebar::setAgentManager(AgentManager *agentManager)
{
    m_agentManager = agentManager;
}

void SkillSidebar::loadSkill(int skillId)
{
    qInfo() << "Sidebar loadSkill:" << skillId;

    if (m_isOpen && m_currentSkillId == skillId) {
        raise();
        setFocus();
        return;
    }

    if (m_isOpen && m_animation) {
        m_animation->stop();
    }

    m_currentSkillId = skillId;

    if (m_dbManager) {
        m_currentSkill = m_dbManager->getSkill(skillId);
    }

    updateUi();
    open();
}

void SkillSidebar::open()
{
    if (m_isOpen) {
        raise();
        setFocus();
        return;
    }
    
    m_isOpen = true;
    m_sidebarWidth = SIDEBAR_WIDTH;
    setFixedWidth(SIDEBAR_WIDTH);
    
    // 设置几何尺寸，再显示。
    if (parentWidget()) {
        QRect parentRect = parentWidget()->rect();
        setGeometry(parentRect.width() - SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, parentRect.height());
    }
    show();
    raise();
    
    // show() 可能改变几何，显示后再确认一次。
    if (parentWidget()) {
        QRect parentRect = parentWidget()->rect();
        setGeometry(parentRect.width() - SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, parentRect.height());
    }
    
    qInfo() << "Sidebar opened, geom:" << geometry() << "visible:" << isVisible();
    
    setFocus();
}

void SkillSidebar::close()
{
    if (!m_isOpen) {
        return;
    }
    
    m_isOpen = false;
    hide();
    hideOverlay();
    emit closed();
}

void SkillSidebar::setSidebarWidth(int width)
{
    m_sidebarWidth = width;
    setFixedWidth(width);

    if (parentWidget()) {
        QRect parentRect = parentWidget()->rect();
        int x = parentRect.width() - width;
        setGeometry(x, 0, width, parentRect.height());
        qInfo() << "Sidebar geom:" << x << 0 << width << parentRect.height()
                << "visible:" << isVisible();
    }

    if (width <= 0) {
        hide();
        m_isOpen = false;
        hideOverlay();
        emit closed();
    }
}

void SkillSidebar::onAnimationFinished()
{
    // 动画结束后的处理
    if (m_sidebarWidth <= 0) {
        hide();
        m_isOpen = false;
        hideOverlay();
        emit closed();
    }
}

void SkillSidebar::onEditClicked()
{
    emit editRequested(m_currentSkillId);
}

void SkillSidebar::onDeleteClicked()
{
    emit deleteRequested(m_currentSkillId);
}

void SkillSidebar::onExportClicked()
{
    emit exportRequested(m_currentSkillId);
}

void SkillSidebar::onCloseClicked()
{
    close();
}

void SkillSidebar::setupUi()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_topBar = createTopBar();
    mainLayout->addWidget(m_topBar);

    m_contentBrowser = new QTextBrowser(this);
    m_contentBrowser->setOpenExternalLinks(true);
    m_contentBrowser->setStyleSheet("QTextBrowser { border: none; padding: 16px; background-color: #fafafa; }");
    mainLayout->addWidget(m_contentBrowser, 1);
}

void SkillSidebar::updateUi()
{
    if (m_currentSkillId <= 0) {
        return;
    }

    QString skillMdPath = "";
    // 从 Skill 对象获取 SKILL.md 路径。
    if (!m_currentSkill.path.isEmpty()) {
        skillMdPath = m_currentSkill.path + "/SKILL.md";
        QFile file(skillMdPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString markdown = in.readAll();
            file.close();
            
            QString html = MarkdownRenderer::toHtml(markdown);
            m_contentBrowser->setHtml(html);
        } else {
            m_contentBrowser->setHtml("<p style='color:#999;'>无法加载 SKILL.md 文件</p>");
        }
    } else {
        m_contentBrowser->setHtml("<p style='color:#999;'>未设置 skill 路径</p>");
    }
}

void SkillSidebar::updateTagsArea()
{
    QLayoutItem *item;
    QBoxLayout *tagsLayout = qobject_cast<QBoxLayout*>(m_tagsWidget->layout());
    if (!tagsLayout) {
        tagsLayout = new QHBoxLayout(m_tagsWidget);
    } else {
        while ((item = tagsLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
    }
    
    QLabel *titleLabel = new QLabel("标签:", m_tagsWidget);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
    tagsLayout->addWidget(titleLabel);

    // 添加标签
    if (m_dbManager) {
        // 从数据库获取标签
        QStringList tags = m_currentSkill.tags;
        for (const QString &tag : tags) {
            QLabel *tagLabel = new QLabel("#" + tag, m_tagsWidget);
            tagLabel->setStyleSheet("QLabel { color: #4a90d9; padding: 2px 6px; "
                                   "background-color: #e8f0fe; border-radius: 3px; "
                                   "font-size: 12px; }");
            tagsLayout->addWidget(tagLabel);
        }
    }
    
    tagsLayout->addStretch();
}

void SkillSidebar::updateAgentsArea()
{
    // 清除旧 Agent 按钮
    QLayoutItem *item;
    QBoxLayout *agentsLayout = qobject_cast<QBoxLayout*>(m_agentsWidget->layout());
    if (!agentsLayout) {
        agentsLayout = new QVBoxLayout(m_agentsWidget);
    } else {
        while ((item = agentsLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
    }
    
    QLabel *titleLabel = new QLabel("启用的 Agent:", m_agentsWidget);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
    agentsLayout->addWidget(titleLabel);

    // 添加 Agent 复选框
    if (m_agentManager) {
        QVector<AgentInfo> allAgents = m_agentManager->getAllAgents();
        QVector<int> enabledAgents = m_agentManager->getSkillEnabledAgents(m_currentSkillId);
        
        for (const AgentInfo &agent : allAgents) {
            QCheckBox *checkBox = new QCheckBox(agent.name, m_agentsWidget);
            checkBox->setChecked(enabledAgents.contains(agent.id));
            checkBox->setEnabled(false);
            agentsLayout->addWidget(checkBox);
        }
    }
    
    agentsLayout->addStretch();
}

QWidget* SkillSidebar::createTopBar()
{
    QWidget *widget = new QWidget(this);
    widget->setFixedHeight(50);
    widget->setStyleSheet("background-color: #f8f9fa; border-bottom: 1px solid #e0e0e0;");
    
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(12, 0, 12, 0);
    
    // 关闭按钮
    m_closeButton = new QPushButton("X", widget);
    m_closeButton->setFixedSize(32, 32);
    m_closeButton->setStyleSheet("QPushButton { border: none; font-size: 16px; "
                                 "color: #666; border-radius: 16px; }"
                                 "QPushButton:hover { background-color: #e0e0e0; }");
    connect(m_closeButton, &QPushButton::clicked, this, &SkillSidebar::onCloseClicked);
    layout->addWidget(m_closeButton);
    
    layout->addStretch();
    
    return widget;
}

QWidget* SkillSidebar::createDescriptionArea()
{
    QWidget *widget = new QWidget(m_contentWidget);
    
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *titleLabel = new QLabel("描述", widget);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
    layout->addWidget(titleLabel);
    
    m_descLabel = new QLabel(widget);
    m_descLabel->setWordWrap(true);
    m_descLabel->setStyleSheet("color: #666; font-size: 13px; line-height: 1.5;");
    layout->addWidget(m_descLabel);
    
    return widget;
}

QWidget* SkillSidebar::createContentArea()
{
    QWidget *widget = new QWidget(m_contentWidget);
    
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *titleLabel = new QLabel("SKILL.md 内容", widget);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
    layout->addWidget(titleLabel);
    
    m_contentBrowser = new QTextBrowser(widget);
    m_contentBrowser->setOpenExternalLinks(true);
    m_contentBrowser->setMaximumHeight(400);
    m_contentBrowser->setStyleSheet("QTextBrowser { border: 1px solid #e0e0e0; "
                                   "border-radius: 4px; padding: 8px; "
                                   "background-color: #fafafa; }");
    layout->addWidget(m_contentBrowser);
    
    return widget;
}

QWidget* SkillSidebar::createTagsArea()
{
    m_tagsWidget = new QWidget(m_contentWidget);
    
    QHBoxLayout *layout = new QHBoxLayout(m_tagsWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    
    QLabel *titleLabel = new QLabel("标签:", m_tagsWidget);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
    layout->addWidget(titleLabel);
    
    layout->addStretch();
    
    return m_tagsWidget;
}

QWidget* SkillSidebar::createAgentsArea()
{
    m_agentsWidget = new QWidget(m_contentWidget);
    
    QVBoxLayout *layout = new QVBoxLayout(m_agentsWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    
    QLabel *titleLabel = new QLabel("启用的 Agent:", m_agentsWidget);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
    layout->addWidget(titleLabel);
    
    QHBoxLayout *agentsLayout = new QHBoxLayout();
    agentsLayout->setContentsMargins(0, 0, 0, 0);
    agentsLayout->addStretch();
    layout->addLayout(agentsLayout);
    
    return m_agentsWidget;
}

QWidget* SkillSidebar::createButtonArea()
{
    QWidget *widget = new QWidget(this);
    widget->setStyleSheet("background-color: #f8f9fa; border-top: 1px solid #e0e0e0;");
    
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(16, 12, 16, 12);
    
    m_editButton = new QPushButton("编辑", widget);
    m_deleteButton = new QPushButton("删除", widget);
    m_exportButton = new QPushButton("导出", widget);
    
    m_editButton->setStyleSheet("QPushButton { padding: 6px 16px; border: 1px solid #4a90d9; "
                                "border-radius: 4px; color: #4a90d9; background-color: white; }"
                                "QPushButton:hover { background-color: #e8f0fe; }");
    m_deleteButton->setStyleSheet("QPushButton { padding: 6px 16px; border: 1px solid #dc3545; "
                                  "border-radius: 4px; color: #dc3545; background-color: white; }"
                                  "QPushButton:hover { background-color: #f8d7da; }");
    m_exportButton->setStyleSheet("QPushButton { padding: 6px 16px; border: 1px solid #28a745; "
                                  "border-radius: 4px; color: #28a745; background-color: white; }"
                                  "QPushButton:hover { background-color: #d4edda; }");
    
    connect(m_editButton, &QPushButton::clicked, this, &SkillSidebar::onEditClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &SkillSidebar::onDeleteClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &SkillSidebar::onExportClicked);
    
    layout->addWidget(m_editButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_exportButton);
    layout->addStretch();
    
    return widget;
}

void SkillSidebar::showOverlay()
{
    if (!m_overlay) {
        m_overlay = new QWidget(parentWidget());
        m_overlay->setStyleSheet("background-color: rgba(0,0,0,50);");
        m_overlay->installEventFilter(this);
        m_overlay->setCursor(Qt::PointingHandCursor);
    }
    
    m_overlay->setGeometry(parentWidget()->rect());
    m_overlay->show();
    m_overlay->lower();
}

void SkillSidebar::hideOverlay()
{
    if (m_overlay) {
        m_overlay->hide();
    }
}

void SkillSidebar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    
    // 缁樺埗鍙充晶闃村奖鏁堟灉
    QLinearGradient gradient(width() - 10, 0, width(), 0);
    gradient.setColorAt(0, QColor(0, 0, 0, 20));
    gradient.setColorAt(1, QColor(0, 0, 0, 0));
    painter.fillRect(width() - 10, 0, 10, height(), gradient);
}

void SkillSidebar::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    
    // 右侧对齐
    if (parentWidget()) {
        QRect parentRect = parentWidget()->rect();
        setGeometry(parentRect.width() - m_sidebarWidth, 0, m_sidebarWidth, parentRect.height());
        
        if (m_overlay) {
            m_overlay->setGeometry(parentRect);
        }
    }
}

void SkillSidebar::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void SkillSidebar::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    // showEvent 中不再调整几何，由 open() 负责。
    qInfo() << "Sidebar showEvent, geom:" << geometry();
}

bool SkillSidebar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_overlay && event->type() == QEvent::MouseButtonPress) {
        close();
        return true;
    }

    if (watched == parentWidget() && event->type() == QEvent::Resize && isVisible()) {
        QRect parentRect = parentWidget()->rect();
        int width = m_isOpen ? SIDEBAR_WIDTH : m_sidebarWidth;
        if (width > 0) {
            m_sidebarWidth = width;
            setGeometry(parentRect.width() - width, 0, width, parentRect.height());
        }
        if (m_overlay) {
            m_overlay->setGeometry(parentRect);
        }
    }

    return QWidget::eventFilter(watched, event);
}
