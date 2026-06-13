#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMessageBox::information(nullptr, "Test", "Qt is working!");
    return 0;
}
