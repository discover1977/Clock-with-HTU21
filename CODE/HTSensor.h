/* 
 * File:   HTU21DF.h
 * Author: Jason McGuire (j.mcguire.2015@ieee.org)
 * Description: A library for a HTU21D-F Humidity/Temperature Sensor
 * for the PIC18F using I2C.
 * See example.c for implementation of the library
 * Sample Product: https://www.adafruit.com/products/1899
 * Created on March 27, 2016, 9:08 AM
 
License Information:
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
 
 */

#ifndef HTU21DF_H
#define HTU21DF_H

#include "i2c-soft.h"

#define HT_WR_ADDR 					0x80 // write address for temp/humidity sensor
#define HT_RD_ADDR 					0x81 // read address for temp/humidity sensor

#define HT_TEMP_MEAS_HOLD_MASTER	0xE3
#define HT_HUM_MEAS_HOLD_MASTER		0xE5
#define HT_TEMP_MEAS_noHOLD_MASTER	0xF3
#define HT_HUM_MEAS_noHOLD_MASTER	0xF5
#define HT_WRITE_USER_REGISTER		0xE6
#define HT_READ_USER_REGISTER		0xE7
#define HT_SOFT_RESET				0xFE
#define HT_TEMP_COEFFICIENT			-0.15

#define POLYNOMIAL  0x0131

// Available Functions:
float HTSensorReadTemp(); // Get a temperature reading
float HTSensorReadHumidity(); // Get a humidity reading
float HTSensorReadCompensatedHumidity(void);
void HTSensorReset(); // Soft reset of sensor 
void HTSensorHeatOn();
void HTSensorHeatOff();
unsigned char HTReadUserReg();
void HTWriteUserReg(unsigned char);

#endif // HTU21DF_H
