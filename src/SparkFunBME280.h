/******************************************************************************
SparkFunBME280.h
BME280 Arduino and Teensy Driver
Marshall Taylor @ SparkFun Electronics
May 20, 2015
https://github.com/sparkfun/BME280_Breakout

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23

This code is released under the [MIT
License](http://opensource.org/licenses/MIT). Please review the LICENSE.md file
included with this example. If you have any questions or concerns with
licensing, please contact techsupport@sparkfun.com. Distributed as-is; no
warranty is given.

TODO:
        roll library ver to 2.0
        remove hard wire.
        write escalating examples


******************************************************************************/

// Test derived class for base class SparkFunIMU
#ifndef __BME280_H__
#define __BME280_H__

#include <cstdint>

#include <driver/i2c.h>

#define MODE_SLEEP 0b00
#define MODE_FORCED 0b01
#define MODE_NORMAL 0b11

// Register names:
#define BME280_DIG_T1_LSB_REG 0x88
#define BME280_DIG_T1_MSB_REG 0x89
#define BME280_DIG_T2_LSB_REG 0x8A
#define BME280_DIG_T2_MSB_REG 0x8B
#define BME280_DIG_T3_LSB_REG 0x8C
#define BME280_DIG_T3_MSB_REG 0x8D
#define BME280_DIG_P1_LSB_REG 0x8E
#define BME280_DIG_P1_MSB_REG 0x8F
#define BME280_DIG_P2_LSB_REG 0x90
#define BME280_DIG_P2_MSB_REG 0x91
#define BME280_DIG_P3_LSB_REG 0x92
#define BME280_DIG_P3_MSB_REG 0x93
#define BME280_DIG_P4_LSB_REG 0x94
#define BME280_DIG_P4_MSB_REG 0x95
#define BME280_DIG_P5_LSB_REG 0x96
#define BME280_DIG_P5_MSB_REG 0x97
#define BME280_DIG_P6_LSB_REG 0x98
#define BME280_DIG_P6_MSB_REG 0x99
#define BME280_DIG_P7_LSB_REG 0x9A
#define BME280_DIG_P7_MSB_REG 0x9B
#define BME280_DIG_P8_LSB_REG 0x9C
#define BME280_DIG_P8_MSB_REG 0x9D
#define BME280_DIG_P9_LSB_REG 0x9E
#define BME280_DIG_P9_MSB_REG 0x9F
#define BME280_DIG_H1_REG 0xA1
#define BME280_CHIP_ID_REG 0xD0 // Chip ID
#define BME280_RST_REG 0xE0     // Softreset Reg
#define BME280_DIG_H2_LSB_REG 0xE1
#define BME280_DIG_H2_MSB_REG 0xE2
#define BME280_DIG_H3_REG 0xE3
#define BME280_DIG_H4_MSB_REG 0xE4
#define BME280_DIG_H4_LSB_REG 0xE5
#define BME280_DIG_H5_MSB_REG 0xE6
#define BME280_DIG_H6_REG 0xE7
#define BME280_CTRL_HUMIDITY_REG 0xF2    // Ctrl Humidity Reg
#define BME280_STAT_REG 0xF3             // Status Reg
#define BME280_CTRL_MEAS_REG 0xF4        // Ctrl Measure Reg
#define BME280_CONFIG_REG 0xF5           // Configuration Reg
#define BME280_MEASUREMENTS_REG 0xF7     // Measurements register start
#define BME280_PRESSURE_MSB_REG 0xF7     // Pressure MSB
#define BME280_PRESSURE_LSB_REG 0xF8     // Pressure LSB
#define BME280_PRESSURE_XLSB_REG 0xF9    // Pressure XLSB
#define BME280_TEMPERATURE_MSB_REG 0xFA  // Temperature MSB
#define BME280_TEMPERATURE_LSB_REG 0xFB  // Temperature LSB
#define BME280_TEMPERATURE_XLSB_REG 0xFC // Temperature XLSB
#define BME280_HUMIDITY_MSB_REG 0xFD     // Humidity MSB
#define BME280_HUMIDITY_LSB_REG 0xFE     // Humidity LSB

