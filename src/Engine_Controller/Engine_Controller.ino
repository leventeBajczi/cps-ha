#include <nano_client_arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <analogWrite.h>


#define CLIENT_KEY          0xC1CA0003
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
    client, BASE_OBJECT_ID, "ACCParticipantLibrary::EngineControl");

XrceSubscriber subscriber(
    client, BASE_OBJECT_ID, participant.id(), "Subscriber");

XrceDataReader reader(
    client, BASE_OBJECT_ID, subscriber.id(), "EngineCommandReader");

struct Command
{
    char payload[32];
};

void on_data(void *const listener, XrceClient& client, const XrceReceivedData& recv_data)
{
    Command data;
    const uint8_t *data_ptr = recv_data.data();
    memcpy(data.payload, data_ptr, sizeof(data.payload));
    if(!strcmp(data.payload, "increase")) {
        analogWrite(2, 255);
    } else if (!strcmp(data.payload, "decrease")) {
        analogWrite(2, 32);
    } else if (!strcmp(data.payload, "hold")) {
        analogWrite(2, 128);
    }
}

void wifi_connect()
{
    ArduinoOTA.setHostname("enginecontroller");
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

    client.on_data_callback(on_data);

    
    if (!participant.create())
    {
        return false;
    }
    if (!subscriber.create())
    {
        return false;
    }

    if (!reader.create())
    {
        return false;
    }

    return true;
}


void setup()
{
    wifi_connect();

    if (!xrce_connect())
    {
        while (1) {}
    }

    pinMode(2, OUTPUT);

}

void loop()
{
    reader.read_data();
    ArduinoOTA.handle();
}
