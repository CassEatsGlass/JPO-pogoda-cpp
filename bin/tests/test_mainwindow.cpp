#include <QtTest/QtTest>
#include "../mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow test;
    return QTest::qExec(&test, argc, argv);  // Manually run the tests
}
