#include <string.h>
#include "MAX11612.h"

/**
 * MAC11612 has address
 * 0x34, 52, 110100
 */
#define I2CAddress 0x34

/**
 * Program internal variables
 */
uint16_t channels[4];
double RATIO = 4.915/4.096;
double v_ce, v_i;

/**
 * Setup - Run only once at startup
 */
void setup()
{
  /**
   * Initialize libraries
   */
  Serial.begin(921600);//115200); // Bits per second

  if (MAX11612_Init(I2CAddress) != 0)
  {
    Serial.println("MAX11612 library failed to initialize!");
    while (1){}
  }
  
  // Setup SCAN mode on all channels single-ended mode
  MAX11612_Configuration(I2CAddress, 0b00, 0b0011, 1);
}

/**
 * Main loop
 */
void loop()
{
  memset(channels, 0, 4*sizeof(uint16_t));
  MAX11612_ADC_ScanAll(I2CAddress, channels);

  v_ce = channels[0] * RATIO;
  v_i = channels[1] * RATIO;

  Serial.print(v_ce);
  Serial.print(",");
  Serial.println(v_i);

  /*
  uint16_t adc_value = 0;

  adc_value = MAX11612_ADC_Read(I2CAddress, 0);
  
  Serial.print("ADC Value CH0: ");
  Serial.println(adc_value);
  */

  delayMicroseconds(1); // 1 us == 100kHz
}
