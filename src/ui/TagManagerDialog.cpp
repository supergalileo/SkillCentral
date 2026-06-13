#include "ui/TagManagerDialog.h"
#include "database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QListWidgetItem>

TagManagerDialog::TagManagerDialog(DatabaseManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , m_dbManager(dbManager)
    , m_tagList(nullptr)
    , m_addButton(nullptr)
    , m_renameButton(nullptr)
    , m_mergeButton(nullptr)
    , m_deleteButton(nullptr)
    , m_clearAllButton(nullptr)
    , m_closeButton(nullptr)
{
    setupUi();
    refreshTags();
}

TagManagerDialog::~TagManagerDialog()
{
}

// ============ 按钮槽 ============

void TagManagerDialog::onAddTag()
{
    bool ok;
    QString name = QInputDialog::getText(this, "添加标签",
        "输入标签名称：", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) {
        return;
    }

    QString trimmed = name.trimmed();
    if (!validateTagName(trimmed)) {
        QMessageBox::warning(this, "警告", "标签名称已存在或无效");
        return;
    }

    int id = m_dbManager->addTag(trimmed);
    if (id == -1) {
        QMessageBox::warning(this, "错误", "添加标签失败");
        return;
    }

    refreshTags();
}

void TagManagerDialog::onRenameTag()
{
    QList<QListWidgetItem *> selected = m_tagList->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    QListWidgetItem *item = selected.first();
    int tagId = item->data(Qt::UserRole).toInt();
    QString oldName = item->text();

    bool ok;
    QString newName = QInputDialog::getText(this, "重命名标签",
        "输入新名称：", QLineEdit::Normal, oldName, &ok);
    if (!ok || newName.trimmed().isEmpty()) {
        return;
    }

    QString trimmed = newName.trimmed();
    if (trimmed == oldName) {
        return;
    }
    if (!validateTagName(trimmed, tagId)) {
        QMessageBox::warning(this, "警告", "标签名称已存在");
        return;
    }

    if (!m_dbManager->renameTag(tagId, trimmed)) {
        QMessageBox::warning(this, "错误", "重命名失败");
        return;
    }

    refreshTags();
}

void TagManagerDialog::onMergeTags()
{
    QList<QListWidgetItem *> selected = m_tagList->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "提示",
            "请先选择一个源标签（将被合并后删除）");
        return;
    }

    int sourceId = selected.first()->data(Qt::UserRole).toInt();
    QString sourceName = selected.first()->text();

    // 收集可合并的目标标签
    QStringList tagNames;
    QVector<int> tagIds;
    for (int i = 0; i < m_tagList->count(); ++i) {
        QListWidgetItem *item = m_tagList->item(i);
        int id = item->data(Qt::UserRole).toInt();
        if (id != sourceId) {
            tagNames << item->text();
            tagIds << id;
        }
    }

    if (tagNames.isEmpty()) {
        QMessageBox::information(this, "提示", "没有其他标签可作为合并目标");
        return;
    }

    bool ok;
    QString targetName = QInputDialog::getItem(this, "合并标签",
        QString("将「%1」合并到：").arg(sourceName),
        tagNames, 0, false, &ok);
    if (!ok) {
        return;
    }

    int targetIndex = tagNames.indexOf(targetName);
    if (targetIndex < 0) {
        return;
    }
    int targetId = tagIds[targetIndex];

    auto reply = QMessageBox::question(this, "确认合并",
        QString("将标签「%1」合并到「%2」？\n合并后「%1」将被删除。")
            .arg(sourceName).arg(targetName),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    if (!m_dbManager->mergeTags(sourceId, targetId)) {
        QMessageBox::warning(this, "错误", "合并失败");
        return;
    }

    refreshTags();
}

