

#include "esp_err.h"
#include "esp_log.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>

#include "app_network.h"
#include "app_config.h"

#define TAG __FILE__

// ==========================================
// Private Functions
// ==========================================
// MQTT message format
#define MQTT_SUB_TOPIC_CNT_FMT "sensor/%s/control/#"
#define MQTT_PUB_TOPIC_FMT "sensor/%s/%s/%d"

const char *pcAppMsgType[MAX_APP_MSG_TYPE] = {
    "test",
    "status",
    "image",
};

#define CONSTRUCT_MQTT_PUB_TOPIC(outTopic, deviceName, kind, seqId) sprintf(outTopic, MQTT_PUB_TOPIC_FMT, deviceName, pcAppMsgType[kind], seqId)
#define CONSTRUCT_MQTT_SUB_TOPIC(outTopic, deviceName) sprintf(outTopic, MQTT_SUB_TOPIC_CNT_FMT, deviceName)
// クライアント設定
WiFiClient espClient;
PubSubClient client(espClient);

// ================================================================
// Private functions
// ================================================================
void app_network_sub_callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
}
// ================================================================
// Public functions
// ================================================================

void app_network_start()
{
    // Wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(appConfig.ssid, appConfig.password);
    Serial.print("\nWifi Connecting");
    int retry = 0;
    const int retry_count = 120;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
        retry++;
        if (retry > retry_count)
        {
            Serial.println("\nWiFi Connection Failed\nRestarting...");
            delay(1000);
            // reset
            ESP.restart();
        }
    }
    WiFi.setTxPower(WIFI_POWER_13dBm);
    Serial.println("\n WiFi connected");
    Serial.println("IP Address: " + WiFi.localIP().toString());

    // mDNS
    // if (MDNS.begin(appConfig.device_id))
    // {
    //     Serial.println("mDNS responder started");
    //     MDNS.addService("http", "tcp", 80);
    // }

    Serial.print("Connecting to MQTT...");
    client.setServer(appConfig.mqtt_broker, appConfig.mqtt_port);
    client.setCallback(app_network_sub_callback);
    if (client.connect(appConfig.device_id) == false)
    {
        Serial.println("failed");
        return;
    }
    Serial.println("connected");

    // subscribe
    char acTopic[128];
    CONSTRUCT_MQTT_SUB_TOPIC(acTopic, appConfig.device_id);
    client.subscribe(acTopic);
    ESP_LOGI(TAG, "** Subscribed to topic: %s\n", acTopic);
}

bool app_network_is_connected()
{
    return client.connected();
}

void app_network_stop()
{
    // MQTT
    client.disconnect();

    // mDNS
    // MDNS.end();

    // Wifi
    WiFi.disconnect(true);
}

void app_network_loop()
{
    if (client.connected())
    {
        client.loop();
    }
}

bool app_network_send(app_msg_type_t kind, const uint8_t *const data, uint16_t data_len, int seqId, bool retain)
{
    char acTopic[128];

    // check kind
    if (kind >= MAX_APP_MSG_TYPE)
    {
        ESP_LOGE(TAG, "Invalid message type\n");
        return false;
    }

    // check connection status
    if (client.connected() == false)
    {
        ESP_LOGE(TAG, "MQTT client is not connected\n");
        return false;
    }

    // publish message
    // device initializtion done
    CONSTRUCT_MQTT_PUB_TOPIC(acTopic, appConfig.device_id, kind, seqId);
    ESP_LOGI(TAG, "** Publishing to topic: %s\n", acTopic);
    if (client.publish(acTopic, data, data_len, retain) == false)
    {
        ESP_LOGE(TAG, "Failed to publish message\n");
        return false;
    }

    return true;
}

bool app_network_send_large(app_msg_type_t kind, const uint8_t *const data, unsigned int data_len, int seqId, bool retain)
{
    char acTopic[128];

    // check kind
    if (kind >= MAX_APP_MSG_TYPE)
    {
        ESP_LOGE(TAG, "Invalid message type\n");
        return false;
    }

    // check connection status
    if (client.connected() == false)
    {
        ESP_LOGE(TAG, "MQTT client is not connected\n");
        return false;
    }

    // publish message
    // device initializtion done
    CONSTRUCT_MQTT_PUB_TOPIC(acTopic, appConfig.device_id, kind, seqId);
    ESP_LOGI(TAG, "** Publishing to topic: %s\n", acTopic);
    if (client.beginPublish(acTopic, data_len, retain) == false)
    {
        ESP_LOGE(TAG, "Failed to begin publish message\n");
        return false;
    }
    const int chunkSize = 1024 - 128;
    for (int i = 0; i < data_len; i += chunkSize)
    {
        int len = chunkSize;
        if (i + len > data_len)
        {
            len = data_len - i;
        }
        if (client.write(data + i, len) == false)
        {
            ESP_LOGE(TAG, "Failed to write message(%d/%d)\n", i, data_len);
            return false;
        }
        app_network_loop();
        delay(100);
    }
    // remain data
    if (data_len % chunkSize != 0)
    {
        if (client.write(data + data_len - (data_len % chunkSize), data_len % chunkSize) == false)
        {
            ESP_LOGE(TAG, "Failed to write message\n");
            return false;
        }
    }
    if (client.endPublish() == false)
    {
        ESP_LOGE(TAG, "Failed to end publish message\n");
        return false;
    }

    return true;
}

bool app_network_reconnect()
{
    int retry = 0;
    const int retry_count = 30;
    Serial.println("Reconnecting to MQTT...");
    while (!client.connected())
    {
        // Wifi
        WiFi.mode(WIFI_STA);
        WiFi.begin(APP_WIFI_SSID, APP_WIFI_PASS);
        Serial.print("\nWifi Connecting");
        int retry = 0;
        const int retry_count = 30;
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
            retry++;
            if (retry > retry_count)
            {
                Serial.println("\nWiFi Connection Failed\n");
                return false;
            }
        }
        WiFi.setTxPower(WIFI_POWER_19_5dBm);
        Serial.println("\n WiFi connected");

        // MQTT
        Serial.print("Connecting to MQTT...");
        if (client.connect(appConfig.device_id))
        {
            Serial.println("connected");
            break;
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            retry++;
            if (retry > retry_count)
            {
                Serial.println("MQTT Connection Failed");
                return false;
            }
            delay(5000);
        }
    }

    return client.connected();
}