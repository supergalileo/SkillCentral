#ifndef AGENTDIALOG_H
#define AGENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include "database/DatabaseManager.h"

QT_FORWARD_DECLARE_CLASS(QLineEdit)

class AgentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AgentDialog(QWidget *parent = nullptr, const AgentInfo *agent = nullptr);
    ~AgentDialog();

    QString getName() const;
    QString getPath() const;
    QString getIcon() const;
    bool isEnabled() const;

private slots:
    void onBrowsePath();
    void onBrowseIcon();
    void onAccept();

private:
    void setupUi();
    bool validateName(const QString &name);
    bool validatePath(const QString &path);

    QLineEdit *m_nameEdit;
    QLineEdit *m_pathEdit;
    QLineEdit *m_iconEdit;
    QCheckBox *m_enabledCheck;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    bool m_editMode;
    int m_editId;
};

#endif // AGENTDIALOG_H
