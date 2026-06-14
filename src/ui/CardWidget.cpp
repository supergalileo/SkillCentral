#include "ui/CardWidget.h"
#include "ui/SkillCard.h"
#include <QResizeEvent>
#include <QScrollArea>
#include <QLayoutItem>
#include <QMouseEvent>
#include <QTimer>
#include <algorithm>

CardScrollArea::CardScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->setAutoFillBackground(false);
}

bool CardScrollArea::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        qInfo() << "CardScrollArea viewportEvent: button=" << me->button() << "pos=" << me->pos();
        if (me->button() == Qt::LeftButton) {
            emit viewportClicked(me->pos());
        }
    }
    return QScrollArea::viewportEvent(event);
}

CardWidget::CardWidget(QWidget *parent)
    : QWidget(parent)
    , m_largeMode(true)
    , m_gridLayout(nullptr)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(12);
    m_gridLayout->setContentsMargins(12, 12, 12, 12);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumWidth(0);
}

void CardWidget::onViewportClicked(const QPoint &viewportPos)
{
    QScrollArea *scrollArea = qobject_cast<QScrollArea*>(parentWidget());
    if (!scrollArea) return;

    QPoint posInWidget = scrollArea->viewport()->mapTo(this, viewportPos);

    for (SkillCard *card : m_cards) {
        if (card->isVisible() && card->geometry().contains(posInWidget)) {
            qInfo() << "CardWidget: clicked card" << card->getSkillId();
            emit cardClicked(card->getSkillId());
            return;
        }
    }
}

