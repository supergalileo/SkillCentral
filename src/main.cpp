#include <QApplication>
#include "ui/MainWindow.h"
#include "database/DatabaseManager.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>

// 崩溃日志输出到 startup.log
void logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    QFile file("D:/skillLibrary/logs/startup.log");
    file.open(QIODevice::Append | QIODevice::Text | QIODevice::Unbuffered);
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    QString prefix;
    switch (type) {
        case QtDebugMsg: prefix = "[DEBUG]"; break;
        case QtInfoMsg:  prefix = "[INFO]";  break;
        case QtWarningMsg: prefix = "[WARN]"; break;
        case QtCriticalMsg: prefix = "[ERROR]"; break;
        case QtFatalMsg: prefix = "[FATAL]"; break;
    }
    out << QDateTime::currentDateTime().toString("hh:mm:ss") << " " << prefix << " " << msg << "\n";
    out.flush();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 安装日志钩子（必须在 QApplication 之后）
    QDir().mkpath("D:/skillLibrary/logs");
    qInstallMessageHandler(logMessage);

    qInfo() << "=== SkillCentral starting ===";

    // 创建数据库管理器
    DatabaseManager *dbManager = new DatabaseManager(&app);
    QString dbPath = "D:/skillLibrary/library.db";

    if (!dbManager->openDatabase(dbPath)) {
        qCritical() << "Failed to open database:" << dbPath;
        QMessageBox::critical(nullptr, "错误", QString("无法打开数据库：%1").arg(dbPath));
        delete dbManager;
        return -1;
    }

    qInfo() << "Database opened:" << dbPath;

    if (!dbManager->initializeDatabase()) {
        qCritical() << "Database initialization failed";
        QMessageBox::critical(nullptr, "错误", "数据库初始化失败（表创建失败）");
        delete dbManager;
        return -1;
    }

    qInfo() << "Database initialized";

    MainWindow window;
    qInfo() << "MainWindow created, calling setDatabaseManager...";
    window.setDatabaseManager(dbManager);
    qInfo() << "setDatabaseManager done, showing window...";
    window.show();
    qInfo() << "Window shown, entering event loop";

    int ret = app.exec();
    qInfo() << "Event loop exited, return code:" << ret;
    return ret;
}
