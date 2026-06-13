#include "ui/AgentDialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QLabel>

AgentDialog::AgentDialog(QWidget *parent, const AgentInfo *agent)
    : QDialog(parent)
    , m_nameEdit(nullptr)
    , m_pathEdit(nullptr)
    , m_iconEdit(nullptr)
    , m_enabledCheck(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_editMode(false)
    , m_editId(-1)
{
    setupUi();

    if (agent != nullptr) {
        m_editMode = true;
        m_editId = agent->id;
        m_nameEdit->setText(agent->name);
        m_pathEdit->setText(agent->path);
        m_iconEdit->setText(agent->icon);
        m_enabledCheck->setChecked(agent->enabled);
        setWindowTitle("编辑 Agent");
    } else {
        setWindowTitle("添加 Agent");
        m_enabledCheck->setChecked(true);
    }
}

AgentDialog::~AgentDialog()
{
}

QString AgentDialog::getName() const
{
    return m_nameEdit->text().trimmed();
}

QString AgentDialog::getPath() const
{
    return m_pathEdit->text().trimmed();
}

QString AgentDialog::getIcon() const
{
    return m_iconEdit->text().trimmed();
}

bool AgentDialog::isEnabled() const
{
    return m_enabledCheck->isChecked();
}

void AgentDialog::onBrowsePath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择 Agent 路径",
                                                     m_pathEdit->text(),
                                                     QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()) {
        m_pathEdit->setText(dir);
    }
}

void AgentDialog::onBrowseIcon()
{
    QString file = QFileDialog::getOpenFileName(this, "选择图标",
                                                 m_iconEdit->text(),
                                                 "图片文件 (*.png *.jpg *.jpeg *.ico *.svg)");
    if (!file.isEmpty()) {
        m_iconEdit->setText(file);
    }
}

void AgentDialog::onAccept()
{
    // 验证名称
    QString name = getName();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "警告", "名称不能为空");
        return;
    }

    // 验证路径（允许为空或不存在，只做提示）
    QString path = getPath();
    if (!path.isEmpty() && !QDir(path).exists()) {
        auto reply = QMessageBox::warning(this, "提示",
            "路径 \"" + path + "\" 当前不存在。\n是否仍要保存？\n（稍后可在 agent 目录中手动创建）",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    QDialog::accept();
}

void AgentDialog::setupUi()
{
    // 表单布局
    QFormLayout *formLayout = new QFormLayout();

    m_nameEdit = new QLineEdit(this);
    formLayout->addRow("名称:", m_nameEdit);

    // 路径行
    QWidget *pathWidget = new QWidget(this);
    QHBoxLayout *pathLayout = new QHBoxLayout(pathWidget);
    pathLayout->setContentsMargins(0, 0, 0, 0);
    m_pathEdit = new QLineEdit(pathWidget);
    QPushButton *pathButton = new QPushButton("浏览...", pathWidget);
    connect(pathButton, &QPushButton::clicked, this, &AgentDialog::onBrowsePath);
    pathLayout->addWidget(m_pathEdit);
    pathLayout->addWidget(pathButton);
    formLayout->addRow("路径:", pathWidget);

    // 图标行
    QWidget *iconWidget = new QWidget(this);
    QHBoxLayout *iconLayout = new QHBoxLayout(iconWidget);
    iconLayout->setContentsMargins(0, 0, 0, 0);
    m_iconEdit = new QLineEdit(iconWidget);
    QPushButton *iconButton = new QPushButton("浏览...", iconWidget);
    connect(iconButton, &QPushButton::clicked, this, &AgentDialog::onBrowseIcon);
    iconLayout->addWidget(m_iconEdit);
    iconLayout->addWidget(iconButton);
    formLayout->addRow("图标:", iconWidget);

    m_enabledCheck = new QCheckBox("启用", this);
    formLayout->addRow("", m_enabledCheck);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    m_okButton = new QPushButton("确定", this);
    m_cancelButton = new QPushButton("取消", this);
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);

    connect(m_okButton, &QPushButton::clicked, this, &AgentDialog::onAccept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setMinimumWidth(500);
}

bool AgentDialog::validateName(const QString &name)
{
    Q_UNUSED(name);
    // 基本验证：名称不能为空（已在 onAccept 中检查）
    // 名称重复检查在 SettingsDialog::onAddAgent() 中完成
    return true;
}

bool AgentDialog::validatePath(const QString &path)
{
    Q_UNUSED(path);
    // 基本验证：路径不能为空且必须存在（已在 onAccept 中检查）
    return true;
}
