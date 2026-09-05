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
    // 1. Получаем текущую Activity приложения Qt
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/bindings/QtActivity", "className", "()Lorg/qtproject/qt/android/bindings/QtActivity;");

    // Альтернативный надежный способ получить текущее контекстное Activity в Qt6
    QJniObject currentActivity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative", "activity", "()Landroid/app/Activity;");

    if (!currentActivity.isValid()) {
        emit smsStatus(false, "Ошибка: не удалось получить Android Activity");
        return;
    }

    // Строка разрешения для Android
    QString permissionStr = "android.permission.SEND_SMS";
    QJniObject jPermission = QJniObject::fromString(permissionStr);

    // 2. Проверяем, дано ли разрешение (PackageManager.PERMISSION_GRANTED = 0)
    jint checkResult = currentActivity.callMethod<jint>(
        "checkSelfPermission", "(Ljava/lang/String;)I", jPermission.object<jstring>());

    if (checkResult != 0) { // 0 означает GRANTED, всё что кроме — DENIED
        qDebug() << "Разрешение на SMS не получено, запрашиваем у пользователя...";

        // Создаем массив строк Java из одного элемента для передачи в метод requestPermissions
        QJniEnvironment env;
        jobjectArray permArray = env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF("android.permission.SEND_SMS"));

        // Вызываем нативный запрос прав у пользователя (101 — произвольный ID запроса)
        currentActivity.callMethod<void>(
            "requestPermissions", "([Ljava/lang/String;I)V", permArray, jint(101));

        emit smsStatus(false, "Запрошено разрешение на отправку SMS. Повторите команду после подтверждения.");
        return;
    }

    // 3. Если разрешение есть, вызываем наш кастомный Java-класс SmsSender
    QJniObject jTo = QJniObject::fromString(to);
    QJniObject jText = QJniObject::fromString(text);

    // Вызываем статический метод: boolean sendSms(String, String) из com.app.SmsSender
    jboolean success = QJniObject::callStaticMethod<jboolean>(
        "com/app/SmsSender",
        "sendSms",
        "(Ljava/lang/String;Ljava/lang/String;)Z",
        jTo.object<jstring>(),
        jText.object<jstring>()
        );

    if (success) {
        emit smsStatus(true, "Команда успешно передана в подсистему Android SMS");
    } else {
        emit smsStatus(false, "Ошибка Java-модуля при отправке сообщения");
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
