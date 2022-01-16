#include <nano_client_arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <SoftwareSerial.h>

/**
 * Reads input distanecs on TX1/RX1 and TX2/RX2 serial lines, and forwards it onto the Distance topic of the DDS domain.
 */

#define TX1 15
#define RX1 2
#define TX2 4
#define RX2 16

#define CLIENT_KEY          0xC1CA0002
#define MEASUREMENT_DELAY   1000

#define BASE_OBJECT_ID      0x0010
#define INIT_DELAY          1000
#define TRANSPORT_MTU       128
#define WIFI_SSID           "CPS"
#define WIFI_PASSWORD       "12345678"
#define SERIAL_SPEED        115200
#define AGENT_ADDRESS       { 10, 42, 0, 1 }

const uint8_t agent_address[4] = AGENT_ADDRESS;

XrceData transport_recv_buffer[
            XRCE_TRANSPORT_RECV_BUFFER_SIZE(TRANSPORT_MTU)] = { 0 };

XrceUdpTransport transport(
    transport_recv_buffer, sizeof(transport_recv_buffer), agent_address);

XrceClient client(transport, CLIENT_KEY);

XrceDomainParticipant participant(
    client, BASE_OBJECT_ID, "ACCParticipantLibrary::McuDistance");

XrcePublisher publisher(
    client, BASE_OBJECT_ID, participant.id(), "Publisher");

XrceDataWriter writer(
    client, BASE_OBJECT_ID, publisher.id(), "DistanceWriter");

struct FloatSensorData
{
    float payload;
    uint64_t timestamp;
};

SoftwareSerial slave1;
SoftwareSerial slave2;

void serial_setup() {
  slave1.begin(9600, SWSERIAL_8N1, RX1, TX1, false);
  slave2.begin(9600, SWSERIAL_8N1, RX2, TX2, false);
}

// Read until 25ms pass without new data
float serial_read(SoftwareSerial& ss) {
    uint32_t ret = 0;
    bool waited = false;
    while(1) {
      if(ss.available()) {
          ret = (ret << 8) | ss.read();
          waited = false;
      } else if (waited) {
          return *(float*)&ret;
      } else {
          delay(25);
          waited = true;
      }
    }
}

void wifi_connect()
{
    ArduinoOTA.setHostname("distancemcu");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(INIT_DELAY);
    }
    ArduinoOTA.begin();
}

bool xrce_connect()
{
    if (!client.initialize())
    {
      return false;
    }
    
    while (!client.connected())
    {
      client.connect(INIT_DELAY);
    }
    
    if (!participant.create())
    {
        return false;
    }
    if (!publisher.create())
    {
        return false;
    }

    if (!writer.create())
    {
        return false;
    }

    return true;
}


void setup()
{
    serial_setup();
    wifi_connect();

    if (!xrce_connect())
    {
        while (1) {}
    }

}

void loop()
{
    static long last_millis = - MEASUREMENT_DELAY;
    long current_millis = millis();
    if(current_millis - last_millis > MEASUREMENT_DELAY) {
        FloatSensorData data;    
        float dst1 = serial_read(slave1);
        float dst2 = serial_read(slave2);

        data.payload = dst1;
        data.timestamp = current_millis;
        writer.write_data((uint8_t*)&data, sizeof(data));
        last_millis = current_millis;
    }
    ArduinoOTA.handle();
}
