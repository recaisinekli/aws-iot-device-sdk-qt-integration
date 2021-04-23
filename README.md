#aws-iot-device-sdk-qt-integration

## Prerequisites
Build aws-iot-device-sdk-cpp from source with shared libs enabled

```
mkdir sdk-cpp-workspace
cd sdk-cpp-workspace
git clone --recursive https://github.com/aws/aws-iot-device-sdk-cpp-v2.git
mkdir aws-iot-device-sdk-cpp-v2-build
cd aws-iot-device-sdk-cpp-v2-build
cmake -DCMAKE_INSTALL_PREFIX="<absolute path sdk-cpp-workspace dir>" -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE="<Release|RelWithDebInfo|Debug>" ../aws-iot-device-sdk-cpp-v2
cmake --build . --target install
```
