#ifndef MAX11612
#define MAX11612

#include <Wire.h>

uint8_t MAX11612_Init(uint8_t I2CAddress);
uint8_t MAX11612_Setup(uint8_t I2CAddress, uint8_t vref, uint8_t internalClock, uint8_t unipolar, uint8_t resetConfig);
uint8_t MAX11612_Configuration(uint8_t I2CAddress, uint8_t SCAN, uint8_t CS, uint8_t sglDig);
uint16_t MAX11612_ADC_Read(uint8_t I2CAddress, uint8_t channel);
uint8_t MAX11612_ADC_ScanAll(uint8_t I2CAddress, uint16_t *buffer);

/**
 * @param I2CAddress I2C Address of device
 */
uint8_t MAX11612_Init(uint8_t I2CAddress)
{
  uint8_t ret = 0;

  Wire.begin();
  Serial.print("MAX11612 I2C Address: ");
  Serial.println(I2CAddress, HEX);


  ret += MAX11612_Setup(I2CAddress, 0, 0, 0, 0);
  ret += MAX11612_Configuration(I2CAddress, 0b11, 0, 1);

  return ret;
}

/**
 * @param I2CAddress I2C Address of device
 * @param vref Reference voltage
 * @param internalClock Use internal or external clock
 * @param unipolar Bipolar/unipolar mode
 * @param resetConfig Reset configuration to default values
 * @return formed register
 */
uint8_t MAX11612_Setup(uint8_t I2CAddress, uint8_t vref, uint8_t internalClock, uint8_t unipolar, uint8_t resetConfig)
{
  // REG bit 7 = 1 (setup byte)
  uint8_t data = 0x80;

  // vref
  data = data | ((vref & 0x07) << 4);

  // Internal clock
  data = data | ((internalClock & 0x01) << 3);

  // unipolar
  data = data | ((unipolar & 0x01) << 2);

  // resetConfig
  data = data | ((resetConfig & 0x01) << 1);

  // Don't care bit

  Wire.beginTransmission(I2CAddress);
  Wire.write(data);
  Wire.endTransmission();
  
  // Serial.print("Setup: 0x");
  // Serial.println(data, HEX);

  return 0;
  
}

/**
 * @param I2CAddress I2C Address of device
 * @param SCAN Scan mode; 0b00 = Scan from AIN0 ip until selected channel by CS3-CS0, 0b11 = Selected channel
 * @param CS Channel select, 0-3
 * @param sglDig Single-ended more or differential mode
 * @return formed register
 */
uint8_t MAX11612_Configuration(uint8_t I2CAddress, uint8_t SCAN, uint8_t CS, uint8_t sglDig)
{
  // REG config bit = 0
  uint8_t data = 0;

  // Scan select bits
  data = data | ((SCAN & 0x03) << 5);

  // Channel select bits
  data = data | ((CS & 0x0F) << 1);

  // Single-ended, differential mode
  data = data | ((sglDig & 0x01));

  Wire.beginTransmission(I2CAddress);
  Wire.write(data);
  Wire.endTransmission();

  // Serial.print("Configuration: 0x");
  // Serial.println(data, HEX);

  return 0;
}

/**
 * Reads a single byte
 */
// void readBytes(uint8_t *buffer, uint8_t reg, uint8_t length)
// {
//   Wire.requestFrom((uint8_t)I2CAddress, length);
//   for (int i=0; Wire.available() && (i < length); i++)
//   {
//     uint8_t c = Wire.read();

//     Serial.print("  Received: ");
//     Serial.println(c,HEX);

//     *buffer = c;
//     buffer++;
//   }
// }

/**
 * The result is transmitted in two bytes; first four bits of the first bytes are high, then MSB through LSB are consecutively clocked out.
 * After master received, it can issue an ack if it wants to continue reading or not ack if it no longer wishes to read.
 * @param I2CAddress I2C Address of device
 * @param channel Channel to read from
 * @return ADC value referenced to INTERNAL VOLAGE (4,096mV)
 */
uint16_t MAX11612_ADC_Read(uint8_t I2CAddress, uint8_t channel)
{
  uint16_t result = 0;

  MAX11612_Configuration(I2CAddress, 0b11, channel, 1);
  Wire.requestFrom(I2CAddress, (uint8_t)2);

  if (Wire.available() != 2)
    return -1;

  result = ((uint8_t)Wire.read() & 0x0F) << 8; // Read MSB (4 bits)
  result |= ((uint8_t)Wire.read() & 0x00FF);  // Read LSB
  return result;
}

/**
 * Scan mode
 * @param I2CAddress I2C Address of device
 * @param buffer uint16_t * 4 buffer where results will be written into
 * @return status
 */
uint8_t MAX11612_ADC_ScanAll(uint8_t I2CAddress, uint16_t *buffer)
{
  //MAX11612_Configuration(I2CAddress, 0b00, 0b0011, 1);
  Wire.requestFrom(I2CAddress, (uint8_t)8); // 2 bytes * 4 channels

  if (Wire.available() != 8)
    return -1;
  
  for (int i=0; i<4; i++)
  {
    *(buffer+i) = (Wire.read() & 0x0F) << 8; // Read MSB
    *(buffer+i) |= Wire.read() & 0x00FF; // Read LSB
  }

  return 0;
}

#endif