bool CardWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick && obj != this) {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            qInfo() << "CardWidget eventFilter on" << obj->metaObject()->className();
            QWidget *viewport = qobject_cast<QWidget*>(obj);
            if (viewport) {
                QPoint posInWidget = viewport->mapTo(this, me->pos());
                for (SkillCard *card : m_cards) {
                    if (card->isVisible() && card->geometry().contains(posInWidget)) {
                        qInfo() << "CardWidget filter: matched card" << card->getSkillId();
                        emit cardClicked(card->getSkillId());
                        return false;
                    }
                }
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

CardWidget::~CardWidget()
{
    clearSkills();
}

void CardWidget::setSkills(const QVector<SkillInfo> &skills)
{
    clearSkills();
    m_allSkills = skills;
    for (const SkillInfo &skill : skills) {
        SkillCard *card = new SkillCard(skill, this);
        card->setLargeMode(m_largeMode);
        connect(card, &SkillCard::cardClicked, this, &CardWidget::onCardClicked);
        connect(card, &SkillCard::checkBoxToggled, this, &CardWidget::onCheckBoxToggled);
        connect(card, &SkillCard::frequencyClicked, this, &CardWidget::frequencyChanged);
        connect(card, &SkillCard::agentToggled, this, &CardWidget::agentToggled);
        connect(card, &SkillCard::tagAddRequested, this, &CardWidget::tagAddRequested);
        connect(card, &SkillCard::tagRemoveRequested, this, &CardWidget::tagRemoveRequested);
        connect(card, &SkillCard::deleteRequested, this, &CardWidget::deleteRequested);
        connect(card, &SkillCard::openFolderRequested, this, &CardWidget::openFolderRequested);
        connect(card, &SkillCard::agentFolderRequested, this, &CardWidget::agentFolderRequested);
        card->setAgentList(m_agentList);
        m_cards.append(card);
    }
    filter(m_filterText, m_filterTags);
}

void CardWidget::setAgentList(const QVector<AgentInfo> &agents)
{
    m_agentList = agents;
    for (SkillCard *card : m_cards) {
        card->setAgentList(m_agentList);
    }
}

void CardWidget::addSkill(const SkillInfo &skill)
{
    m_allSkills.append(skill);
    SkillCard *card = new SkillCard(skill, this);
    card->setLargeMode(m_largeMode);
    connect(card, &SkillCard::cardClicked, this, &CardWidget::onCardClicked);
    connect(card, &SkillCard::checkBoxToggled, this, &CardWidget::onCheckBoxToggled);
    connect(card, &SkillCard::frequencyClicked, this, &CardWidget::frequencyChanged);
    connect(card, &SkillCard::agentToggled, this, &CardWidget::agentToggled);
    connect(card, &SkillCard::tagAddRequested, this, &CardWidget::tagAddRequested);
    connect(card, &SkillCard::tagRemoveRequested, this, &CardWidget::tagRemoveRequested);
    connect(card, &SkillCard::deleteRequested, this, &CardWidget::deleteRequested);
    connect(card, &SkillCard::openFolderRequested, this, &CardWidget::openFolderRequested);
    connect(card, &SkillCard::agentFolderRequested, this, &CardWidget::agentFolderRequested);
    card->setAgentList(m_agentList);
    m_cards.append(card);
    filter(m_filterText, m_filterTags);
}

void CardWidget::clearSkills()
{
    for (SkillCard *card : m_cards) {
        card->hide();
        card->setParent(nullptr);
        card->deleteLater();
    }
    m_cards.clear();
    m_allSkills.clear();
}

void CardWidget::updateSkill(int skillId, const SkillInfo &newSkill)
{
    // 更新内存数据
    for (int i = 0; i < m_allSkills.size(); ++i) {
        if (m_allSkills[i].id == skillId) {
            m_allSkills[i] = newSkill;
            break;
        }
    }
    // 更新对应卡片（不重建、不重排）
    for (SkillCard *card : m_cards) {
        if (card->getSkillId() == skillId) {
            card->updateSkillData(newSkill);
            break;
        }
    }
}

void CardWidget::setCardSize(bool large)
{
    if (m_largeMode != large) {
        m_largeMode = large;
        for (SkillCard *card : m_cards) {
            card->setLargeMode(large);
        }
        relayout();
    }
}

void CardWidget::filter(const QString &text, const QStringList &tags)
{
    m_filterText = text;
    m_filterTags = tags;
    applyFilter();
}

void CardWidget::selectAll(bool checked)
{
    for (SkillCard *card : m_cards) {
        if (card->isVisible()) {
            card->setChecked(checked);
        }
    }
    emit selectionChanged(getSelectedCount());
}

void CardWidget::filterByFrequency(const QVector<int> &frequencies)
{
    m_filterFrequencies = frequencies;
    applyFilter();
}

void CardWidget::applyFilter()
{
    for (SkillCard *card : m_cards) {
        SkillInfo skill;
        for (const SkillInfo &s : m_allSkills) {
            if (s.id == card->getSkillId()) {
                skill = s;
                break;
            }
        }
        bool visible = matchesFilter(skill);
        card->setVisible(visible);
    }
    relayout();
}

bool CardWidget::matchesFilter(const SkillInfo &skill) const
{
    if (!m_filterText.isEmpty()) {
        QStringList keywords = m_filterText.split(" ", Qt::SkipEmptyParts);
        for (const QString &kw : keywords) {
            QString lowerKw = kw.toLower();
            if (!skill.name.toLower().contains(lowerKw) &&
                !skill.description.toLower().contains(lowerKw) &&
                !skill.tags.join(" ").toLower().contains(lowerKw)) {
                return false;
            }
        }
    }

    if (!m_filterTags.isEmpty()) {
        bool matchAny = false;
        for (const QString &tag : m_filterTags) {
            if (skill.tags.contains(tag)) {
                matchAny = true;
                break;
            }
        }
        if (!matchAny) return false;
    }

    if (!m_filterFrequencies.isEmpty()) {
        if (!m_filterFrequencies.contains(skill.frequency)) {
            return false;
        }
    }

    return true;
}

void CardWidget::sortBy(const QString &criteria)
{
    m_sortCriteria = criteria;

    QVector<SkillInfo> sorted = m_allSkills;
    if (criteria == "frequency") {
        std::sort(sorted.begin(), sorted.end(),
            [](const SkillInfo &a, const SkillInfo &b) {
                return a.frequency < b.frequency;
            });
    } else if (criteria == "name") {
        std::sort(sorted.begin(), sorted.end(),
            [](const SkillInfo &a, const SkillInfo &b) {
                return a.name.toLower() < b.name.toLower();
            });
    } else if (criteria == "createdAt") {
        std::sort(sorted.begin(), sorted.end(),
            [](const SkillInfo &a, const SkillInfo &b) {
                return a.createdAt > b.createdAt;
            });
    } else if (criteria == "lastUsed") {
        std::sort(sorted.begin(), sorted.end(),
            [](const SkillInfo &a, const SkillInfo &b) {
                return a.lastUsed > b.lastUsed;
            });
    }

    setSkills(sorted);
}

QVector<int> CardWidget::getSelectedSkillIds() const
{
    QVector<int> ids;
    for (SkillCard *card : m_cards) {
        if (card->isVisible() && card->isChecked()) {
            ids.append(card->getSkillId());
        }
    }
    return ids;
}

int CardWidget::getSelectedCount() const
{
    int count = 0;
    for (SkillCard *card : m_cards) {
        if (card->isVisible() && card->isChecked()) {
            count++;
        }
    }
    return count;
}

void CardWidget::onCardClicked(int skillId)
{
    emit cardClicked(skillId);
}

void CardWidget::onCheckBoxToggled(int skillId, bool checked)
{
    Q_UNUSED(skillId);
    Q_UNUSED(checked);
    emit selectionChanged(getSelectedCount());
}

void CardWidget::relayout()
{
    clearLayout();

    int col = 0;
    int row = 0;
    int cols = calculateColumns();

    for (SkillCard *card : m_cards) {
        if (card->isVisible()) {
            m_gridLayout->addWidget(card, row, col);
            col++;
            if (col >= cols) {
                col = 0;
                row++;
            }
        }
    }
}

void CardWidget::clearLayout()
{
    QLayoutItem *item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        delete item;
    }
}

int CardWidget::calculateColumns() const
{
    int cardWidth = m_largeMode ? 302 : 152;
    int availableWidth = width();
    // 使用 viewport 宽度限制，防止全屏恢复后列数超出边界
    if (parentWidget()) {
        QScrollArea *scrollArea = qobject_cast<QScrollArea*>(parentWidget()->parentWidget());
        if (scrollArea && scrollArea->viewport()) {
            availableWidth = qMin(availableWidth, scrollArea->viewport()->width());
        }
    }
    if (availableWidth <= 0) availableWidth = 800;
    int cols = qMax(1, availableWidth / cardWidth);
    return cols;
}

void CardWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 强制重新计算布局，防止全屏恢复后列数异常
    QTimer::singleShot(0, this, [this]() {
        relayout();
    });
}
