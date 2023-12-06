


/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#include "orbbec_device.hpp"

// std
#include <format>

// orbbec
//#include "libobsensor/ObSensor.hpp"

// local
#include "utility/logger.hpp"
//#include "camera/dc_types.hpp"

using namespace tool::camera;


struct OrbbecDevice::Impl{


    OrbbecDevice *orbbecD = nullptr;

    // device
    uint32_t deviceCount = 0;
    std::string serialNumber;
    std::string deviceName;
//    std::unique_ptr<ob::Context> context = nullptr;
//    std::shared_ptr<ob::Device> device = nullptr;
//    std::shared_ptr<ob::DeviceList> deviceList = nullptr;
//    std::shared_ptr<ob::SensorList> sensorList = nullptr;
//    std::unique_ptr<ob::Pipeline> pipe = nullptr;
//    std::unique_ptr<ob::PointCloudFilter> pointCloud = nullptr;

    DCConfigSettings config;

    // state
    bool camerasStarted = false;

};

OrbbecDevice::OrbbecDevice(): i(std::make_unique<Impl>()){
    i->orbbecD = this;

//    i->context = std::make_unique<ob::Context>();


    //        // register device callback
    //        ctx.setDeviceChangedCallback([](std::shared_ptr<ob::DeviceList> removedList, std::shared_ptr<ob::DeviceList> addedList) {
    //            handleDeviceDisconnected(removedList);
    //            handleDeviceConnected(addedList);
    //        });
    //        ctx.setDeviceChangedCallback([](std::shared_ptr<ob::DeviceList> removedList, std::shared_ptr<ob::DeviceList> addedList) {
    //            if(isWaitRebootComplete_) {
    //                if(addedList && addedList->deviceCount() > 0) {
    //                    auto device = addedList->getDevice(0);
    //                    if(isDeviceRemoved_ && deviceSN_ == std::string(device->getDeviceInfo()->serialNumber())) {
    //                        rebootedDevice_       = device;
    //                        isWaitRebootComplete_ = false;

    //                        std::unique_lock<std::mutex> lk(waitRebootMutex_);
    //                        waitRebootCondition_.notify_all();
    //                    }
    //                }

    //                if(removedList && removedList->deviceCount() > 0) {
    //                    if(deviceUid_ == std::string(removedList->uid(0))) {
    //                        isDeviceRemoved_ = true;
    //                    }
    //                }
    //            }  // if isWaitRebootComplete_
    //        });



    // callbacks
    // i->context->setDeviceChangedCallback();
    // i->context->setLoggerToCallback();
    // logger
    // i->context->setLoggerSeverity();
    // i->context->setLoggerToConsole();
    // i->context->setLoggerToFile();
    // net
    // i->context->createNetDevice();
    // i->context->changeNetDeviceIpConfig();
    // i->context->enableNetDeviceEnumeration()
    // license
    // i->context->loadLicense();
    // i->context->loadLicenseFromData();
    // synch
    // i->context->enableDeviceClockSync(true);
    // memory
    // i->context->freeIdleMemory();

    OrbbecDevice::refresh_devices_list();
}

OrbbecDevice::~OrbbecDevice(){
    OrbbecDevice::clean();
}


auto OrbbecDevice::refresh_devices_list() -> void{

    // cameras
//    i->deviceList = i->context->queryDeviceList();
//    i->deviceCount = i->deviceList->deviceCount();
}

