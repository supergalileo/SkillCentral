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
    , m_closeButton(nullptr)
{
    setupUi();
    refreshTags();
}

TagManagerDialog::~TagManagerDialog()
{
}

void TagManagerDialog::onAddTag()
{
    bool ok;
    QString name = QInputDialog::getText(this, "添加标签",
                                          "标签名称:", QLineEdit::Normal,
                                          "", &ok);
    if (ok && !name.isEmpty()) {
        if (!validateTagName(name)) {
            QMessageBox::warning(this, "警告", "标签名称已存在");
            return;
        }

        int id = m_dbManager->addTag(name);
        if (id > 0) {
            refreshTags();
        } else {
            QMessageBox::warning(this, "错误", "添加标签失败");
        }
    }
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
                                            "新名称:", QLineEdit::Normal,
                                            oldName, &ok);
    if (ok && !newName.isEmpty() && newName != oldName) {
        if (!validateTagName(newName, tagId)) {
            QMessageBox::warning(this, "警告", "标签名称已存在");
            return;
        }

        if (m_dbManager->renameTag(tagId, newName)) {
            refreshTags();
        } else {
            QMessageBox::warning(this, "错误", "重命名标签失败");
        }
    }
}

void TagManagerDialog::onMergeTags()
{
    QList<QListWidgetItem *> selected = m_tagList->selectedItems();
    if (selected.size() != 2) {
        QMessageBox::warning(this, "警告", "请选择两个标签进行合并");
        return;
    }

    int sourceId = selected[0]->data(Qt::UserRole).toInt();
    int targetId = selected[1]->data(Qt::UserRole).toInt();

    QString sourceName = selected[0]->text();
    QString targetName = selected[1]->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认合并",
                                  QString("确定要将标签 '%1' 合并到 '%2' 吗？\n"
                                           "合并后 '%1' 将被删除，所有关联将转移到 '%2'。")
                                          .arg(sourceName).arg(targetName),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_dbManager->mergeTags(sourceId, targetId)) {
            refreshTags();
        } else {
            QMessageBox::warning(this, "错误", "合并标签失败");
        }
    }
}

void TagManagerDialog::onDeleteTag()
{
    QList<QListWidgetItem *> selected = m_tagList->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    QListWidgetItem *item = selected.first();
    int tagId = item->data(Qt::UserRole).toInt();
    QString tagName = item->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                  QString("确定要删除标签 '%1' 吗？\n"
                                           "删除后所有关联将同时删除。")
                                          .arg(tagName),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_dbManager->deleteTag(tagId)) {
            refreshTags();
        } else {
            QMessageBox::warning(this, "错误", "删除标签失败");
        }
    }
}

void TagManagerDialog::onSelectionChanged()
{
    QList<QListWidgetItem *> selected = m_tagList->selectedItems();
    int count = selected.size();

    m_renameButton->setEnabled(count == 1);
    m_mergeButton->setEnabled(count == 2);
    m_deleteButton->setEnabled(count == 1);
}

void TagManagerDialog::onContextMenu(const QPoint &pos)
{
    QList<QListWidgetItem *> selected = m_tagList->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    QMenu contextMenu(this);

    if (selected.size() == 1) {
        QAction *renameAction = contextMenu.addAction("重命名");
        connect(renameAction, &QAction::triggered, this, &TagManagerDialog::onRenameTag);

        QAction *deleteAction = contextMenu.addAction("删除");
        connect(deleteAction, &QAction::triggered, this, &TagManagerDialog::onDeleteTag);
    } else if (selected.size() == 2) {
        QAction *mergeAction = contextMenu.addAction("合并");
        connect(mergeAction, &QAction::triggered, this, &TagManagerDialog::onMergeTags);
    }

    contextMenu.exec(m_tagList->mapToGlobal(pos));
}

void TagManagerDialog::setupUi()
{
    setWindowTitle("标签管理");
    setMinimumSize(400, 500);

    // 标签列表
    m_tagList = new QListWidget(this);
    m_tagList->setSelectionMode(QListWidget::MultiSelection);
    connect(m_tagList, &QListWidget::itemSelectionChanged,
            this, &TagManagerDialog::onSelectionChanged);
    connect(m_tagList, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem *item) {
                onRenameTag();
            });
    m_tagList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tagList, &QListWidget::customContextMenuRequested,
            this, &TagManagerDialog::onContextMenu);

    // 按钮
    m_addButton = new QPushButton("添加", this);
    m_renameButton = new QPushButton("重命名", this);
    m_mergeButton = new QPushButton("合并", this);
    m_deleteButton = new QPushButton("删除", this);
    m_closeButton = new QPushButton("关闭", this);

    m_renameButton->setEnabled(false);
    m_mergeButton->setEnabled(false);
    m_deleteButton->setEnabled(false);

    connect(m_addButton, &QPushButton::clicked, this, &TagManagerDialog::onAddTag);
    connect(m_renameButton, &QPushButton::clicked, this, &TagManagerDialog::onRenameTag);
    connect(m_mergeButton, &QPushButton::clicked, this, &TagManagerDialog::onMergeTags);
    connect(m_deleteButton, &QPushButton::clicked, this, &TagManagerDialog::onDeleteTag);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_renameButton);
    buttonLayout->addWidget(m_mergeButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeButton);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tagList);
    mainLayout->addLayout(buttonLayout);

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
}

bool TagManagerDialog::validateTagName(const QString &name, int excludeId)
{
    QVector<TagInfo> tags = m_dbManager->getTags();
    for (const TagInfo &tag : tags) {
        if (tag.id != excludeId && tag.name == name) {
            return false;
        }
    }
    return true;
}
