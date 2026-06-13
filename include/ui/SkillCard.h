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

public slots:
    void setChecked(bool checked);

signals:
    void cardClicked(int skillId);
    void checkBoxToggled(int skillId, bool checked);
    void frequencyClicked(int skillId, int newFrequency);
    void agentToggled(int skillId, int agentId, bool enabled);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUi();
    void updateUi();
    void updateFrequencyColor();
    void setupLargeMode();
    void setupSmallMode();
    void showFrequencyMenu();
    QString frequencyToColor(int freq) const;
    QString frequencyToText(int freq) const;

    int m_skillId;
    QString m_name;
    QString m_description;
    int m_frequency;       // 1=常用 2=一般 3=很少 4=废弃
    QStringList m_tags;
    QVector<int> m_enabledAgentIds;  // 简单记录已启用的 agent ID
    bool m_largeMode;

    QCheckBox *m_checkBox;
    QLabel *m_nameLabel;
    QLabel *m_descLabel;
    QPushButton *m_freqButton;
    QWidget *m_tagsWidget;
    QHBoxLayout *m_tagsLayout;
    QWidget *m_agentsWidget;
    QHBoxLayout *m_agentsLayout;
    QLabel *m_smallIconLabel;

    QVector<QPushButton *> m_agentButtons;
};

#endif // SKILLCARD_H
