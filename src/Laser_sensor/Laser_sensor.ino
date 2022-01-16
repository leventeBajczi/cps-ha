#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void(* resetFunc) (void) = 0;

void setup() {
  Serial.begin(9600);
  
  if (!lox.begin()) {
    resetFunc();
  }
}

void serial_write(uint32_t data) {
    Serial.write((char)((data >> 24)));
    Serial.write((char)((data >> 16)));
    Serial.write((char)((data >> 8)));
    Serial.write((char)((data >> 0)));
}



void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {  
    float value = measure.RangeMilliMeter / 1000.0f;
    serial_write(*(uint32_t*)&value);
  } else {
    float value = 2.0f;
    serial_write(*(uint32_t*)&value);
  }
    
  delay(1000);
}
