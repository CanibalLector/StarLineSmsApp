import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StarLineSmsApp 1.0 // Импортируем наш C++ модуль

Window {
    id: root
    width: 360
    height: 640
    visible: true
    title: "StarLine SMS Контроль"

    // Светлая/темная тема (можно расширить)
    readonly property color colorPrimary: "#0066CC"     // Фирменный синий
    readonly property color colorDanger: "#D32F2F"      // Красный для стоп/тревоги
    readonly property color colorSuccess: "#388E3C"     // Зеленый для запуска
    readonly property color colorBg: "#F5F7FA"          // Светлый фон приложения

    color: colorBg

    // Экземпляр нашего C++ класса
    SmsManager {
        id: smsManager
        phoneNumber: phoneField.text

        onSmsStatus: (success, message) => {
            statusText.text = message
            statusText.color = success ? colorSuccess : colorDanger
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // --- БЛОК НАСТРОЕК НОМЕРА ---
        GroupBox {
            title: "Настройки устройства"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 8

                Label {
                    text: "Номер телефона сигнализации:"
                    font.bold: true
                }

                TextField {
                    id: phoneField
                    Layout.fillWidth: true
                    placeholderText: "+79XXXXXXXXX"
                    // Вызывает цифровую клавиатуру на смартфонах
                    inputMethodHints: Qt.ImhDialableCharactersOnly
                    text: "+79123456789" // Временный тестовый номер
                }
            }
        }

        // --- БЛОК ОСНОВНЫХ КОМАНД (ОХРАНА) ---
        GroupBox {
            title: "Режимы охраны"
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                spacing: 12

                Button {
                    text: "🔒 Поставить"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    onClicked: smsManager.sendCommand("11")
                }

                Button {
                    text: "🔓 Снять"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    onClicked: smsManager.sendCommand("10")
                }
            }
        }

        // --- БЛОК ДВИГАТЕЛЯ (АКЦЕНТИРОВАННЫЙ) ---
        GroupBox {
            title: "Управление двигателем"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 12

                Button {
                    text: "🚀 Запустить двигатель"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 55

                    // Выделяем кнопку цветом
                    contentItem: Text {
                        text: parent.text
                        font.pixelSize: 16
                        font.bold: true
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: parent.down ? "#2E7D32" : colorSuccess
                        radius: 8
                    }

                    onClicked: smsManager.sendCommand("21")
                }

                Button {
                    text: "🛑 Заглушить двигатель"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    onClicked: smsManager.sendCommand("20")
                }
            }
        }

        // --- ИНФОРМАЦИОННЫЙ БЛОК ---
        Button {
            text: "📊 Запросить состояние системы"
            Layout.fillWidth: true
            Layout.preferredHeight: 45
            onClicked: smsManager.sendCommand("41")
        }

        // Пружина, чтобы прижать статус-бар к низу
        Item {
            Layout.fillHeight: true
        }

        // --- СТАТУС БАР ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#EEEFF2"
            radius: 6

            Text {
                id: statusText
                anchors.centerIn: parent
                text: "Приложение готово к работе"
                font.pixelSize: 13
                font.bold: true
                color: "#555555"
            }
        }
    }
}