auto OrbbecDevice::open(uint32_t deviceId) -> bool {

    static_cast<void>(deviceId);
    if(is_opened()){
        return false;
    }

//    i->serialNumber  = "";

//    if(deviceId >= i->deviceCount){
//        Logger::error("[K4Device] Invalid device id\n");
//        return false;
//    }

//    try {
//        i->device = i->deviceList->getDevice(i->config.idDevice = deviceId);
//        i->sensorList = i->device->getSensorList();
//        // callbacks
////        i->device->setDeviceStateChangedCallback([&](OBDeviceState state, const char *message){
////            // ...
////        });

//    }catch(ob::Error &e) {
//        Logger::error(std::format("[OrbbecDevice] open error: {}\n", e.getMessage()));
//        i->device = nullptr;
//        return false;
//    }

//    auto protocolV = i->device->getProtocolVersion();
//    auto infos     = i->device->getDeviceInfo();


//    Logger::message("[OrbbecDevice] Device opened:\n");
//    Logger::message(std::format("  Name: {}\n", i->deviceName = infos->name()));
//    Logger::message(std::format("  Chip type: {}\n", infos->asicName()));
//    Logger::message(std::format("  Serialnum: {}\n", i->serialNumber = infos->serialNumber()));
//    Logger::message(std::format("  PID: {}\n", infos->pid()));
//    Logger::message(std::format("  VID: {}\n", infos->vid()));
//    Logger::message(std::format("  UID: {}\n", infos->uid()));
//    Logger::message(std::format("  IP: {}\n", infos->ipAddress()));
//    Logger::message("  Version:\n");
//    Logger::message(std::format("      Protocol: {}.{}.{}\n", protocolV.major, protocolV.minor, protocolV.patch));
//    Logger::message(std::format("      Firmware: {}\n", infos->firmwareVersion()));
//    Logger::message(std::format("      Hardware: {}\n", infos->hardwareVersion()));
//    Logger::message(std::format("      SDK minimum supported: {}\n", infos->supportedMinSdkVersion()));
//    Logger::message(std::format("      Extension info: {}\n", infos->extensionInfo()));
//    Logger::message("  Types:\n");
//    std::string deviceT;
//    switch(infos->deviceType()){
//        case OB_STRUCTURED_LIGHT_MONOCULAR_CAMERA:
//            deviceT = "Monocular structured light camera";
//        break;
//        case OB_STRUCTURED_LIGHT_BINOCULAR_CAMERA:
//            deviceT = "Binocular structured light camera";
//        break;
//        case OB_TOF_CAMERA:
//            deviceT = "Time-of-flight camera";
//        break;
//    }
//    Logger::message(std::format("      Device: {}\n", deviceT));
//    Logger::message(std::format("      Connection: {}\n", infos->connectionType()));


//    auto pCount = i->device->getSupportedPropertyCount();
//    for(std::uint32_t idP = 0; idP< pCount; ++idP){
//        auto property = i->device->getSupportedProperty(idP);
//        // property.id;
//        // property.type;
//        // property.name;
//        //  OB_BOOL_PROPERTY   = 0, /**< Boolean property */
//        //  OB_INT_PROPERTY    = 1, /**< Integer property */
//        //  OB_FLOAT_PROPERTY  = 2, /**< Floating-point property */
//        //  OB_STRUCT_PROPERTY = 3, /**< Struct property */
//        // property.permission;
//        //  OB_PERMISSION_DENY       = 0,   /**< no permission */
//        //  OB_PERMISSION_READ       = 1,   /**< can read */
//        //  OB_PERMISSION_WRITE      = 2,   /**< can write */
//        //  OB_PERMISSION_READ_WRITE = 3,   /**< can read and write */
//        //  OB_PERMISSION_ANY        = 255, /**< any situation above */

//        // SET
//        //  void setIntProperty(OBPropertyID propertyId, int32_t property);
//        //  void setFloatProperty(OBPropertyID propertyId, float property);
//        //  void setBoolProperty(OBPropertyID propertyId, bool property);
//        //  void setRawData(OBPropertyID propertyId, const void *data, uint32_t dataSize, SetDataCallback callback, bool async = false);
//        //  void setStructuredData(OBPropertyID propertyId, const void *data, uint32_t dataSize);
//        //  void setStructuredDataExt(OBPropertyID propertyId, std::shared_ptr<OBDataBundle> dataBundle, SetDataCallback callback);

//        // GET
//        //  int32_t getIntProperty(OBPropertyID propertyId);
//        //  float getFloatProperty(OBPropertyID propertyId);
//        //  bool getBoolProperty(OBPropertyID propertyId);
//        //  std::shared_ptr<OBDataBundle> getStructuredDataExt(OBPropertyID propertyId);
//        //  void getStructuredData(OBPropertyID propertyId, void *data, uint32_t *dataSize);
//        //  void getRawData(OBPropertyID propertyId, GetDataCallback callback, bool async = false);

//        // GET RANGE
//        //  OBIntPropertyRange getIntPropertyRange(OBPropertyID propertyId);
//        //  OBFloatPropertyRange getFloatPropertyRange(OBPropertyID propertyId);
//        //  OBBoolPropertyRange getBoolPropertyRange(OBPropertyID propertyId);

//        // VERSION
//        //  OBCmdVersion getCmdVersion(OBPropertyID propertyId);

//        // PERMISSION
//        //  bool isPropertySupported(OBPropertyID propertyId, OBPermissionType permission);

//    }

//    // sensor
//    //  std::shared_ptr<SensorList> getSensorList();
//    //  std::shared_ptr<Sensor> getSensor(OBSensorType type);

//    // multidevice
//    //  uint16_t getSupportedMultiDeviceSyncModeBitmap();
//    //  void setMultiDeviceSyncConfig(const OBMultiDeviceSyncConfig &config);
//    //  OBMultiDeviceSyncConfig getMultiDeviceSyncConfig();

//    // timestamp
//    //  void setTimestampResetConfig(const OBDeviceTimestampResetConfig &config);
//    //  OBDeviceTimestampResetConfig getTimestampResetConfig();
//    //  void timestampReset();
//    //  void timerSyncWithHost();

//    // depthmode
//    //  OBDepthWorkMode getCurrentDepthWorkMode();
//    //  OBStatus switchDepthWorkMode(const OBDepthWorkMode &workMode);
//    //  OBStatus switchDepthWorkMode(const char *modeName);
//    //  std::shared_ptr<OBDepthWorkModeList> getDepthWorkModeList();

//    // calibration
//    //  std::shared_ptr<CameraParamList> getCalibrationCameraParamList();

//    // upgrade
//    //  void deviceUpgrade(const char *filePath, DeviceUpgradeCallback callback, bool async = true);
//    //  void deviceUpgradeFromData(const char *fileData, uint32_t fileSize, DeviceUpgradeCallback callback, bool async = true);

//    // state
//    //  OBDeviceState getDeviceState();
//    //  void setDeviceStateChangedCallback(DeviceStateChangedCallback callback);

//    // authorization
//    //  bool activateAuthorization(const char *authCode);
//    //  void writeAuthorizationCode(const char *authCodeStr);

//    // AHB
//    //  void writeAHB(uint32_t reg, uint32_t mask, uint32_t value);
//    //  void readAHB(uint32_t reg, uint32_t mask, uint32_t *value);

//    // I2C
//    //  void writeI2C(uint32_t moduleId, uint32_t reg, uint32_t mask, uint32_t value);
//    //  void readI2C(uint32_t moduleId, uint32_t reg, uint32_t mask, uint32_t *value);

//    // Flash
//    //  void writeFlash(uint32_t offset, const void *data, uint32_t dataSize, SetDataCallback callback, bool async = false);
//    //  void readFlash(uint32_t offset, uint32_t dataSize, GetDataCallback callback, bool async = false);

//    // Customer data
//    //  void writeCustomerData(const void *data, uint32_t dataSize);
//    //  void readCustomerData(void *data, uint32_t *dataSize);

//    // Reboot
//    //  void reboot();
//    //  void reboot(uint32_t delayMs);

//    // Other
//    //  void triggerCapture();
//    //  void sendFile(const char *filePath, const char *dstPath, SendFileCallback callback, bool async = true);

//    return true;
    return false;
}


auto OrbbecDevice::clean() -> void {
    // ...
}

auto OrbbecDevice::nb_devices() const noexcept -> uint32_t {
    return i->deviceCount;
}

auto OrbbecDevice::is_opened() const noexcept -> bool {
//    if(i->device){
//        return true;
//    }
    return false;
}

auto OrbbecDevice::cameras_started() const noexcept -> bool{
    return i->camerasStarted;
}

