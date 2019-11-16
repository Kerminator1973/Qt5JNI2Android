# Включение в Qt-проект кода на Java для обеспечения доступа к Android SDK

Сильной чертой C++ является возможность создания высокоэффективного кода (в терминах производительности и расхода энергии), практически, на любой аппаратной платформе. Разработка на C++ с использованием Qt является одним из типовых способов создания приложений для Android. Вместе с тем, на практике обойтись без использования системных и "чужих" Activities в Android невозможно. Это связано с тем, что требующийся функционал либо не доступ в Qt, либо реализуется только системными Activity. Соответственно, задача расширения Qt-приложения Java/Kotlin-кодом кажется насущной.

Очень детальное и понятное описание процесса в блоге [KDAD](https://www.kdab.com/qt-android-episode-7/). Дополнительная статья на [habr.ru](https://www.habrahabr.net/thread/8730).

# Главное замечание: всё необходимо делать средствами Qt Creator. Применение Android Studio приводит к ошибкам сборки!

Начинать рекомендуется с генерации Qt Quick-проекта. На момент проведения эксперимента, с поддержкой toolkit-ов для Android в Qt 5.13 «работало не очень». После череды экспериментом был выбран Qt 5.12.

# Первый шаг : В QML-приложении был добавлен функционал обработки нажатия на QML-кнопку в коде на C++.

Приложение было собрано под Android. 

Контрольная точка: проект собирается и запускается под эмулятором Android.

# Второй шаг: В настройках параметров сборки указываем на необходимость создания шаблонов проекта Android. 

В разделе «Сборка Android APK» нужно нажать кнопку «Создать шаблоны». В случае успеха, система сгенерирует все необходимые скрипты в подкаталоге «android»: «AndroidManifest.xml», «build.gradle», и т.д. Также будет изменён pro-файл.

В pro-файл следует добавить строку:

```
QT += androidextras
```

Пример включаемого Java-кода:

```java
package ru.kerminator.qt5research;
import android.*;
import android.Manifest;
import android.app.Activity;
import android.os.Bundle;
import org.qtproject.qt5.android.bindings.QtActivity;
public class MainActivity extends QtActivity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
}
```

Ключевым является импортирование QtActivity (это jar-архив из состава Qt5) и наследование класса с реализацией основной Activity от QtActivity.

Контрольная точка: проект собирается и запускается под эмулятором Android.

# Третий шаг: замена QtActivity на Activity из java-кода.

Для этого необходимо в файле «AndroidManifest.xm» заменить <activity name=…> с "org.qtproject.qt5.android.bindings.QtActivity" на "ru.kerminator.qt5research.MainActivity". Это означает, что запускаться должна Java Activity, которая далее запустит уже QtActivity.

ВНИМАНИЕ! В разделе "<application" тоже есть свойство "android:name", но его значение должно быть сохранено как "org.qtproject.qt5.android.bindings.QtApplication".

# Четвёртый шаг: вызов Java-методов из C++ кода

Разработчики Qt максимально упростили задачу, предоставив специальзированный API, доступный через заголовочные файлы:

'''cpp
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>
#include <jni.h>
'''
 
Вызов статических методов и методов конкретного экземпляра класса различаются. Статические методы вызываются так:

```cpp
const char* HELPER_CLASS_PATH = "ru/kerminator/qt5research/MainActivity";

// Вызываем статический метод главной Activity:
QAndroidJniObject strValue = QAndroidJniObject::callStaticObjectMethod(
                HELPER_CLASS_PATH,          // Package и класс, метод которого следует вызвать
                "justGiveMeString",         // Имя метода
                "(I)Ljava/lang/String;",    // Описание параметров
                10);                        // Значение входных параметров

// Если нужно передать в Java-метод несколько параметров, то их описание
//  "(Ljava/lang/String;Ljava/lang/String;)[Ljava/lang/String;"
//  string1.object<jstring>(),
//  string2.object<jstring>())
```

Для вызова метода объекта класса следует получить ссылку на этот класс. Проще всего работать с главной Activity:

```cpp
QAndroidJniObject activity;
activity = QtAndroid::androidActivity();
```

Соответственно, вызов метода вышлядит так:

```cpp
jint x = activity.callMethod<int>("justGiveMeInt", "()I");
```

Разница между вызовом callMethod<...>() и callObjectMethod() состоит в том, что первый метод возвращает только примитивные типы, а второй - сложные типы, такие как: jobject, jstring, jarray. Пример вызова:

```cpp
QAndroidJniObject result = activity.callObjectMethod("justGiveMeObject", "...");
```
