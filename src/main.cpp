#include <Arduino.h>
#include "LittleFS.h"
#include "app_config.h"
#include "app_camera.h"
#include "app_audio.h"
#include "app_network.h"
#include "app_sensor.h"
#include "app_utils.h"

#define TAG "main"

void core0_loop(void *arg);

void setup()
{
    Serial.begin(115200);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    ESP_LOGI(TAG, "Start");

    // LittleFS
    Serial.println("Mounting LittleFS...");
    if (!LittleFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    else
    {
        Serial.println("LittleFS mounted successfully");
    }

    // config
    Serial.println("Load Config...");
    appConfig.init();

    // sensor
    app_sensor_init();

    // network
    app_network_start();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // LED Off

    // show files in LittleFS
    Serial.println("Files in LittleFS:");
    {
        File root = LittleFS.open("/");
        File file = root.openNextFile();
        while (file)
        {
            Serial.print("FILE: ");
            Serial.println(file.name());
            file.close();
            file = root.openNextFile();
        }
    }

    File file = LittleFS.open("/text.txt");
    if (!file)
    {
        Serial.println("File open failed");
    }
    else
    {
        Serial.println("File open success");
        String read_text = file.readString();
        Serial.println("Read text: " + read_text);
        // check is readabled
        String text_txt = "hello, world!!!!!!!!!!";
        if (read_text != text_txt)
        {
            Serial.println("!!!! File read failed:");
            Serial.println("Expected: " + text_txt);
            Serial.println("Actual: " + read_text);
        }
        file.close();
    }

    app_audio_init();
    app_camera_init();
}

void loop()
{
    uint32_t seqId = esp_random() & 0x3FF; // 10bit
    int retry = 0;
    const int retry_count = 3;
    time_t task_start_time = time(NULL);
    if (app_network_is_connected() == false)
    {
        // reconnect
        if (!app_network_reconnect())
        {
            ESP_LOGE(TAG, "Failed to reconnect\nRestarting...");
            // reboot
            ESP.restart();
        }
    }
    app_network_loop();

    // sensor
    CALL_WITH_RETRY(app_sensor_report(seqId), retry);

    // camera
    CALL_WITH_RETRY(app_camera_report(seqId), retry);

    // audio
    // CALL_WITH_RETRY(app_audio_report(seqId), retry);

    seqId = esp_random() & 0x3FF; // 10bit

    // sleep
    int sleep_sec = APP_REPORT_INTERVAL_SEC - (time(NULL) - task_start_time);
    if (sleep_sec > 0)
    {
        // disconnect
        app_network_stop();

        // recalculate sleep time
        sleep_sec = APP_REPORT_INTERVAL_SEC - (time(NULL) - task_start_time);
        if (sleep_sec > 0)
        {
            Serial.printf("Sleep %d sec\n", sleep_sec);
            Serial.flush();
            // deep sleep
            esp_deep_sleep(sleep_sec * 1000000);
        }
    }
    else
    {
        Serial.println("Task duration is too long");
    }
}

void core0_loop(void *arg)
{
    while (1)
    {
        delay(1);
    }
}