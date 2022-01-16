#include <SoftwareSerial.h>

#define RX1  5
#define TX1  12
#define RX2  6
#define TX2  11
#define RX3  7
#define TX3  10
#define RX4  8
#define TX4  9

SoftwareSerial serial1(RX1, TX1);
SoftwareSerial serial2(RX2, TX2);
SoftwareSerial serial3(RX3, TX3);
SoftwareSerial serial4(RX4, TX4);

// Read until 25ms pass without new data
uint32_t serial_read(SoftwareSerial& ss) {
    uint32_t ret = 0;
    bool waited = false;
    while(1) {
      if(ss.available()) {
          ret = (ret << 8) | ss.read();
          waited = false;
      } else if (waited) {
          return ret;
      } else {
          delay(25);
          waited = true;
      }
    }
}

void serial_write(uint32_t data) {
    Serial.write((char)((data >> 24)));
    Serial.write((char)((data >> 16)));
    Serial.write((char)((data >> 8)));
    Serial.write((char)((data >> 0)));
}

void setup()
{
    serial1.begin(9600);
    serial2.begin(9600);
    serial3.begin(9600);
    serial4.begin(9600);
    Serial.begin(9600);

    serial1.listen();
}

void loop()
{
    static uint64_t sent_last_millis = millis();
    uint64_t current_millis = millis();
  
    if(current_millis - sent_last_millis > 1000) {
        uint32_t input1 = serial_read(serial1);
        uint32_t input2 = serial_read(serial2);
        uint32_t input3 = serial_read(serial3);
        uint32_t input4 = serial_read(serial4);

        serial_write(input1);
        
        sent_last_millis = current_millis;
    }

}
