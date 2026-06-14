#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QScrollArea>
#include <QGridLayout>
#include <QVector>
#include <QStringList>
#include <QTimer>
#include "database/DatabaseManager.h"

class SkillCard;

class CardScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit CardScrollArea(QWidget *parent = nullptr);
signals:
    void viewportClicked(QPoint pos);
protected:
    bool viewportEvent(QEvent *event) override;
};

class CardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CardWidget(QWidget *parent = nullptr);
    ~CardWidget();

    void setSkills(const QVector<SkillInfo> &skills);
    void setAgentList(const QVector<AgentInfo> &agents);
    void addSkill(const SkillInfo &skill);
    void clearSkills();

    // 原地更新单个 skill（不重建卡片、不跳动）
    void updateSkill(int skillId, const SkillInfo &newSkill);

    void setCardSize(bool large);
    bool isLargeMode() const { return m_largeMode; }

    void filter(const QString &text, const QStringList &tags);
    void filterByFrequency(const QVector<int> &frequencies);
    void sortBy(const QString &criteria);

    QVector<int> getSelectedSkillIds() const;
    int getSelectedCount() const;
    int getTotalCount() const { return m_allSkills.size(); }

    // 全选/取消全选当前可见的卡片
    void selectAll(bool checked);

public slots:
    void onViewportClicked(const QPoint &pos);

signals:
    void selectionChanged(int count);
    void cardClicked(int skillId);
    void frequencyChanged(int skillId, int newFrequency);
    void agentToggled(int skillId, int agentId, bool enabled);
    void tagAddRequested(int skillId);
    void tagRemoveRequested(int skillId, const QString &tag);
    void deleteRequested(int skillId);
    void openFolderRequested(int skillId);
    void agentFolderRequested(int skillId, int agentId);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onCardClicked(int skillId);
    void onCheckBoxToggled(int skillId, bool checked);
    void relayout();

private:
    void clearLayout();
    void layoutCards();
    bool matchesFilter(const SkillInfo &skill) const;
    void applyFilter();
    int calculateColumns() const;

    QVector<SkillInfo> m_allSkills;
    QVector<AgentInfo> m_agentList;
    QVector<SkillCard *> m_cards;
    QGridLayout *m_gridLayout;
    bool m_largeMode;
    QString m_filterText;
    QStringList m_filterTags;
    QVector<int> m_filterFrequencies;
    QString m_sortCriteria;
};

#endif // CARDWIDGET_H
