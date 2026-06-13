#ifndef TAGMANAGERDIALOG_H
#define TAGMANAGERDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>

class DatabaseManager;

class TagManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagManagerDialog(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~TagManagerDialog();

private slots:
    void onAddTag();
    void onRenameTag();
    void onMergeTags();
    void onDeleteTag();
    void onSelectionChanged();
    void onContextMenu(const QPoint &pos);

private:
    void setupUi();
    void refreshTags();
    bool validateTagName(const QString &name, int excludeId = -1);

    DatabaseManager *m_dbManager;
    QListWidget *m_tagList;
    QPushButton *m_addButton;
    QPushButton *m_renameButton;
    QPushButton *m_mergeButton;
    QPushButton *m_deleteButton;
    QPushButton *m_closeButton;
};

#endif // TAGMANAGERDIALOG_H