auto OrbbecDevice::start_cameras(const DCConfigSettings &configS) -> bool{

    static_cast<void>(configS);
//    if(cameras_started() || !is_opened()){
//        return false;
//    }

//    i->config = configS;

//    try {

//        Logger::message("[K4Device] start cameras\n");
//        i->pipe = std::make_unique<ob::Pipeline>(i->device);

//        std::shared_ptr<ob::StreamProfileList> colorStreamProfileList = nullptr;
//        std::shared_ptr<ob::StreamProfileList> depthStreamProfileList = nullptr;
//        std::shared_ptr<ob::StreamProfileList> infraStreamProfileList = nullptr;
//        std::shared_ptr<ob::StreamProfileList> accelStreamProfileList = nullptr;
//        std::shared_ptr<ob::StreamProfileList> gyroStreamProfileList  = nullptr;
//        if(i->sensorList->getSensor(OBSensorType::OB_SENSOR_COLOR)){
//            colorStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_COLOR);
//        }
//        if(i->sensorList->getSensor(OBSensorType::OB_SENSOR_DEPTH)){
//            depthStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_DEPTH);
//        }
//        if(i->sensorList->getSensor(OBSensorType::OB_SENSOR_IR)){
//            infraStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_IR);
//        }
//        if(i->sensorList->getSensor(OBSensorType::OB_SENSOR_ACCEL)){
//            accelStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_ACCEL);
//        }
//        if(i->sensorList->getSensor(OBSensorType::OB_SENSOR_GYRO)){
//            gyroStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_GYRO);
//        }

//        std::shared_ptr<ob::VideoStreamProfile> colorProfile = nullptr;
//        std::shared_ptr<ob::VideoStreamProfile> depthProfile = nullptr;
//        std::shared_ptr<ob::VideoStreamProfile> infraProfile = nullptr;
//        std::shared_ptr<ob::AccelStreamProfile> accelProfile = nullptr;
//        std::shared_ptr<ob::GyroStreamProfile> gyraProfile   = nullptr;
//        if(colorStreamProfileList){
//            colorProfile = colorStreamProfileList->getVideoStreamProfile(1280, OB_HEIGHT_ANY, OB_FORMAT_RGB, 30);
//        }
//        if(depthStreamProfileList){
//            depthProfile = depthStreamProfileList->getVideoStreamProfile(640, OB_HEIGHT_ANY, OB_FORMAT_Y16, 30);
//        }
//        if(infraStreamProfileList){
//            infraProfile = infraStreamProfileList->getVideoStreamProfile(640, OB_HEIGHT_ANY, OB_FORMAT_Y16, 30);
//        }
//        if(accelStreamProfileList){
//            OBAccelFullScaleRange fullScaleRange    = OB_ACCEL_FS_2g;
//            OBAccelSampleRate sampleRate            = OB_SAMPLE_RATE_50_HZ;
//            accelProfile = accelStreamProfileList->getAccelStreamProfile(fullScaleRange, sampleRate);
//        }
//        if(gyroStreamProfileList){
//            OBGyroFullScaleRange fullScaleRange = OB_GYRO_FS_250dps;
//            OBGyroSampleRate sampleRate         = OB_SAMPLE_RATE_50_HZ;
//            gyraProfile = gyroStreamProfileList->getGyroStreamProfile(fullScaleRange, sampleRate);
//        }

//        // configure which streams to enable or disable for the Pipeline by creating a Config
//        std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

//        // enable streams depening parameters
//        if(colorProfile){
//            config->enableStream(colorProfile);
//        }
//        if(depthProfile){
//            config->enableStream(depthProfile);
//        }
//        if(infraProfile){
//            config->enableStream(infraProfile);
//        }
//        if(accelProfile){
//            config->enableStream(accelProfile);
//        }
//        if(gyraProfile){
//            config->enableStream(gyraProfile);
//        }

//        i->pipe->start(config);

//        // Create a point cloud Filter object (the device parameters will be obtained inside the Pipeline when the point cloud filter is created, so try to
//        // configure the device before creating the filter)
//        i->pointCloud = std::make_unique<ob::PointCloudFilter>();
//        i->pointCloud->setCreatePointFormat(OB_FORMAT_RGB_POINT); // OB_FORMAT_POINT

//        // get camera intrinsic and extrinsic parameters form pipeline and set to point cloud filter
//        auto cameraParam = i->pipe->getCameraParam();
//        i->pointCloud->setCameraParam(cameraParam);



////        OBCameraIntrinsic
////        float   fx;      ///< Focal length in x direction
////        float   fy;      ///< Focal length in y direction
////        float   cx;      ///< Optical center abscissa
////        float   cy;      ///< Optical center ordinate
////        int16_t width;   ///< Image width
////        int16_t height;  ///< Image height
////        OBCameraDistortion
////        float k1;  ///< Radial distortion factor 1
////        float k2;  ///< Radial distortion factor 2
////        float k3;  ///< Radial distortion factor 3
////        float k4;  ///< Radial distortion factor 4
////        float k5;  ///< Radial distortion factor 5
////        float k6;  ///< Radial distortion factor 6
////        float p1;  ///< Tangential distortion factor 1
////        float p2;  ///< Tangential distortion factor 2
////        OBD2CTransform
////        float rot[9];    ///< Rotation matrix
////        float trans[3];  ///< Transformation matrix
////        typedef struct {
////            OBCameraIntrinsic  depthIntrinsic;   ///< Depth camera internal parameters
////            OBCameraIntrinsic  rgbIntrinsic;     ///< Color camera internal parameters
////            OBCameraDistortion depthDistortion;  ///< Depth camera distortion parameters
////            OBCameraDistortion rgbDistortion;    ///< Color camera distortion parameters
////            OBD2CTransform     transform;        ///< Rotation/transformation matrix
////            bool               isMirrored;       ///< Whether the image frame corresponding to this group of parameters is mirrored
////        } OBCameraParam, ob_camera_param;




////        // Configure the alignment mode as hardware D2C alignment
////    //    ALIGN_DISABLE,     /**< Turn off alignment */
////    //    ALIGN_D2C_HW_MODE, /**< Hardware D2C alignment mode */
////    //    ALIGN_D2C_SW_MODE, /**< Software D2C alignment mode */
////        config->setAlignMode(ALIGN_D2C_HW_MODE);
////    //    void setDepthScaleRequire(bool enable);
////    //    void setD2CTargetResolution(uint32_t d2cTargetWidth, uint32_t d2cTargetHeight);


////        try {
////            colorProfile = cProfilesL->getVideoStreamProfile(1280, OB_HEIGHT_ANY, OB_FORMAT_RGB, 30);
////            depthProfile = dProfilesL->getVideoStreamProfile(640, OB_HEIGHT_ANY, OB_FORMAT_Y16, 30);
////            irProfile    = irProfilesL->getVideoStreamProfile(640, OB_HEIGHT_ANY, OB_FORMAT_Y16, 30);
////        }
////        catch(ob::Error &e) {
////            // If the specified format is not found, search for the default profile to open the stream
////            colorProfile = std::const_pointer_cast<ob::StreamProfile>(cProfilesL->getProfile(OB_PROFILE_DEFAULT))->as<ob::VideoStreamProfile>();
////            depthProfile = std::const_pointer_cast<ob::StreamProfile>(dProfilesL->getProfile(OB_PROFILE_DEFAULT))->as<ob::VideoStreamProfile>();
////            irProfile    = std::const_pointer_cast<ob::StreamProfile>(irProfilesL->getProfile(OB_PROFILE_DEFAULT))->as<ob::VideoStreamProfile>();
////        }

//        //    i->device->setMultiDeviceSyncConfig();
//        //    i->device->setTimestampResetConfig()
//        //    OBMultiDeviceSyncConfig c;
//        //    auto curConfig = dev->getMultiDeviceSyncConfig();
//        //    // Update the configuration items of the configuration file, and keep the original configuration for other items
//        //    curConfig.syncMode             = c.syncMode;
//        //    curConfig.depthDelayUs         = c.depthDelayUs;
//        //    curConfig.colorDelayUs         = c.colorDelayUs;
//        //    curConfig.trigger2ImageDelayUs = c.trigger2ImageDelayUs;
//        //    curConfig.triggerOutEnable     = c.triggerOutEnable;
//        //    curConfig.triggerOutDelayUs    = c.triggerOutDelayUs;
//        //    curConfig.framesPerTrigger     = c.framesPerTrigger;
//        //    dev->setMultiDeviceSyncConfig(curConfig);



//        //        for(uint32_t i = 0; i < sensorList->count(); i++) {

//        //        }
//        //        sensorList->getSensor(OBSensorType::OB_SENSOR_COLOR);


//    }catch(ob::Error &e) {
//        Logger::error(std::format("[OrbbecDevice] start_cameras error: {}\n", e.getMessage()));
//        i->device = nullptr;
//        return false;
//    }



    return false;
}

