#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "smsmanager.h"

// Используем новые возможности C++23 (например, атрибуты или лаконичный синтаксис, если необходимо)
[[nodiscard]] int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // Устанавливаем метаданные приложения
    app.setOrganizationName("StarLineUser");
    app.setOrganizationDomain("starline.smsapp");
    app.setApplicationName("StarLineSmsApp");

    QQmlApplicationEngine engine;

    // Регистрация SmsManager как синглтона или обычного типа для QML.
    // Благодаря Qt6 и qt_add_qml_module, типы, указанные в SOURCES в CMake,
    // автоматически регистрируются, но мы можем явно инициализировать логику, если нужно,
    // либо предоставить движку самому управлять экземпляром.

    const QUrl url(u"qrc:/StarLineSmsApp/main.qml"_uri);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
