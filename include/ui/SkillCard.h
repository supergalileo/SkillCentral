#ifndef SKILLCARD_H
#define SKILLCARD_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include "database/DatabaseManager.h"

class SkillCard : public QWidget
{
    Q_OBJECT

public:
    explicit SkillCard(const SkillInfo &skill, QWidget *parent = nullptr);
    ~SkillCard();

    void setLargeMode(bool large);
    bool isLargeMode() const { return m_largeMode; }

    int getSkillId() const { return m_skillId; }
    bool isChecked() const;

    // 设置实际 agent 列表（动态生成按钮）
    void setAgentList(const QVector<AgentInfo> &agents);

    // 原地更新数据（不重建控件、不重排布局）
    void updateSkillData(const SkillInfo &skill);

public slots:
    void setChecked(bool checked);

signals:
    void cardClicked(int skillId);
    void checkBoxToggled(int skillId, bool checked);
    void frequencyClicked(int skillId, int newFrequency);
    void agentToggled(int skillId, int agentId, bool enabled);
    void tagAddRequested(int skillId);
    void tagRemoveRequested(int skillId, const QString &tag);
    void deleteRequested(int skillId);
    void openFolderRequested(int skillId);
    void agentFolderRequested(int skillId, int agentId);

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    // 控件创建（只调用一次）
    void setupUi();
    // 样式设置
    void applyStyle();
    // 数据更新（不重建控件）
    void updateData();
    // 布局安排
    void arrangeLayout();
    void arrangeLargeMode();
    void arrangeSmallMode();
    // 清理布局中的子控件
    void clearLayoutWidgets(QLayout *layout);

    // 频率菜单
    void showFrequencyMenu();
    QString frequencyToColor(int freq) const;
    QString frequencyToText(int freq) const;

    // 数据字段
    int m_skillId;
    QString m_name;
    QString m_description;
    int m_frequency;       // 1=常用 2=一般 3=很少 4=废弃
    QStringList m_tags;
    QVector<int> m_enabledAgentIds;
    QVector<AgentInfo> m_agentList;
    bool m_largeMode;

    // UI 控件（成员变量，生命周期 = 卡片生命周期）
    QCheckBox *m_checkBox = nullptr;
    QLabel *m_nameLabel = nullptr;
    QLabel *m_descLabel = nullptr;
    QPushButton *m_freqButton = nullptr;
    QLabel *m_freqLabel = nullptr;
    QWidget *m_tagsWidget = nullptr;
    QHBoxLayout *m_tagsLayout = nullptr;
    QWidget *m_agentsWidget = nullptr;
    QHBoxLayout *m_agentsLayout = nullptr;
    QLabel *m_smallIconLabel = nullptr;
};

#endif // SKILLCARD_H