//auto set_value(ob::Device *dev, OBPropertyID pId, bool value) -> void{

//    if(!dev->isPropertySupported(pId, OB_PERMISSION_READ_WRITE)){
//        return;
//    }
//    if(dev->getBoolProperty(pId) != value){
//        dev->setBoolProperty(pId, value);
//    }
//}

//auto set_value(ob::Device *dev, OBPropertyID pId, std::int32_t value) -> void{

//    if(!dev->isPropertySupported(pId, OB_PERMISSION_READ_WRITE)){
//        return;
//    }

//    auto range = dev->getIntPropertyRange(pId);
//    if(value < range.min){
//        value = range.min;
//    }
//    value += (value - range.min)%range.step;

//    if(value > range.max){
//        value = range.max;
//    }

//    if(range.cur != value){
//        dev->setIntProperty(pId, value);
//    }
//}

auto OrbbecDevice::set_color_settings(const DCColorSettings &colorS) -> void {

    static_cast<void>(colorS);
    if(!is_opened()){
        return;
    }

//    // color auto exposure
//    set_value(i->device.get(), OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, colorS.autoExposureTime);
//    // color auto white balance
//    set_value(i->device.get(), OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL, colorS.autoWhiteBalance);
//    // color exposure
//    {
//        std::int32_t value = 0;
//        switch (static_cast<K4ExposureTimesMicroS>(colorS.exposureTimeAbsolute)) {
//            case K4ExposureTimesMicroS::t500:   value = 500; break;
//            case K4ExposureTimesMicroS::t1250:  value = 1250; break;
//            case K4ExposureTimesMicroS::t2500:  value = 2500; break;
//            case K4ExposureTimesMicroS::t8330:  value = 8330; break;
//            case K4ExposureTimesMicroS::t16670: value = 16670; break;
//            case K4ExposureTimesMicroS::t33330: value = 33330; break;
//        }
//        set_value(i->device.get(), OB_PROP_COLOR_EXPOSURE_INT, value);
//    }
//    // color white balance
//    set_value(i->device.get(), OB_PROP_COLOR_WHITE_BALANCE_INT, colorS.whiteBalance);
//    // color brightness
//    set_value(i->device.get(), OB_PROP_COLOR_BRIGHTNESS_INT, colorS.brightness);
//    // color contrast
//    set_value(i->device.get(), OB_PROP_COLOR_CONTRAST_INT, colorS.contrast);
//    // color sharpness
//    set_value(i->device.get(), OB_PROP_COLOR_SHARPNESS_INT, colorS.sharpness);
//    // color saturation
//    set_value(i->device.get(), OB_PROP_COLOR_SATURATION_INT, colorS.saturation);
//    // color backlight compensation
//    set_value(i->device.get(), OB_PROP_COLOR_BACKLIGHT_COMPENSATION_INT, colorS.backlightCompensation ? 1 : 0); // TODO: INSPECT
//    // color gain
//    set_value(i->device.get(), OB_PROP_COLOR_GAIN_INT, colorS.gain);
//    // color powerline frequency
//    {
//        std::int32_t value = 0;
//        switch (colorS.powerlineFrequency){
//            case PowerlineFrequency::F50: value = 50; break;
//            case PowerlineFrequency::F60: value = 60; break;
//        }
//        set_value(i->device.get(), OB_PROP_COLOR_POWER_LINE_FREQUENCY_INT, value); // TODO: INSPECT
//    }
//    // color HDR
//    set_value(i->device.get(), OB_PROP_COLOR_HDR_BOOL, true); // TODO: INSPECT
}

auto OrbbecDevice::device_name() const noexcept-> std::string{
    //    return i->serialNumber;
    return i->deviceName;
}

auto OrbbecDevice::device_id() const noexcept -> std::uint32_t{
    return i->config.idDevice;
}



//auto test() -> int {

//    // Create a Context.

//    ob::Context ctx;

//    // Query the list of connected devices
//    auto devList = ctx.queryDeviceList();

//    // Get the number of connected devices
//    if(devList->deviceCount() == 0) {
//        std::cerr << "Device not found!" << std::endl;
//        return -1;
//    }
//    // devList->pid(0) / devList->vid(0) / devList->uid(0)
//    // devList->serialNumber(0);
//    // devList->ipAddress(0); // for ethernet device
//    // devList->connectionType(0); // ："USB", "USB1.0", "USB1.1", "USB2.0", "USB2.1", "USB3.0", "USB3.1", "USB3.2", "Ethernet"
//    // devList->getDeviceBySN(..); // serial number
//    // devList->getDeviceByUid(..); // uid;

//    for(std::uint32_t ii = 0; ii < devList->deviceCount(); ++ii){
////        std::shared_ptr<ob::Device> dev = devList->getDevice(ii);
//    }

//    // Create a device, 0 means the index of the first device
//    std::shared_ptr<ob::Device> dev = devList->getDevice(0);

////    auto cpl = dev->getCalibrationCameraParamList();
////    dev->getDepthWorkModeList();


//    OBMultiDeviceSyncConfig c;
//    auto curConfig = dev->getMultiDeviceSyncConfig();
//    // Update the configuration items of the configuration file, and keep the original configuration for other items
//    curConfig.syncMode             = c.syncMode;
//    curConfig.depthDelayUs         = c.depthDelayUs;
//    curConfig.colorDelayUs         = c.colorDelayUs;
//    curConfig.trigger2ImageDelayUs = c.trigger2ImageDelayUs;
//    curConfig.triggerOutEnable     = c.triggerOutEnable;
//    curConfig.triggerOutDelayUs    = c.triggerOutDelayUs;
//    curConfig.framesPerTrigger     = c.framesPerTrigger;
//    dev->setMultiDeviceSyncConfig(curConfig);
////    dev->reboot();

//    // dev->getDeviceInfo();
//    // dev->deviceUpgrade();
//    // dev->deviceUpgradeFromData();
//    // auto state = dev->getDeviceState();

//    // Get device information
//    auto devInfo = dev->getDeviceInfo();
//    // Get the name of the device
//    std::cout << "Device name: " << devInfo->name() << std::endl;
//    // Get the pid, vid, uid of the device
//    std::cout << "Device pid: " << devInfo->pid() << " vid: " << devInfo->vid() << " uid: " << devInfo->uid() << std::endl;

