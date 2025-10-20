#include "sensor.h"
#include "Arduino.h"

float read_temperature() 
{
  float temp = 20.0 + random(0, 1000) / 100.0; 
  return temp;
}
