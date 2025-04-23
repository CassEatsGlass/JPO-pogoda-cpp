#include <QApplication>
#include <QtWidgets/QMainWindow>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "mainwindow.h"
#include <QQuickWindow>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // UCreating our main windows
    MainWindow mainWindow;

    QQmlApplicationEngine engine;

    // Exposing our mainWindow to qml
    engine.rootContext()->setContextProperty("mainWindow", &mainWindow);

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    engine.load(url);

    // Exposing it also as aqmlWindow, because resizing was not working properly
    if (!engine.rootObjects().isEmpty()) {
        QObject *topLevel = engine.rootObjects().first();
        QQuickWindow *qmlWindow = qobject_cast<QQuickWindow *>(topLevel);
        if (qmlWindow) {
            engine.rootContext()->setContextProperty("appWindow", qmlWindow);
            qmlWindow->show();
        }
    }


    return app.exec();
}