//    // By getting the firmware version number of the device
//    auto fwVer = devInfo->firmwareVersion();
//    std::cout << "Firmware version: " << fwVer << std::endl;

//    // By getting the serial number of the device
//    auto sn = devInfo->serialNumber();
//    std::cout << "Serial number: " << sn << std::endl;

//    // By getting the connection type of the device
//    auto connectType = devInfo->connectionType();
//    std::cout << "ConnectionType: " << connectType << std::endl;

//    // Get the list of supported sensors
//    std::cout << "Sensor types: " << std::endl;
//    auto sensorList = dev->getSensorList();
//    for(uint32_t i = 0; i < sensorList->count(); i++) {
//        auto sensor = sensorList->getSensor(i);
//        switch(sensor->type()) {
//        case OB_SENSOR_COLOR:
//            std::cout << "\tColor sensor" << std::endl;
//            break;
//        case OB_SENSOR_DEPTH:
//            std::cout << "\tDepth sensor" << std::endl;
//            break;
//        case OB_SENSOR_IR:
//            std::cout << "\tIR sensor" << std::endl;
//            break;
//        case OB_SENSOR_IR_LEFT:
//            std::cout << "\tIR Left sensor" << std::endl;
//            break;
//        case OB_SENSOR_IR_RIGHT:
//            std::cout << "\tIR Right sensor" << std::endl;
//            break;
//        case OB_SENSOR_GYRO:
//            std::cout << "\tGyro sensor" << std::endl;
//            break;
//        case OB_SENSOR_ACCEL:
//            std::cout << "\tAccel sensor" << std::endl;
//            break;
//        default:
//            break;
//        }
//    }


//    // create a pipeline with default device
//    ob::Pipeline pipe;



//    // get profiles list from sensors
//    auto dProfilesL = pipe.getStreamProfileList(OB_SENSOR_DEPTH);
//    auto cProfilesL = pipe.getStreamProfileList(OB_SENSOR_COLOR);
//    auto irProfilesL = pipe.getStreamProfileList(OB_SENSOR_IR);

//    // retrieve video profiles
//    std::shared_ptr<ob::VideoStreamProfile> colorProfile = nullptr;
//    std::shared_ptr<ob::VideoStreamProfile> irProfile    = nullptr;
//    std::shared_ptr<ob::VideoStreamProfile> depthProfile = nullptr;

//    try {
//        colorProfile = cProfilesL->getVideoStreamProfile(1280, OB_HEIGHT_ANY, OB_FORMAT_RGB, 30);
//        depthProfile = dProfilesL->getVideoStreamProfile(640, OB_HEIGHT_ANY, OB_FORMAT_Y16, 30);
//        irProfile    = irProfilesL->getVideoStreamProfile(640, OB_HEIGHT_ANY, OB_FORMAT_Y16, 30);
//    }
//    catch(ob::Error &e) {
//        // If the specified format is not found, search for the default profile to open the stream
//        colorProfile = std::const_pointer_cast<ob::StreamProfile>(cProfilesL->getProfile(OB_PROFILE_DEFAULT))->as<ob::VideoStreamProfile>();
//        depthProfile = std::const_pointer_cast<ob::StreamProfile>(dProfilesL->getProfile(OB_PROFILE_DEFAULT))->as<ob::VideoStreamProfile>();
//        irProfile    = std::const_pointer_cast<ob::StreamProfile>(irProfilesL->getProfile(OB_PROFILE_DEFAULT))->as<ob::VideoStreamProfile>();
//    }

//    // configure which streams to enable or disable for the Pipeline by creating a Config
//    std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();
//    config->enableStream(colorProfile);
//    config->enableStream(irProfile);
//    config->enableStream(depthProfile);

//    // Configure the alignment mode as hardware D2C alignment
////    ALIGN_DISABLE,     /**< Turn off alignment */
////    ALIGN_D2C_HW_MODE, /**< Hardware D2C alignment mode */
////    ALIGN_D2C_SW_MODE, /**< Software D2C alignment mode */
//    config->setAlignMode(ALIGN_D2C_HW_MODE);
////    void setDepthScaleRequire(bool enable);
////    void setD2CTargetResolution(uint32_t d2cTargetWidth, uint32_t d2cTargetHeight);

//    // Start the pipeline with config
//    pipe.start(config);

////    // Start the pipeline and pass in the configuration
////    pipe->start(config, [i](std::shared_ptr<ob::FrameSet> frameSet) {
////        std::lock_guard<std::mutex> lock(frameMutex);
////        if(frameSet->colorFrame()) {
////            colorFrames[i] = frameSet->colorFrame();
////        }
////        if(frameSet->depthFrame()) {
////            depthFrames[i] = frameSet->depthFrame();
////        }
////    });

////    pipe.enableFrameSync();
////    pipe.disableFrameSync();

//    bool readData = true;
//    while(readData){
//        // Wait for up to 100ms for a frameset in blocking mode.
//        std::shared_ptr<ob::FrameSet> frameSet = pipe.waitForFrames(100);
//        if(frameSet == nullptr) {
//            continue;
//        }

//        // get per frame
////        frameSet->frameCount();
////        frameSet->getFrame(0);
////        frameSet->getFrame(OBFrameType::OB_FRAME_COLOR);
////        OB_FRAME_VIDEO     = 0,  /**< Video frame */
////        OB_FRAME_IR        = 1,  /**< IR frame */
////        OB_FRAME_COLOR     = 2,  /**< Color frame */
////        OB_FRAME_DEPTH     = 3,  /**< Depth frame */
////        OB_FRAME_ACCEL     = 4,  /**< Accelerometer data frame */
////        OB_FRAME_SET       = 5,  /**< Frame collection (internally contains a variety of data frames) */
////        OB_FRAME_POINTS    = 6,  /**< Point cloud frame */
////        OB_FRAME_GYRO      = 7,  /**< Gyroscope data frame */
////        OB_FRAME_IR_LEFT   = 8,  /**< Left IR frame */
////        OB_FRAME_IR_RIGHT  = 9,  /**< Right IR frame */
////        OB_FRAME_RAW_PHASE = 10, /**< Rawphase frame*/

//        // get specific
//        if(frameSet->colorFrame()) {
//            // https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
//            auto cf = frameSet->colorFrame();
//            // videoframe
////            cf->data();
////            uint32_t width();
////            uint32_t height();
////            void *metadata();
////            uint32_t metadataSize();
////            uint8_t pixelAvailableBitSize();

