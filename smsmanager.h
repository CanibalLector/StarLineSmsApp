#pragma once

#include <QObject>
#include <QString>
#include <QtQml/QQmlEngine>

class SmsManager : public QObject {
    Q_OBJECT
    // Регистрирует класс в QML под его именем SmsManager
    QML_ELEMENT

    // Свойство для хранения номера телефона сигнализации
    Q_PROPERTY(QString phoneNumber READ phoneNumber WRITE setPhoneNumber NOTIFY phoneNumberChanged)

public:
    explicit SmsManager(QObject *parent = nullptr);
    ~SmsManager() override = default;

    // Метод для отправки команд, доступный из QML.
    // [[nodiscard]] из C++23 подскажет компилятору выдать предупреждение, если результат проигнорирован.
    Q_INVOKABLE [[nodiscard]] void sendCommand(const QString &commandCode);

    // Геттер и сеттер для номера телефона
    [[nodiscard]] QString phoneNumber() const noexcept { return m_phoneNumber; }
    void setPhoneNumber(const QString &number);

signals:
    // Сигнал, уведомляющий об изменении номера телефона
    void phoneNumberChanged();

    // Сигнал для передачи статуса отправки в QML
    void smsStatus(bool success, const QString &message);

private:
    QString m_phoneNumber;

    // Внутренние методы для разделения логики по платформам
    void sendAndroidSms(const QString &to, const QString &text);
    void sendIosSms(const QString &to, const QString &text);
};
