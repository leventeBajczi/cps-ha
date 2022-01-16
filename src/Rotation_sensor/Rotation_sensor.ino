#include <SoftwareSerial.h>

#define RX  5
#define TX  2
#define BTN 0
#define LED 1

SoftwareSerial serial(RX, TX);

uint8_t failed_speed[4]  = {0x00, 0x00, 0x00, 0x0A};
uint8_t working_speed[4] = {0x00, 0x00, 0x00, 0x64};

void setup()
{
    serial.begin(9600);
    pinMode(BTN, INPUT);
    pinMode(LED, OUTPUT);
}

void loop()
{
    static uint64_t led_last_millis = millis();
    static uint64_t sent_last_millis = millis();
    uint64_t current_millis = millis();
    static bool ledState = false;
    static bool failed = false;
    static uint64_t btnDebounceAt = 0;
    if(failed) {
        if(current_millis - led_last_millis > 100) {
            ledState = !ledState;
            digitalWrite(LED, ledState);
            led_last_millis = current_millis;
        }
        if(current_millis - sent_last_millis > 1000) {
            serial.write(failed_speed, 4);
            sent_last_millis = current_millis;
        }
    } else {
        if(current_millis - led_last_millis > 500) {
            ledState = !ledState;
            digitalWrite(LED, ledState);
            led_last_millis = current_millis;
        }
        if(current_millis - sent_last_millis > 1000) {
            serial.write(working_speed, 4);
            sent_last_millis = current_millis;
        }
    }
    if(digitalRead(BTN) && btnDebounceAt <= millis()) {
        failed = !failed;
        btnDebounceAt = millis() + 1000;
    }
}
