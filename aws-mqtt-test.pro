QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        awsconnection.cpp \
        main.cpp \
        maincontroller.cpp

INCLUDEPATH += $$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/include
DEPENDPATH += $$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/include

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

unix:!macx: LIBS += -L$$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/lib/ -laws-c-auth
unix:!macx: LIBS += -L$$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/lib/ -laws-c-common
unix:!macx: LIBS += -L$$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/lib/ -laws-c-mqtt
unix:!macx: LIBS += -L$$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/lib/ -laws-crt-cpp
unix:!macx: LIBS += -L$$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/lib/ -ls2n
unix:!macx: LIBS += -L$$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/lib/ -laws-c-cal
unix:!macx: LIBS += -L$$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/lib/ -laws-c-compression
unix:!macx: LIBS += -L$$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/lib/ -laws-c-http
unix:!macx: LIBS += -L$$PWD/../sdk-cpp-workspace/aws-iot-device-sdk-cpp-v2/lib/ -laws-c-io

HEADERS += \
    awsconnection.h \
    maincontroller.h
