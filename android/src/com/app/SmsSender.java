package com.app;

import android.content.Context;
import android.telephony.SmsManager;
import android.os.Build;
import org.qtproject.qt.android.QtNative;

public class SmsSender {
    
    // Статический метод, который мы будем вызывать из C++ через JNI
    public static boolean sendSms(String phoneNumber, String messageText) {
        try {
            // Получаем контекст текущего Activity приложения Qt
            Context context = QtNative.activity();
            if (context == null) {
                return false;
            }

            SmsManager smsManager;
            
            // Современный способ получения SmsManager для Android 10+ (API 31+)
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                smsManager = context.getSystemService(SmsManager.class);
            } else {
                // Старый способ для предыдущих версий Android
                smsManager = SmsManager.getDefault();
            }

            if (smsManager != null) {
                // Отправляем простое текстовое сообщение без отслеживания статусов доставки
                smsManager.sendTextMessage(phoneNumber, null, messageText, null, null);
                return true;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }
}

