/* 
 *  RRIV - Open Source Environmental Data Logging Platform
 *  Copyright (C) 20202  Zaven Arra  zaven.arra@gmail.com
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

/**
 * @file   atlas_rgb.cpp
 * @author Shayne Marques marques.shayne24@gmail.com
 * @brief  Class declaration to interface with AtlasScientific EZO-RGB Color Sensor in i2C mode
 * https://atlas-scientific.com/probes/color-sensor/
 */

#include "atlas_rgb.h"
#include <Wire_slave.h>
#include "system/monitor.h"

// Reference object
AtlasRGB * rgbSensor = new AtlasRGB();


// Returns instance
AtlasRGB * AtlasRGB::instance() {
  return rgbSensor;
}

// Dummy constructor
AtlasRGB::AtlasRGB(){}

void AtlasRGB::setup(TwoWire * wire)
{
  AtlasRGB::instance()->start(wire);
  AtlasRGB::instance()->deviceInformation();
  AtlasRGB::instance()->sendCommand();
  debug(AtlasRGB::instance()->receiveResponse());
}

void AtlasRGB::stop(){
  // hibernate
}

char * AtlasRGB::mallocDataMemory(){
  return (char * ) malloc(sizeof(char) * this->dataMemorySize);
}


void AtlasRGB::takeMeasurement(char * data)
{
  AtlasRGB::instance()->singleMode();
  AtlasRGB::instance()->sendCommand();
  debug(AtlasRGB::instance()->receiveResponse());
}

// Constructor/Starter
void AtlasRGB::start(TwoWire * wire)
{
  debug(F("In RGB constructor"));
  strcpy(this->inputString, "");
  strcpy(this->sensorString, "");
  this->wire = wire;
  this->code = 0;
  this->red = 0;
  this->green = 0;
  this->blue = 0;
  this->address = 112; // Default address for RGB sensor 0x70
  this->time = 300; // Response delay time in ms
  debug(F("RGB Constructor"));
}

void AtlasRGB::sendCommand() {
  debug("In send command: ");
  debug(this->inputString);
  this->wire->beginTransmission(this->address);
  this->wire->write(this->inputString);
  this->wire->endTransmission();
  strcpy(this->inputString, "");
}

char * AtlasRGB::receiveResponse() {
  static int i = 0;
  if (strcmp(this->inputString, "sleep") != 0) {
    
    // Delay for response
    delay(this->time); 

    // Request for 52 bytes from the circuit
    this->wire->requestFrom(this->address, 52);
    
    // Get response code           
    this->code = this->wire->read();                 
    
    switch (this->code) {                  
      case 1:              
        debug(F("Success"));
        break;                         
      case 2:              
        debug(F("Failed"));             
        break;                         
      case 254:              
        debug(F("Pending"));            
        break;                         
      case 255:              
        debug(F("No Data"));
        break;                         
    }

    // Constructing response array
    while (this->wire->available()) {
      char res = this->wire->read();
      this->sensorString[i++] = res;   
      if (res == 0) {                
        i = 0;            
        break;       
      }
    }
    return this->sensorString;
  }
  return (char *) "";
}

// Sends most recent command and receives latest response from sensor
char * AtlasRGB::run() {
  if (strcmp(this->inputString, "")) {
    sendCommand();
  }
  receiveResponse();
  return this->sensorString;
}

/**************************************************************************************** 
 Add individual sensor commands below
 Negative or false first argument - status query
 Return 0 on success, -1 on failure
*****************************************************************************************/

// Setting LED Brightness
int AtlasRGB::setLEDBrightness(int value, bool powerSaving) {
  if (value < 0) {
    strcpy(this->inputString, "L,");
    strcat(this->inputString, "?");
    return 0;
  }
  if (value >= 0 && value <= 100) {
    if (powerSaving) {
      sprintf(this->inputString, "L,%d,T", value);
      return 0;
    }
    sprintf(this->inputString, "L,%d", value);
    return 0;
  }
  return -1;
}

// Setting Indicator LED on/off
void AtlasRGB::setIndicatorLED(bool status, bool power) {
  strcpy(this->inputString, "iL,");
  if (status) {
    strcat(this->inputString, "?");
  }
  else {
    if (power) {
      strcat(this->inputString, "1");
    }
    else {
      strcat(this->inputString, "0");
    }
  }
}

// Switches back to UART Mode
// Sets baud rate: 300, 1200, 2400, 9600, 19200, 38400, 57600, 115200
int AtlasRGB::setBaudRate(int value) {
  if (value < 0) {
    strcpy(this->inputString, "Baud,?");
    return 0;
  }
  else {
    switch(value) {
      case 300: case 1200: case 2400: case 9600: case 19200: case 38400: case 57600: case 115200: 
        sprintf(this->inputString, "Baud,%d", value);
        return 0;
      default:
        return -1;
    }
  }
}

// Sets gamma correction: 0.01 - 4.99
int AtlasRGB::gammaCorrection(float value) {
  if (value < 0) {
    strcpy(this->inputString, "G,?");
    return 0;
  }
  if (value >= 0.01 && value <= 4.99) {
    sprintf(this->inputString, "G,%f", value);
    return 0;
  }
  return -1;
}

// Sets custom name for sensor
void AtlasRGB::nameDevice(char * value) {
  if (!strcmp(value, "")) {
    strcpy(this->inputString, "Name,?");
  }
  else {
    sprintf(this->inputString, "Name,%s", value);
  }
}

// Returns device information 
void AtlasRGB::deviceInformation() {
  strcpy(this->inputString, "i");
}

// Enters sleep mode
void AtlasRGB::sleepSensor() {
  strcpy(this->inputString, "Sleep");
}

// Performs a factory reset
void AtlasRGB::factoryReset() {
  strcpy(this->inputString, "Factory");
}

// Takes a single reading
void AtlasRGB::singleMode() {
  strcpy(this->inputString, "R");
}

// Calibrates sensor
void AtlasRGB::calibrateSensor() {
  strcpy(this->inputString, "Cal");
}

// Flashes LED to find sensor
void AtlasRGB::findSensor() {
  strcpy(this->inputString, "Find");
}

// Returns device status
void AtlasRGB::getStatus() {
  strcpy(this->inputString, "Status");
}
