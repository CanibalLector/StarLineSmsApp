cmake_minimum_required(VERSION 3.20)

project(StarLineSmsApp VERSION 0.1 LANGUAGES CXX)

# Включаем стандарт C++23 на уровне CMake
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Чистый C++ без GNU-расширений для лучшей переносимости

# Поиск необходимых модулей Qt
find_package(Qt6 REQUIRED COMPONENTS Core Quick)

# Исходные файлы C++ (исключаем main.qml отсюда, его заберет qt_add_qml_module)
set(PROJECT_SOURCES
    main.cpp
    smsmanager.h
    smsmanager.cpp
)

# --- НАСТРОЙКА ДЛЯ ANDROID ---
if(ANDROID)
    # Указываем Qt, где искать Java-файлы и AndroidManifest
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")

    list(APPEND PROJECT_SOURCES
        android/src/com/app/SmsSender.java
    )
endif()

# --- НАСТРОЙКА ДЛЯ iOS ---
if(IOS)
    # Включаем поддержку Objective-C++ (.mm файлы)
    enable_language(OBJCXX)

    list(APPEND PROJECT_SOURCES
        ios/ios_smssender.mm
    )

    # Включаем автоматический подсчет ссылок Apple (ARC) для Objective-C++
    set_source_files_properties(ios/ios_smssender.mm PROPERTIES COMPILE_FLAGS "-fobjc-arc")
endif()

# Создание исполняемого файла приложения (только из С++ и нативных исходников)
qt_add_executable(StarLineSmsApp ${PROJECT_SOURCES})

# Принудительно задаем флаг C++23 для компиляторов Clang (Android NDK и Xcode)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(StarLineSmsApp PRIVATE -std=c++23)
endif()

# Определение целевых настроек для QML и регистрация C++ классов приложения
qt_add_qml_module(StarLineSmsApp
    URI StarLineSmsApp
    VERSION 1.0
    QML_FILES main.qml # QML-файлы регистрируются здесь
    SOURCES smsmanager.h smsmanager.cpp # Позволяет QML «видеть» типы из этих файлов
)

# Линковка модулей Qt
target_link_libraries(StarLineSmsApp PRIVATE Qt6::Core Qt6::Quick)

# Дополнительная линковка для Android (библиотека логирования)
if(ANDROID)
    target_link_libraries(StarLineSmsApp PRIVATE log)
endif()

# Дополнительная линковка для iOS (фреймворки Apple)
if(IOS)
    target_link_libraries(StarLineSmsApp PRIVATE "-framework Foundation")
    target_link_libraries(StarLineSmsApp PRIVATE "-framework UIKit")
    target_link_libraries(StarLineSmsApp PRIVATE "-framework MessageUI")
endif()
