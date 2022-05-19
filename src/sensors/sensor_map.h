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

#ifndef WATERBEAR_SENSOR_MAP
#define WATERBEAR_SENSOR_MAP

#include "sensor.h"
#include "drivers/generic_analog.h"
#include "drivers/atlas_ec.h"
#include "drivers/driver_template.h"
#include "drivers/adafruit_dht22.h"
// #include <map>
// #include <string>

template<typename T> SensorDriver * createInstance() { return new T; }

typedef std::map<short, SensorDriver*(*)()> sensor_type_map_type;

template <class T>
void setupSensorMaps(short sensorCode, const __FlashStringHelper * sensorTypeString );
void buildDriverSensorMap();
short typeCodeForSensorTypeString(const char * type);
SensorDriver * driverForSensorTypeCode(short type);

#endif