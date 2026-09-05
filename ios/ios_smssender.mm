#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <MessageUI/MessageUI.h>
#include <QString>

// Объявляем делегат, который будет закрывать окно СМС после отправки или отмены
@interface SmsDelegate : NSObject <MFMessageComposeViewControllerDelegate>
+ (instancetype)sharedInstance;
@end

@implementation SmsDelegate
+ (instancetype)sharedInstance {
    static SmsDelegate *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[SmsDelegate alloc] init];
    });
    return instance;
}

- (void)messageComposeViewController:(MFMessageComposeViewController *)controller 
                 didFinishWithResult:(MessageComposeResult)result {
    // Закрываем окно отправки СМС и возвращаемся в наше Qt приложение
    [controller dismissViewControllerAnimated:YES completion:nil];
}
@end

// Реализация C-функции, которую мы объявили через extern в smsmanager.cpp
void sendIOSMessage(const QString &to, const QString &text) {
    // Проверяем, может ли устройство вообще отправлять СМС (например, это не iPad без сим-карты)
    if (![MFMessageComposeViewController canSendText]) {
        NSLog(@"Это устройство не поддерживает отправку SMS");
        return;
    }

    // Конвертируем QString в нативные NSString для iOS
    NSString *iosTo = [NSString stringWithUTF8String:to.toUtf8().constData()];
    NSString *iosText = [NSString stringWithUTF8String:text.toUtf8().constData()];

    // Создаем и настраиваем контроллер отправки сообщений
    MFMessageComposeViewController *messageController = [[MFMessageComposeViewController alloc] init];
    messageController.messageComposeDelegate = [SmsDelegate sharedInstance];
    messageController.recipients = @[iosTo];
    messageController.body = iosText;

    // Находим корневое окно приложения Qt, чтобы отобразить контроллер поверх него
    dispatch_async(dispatch_get_main_queue(), ^{
        UIWindow *keyWindow = nil;
        
        // Современный способ получения окна для iOS 13/14/15+
        for (UIWindowScene *scene in [UIApplication sharedApplication].connectedScenes) {
            if (scene.activationState == UISceneActivationStateForegroundActive) {
                for (UIWindow *window in scene.windows) {
                    if (window.isKeyWindow) {
                        keyWindow = window;
                        break;
                    }
                }
            }
        }
        
        // Фолбэк для старых версий iOS, если сцена не нашлась
        if (!keyWindow) {
            keyWindow = [UIApplication sharedApplication].keyWindow;
        }

        UIViewController *rootViewController = keyWindow.rootViewController;
        
        // Если поверх уже что-то открыто, ищем самый верхний контроллер
        while (rootViewController.presentedViewController) {
            rootViewController = rootViewController.presentedViewController;
        }

        // Показываем стандартное окно отправки СМС Apple
        [rootViewController presentViewController:messageController animated:YES completion:nil];
    });
}

