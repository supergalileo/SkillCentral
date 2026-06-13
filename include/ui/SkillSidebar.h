#ifndef SKILLSIDEBAR_H
#define SKILLSIDEBAR_H

#include <QWidget>
#include <QPropertyAnimation>
#include "database/DatabaseManager.h"

class AgentManager;  // 前向声明

QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QTextBrowser)
QT_FORWARD_DECLARE_CLASS(QVBoxLayout)
QT_FORWARD_DECLARE_CLASS(QHBoxLayout)
QT_FORWARD_DECLARE_CLASS(QScrollArea)

/**
 * @brief Skill 详情侧边栏
 * 
 * 从主窗口右侧滑出，显示 skill 的详细信息，
 * 包括描述、SKILL.md 内容、标签和启用的 Agent 列表。
 * 
 * 功能：
 * - 点击卡片 → 右侧滑入（QPropertyAnimation, 300ms）
 * - 点击 X 或侧边栏外区域 → 滑出关闭
 * - 按 Escape 键 → 关闭
 * - 侧边栏打开时，主窗口卡片区域变暗（半透明遮罩）
 */
class SkillSidebar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int sidebarWidth READ sidebarWidth WRITE setSidebarWidth)

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口（通常是 MainWindow）
     */
    explicit SkillSidebar(QWidget *parent = nullptr);
    ~SkillSidebar();

    /**
     * @brief 设置数据库管理器
     * @param dbManager 数据库管理器实例
     */
    void setDatabaseManager(DatabaseManager *dbManager);
    void setAgentManager(AgentManager *agentManager);

    /**
     * @brief 加载并显示指定 skill 的详情
     * @param skillId Skill ID
     */
    void loadSkill(int skillId);

    /**
     * @brief 打开侧边栏（滑入动画）
     */
    void open();

    /**
     * @brief 关闭侧边栏（滑出动画）
     */
    void close();

    /**
     * @brief 检查侧边栏是否打开
     * @return 是否打开
     */
    bool isOpen() const { return m_isOpen; }

signals:
    /**
     * @brief 请求编辑 skill
     * @param skillId Skill ID
     */
    void editRequested(int skillId);

    /**
     * @brief 请求删除 skill
     * @param skillId Skill ID
     */
    void deleteRequested(int skillId);

    /**
     * @brief 请求导出 skill
     * @param skillId Skill ID
     */
    void exportRequested(int skillId);

    /**
     * @brief 侧边栏已关闭
     */
    void closed();

protected:
    /**
     * @brief 绘制事件（绘制阴影）
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief 调整大小事件
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief 按键事件（处理 Escape 键）
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief 显示事件
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief 事件过滤器（处理遮罩点击）
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    /**
     * @brief 动画结束处理
     */
    void onAnimationFinished();

    /**
     * @brief 编辑按钮点击
     */
    void onEditClicked();

    /**
     * @brief 删除按钮点击
     */
    void onDeleteClicked();

    /**
     * @brief 导出按钮点击
     */
    void onExportClicked();

    /**
     * @brief 关闭按钮点击
     */
    void onCloseClicked();

private:
    /**
     * @brief 初始化 UI
     */
    void setupUi();

    /**
     * @brief 更新 UI 显示
     */
    void updateUi();

    /**
     * @brief 更新标签区域
     */
    void updateTagsArea();

    /**
     * @brief 更新 Agent 区域
     */
    void updateAgentsArea();

    /**
     * @brief 创建顶部栏
     */
    QWidget* createTopBar();

    /**
     * @brief 创建描述区域
     */
    QWidget* createDescriptionArea();

    /**
     * @brief 创建 SKILL.md 内容区域
     */
    QWidget* createContentArea();

    /**
     * @brief 创建标签区域
     */
    QWidget* createTagsArea();

    /**
     * @brief 创建 Agent 区域
     */
    QWidget* createAgentsArea();

    /**
     * @brief 创建底部按钮区域
     */
    QWidget* createButtonArea();

    /**
     * @brief 显示遮罩
     */
    void showOverlay();

    /**
     * @brief 隐藏遮罩
     */
    void hideOverlay();

    /**
     * @brief 侧边栏宽度属性（用于动画）
     */
    int sidebarWidth() const { return m_sidebarWidth; }

    /**
     * @brief 设置侧边栏宽度（用于动画）
     */
    void setSidebarWidth(int width);

    // UI 组件
    QWidget *m_topBar;
    QPushButton *m_closeButton;
    QLabel *m_titleLabel;
    
    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;
    
    QLabel *m_descLabel;
    QTextBrowser *m_contentBrowser;
    QWidget *m_tagsWidget;
    QWidget *m_agentsWidget;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
    QPushButton *m_exportButton;

    // 遮罩
    QWidget *m_overlay;

    // 动画
    QPropertyAnimation *m_animation;

    // 数据
    DatabaseManager *m_dbManager;
    AgentManager *m_agentManager;
    int m_currentSkillId;
    SkillInfo m_currentSkill;
    
    // 状态
    bool m_isOpen;
    int m_sidebarWidth;
    static const int SIDEBAR_WIDTH = 400;
};

#endif // SKILLSIDEBAR_H
