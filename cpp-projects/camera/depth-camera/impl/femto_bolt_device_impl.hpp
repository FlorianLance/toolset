
/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#pragma once

// local
#include "depth-camera/dc_device_impl.hpp"
#include "femto_base_device.hpp"


namespace tool::cam {

struct FemtoBoltDeviceImpl : public DCDeviceImpl{

    FemtoBoltDeviceImpl();

    // actions
    auto open(const DCConfigSettings &newConfigS) -> bool override final;
    auto close() -> void override final;

    // getters
    auto is_opened() const noexcept -> bool override final;
    // auto nb_devices() const noexcept -> std::uint32_t override final;
    auto device_name() const noexcept -> std::string override final;

private:

    // settigns
    auto update_from_colors_settings() -> void override final;

    // read data
    auto capture_frame(std::int32_t timeoutMs) -> bool override final;
    auto read_color_image(bool enable) -> bool override final;
    auto read_depth_image(bool enable) -> bool override final;
    auto read_infra_image(bool enable) -> bool override final;
    // auto read_audio(bool enable) -> void override final {/** NO AUDIO SENSOR */}
    auto read_IMU(bool enable) -> void override final;
    auto read_body_tracking(bool enable) -> void override final;

    // process data
    auto resize_images() -> void override final;
    auto generate_cloud(bool enable) -> void override final;

    std::unique_ptr<FemtoBaseDevice> orbbecD = nullptr;
};
}


// FEMTO BOLT SUPPORTED
// name OB_PROP_DEPTH_MIRROR_BOOL
// name OB_PROP_DEPTH_FLIP_BOOL
// name OB_PROP_IR_MIRROR_BOOL
// name OB_PROP_IR_FLIP_BOOL
// name OB_PROP_TIMESTAMP_OFFSET_INT
// name OB_PROP_COLOR_MIRROR_BOOL
// name OB_PROP_INDICATOR_LIGHT_BOOL
// name OB_PROP_HEARTBEAT_BOOL
// name OB_PROP_SWITCH_IR_MODE_INT
// name OB_PROP_TIMER_RESET_SIGNAL_BOOL
// name OB_PROP_IR_ROTATE_INT
// name OB_PROP_DEPTH_ROTATE_INT
// name OB_PROP_USB_POWER_STATE_INT
// name OB_PROP_DC_POWER_STATE_INT
// name OB_PROP_BOOT_INTO_RECOVERY_MODE_BOOL
// name OB_PROP_TIMER_RESET_ENABLE_BOOL
// name OB_PROP_DEVICE_IQ_DEBUG_BOOL
// name OB_STRUCT_VERSION
// name OB_STRUCT_DEVICE_TEMPERATURE
// name OB_STRUCT_TOF_MODULATION_FREQ
// name OB_STRUCT_TOF_EXPOSURE_THRESHOLD_CONTROL
// name OB_STRUCT_DEVICE_SERIAL_NUMBER
// name OB_STRUCT_DEVICE_TIME
// name OB_STRUCT_MULTI_DEVICE_SYNC_CONFIG
// name OB_STRUCT_LED_CONTROL
// name OB_PROP_COLOR_AUTO_EXPOSURE_BOOL
// name OB_PROP_COLOR_EXPOSURE_INT
// name OB_PROP_COLOR_GAIN_INT
// name OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL
// name OB_PROP_COLOR_WHITE_BALANCE_INT
// name OB_PROP_COLOR_BRIGHTNESS_INT
// name OB_PROP_COLOR_SHARPNESS_INT
// name OB_PROP_COLOR_SATURATION_INT
// name OB_PROP_COLOR_CONTRAST_INT
// name OB_PROP_COLOR_POWER_LINE_FREQUENCY_INT
// name OB_PROP_DEPTH_EXPOSURE_INT
// name OB_PROP_GYRO_ODR_INT
// name OB_PROP_ACCEL_ODR_INT
// name OB_PROP_GYRO_FULL_SCALE_INT
// name OB_PROP_ACCEL_FULL_SCALE_INT
// name OB_PROP_IR_EXPOSURE_INT
// name OB_PROP_COLOR_HDR_BOOL
// name OB_PROP_SDK_ACCEL_FRAME_TRANSFORMED_BOOL
// name OB_PROP_SDK_GYRO_FRAME_TRANSFORMED_BOOL
// name OB_RAW_DATA_HARDWARE_ALIGN_PARAM
// name OB_RAW_DATA_CAMERA_CALIB_JSON_FILE
// name OB_RAW_DATA_MCU_UPGRADE_FILE
// name OB_RAW_DATA_IMU_CALIB_PARAM


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



// auto prop() -> void{

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
// }


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