namespace SparkFun {

// Class BME280_SensorSettings.  This object is used to hold settings data.  The
// application uses this classes' data directly.  The settings are adopted and
// sent to the sensor at special times, such as .begin.  Some are used for doing
// math.
//
// This is a kind of bloated way to do this.  The trade-off is that the user
// doesn't need to deal with #defines or enums with bizarre names.
//
// A power user would strip out BME280_SensorSettings entirely, and send
// specific read and write command directly to the IC. (ST #defines below)
//
struct BME280_SensorSettings {
  float tempCorrection;
};

// Used to hold the calibration constants.  These are used
// by the driver as measurements are being taking
struct SensorCalibration {
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;

  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;

  uint8_t dig_H1;
  int16_t dig_H2;
  uint8_t dig_H3;
  int16_t dig_H4;
  int16_t dig_H5;
  int8_t dig_H6;
};

struct BME280_SensorMeasurements {
  float temperature;
  float pressure;
  float humidity;
};

// This is the main operational class of the driver.

class BME280 {
public:
  // settings
  BME280_SensorSettings settings;
  SensorCalibration calibration;
  int32_t t_fine;

  // Constructor generates default BME280_SensorSettings.
  //(over-ride after construction if desired)
  BME280();
  //~BME280() = default;
  BME280(i2c_port_t hardPort);

  BME280(const BME280 &) = delete;
  BME280 &operator=(const BME280 &) = delete;

  // Call to apply BME280_SensorSettings.
  // This also gets the SensorCalibration constants
  uint8_t begin();

  uint8_t getMode();          // Get the current mode: sleep, forced, or normal
  void setMode(uint8_t mode); // Set the current mode

  void setTempOverSample(
      uint8_t overSampleAmount); // Set the temperature sample mode
  void setPressureOverSample(
      uint8_t overSampleAmount); // Set the pressure sample mode
  void setHumidityOverSample(
      uint8_t overSampleAmount); // Set the humidity sample mode
  void setStandbyTime(
      uint8_t timeSetting); // Set the standby time between measurements
  void setFilter(uint8_t filterSetting); // Set the filter

  void setI2CAddress(uint8_t i2caddress); // Set the address the library should
                                          // use to communicate. Use if address
                                          // jumper is closed (0x76).

  void setReferencePressure(float refPressure); // Allows user to set local sea
                                                // level reference pressure
  float getReferencePressure();

  bool isMeasuring(); // Returns true while the device is taking measurement

  // Software reset routine
  void reset();
  void readAllMeasurements(BME280_SensorMeasurements *measurements,
                           bool tempScale = false);

  // Returns the values as floats.
  float readFloatPressure();
  float readFloatAltitudeMeters();
  float readFloatAltitudeFeet();
  void readFloatPressureFromBurst(uint8_t buffer[],
                                  BME280_SensorMeasurements *measurements);

  float readFloatHumidity();
  void readFloatHumidityFromBurst(uint8_t buffer[],
                                  BME280_SensorMeasurements *measurements);

  // Temperature related methods
  void setTemperatureCorrection(float corr);
  float readTempC();
  float readTempF();
  float readTempFromBurst(uint8_t buffer[]);

  // Dewpoint related methods
  // From Pavel-Sayekat:
  // https://github.com/sparkfun/SparkFun_BME280_Breakout_Board/pull/6/files
  double dewPointC();
  double dewPointF();

  // The following utilities read and write

  // ReadRegisterRegion takes a uint8 array address as input and reads
  // a chunk of memory into that array.
  void readRegisterRegion(uint8_t *, uint8_t, uint8_t);
  // readRegister reads one register
  uint8_t readRegister(uint8_t);
  // Reads two regs, LSByte then MSByte order, and concatenates them
  // Used for two-byte reads
  int16_t readRegisterInt16(uint8_t offset);
  // Writes a byte;
  void writeRegister(uint8_t, uint8_t);

private:
  static constexpr float DEFAULT_REFERENCE_PRESSURE = 101325.0;
  static constexpr auto DEFAULT_HARD_PORT = I2C_NUM_0;
  static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0x77;

  // Checks for valid over sample values
  uint8_t checkSampleValue(uint8_t userValue);
  void readTempCFromBurst(uint8_t buffer[],
                          BME280_SensorMeasurements *measurements);
  void readTempFFromBurst(uint8_t buffer[],
                          BME280_SensorMeasurements *measurements);

  float referencePressure_;
  i2c_port_t hardPort_;
  uint8_t hardAddr_;
};

} // namespace SparkFun
#endif // End of __BME280_H__ definition check
