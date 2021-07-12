#include "sensor.h"

void SensorDriver::initializeBurst(){
  burstCount = 0;
}

void SensorDriver::incrementBurst(){
  burstCount++;
}

bool SensorDriver::burstCompleted(){
  return burstCount == burstLength;
}

void getDefaultsCommon(common_config_sensor *fillValues)
{
  Serial2.println("getDefaultsCCS");
  Serial2.flush();
  fillValues->sensor_type = 1;
  fillValues->slot = 1;
  fillValues->sensor_burst = 10;
  fillValues->warmup = 54321;
  strcpy(fillValues->tag, "CCS");
  strcpy(fillValues->column_prefix, "test");
  strcpy(fillValues->padding, "0000000000000000");
}