void TagManagerDialog::onDeleteTag()
{
    QList<QListWidgetItem *> selected = m_tagList->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    QString name = selected.first()->text();

    auto reply = QMessageBox::question(this, "确认删除",
        QString("确定要删除标签「%1」吗？\n该标签将从所有 skill 中移除。").arg(name),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    int tagId = selected.first()->data(Qt::UserRole).toInt();
    if (!m_dbManager->deleteTag(tagId)) {
        QMessageBox::warning(this, "错误", "删除失败");
        return;
    }

    refreshTags();
}

void TagManagerDialog::onClearAllTags()
{
    auto reply = QMessageBox::question(this, "确认清空",
        "确定要清空所有标签吗？\n所有标签将从所有 skill 中移除，此操作不可撤销。",
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    if (!m_dbManager->clearAllTags()) {
        QMessageBox::warning(this, "错误", "清空失败");
        return;
    }

    refreshTags();
}

void TagManagerDialog::onSelectionChanged()
{
    bool hasSelection = !m_tagList->selectedItems().isEmpty();
    m_renameButton->setEnabled(hasSelection);
    m_mergeButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}

void TagManagerDialog::onContextMenu(const QPoint &pos)
{
    QList<QListWidgetItem *> selected = m_tagList->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    QMenu menu(this);
    QAction *renameAction = menu.addAction("重命名");
    QAction *mergeAction = menu.addAction("合并到...");
    QAction *deleteAction = menu.addAction("删除");

    QAction *chosen = menu.exec(m_tagList->mapToGlobal(pos));
    if (!chosen) {
        return;
    }

    if (chosen == renameAction) {
        onRenameTag();
    } else if (chosen == mergeAction) {
        onMergeTags();
    } else if (chosen == deleteAction) {
        onDeleteTag();
    }
}

// ============ 私有方法 ============

void TagManagerDialog::setupUi()
{
    setWindowTitle("管理标签");
    setMinimumSize(400, 500);

    // 列表
    m_tagList = new QListWidget(this);
    m_tagList->setSelectionMode(QListWidget::SingleSelection);
    m_tagList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tagList, &QListWidget::itemSelectionChanged,
            this, &TagManagerDialog::onSelectionChanged);
    connect(m_tagList, &QListWidget::customContextMenuRequested,
            this, &TagManagerDialog::onContextMenu);

    // 按钮区
    m_addButton = new QPushButton("添加", this);
    m_renameButton = new QPushButton("重命名", this);
    m_mergeButton = new QPushButton("合并", this);
    m_deleteButton = new QPushButton("删除", this);
    m_clearAllButton = new QPushButton("清空所有", this);
    m_closeButton = new QPushButton("关闭", this);

    m_renameButton->setEnabled(false);
    m_mergeButton->setEnabled(false);
    m_deleteButton->setEnabled(false);

    connect(m_addButton, &QPushButton::clicked, this, &TagManagerDialog::onAddTag);
    connect(m_renameButton, &QPushButton::clicked, this, &TagManagerDialog::onRenameTag);
    connect(m_mergeButton, &QPushButton::clicked, this, &TagManagerDialog::onMergeTags);
    connect(m_deleteButton, &QPushButton::clicked, this, &TagManagerDialog::onDeleteTag);
    connect(m_clearAllButton, &QPushButton::clicked, this, &TagManagerDialog::onClearAllTags);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_addButton);
    btnLayout->addWidget(m_renameButton);
    btnLayout->addWidget(m_mergeButton);
    btnLayout->addWidget(m_deleteButton);
    btnLayout->addWidget(m_clearAllButton);
    btnLayout->addStretch();
    btnLayout->addWidget(m_closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tagList, 1);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);
}

void TagManagerDialog::refreshTags()
{
    m_tagList->clear();

    QVector<TagInfo> tags = m_dbManager->getTags();
    for (const TagInfo &tag : tags) {
        QListWidgetItem *item = new QListWidgetItem(tag.name, m_tagList);
        item->setData(Qt::UserRole, tag.id);
    }

    onSelectionChanged();
}

bool TagManagerDialog::validateTagName(const QString &name, int excludeId)
{
    if (name.isEmpty() || name.length() > 50) {
        return false;
    }

    QVector<TagInfo> tags = m_dbManager->getTags();
    for (const TagInfo &tag : tags) {
        if (tag.name == name && tag.id != excludeId) {
            return false;
        }
    }
    return true;
}
