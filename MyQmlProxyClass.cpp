#include <QtGlobal>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

#include <QException>

#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>
#include <jni.h>

#include "MyQmlProxyClass.h"


void MyQmlProxyClass::cppOnButtonClicked() {

    qDebug() << "Called the C++ slot";

    // Ключевая статья: https://www.kdab.com/qt-android-episode-7/

    const char* HELPER_CLASS_PATH = "ru/kerminator/qt5research/MainActivity";

    // Вызываем статический метод конкретного класса
    QAndroidJniObject strValue = QAndroidJniObject::callStaticObjectMethod(
                HELPER_CLASS_PATH,          // Package и класс, метод которого следует вызвать
                "justGiveMeString",         // Имя метода
                "(I)Ljava/lang/String;",    // Описание параметров
                10);                        // Значение входных параметров

    // Если нужно передать в Java-метод несколько параметров, то их описание
    //  "(Ljava/lang/String;Ljava/lang/String;)[Ljava/lang/String;"
    //  string1.object<jstring>(),
    //  string2.object<jstring>())

    qDebug() << "The result is: " << strValue.toString();

    // Теперь пытаемся вызвать метод экземпляра Activity
    QAndroidJniObject activity;

    try{

        activity = QtAndroid::androidActivity();
        if (!activity.isValid())
            throw QException();
    }
    catch (QException &e){
        activity = nullptr;
        qDebug() << "Android activity has not been initialized";
        qDebug() << "Exception: " << e.what();
        return;
    }
    catch (...){
        activity = nullptr;
        qDebug() << "Android activity has not been initialized";
        qDebug() << "Unknown exception";
        return;
    }

    // Вызываем не статический метод главной Activity
    jint x = activity.callMethod<int>("justGiveMeInt", "()I");
    qDebug() << "The value is: " << x;

    // Разница между вызовом callMethod<...>() и callObjectMethod() состоит в том,
    // что первый метод возвращает только примитивные типы, а второй - сложные
    // типы, такие как: jobject, jstring, jarray
    // QAndroidJniObject result = activity.callObjectMethod("justGiveMeObject", "...");

    // Если нам нужно передать в метод какой-то сложный тип, то нам следует
    // воспользоваться одним из статических методов класса QAndroidJniObject
    // QAndroidJniObject string1 = QAndroidJniObject::fromString("String1");
    // QAndroidJniObject string2 = QAndroidJniObject::fromString("String2");
    // QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/TestClass",
    //                                                                          "stringArray"
    //                                                                          "(Ljava/lang/String;Ljava/lang/String;)[Ljava/lang/String;"
    //                                                                           string1.object<jstring>(),
    //                                                                           string2.object<jstring>());
}
