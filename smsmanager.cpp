#include "smsmanager.h"
#include <QDebug>

// Подключение специфичных заголовочных файлов в зависимости от ОС
#if defined(Q_OS_ANDROID)
#include <QJniObject> // Новый стандарт Qt6 для работы с JNI
#elif defined(Q_OS_IOS)
// Объявляем функцию, которая будет реализована в ios_smssender.mm
extern void sendIOSMessage(const QString &to, const QString &text);
#endif

SmsManager::SmsManager(QObject *parent)
    : QObject(parent) {}

void SmsManager::setPhoneNumber(const QString &number) {
    if (m_phoneNumber != number) {
        m_phoneNumber = number;
        emit phoneNumberChanged();
    }
}

void SmsManager::sendCommand(const QString &commandCode) {
    // В C++23 можно использовать инициализаторы в if для компактности
    if (m_phoneNumber.isEmpty()) [[unlikely]] {
        emit smsStatus(false, "Ошибка: Номер телефона сигнализации не задан!");
        return;
    }

    qDebug() << "Инициирована отправка команды:" << commandCode << "на номер:" << m_phoneNumber;

#if defined(Q_OS_ANDROID)
    sendAndroidSms(m_phoneNumber, commandCode);
#elif defined(Q_OS_IOS)
    sendIosSms(m_phoneNumber, commandCode);
#else
    // Отладка на десктопе (Windows/macOS/Linux)
    qDebug() << "Режим симуляции (Десктоп). Текст:" << commandCode;
    emit smsStatus(true, QString("Имитация отправки команды [%1] (Десктоп)").arg(commandCode));
#endif
}

void SmsManager::sendAndroidSms(const QString &to, const QString &text) {
#if defined(Q_OS_ANDROID)
    // Преобразуем QString в JNI-совместимые объекты Java String
    QJniObject jTo = QJniObject::fromString(to);
    QJniObject jText = QJniObject::fromString(text);

    // Получаем стандартный экземпляр SmsManager из Android SDK
    QJniObject smsManager = QJniObject::callStaticObjectMethod(
        "android/telephony/SmsManager",
        "getDefault",
        "()Landroid/telephony/SmsManager;"
        );

    if (smsManager.isValid()) {
        // Вызываем нативный метод отправки SMS
        smsManager.callMethod<void>(
            "sendTextMessage",
            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/app/PendingIntent;Landroid/app/PendingIntent;)V",
            jTo.object<jstring>(),
            nullptr,
            jText.object<jstring>(),
            nullptr,
            nullptr
            );
        emit smsStatus(true, "Команда отправлена в Android SMS-шлюз");
    } else {
        emit smsStatus(false, "Критическая ошибка: Android SmsManager не инициализирован");
    }
#else
    Q_UNUSED(to);
    Q_UNUSED(text);
#endif
}

void SmsManager::sendIosSms(const QString &to, const QString &text) {
#if defined(Q_OS_IOS)
    // Вызываем Objective-C++ код для открытия интерфейса отправки сообщения
    sendIOSMessage(to, text);
    emit smsStatus(true, "Открытие системного экрана отправки iOS...");
#else
    Q_UNUSED(to);
    Q_UNUSED(text);
#endif
}
