#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVector>
#include <QStringList>
#include <QCloseEvent>
#include "database/DatabaseManager.h"
#include "ui/SkillSidebar.h"
#include "models/SkillManager.h"

class CardWidget;
class SkillSidebar;
class AgentManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setDatabaseManager(DatabaseManager *dbManager);
    void loadSkills();
    void loadTags();
    void loadAgents();

public slots:
    /**
     * @brief 打开 skill 详情侧边栏
     * @param skillId Skill ID
     */
    void openSkillSidebar(int skillId);

signals:
    void skillCardClicked(int skillId);
    void skillSelectionChanged(int count);
    void requestSettings();
    void requestAddSkill();
    void requestBackup();

public slots:
    void applyTheme(const QString &theme);

private slots:
    void onSearchTextChanged(const QString &text);
    void onTagClicked(const QString &tag);
    void onSelectAllClicked();
    void onBatchEnableClicked();
    void onBatchDisableClicked();
    void onBatchDeleteClicked();
    void onBatchTagClicked();
    void onToggleCardSize();
    void onSortChanged(int index);
    void onCardClicked(int skillId);
    void onSelectionChanged(int count);
    void onFrequencyChanged(int skillId, int newFrequency);
    void onAgentToggled(int skillId, int agentId, bool enabled);

    // 菜单栏槽
    void onRequestSettings();
    void onManageTagsClicked();
    void onAddSkillClicked();
    void onBackupClicked();
    void onFrequencyFilterClicked(QPushButton *btn);
    void onRefreshClicked();

    // 侧边栏相关槽
    void onEditSkillRequested(int skillId);
    void onDeleteSkillRequested(int skillId);
    void onExportSkillRequested(int skillId);
    void onSidebarClosed();

    // 标签相关槽
    void onCardTagAddRequested(int skillId);
    void onCardTagRemoveRequested(int skillId, const QString &tag);
    void onCardDeleteRequested(int skillId);

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void setupUi();
    void updateStatus();
    void updateTagButtons();
    void createTestData();

    DatabaseManager *m_dbManager;
    AgentManager *m_agentManager;
    SkillManager *m_skillManager;
    SkillSidebar *m_skillSidebar;

    // UI 组件
    QLineEdit *m_searchEdit;
    QScrollArea *m_tagScrollArea;
    QWidget *m_tagContainer;
    QHBoxLayout *m_tagLayout;
    QPushButton *m_manageTagsButton;

    QCheckBox *m_selectAllCheck;
    QPushButton *m_batchEnableBtn;
    QPushButton *m_batchDisableBtn;
    QPushButton *m_batchDeleteBtn;
    QPushButton *m_batchTagBtn;
    QPushButton *m_toggleSizeBtn;
    QComboBox *m_sortCombo;

    QScrollArea *m_cardScrollArea;
    CardWidget *m_cardWidget;

    QLabel *m_statusLabel;

    // 状态
    QStringList m_activeFilterTags;
    QVector<int> m_activeFilterFrequencies;
    bool m_largeMode;
    QVector<SkillInfo> m_allSkills;
    QStringList m_allTags;
    QVector<AgentInfo> m_allAgents;

    // UI 组件指针
    QVector<QPushButton *> m_tagButtons;
    QVector<QPushButton *> m_freqButtons;
    QPushButton *m_refreshBtn;
};

#endif // MAINWINDOW_H
