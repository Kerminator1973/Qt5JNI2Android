#ifndef MYQMLPROXYCLASS_H
#define MYQMLPROXYCLASS_H

// В идеологии Qt, для обработки событий от компонентов QML в C++ коде
// используются proxy-классы

#include <QGuiApplication>
#include <QDebug>   // Заголовочный файл нужен для обеспечения возможности использовать qDebug()

// Proxy-класс, через который обмениваются информацией
// два компонента пользовательского интерфейса (QML)
class MyQmlProxyClass : public QObject
{
    Q_OBJECT
public slots:

    void cppOnButtonClicked();
};

#endif // MYQMLPROXYCLASS_H