//            // frame
////            virtual OBFrameType type();
////            virtual OBFormat format();
////            OB_FORMAT_YUYV       = 0,    /**< YUYV format */
////            OB_FORMAT_YUY2       = 1,    /**< YUY2 format (the actual format is the same as YUYV) */
////            OB_FORMAT_UYVY       = 2,    /**< UYVY format */
////            OB_FORMAT_NV12       = 3,    /**< NV12 format */
////            OB_FORMAT_NV21       = 4,    /**< NV21 format */
////            OB_FORMAT_MJPG       = 5,    /**< MJPEG encoding format */
////            OB_FORMAT_H264       = 6,    /**< H.264 encoding format */
////            OB_FORMAT_H265       = 7,    /**< H.265 encoding format */
////            OB_FORMAT_Y16        = 8,    /**< Y16 format, single channel 16-bit depth */
////            OB_FORMAT_Y8         = 9,    /**< Y8 format, single channel 8-bit depth */
////            OB_FORMAT_Y10        = 10,   /**< Y10 format, single channel 10-bit depth (SDK will unpack into Y16 by default) */
////            OB_FORMAT_Y11        = 11,   /**< Y11 format, single channel 11-bit depth (SDK will unpack into Y16 by default) */
////            OB_FORMAT_Y12        = 12,   /**< Y12 format, single channel 12-bit depth (SDK will unpack into Y16 by default) */
////            OB_FORMAT_GRAY       = 13,   /**< GRAY (the actual format is the same as YUYV) */
////            OB_FORMAT_HEVC       = 14,   /**< HEVC encoding format (the actual format is the same as H265) */
////            OB_FORMAT_I420       = 15,   /**< I420 format */
////            OB_FORMAT_ACCEL      = 16,   /**< Acceleration data format */
////            OB_FORMAT_GYRO       = 17,   /**< Gyroscope data format */
////            OB_FORMAT_POINT      = 19,   /**< XYZ 3D coordinate point format */
////            OB_FORMAT_RGB_POINT  = 20,   /**< XYZ 3D coordinate point format with RGB information */
////            OB_FORMAT_RLE        = 21,   /**< RLE pressure test format (SDK will be unpacked into Y16 by default) */
////            OB_FORMAT_RGB        = 22,   /**< RGB format (actual RGB888)  */
////            OB_FORMAT_BGR        = 23,   /**< BGR format (actual BGR888) */
////            OB_FORMAT_Y14        = 24,   /**< Y14 format, single channel 14-bit depth (SDK will unpack into Y16 by default) */
////            OB_FORMAT_BGRA       = 25,   /**< BGRA format */
////            OB_FORMAT_COMPRESSED = 26,   /**< Compression format */
////            OB_FORMAT_RVL        = 27,   /**< RVL pressure test format (SDK will be unpacked into Y16 by default) */
////            OB_FORMAT_UNKNOWN    = 0xff, /**< unknown format */
////            virtual uint64_t index();
////            virtual void *data();
////            virtual uint32_t dataSize();
////            uint64_t timeStamp();
////            uint64_t timeStampUs();
////            uint64_t systemTimeStamp();
////            template <typename T> bool is();
////            template <typename T> std::shared_ptr<T> as();
//        }


//        frameSet->depthFrame();
//        frameSet->irFrame();
//        frameSet->pointsFrame();

//        readData = false;
//    }

//    pipe.stop();

////        while(app) {


////            // Render frameset in the window, only color frames are rendered here.
////            app.addToRender(frameSet->colorFrame());

////        // for Y16 format depth frame, print the distance of the center pixel every 30 frames
////        if(depthFrame->index() % 30 == 0 && depthFrame->format() == OB_FORMAT_Y16) {
////            uint32_t  width  = depthFrame->width();
////            uint32_t  height = depthFrame->height();
////            float     scale  = depthFrame->getValueScale();
////            uint16_t *data   = (uint16_t *)depthFrame->data();

////            // pixel value multiplied by scale is the actual distance value in millimeters
////            float centerDistance = data[width * height / 2 + width / 2] * scale;

////            // attention: if the distance is 0, it means that the depth camera cannot detect the object（may be out of detection range）
////            std::cout << "Facing an object " << centerDistance << " mm away. " << std::endl;
////        }
////        }

////        // Stop the Pipeline, no frame data will be generated
////        pipe.stop();

//    // ############################ MULTIFRAME

////        // create frame resource
////        std::mutex                 videoFrameMutex;
////        std::shared_ptr<ob::Frame> colorFrame;
////        std::shared_ptr<ob::Frame> depthFrame;
////        std::shared_ptr<ob::Frame> irFrame;

////        std::mutex                 accelFrameMutex;
////        std::shared_ptr<ob::Frame> accelFrame;

////        std::mutex                 gyroFrameMutex;
////        std::shared_ptr<ob::Frame> gyroFrame;

////        std::vector<std::shared_ptr<ob::Frame>> frames;

////        // Create a pipeline with default device
////        ob::Pipeline pipe;

////        // Configure which streams to enable or disable for the Pipeline by creating a Config
////        std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

////        try {
////            auto colorProfiles = pipe.getStreamProfileList(OB_SENSOR_COLOR);
////            auto colorProfile  = colorProfiles->getProfile(OB_PROFILE_DEFAULT);
////            config->enableStream(colorProfile->as<ob::VideoStreamProfile>());
////        }
////        catch(...) {
////            std::cout << "color stream not found!" << std::endl;
////        }

////        try {
////            auto depthProfiles = pipe.getStreamProfileList(OB_SENSOR_DEPTH);
////            auto depthProfile  = depthProfiles->getProfile(OB_PROFILE_DEFAULT);
////            config->enableStream(depthProfile->as<ob::VideoStreamProfile>());
////        }
////        catch(...) {
////            std::cout << "depth stream not found!" << std::endl;
////        }

////        try {
////            auto irProfiles = pipe.getStreamProfileList(OB_SENSOR_IR);
////            auto irProfile  = irProfiles->getProfile(OB_PROFILE_DEFAULT);
////            config->enableStream(irProfile->as<ob::VideoStreamProfile>());
////        }
////        catch(...) {
////            std::cout << "ir stream not found!" << std::endl;
////        }

////        // Configure the alignment mode as hardware D2C alignment
////        config->setAlignMode(ALIGN_D2C_HW_MODE);


////        pipe.start(config, [&](std::shared_ptr<ob::FrameSet> frameset) {
////            std::unique_lock<std::mutex> lk(videoFrameMutex);
////            colorFrame = frameset->colorFrame();
////            depthFrame = frameset->depthFrame();
////            irFrame    = frameset->irFrame();
////        });
//}

