#ifndef AWSCONNECTION_H
#define AWSCONNECTION_H

#include <QObject>
#include <QDebug>

#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/TlsOptions.h>
#include <aws/iot/MqttClient.h>
#include <algorithm>
#include <aws/crt/UUID.h>
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace Aws::Crt;

class AWSConnection : public QObject
{
    Q_OBJECT
public:
    explicit AWSConnection(QObject *parent = nullptr);

    bool init();
    void send_message(QString);
    void set_crt_path(QString);
    void set_ca_path(QString);
    void set_private_key_path(QString);
    void set_enpoint(QString);

private:
    ApiHandle apiHandle;
    std::shared_ptr<Mqtt::MqttConnection> m_connection;
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;
    Aws::Iot::MqttClient *m_mqttClient;

    void message_received_func(QString);
    void disconnect();
    void unsubscribe();

    QString m_path_to_crt;
    QString m_path_to_ca;
    QString m_path_to_private_key;
    QString m_endpoint;

signals:
    void message_received_signal(QString);

};

#endif // AWSCONNECTION_H