// OB_FORMAT_YUYV       = 0,    /**< YUYV format */
// OB_FORMAT_YUY2       = 1,    /**< YUY2 format (the actual format is the same as YUYV) */
// OB_FORMAT_UYVY       = 2,    /**< UYVY format */
// OB_FORMAT_NV12       = 3,    /**< NV12 format */
// OB_FORMAT_NV21       = 4,    /**< NV21 format */
// OB_FORMAT_MJPG       = 5,    /**< MJPEG encoding format */
// OB_FORMAT_H264       = 6,    /**< H.264 encoding format */
// OB_FORMAT_H265       = 7,    /**< H.265 encoding format */
// OB_FORMAT_Y16        = 8,    /**< Y16 format, single channel 16-bit depth */
// OB_FORMAT_Y8         = 9,    /**< Y8 format, single channel 8-bit depth */
// OB_FORMAT_Y10        = 10,   /**< Y10 format, single channel 10-bit depth (SDK will unpack into Y16 by default) */
// OB_FORMAT_Y11        = 11,   /**< Y11 format, single channel 11-bit depth (SDK will unpack into Y16 by default) */
// OB_FORMAT_Y12        = 12,   /**< Y12 format, single channel 12-bit depth (SDK will unpack into Y16 by default) */
// OB_FORMAT_GRAY       = 13,   /**< GRAY (the actual format is the same as YUYV) */
// OB_FORMAT_HEVC       = 14,   /**< HEVC encoding format (the actual format is the same as H265) */
// OB_FORMAT_I420       = 15,   /**< I420 format */
// OB_FORMAT_ACCEL      = 16,   /**< Acceleration data format */
// OB_FORMAT_GYRO       = 17,   /**< Gyroscope data format */
// OB_FORMAT_POINT      = 19,   /**< XYZ 3D coordinate point format */
// OB_FORMAT_RGB_POINT  = 20,   /**< XYZ 3D coordinate point format with RGB information */
// OB_FORMAT_RLE        = 21,   /**< RLE pressure test format (SDK will be unpacked into Y16 by default) */
// OB_FORMAT_RGB        = 22,   /**< RGB format (actual RGB888)  */
// OB_FORMAT_BGR        = 23,   /**< BGR format (actual BGR888) */
// OB_FORMAT_Y14        = 24,   /**< Y14 format, single channel 14-bit depth (SDK will unpack into Y16 by default) */
// OB_FORMAT_BGRA       = 25,   /**< BGRA format */
// OB_FORMAT_COMPRESSED = 26,   /**< Compression format */
// OB_FORMAT_RVL        = 27,   /**< RVL pressure test format (SDK will be unpacked into Y16 by default) */
// OB_FORMAT_UNKNOWN    = 0xff, /**< unknown format */

// ####################### COLOR PROFILES

// ########################################################### FEMTO BOLT PROFILES
// COLOR PROFILES
// FPS: 5/15/25/30
// -> 3840 x 2160 -> NV12 / MJPG / BGRA / RGB
// -> 2560 x 1440 -> NV12 / MJPG / BGRA / RGB
// -> 1920 x 1080 -> NV12 / MJPG / BGRA / RGB
// -> 1280 x 960  -> NV12 / MJPG / BGRA / RGB
// -> 1280 x 720  -> NV12 / MJPG / YUYV / BGRA / RGB

// DEPTH PROFILES
// FPS: 5/15/25/30
// -> 1024 x 1024 -> Y16
// -> 640  x 576  -> Y16
// -> 512  x 512  -> Y16
// -> 320  x 288  -> Y16

// INFRA PROFILES
// FPS: 5/15/25/30
// -> 1024 x 1024 -> Y16
// -> 640  x 576  -> Y16
// -> 512  x 512  -> Y16
// -> 320  x 288  -> Y16

// list accel profiles : 24
//     6 1
//     9 1
//     9 2
//     9 3
//     9 4
//     6 2
//     6 3
//     6 4
//     7 1
//     7 2
//     7 3
//     7 4
//     8 1
//     8 2
//     8 3
//     8 4
//     10 1
//     10 2
//     10 3
//     10 4
//     11 1
//     11 2
//     11 3
//     11 4

// list gyuro profiles : 48
//     6 5
//     9 1
//     9 2
//     9 3
//     9 4
//     9 5
//     9 6
//     9 7
//     9 8
//     6 1
//     6 2
//     6 3
//     6 4
//     6 6
//     6 7
//     6 8
//     7 1
//     7 2
//     7 3
//     7 4
//     7 5
//     7 6
//     7 7
//     7 8
//     8 1
//     8 2
//     8 3
//     8 4
//     8 5
//     8 6
//     8 7
//     8 8
//     10 1
//     10 2
//     10 3
//     10 4
//     10 5
//     10 6
//     10 7
//     10 8
//     11 1
//     11 2
//     11 3
//     11 4
//     11 5
//     11 6
//     11 7
//     11 8