auto prop() -> void{

//       OB_PROP_LDP_BOOL LDP switch
//       OB_PROP_LASER_BOOL Laser switch
//       OB_PROP_LASER_PULSE_WIDTH_INT laser pulse width
//       OB_PROP_LASER_CURRENT_FLOAT Laser current (uint: mA)
//       OB_PROP_FLOOD_BOOL IR flood switch
//       OB_PROP_FLOOD_LEVEL_INT IR flood level
//       OB_PROP_DEPTH_MIRROR_BOOL Depth mirror
//       OB_PROP_DEPTH_FLIP_BOOL Depth flip
//       OB_PROP_DEPTH_POSTFILTER_BOOL Depth Postfilter
//       OB_PROP_DEPTH_HOLEFILTER_BOOL Depth Holefilter
//       OB_PROP_IR_MIRROR_BOOL IR mirror
//       OB_PROP_IR_FLIP_BOOL IR flip
//       OB_PROP_MIN_DEPTH_INT Minimum depth threshold
//       OB_PROP_MAX_DEPTH_INT Maximum depth threshold
//       OB_PROP_DEPTH_SOFT_FILTER_BOOL Software filter switch
//       OB_PROP_LDP_STATUS_BOOL LDP status
//       OB_PROP_DEPTH_MAX_DIFF_INT soft filter maxdiff param
//       OB_PROP_DEPTH_MAX_SPECKLE_SIZE_INT soft filter maxSpeckleSize
//       OB_PROP_DEPTH_ALIGN_HARDWARE_BOOL Hardware d2c is on
//       OB_PROP_TIMESTAMP_OFFSET_INT Timestamp adjustment
//       OB_PROP_HARDWARE_DISTORTION_SWITCH_BOOL Hardware distortion switch Rectify
//       OB_PROP_FAN_WORK_MODE_INT Fan mode switch
//       OB_PROP_DEPTH_ALIGN_HARDWARE_MODE_INT Multi-resolution D2C mode
//       OB_PROP_ANTI_COLLUSION_ACTIVATION_STATUS_BOOL Anti_collusion activation status
//       OB_PROP_DEPTH_PRECISION_LEVEL_INT the depth precision level, which may change the depth frame data unit, needs to be confirmed through the ValueScale interface of DepthFrame
//       OB_PROP_TOF_FILTER_RANGE_INT tof filter range configuration
//       OB_PROP_LASER_MODE_INT laser mode, the firmware terminal currently only return 1: IR Drive, 2: Torch
//       OB_PROP_RECTIFY2_BOOL brt2r-rectify function switch (brt2r is a special module on mx6600), 0: Disable, 1: Rectify Enable
//       OB_PROP_COLOR_MIRROR_BOOL Color mirror
//       OB_PROP_COLOR_FLIP_BOOL Color flip
//       OB_PROP_INDICATOR_LIGHT_BOOL Indicator switch, 0: Disable, 1: Enable
//       OB_PROP_DISPARITY_TO_DEPTH_BOOL Disparity to depth switch, 0: off, the depth stream outputs the disparity map; 1. On, the depth stream outputs the depth map.
//       OB_PROP_BRT_BOOL BRT function switch (anti-background interference), 0: Disable, 1: Enable
//       OB_PROP_WATCHDOG_BOOL Watchdog function switch, 0: Disable, 1: Enable
//       OB_PROP_EXTERNAL_SIGNAL_RESET_BOOL External signal trigger restart function switch, 0: Disable, 1: Enable
//       OB_PROP_HEARTBEAT_BOOL Heartbeat monitoring function switch, 0: Disable, 1: Enable
//       OB_PROP_DEPTH_CROPPING_MODE_INT Depth cropping mode device: OB_DEPTH_CROPPING_MODE
//       OB_PROP_D2C_PREPROCESS_BOOL D2C preprocessing switch (such as RGB cropping), 0: off, 1: on
//       OB_PROP_RGB_CUSTOM_CROP_BOOL Custom RGB cropping switch, 0 is off, 1 is on custom cropping, and the ROI cropping area is issued
//       OB_PROP_DEVICE_WORK_MODE_INT Device operating mode (power consumption)
//       OB_PROP_DEVICE_COMMUNICATION_TYPE_INT Device communication type, 0: USB; 1: Ethernet(RTSP)
//       OB_PROP_SWITCH_IR_MODE_INT Switch infrared imaging mode, 0: active IR mode, 1: passive IR mode
//       OB_PROP_LASER_ENERGY_LEVEL_INT Laser energy level
//       OB_PROP_LDP_MEASURE_DISTANCE_INT LDP's measure distance, unit: mm
//       OB_PROP_TIMER_RESET_SIGNAL_BOOL Reset device time to zero
//       OB_PROP_TIMER_RESET_TRIGGER_OUT_ENABLE_BOOLf Enable send reset device time signal to other device. true: enable, false: disable
//       OB_PROP_TIMER_RESET_DELAY_US_INT Delay to reset device time, unit: us
//       OB_PROP_CAPTURE_IMAGE_SIGNAL_BOOL Signal to capture image
//       OB_PROP_IR_RIGHT_MIRROR_BOOL Right IR sensor mirror state
//       OB_PROP_CAPTURE_IMAGE_FRAME_NUMBER_INT Number frame to capture once a 'OB_PROP_CAPTURE_IMAGE_SIGNAL_BOOL' effect. range: [1, 255]
//       OB_PROP_IR_RIGHT_FLIP_BOOL Right IR sensor flip state. true: flip image, false: origin, default: false
//       OB_PROP_COLOR_ROTATE_INT Color sensor rotation, angle{0, 90, 180, 270}
//       OB_PROP_IR_ROTATE_INT IR/Left-IR sensor rotation, angle{0, 90, 180, 270}
//       OB_PROP_IR_RIGHT_ROTATE_INT Right IR sensor rotation, angle{0, 90, 180, 270}
//       OB_PROP_DEPTH_ROTATE_INT Depth sensor rotation, angle{0, 90, 180, 270}
//       OB_PROP_LASER_HW_ENERGY_LEVEL_INT Get hardware laser energy level which real state of laser element. OB_PROP_LASER_ENERGY_LEVEL_INT（99）will effect this command which it setting and changed the hardware laser energy level.
//       OB_PROP_USB_POWER_STATE_INT USB's power state, enum type: OBUSBPowerState
//       OB_PROP_DC_POWER_STATE_INT DC's power state, enum type: OBDCPowerState
//       OB_PROP_DEVICE_DEVELOPMENT_MODE_INT Device development mode switch, optional modes can refer to the definition in @ref OBDeviceDevelopmentMode,the default mode is @ref OB_USER_MODE @attention The device takes effect after rebooting when switching modes.
//       OB_PROP_SYNC_SIGNAL_TRIGGER_OUT_BOOL Multi-DeviceSync synchronized signal trigger out is enable state. true: enable, false: disable
//       OB_PROP_RESTORE_FACTORY_SETTINGS_BOOL Restore factory settings and factory parameters @attention This command can only be written, and the parameter value must be true. The command takes effect after restarting the device.
//       OB_PROP_BOOT_INTO_RECOVERY_MODE_BOOL Enter recovery mode (flashing mode) when boot the device @attention The device will take effect after rebooting with the enable option. After entering recovery mode, you can upgrade the device system. Upgrading the system may cause system damage, please use it with caution.
//       OB_PROP_DEVICE_IN_RECOVERY_MODE_BOOL Query whether the current device is running in recovery mode (read-only)
//       OB_PROP_CAPTURE_INTERVAL_MODE_INT Capture interval mode, 0:time interval, 1:number interval
//       OB_PROP_CAPTURE_IMAGE_TIME_INTERVAL_INT Capture time interval
//       OB_PROP_CAPTURE_IMAGE_NUMBER_INTERVAL_INT Capture number interval
//       OB_PROP_TIMER_RESET_ENABLE_BOOL Timer reset function enable
//       OB_PROP_DEVICE_USB3_REPEAT_IDENTIFY_BOOL Enable switch for USB3.0 repeated recognition on the device.
//       OB_PROP_DEVICE_REBOOT_DELAY_INT Reboot device delay mode. Delay time unit: ms, range: [0, 8000).
//       OB_PROP_LASER_OVERCURRENT_PROTECTION_STATUS_BOOL Query the status of laser overcurrent protection (read-only)
//       OB_PROP_LASER_PULSE_WIDTH_PROTECTION_STATUS_BOOL Query the status of laser pulse width protection (read-only)
//       OB_STRUCT_BASELINE_CALIBRATION_PARAM Baseline calibration parameters
//       OB_STRUCT_DEVICE_TEMPERATURE Device temperature information
//       OB_STRUCT_TOF_EXPOSURE_THRESHOLD_CONTROL TOF exposure threshold range
//       OB_STRUCT_DEVICE_SERIAL_NUMBER get/set serial number
//       OB_STRUCT_DEVICE_TIME get/set device time
//       OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG Multi-device synchronization mode and parameter configuration
//       OB_STRUCT_RGB_CROP_ROI RGB cropping ROI
//       OB_STRUCT_DEVICE_IP_ADDR_CONFIG Device IP address configuration
//       OB_STRUCT_CURRENT_DEPTH_ALG_MODE The current camera depth mode
//       OB_STRUCT_DEPTH_PRECISION_SUPPORT_LIST A list of depth accuracy levels, returning an array of uin16_t, corresponding to the enumeration
//       OB_STRUCT_DEVICE_STATIC_IP_CONFIG_RECORD Device network static ip config record@brief Using for get last static ip config，witch is record in device flash when user set static ip config@attention read only
//       OB_PROP_COLOR_AUTO_EXPOSURE_BOOL Color camera auto exposure
//       OB_PROP_COLOR_EXPOSURE_INT Color camera exposure adjustment
//       OB_PROP_COLOR_GAIN_INT Color camera gain adjustment
//       OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL Color camera automatic white balance
//       OB_PROP_COLOR_WHITE_BALANCE_INT Color camera white balance adjustment
//       OB_PROP_COLOR_BRIGHTNESS_INT Color camera brightness adjustment
//       OB_PROP_COLOR_SHARPNESS_INT Color camera sharpness adjustment
//       OB_PROP_COLOR_SHUTTER_INT Color camera shutter adjustment
//       OB_PROP_COLOR_SATURATION_INT Color camera saturation adjustment
//       OB_PROP_COLOR_CONTRAST_INT Color camera contrast adjustment
//       OB_PROP_COLOR_GAMMA_INT Color camera gamma adjustment
//       OB_PROP_COLOR_ROLL_INT Color camera image rotation
//       OB_PROP_COLOR_AUTO_EXPOSURE_PRIORITY_INT Color camera auto exposure priority
//       OB_PROP_COLOR_BACKLIGHT_COMPENSATION_INT Color camera brightness compensation
//       OB_PROP_COLOR_HUE_INT Color camera color tint
//       OB_PROP_COLOR_POWER_LINE_FREQUENCY_INT Color Camera Power Line Frequency
//       OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL Automatic exposure of depth camera (infrared camera will be set synchronously under some models of devices)
//       OB_PROP_DEPTH_EXPOSURE_INT Depth camera exposure adjustment (infrared cameras will be set synchronously under some models of devices)
//       OB_PROP_DEPTH_GAIN_INT Depth camera gain adjustment (infrared cameras will be set synchronously under some models of devices)
//       OB_PROP_IR_AUTO_EXPOSURE_BOOL Infrared camera auto exposure (depth camera will be set synchronously under some models of devices)
//       OB_PROP_IR_EXPOSURE_INT Infrared camera exposure adjustment (some models of devices will set the depth camera synchronously)
//       OB_PROP_IR_GAIN_INT Infrared camera gain adjustment (the depth camera will be set synchronously under some models of devices)
//       OB_PROP_IR_CHANNEL_DATA_SOURCE_INT Select Infrared camera data source channel. If not support throw exception. 0 : IR stream from IR Left sensor; 1 : IR stream from IR Right sensor;
//       OB_PROP_DEPTH_RM_FILTER_BOOL Depth effect dedistortion, true: on, false: off. mutually exclusive with D2C function, RM_Filter disable When hardware or software D2C is enabled.
//       OB_PROP_COLOR_MAXIMAL_GAIN_INT Color camera maximal gain
//       OB_PROP_COLOR_MAXIMAL_SHUTTER_INT Color camera shutter gain
//       OB_PROP_IR_SHORT_EXPOSURE_BOOL The enable/disable switch for IR short exposure function, supported only by a few devices.
//       OB_PROP_COLOR_HDR_BOOL Color camera HDR
//       OB_PROP_SDK_DISPARITY_TO_DEPTH_BOOL Software disparity to depth
//       OB_PROP_SDK_DEPTH_FRAME_UNPACK_BOOL Depth data unpacking function switch (each open stream will be turned on by default, support RLE/Y10/Y11/Y12/Y14 format)
//       OB_PROP_SDK_IR_FRAME_UNPACK_BOOL IR data unpacking function switch (each current will be turned on by default, support RLE/Y10/Y11/Y12/Y14 format)
//       OB_PROP_SDK_ACCEL_FRAME_TRANSFORMED_BOOL Accel data conversion function switch (on by default)
//       OB_PROP_SDK_GYRO_FRAME_TRANSFORMED_BOOL Gyro data conversion function switch (on by default)
//       OB_PROP_SDK_IR_LEFT_FRAME_UNPACK_BOOL Left IR frame data unpacking function switch (each current will be turned on by default, support RLE/Y10/Y11/Y12/Y14 format)
//       OB_PROP_SDK_IR_RIGHT_FRAME_UNPACK_BOOL Right IR frame data unpacking function switch (each current will be turned on by default, support RLE/Y10/Y11/Y12/Y14 format)
//       OB_RAW_DATA_CAMERA_CALIB_JSON_FILE Calibration JSON file read from device (Femto Mega, read only)
}
