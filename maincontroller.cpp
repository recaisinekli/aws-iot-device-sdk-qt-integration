#include "maincontroller.h"

MainController::MainController(QObject *parent) : QObject(parent)
{
    connect(&aws_connect, SIGNAL(message_received_signal(QString)), this, SLOT(message_received(QString)));

    aws_connect.set_crt_path("path/to/your/certificate.pem.crt");
    aws_connect.set_private_key_path("path/to/your/private.pem.key");
    aws_connect.set_ca_path("path/to/AmazonRootCA1.pem");
    aws_connect.set_enpoint("endpoint");

    is_aws_connection_ok = aws_connect.init();

    connect(&send_timer, SIGNAL(timeout()), this, SLOT(send_timer_timeout()));
    send_timer.start(5000);
}

void MainController::message_received(QString message){
    qDebug()<<message.toStdString().c_str();
}

void MainController::send_timer_timeout(){
    if(is_aws_connection_ok){
        aws_connect.send_message("{\"message\":\"This is a test message\"}");
    }
}
