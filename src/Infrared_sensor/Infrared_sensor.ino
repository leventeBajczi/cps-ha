int sensorPin = 12;

void setup() {
    Serial.begin(9600);
    pinMode(sensorPin, INPUT);
}

void serial_write(uint32_t data) {
    Serial.write((char)((data >> 24)));
    Serial.write((char)((data >> 16)));
    Serial.write((char)((data >> 8)));
    Serial.write((char)((data >> 0)));
}


void loop() {
    static uint64_t sent_last_millis = millis();
    uint64_t current_millis = millis();
    if(current_millis - sent_last_millis > 1000) {
        float sensorValue = digitalRead(sensorPin) ? 2.0f : 0.03f;
        serial_write(*(uint32_t*)&sensorValue);
        sent_last_millis = current_millis;
    }
}
