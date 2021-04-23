#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "awsconnection.h"

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = nullptr);

    bool is_aws_connection_ok = false;
    AWSConnection aws_connect;
    QTimer send_timer;

signals:

public slots:
    void message_received(QString);
    void send_timer_timeout();

};

#endif // MAINCONTROLLER_H
