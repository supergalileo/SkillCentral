#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QTableWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QProgressBar>
#include <QDateTime>
#include <QDir>

class DatabaseManager;
class AgentManager;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(DatabaseManager *dbManager, AgentManager *agentManager = nullptr, QWidget *parent = nullptr);
    ~SettingsDialog();

signals:
    void settingsChanged();
    void agentsChanged();
    void libraryPathChanged(const QString &newPath);
    void themeChanged(const QString &theme);

protected:
    void accept() override;
    void reject() override;

private slots:
    // 常规标签页
    void onBrowseLibraryPath();
    void onResetLibraryPath();
    void onThemeChanged(int index);

    // Agent 管理标签页
    void onAddAgent();
    void onEditAgent();
    void onToggleAgent();
    void onDeleteAgent();
    void onAgentSelectionChanged();
    void onAgentContextMenu(const QPoint &pos);

    // 备份标签页
    void onBackupNow();
    void onRestoreBackup();
    void onBackupSelectionChanged();

    // 关于标签页
    void onCheckUpdate();

private:
    void setupUi();
    QWidget* setupGeneralTab();
    QWidget* setupAgentsTab();
    QWidget* setupBackupTab();
    QWidget* setupAboutTab();
    void loadSettings();
    void saveSettings();
    void refreshAgents();
    void refreshBackups();
    QString getDefaultLibraryPath() const;
    QString getBackupDir() const;
    QString getNextBackupTime() const;

    DatabaseManager *m_dbManager;
    AgentManager *m_agentManager;

    QTabWidget *m_tabWidget;

    // 常规标签页
    QLineEdit *m_libraryPathEdit;
    QCheckBox *m_autoScanCheck;
    QCheckBox *m_autoBackupCheck;
    QComboBox *m_backupFrequencyCombo;
    QComboBox *m_themeCombo;
    QLabel *m_themeWarningLabel;

    // Agent 管理标签页
    QTableWidget *m_agentTable;
    QPushButton *m_addAgentButton;
    QPushButton *m_editAgentButton;
    QPushButton *m_toggleAgentButton;
    QPushButton *m_deleteAgentButton;

    // 备份标签页
    QPushButton *m_backupNowButton;
    QListWidget *m_backupList;
    QPushButton *m_restoreButton;
    QLabel *m_nextBackupLabel;

    // 关于标签页
    QLabel *m_versionLabel;
    QLabel *m_copyrightLabel;
    QPushButton *m_checkUpdateButton;
    QPushButton *m_aboutQtButton;
};

#endif // SETTINGSDIALOG_H
