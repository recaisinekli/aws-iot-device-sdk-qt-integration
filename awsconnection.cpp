#include "awsconnection.h"

AWSConnection::AWSConnection(QObject *parent) : QObject(parent)
{

}

bool AWSConnection::init(){

    bool state = true;

    Io::EventLoopGroup eventLoopGroup(1);

    if (!eventLoopGroup)
    {
        fprintf(stderr, "Event Loop Group Creation failed with error %s\n", ErrorDebugString(eventLoopGroup.LastError()));
        state = false;
    }

    Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 1, 5);
    Io::ClientBootstrap bootstrap(eventLoopGroup, defaultHostResolver);

    if (!bootstrap)
    {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        state = false;
    }

    Aws::Iot::MqttClientConnectionConfigBuilder builder;
    builder = Aws::Iot::MqttClientConnectionConfigBuilder(m_path_to_crt.toStdString().c_str(), m_path_to_private_key.toStdString().c_str());
    builder.WithCertificateAuthority(m_path_to_ca.toStdString().c_str());
    builder.WithEndpoint(m_endpoint.toStdString().c_str());

    auto clientConfig = builder.Build();

    if (!clientConfig)
    {
        fprintf(stderr,"Client Configuration initialization failed with error %s\n",ErrorDebugString(clientConfig.LastError()));
        state = false;
    }

    m_mqttClient = new Aws::Iot::MqttClient(bootstrap);

    if (!m_mqttClient)
    {
        fprintf(stderr, "MQTT Client Creation failed with error %s\n", ErrorDebugString(m_mqttClient->LastError()));
        state = false;
    }

    m_connection = m_mqttClient->NewConnection(clientConfig);

    if (!m_connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(m_mqttClient->LastError()));
        state = false;
    }

    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionCompletedPromise.set_value(false);
        }
        else
        {
            if (returnCode != AWS_MQTT_CONNECT_ACCEPTED)
            {
                fprintf(stdout, "Connection failed with mqtt return code %d\n", (int)returnCode);
                connectionCompletedPromise.set_value(false);
            }
            else
            {
                fprintf(stdout, "Connection completed successfully.\n");
                connectionCompletedPromise.set_value(true);
            }
        }
    };

    auto onInterrupted = [&](Mqtt::MqttConnection &, int error) {
        fprintf(stdout, "Connection interrupted with error %s\n", ErrorDebugString(error));
    };

    auto onResumed = [&](Mqtt::MqttConnection &, Mqtt::ReturnCode, bool) { fprintf(stdout, "Connection resumed\n"); };

    auto onDisconnect = [&](Mqtt::MqttConnection &) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosedPromise.set_value();
        }
    };

    m_connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    m_connection->OnDisconnect = std::move(onDisconnect);
    m_connection->OnConnectionInterrupted = std::move(onInterrupted);
    m_connection->OnConnectionResumed = std::move(onResumed);

    String topic(m_topic.toStdString().c_str());
    String clientId(String("test-") + Aws::Crt::UUID().ToString());

    qDebug()<<"Connecting...\n";

    if (!m_connection->Connect(clientId.c_str(), false, 1000))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(m_connection->LastError()));
        state = false;
    }

    if (connectionCompletedPromise.get_future().get())
    {

        auto onMessage = [&](Mqtt::MqttConnection &,
                const String &topic,
                const ByteBuf &byteBuf,
                bool,
                Mqtt::QOS,
                bool) {
            QString _topic = QString(topic.c_str());
            QString _message = QString::fromUtf8(reinterpret_cast<char *>(byteBuf.buffer), byteBuf.len);
            this->message_received_func(_message);
        };


        std::promise<void> subscribeFinishedPromise;
        auto onSubAck =
                [&](Mqtt::MqttConnection &, uint16_t packetId, const String &topic, Mqtt::QOS QoS, int errorCode) {
            if (errorCode)
            {
                fprintf(stderr, "Subscribe failed with error %s\n", aws_error_debug_str(errorCode));
                state = false;
            }
            else
            {
                if (!packetId || QoS == AWS_MQTT_QOS_FAILURE)
                {
                    fprintf(stderr, "Subscribe rejected by the broker.");
                    state = false;
                }
                else
                {
                    fprintf(stdout, "Subscribe on topic %s on packetId %d Succeeded\n", topic.c_str(), packetId);
                }
            }
            subscribeFinishedPromise.set_value();
        };

        m_connection->Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
        subscribeFinishedPromise.get_future().wait();
    }

    return state;
}

void AWSConnection::unsubscribe(){
    std::promise<void> unsubscribeFinishedPromise;
    m_connection->Unsubscribe(
                m_topic.toStdString().c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) { unsubscribeFinishedPromise.set_value(); });
    unsubscribeFinishedPromise.get_future().wait();
}

void AWSConnection::disconnect(){
    if (m_connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }
}

void AWSConnection::set_crt_path(QString crt_path){
    m_path_to_crt = crt_path;
}

void AWSConnection::set_ca_path(QString ca_path){
    m_path_to_ca = ca_path;
}

void AWSConnection::set_private_key_path(QString key_path){
    m_path_to_private_key = key_path;
}

void AWSConnection::set_enpoint(QString endpoint){
    m_endpoint = endpoint;
}

void AWSConnection::set_topic(QString topic){
    m_topic = topic;
}

void AWSConnection::message_received_func(QString message){
    emit message_received_signal(message);
}

void AWSConnection::send_message(QString message){
    std::string input = message.toStdString().c_str();
    ByteBuf payload = ByteBufNewCopy(DefaultAllocator(), (const uint8_t *)input.data(), input.length());
    ByteBuf *payloadPtr = &payload;

    auto onPublishComplete = [payloadPtr](Mqtt::MqttConnection &, uint16_t packetId, int errorCode) {
        aws_byte_buf_clean_up(payloadPtr);

        if (packetId)
        {
            fprintf(stdout, "Operation on packetId %d Succeeded\n", packetId);
        }
        else
        {
            fprintf(stdout, "Operation failed with error %s\n", aws_error_debug_str(errorCode));
        }
    };
    m_connection->Publish(m_topic.toStdString().c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
}
