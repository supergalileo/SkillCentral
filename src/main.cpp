#include <QApplication>
#include "ui/MainWindow.h"
#include "database/DatabaseManager.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 创建数据库管理器
    DatabaseManager *dbManager = new DatabaseManager(&app);
    QString dbPath = "D:/skillLibrary/library.db";

    if (!dbManager->openDatabase(dbPath)) {
        QMessageBox::critical(nullptr, "错误", QString("无法打开数据库：%1").arg(dbPath));
        delete dbManager;
        return -1;
    }

    if (!dbManager->initializeDatabase()) {
        QMessageBox::critical(nullptr, "错误", "数据库初始化失败（表创建失败）");
        delete dbManager;
        return -1;
    }

    MainWindow window;
    window.setDatabaseManager(dbManager);
    window.show();
    return app.exec();
}
