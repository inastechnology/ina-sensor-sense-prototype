#pragma once

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include "hal_config.h"
#include "app_def.h"
#include "app_utils.h"

#define APP_CONFIG_MAGIC 0x011AD1CE
#define DEVICE_ID_PREFIX "INADS-"
#define APP_UNIQUE_ID_SIZE (36)
#define DEVICE_ID_LEN (sizeof(DEVICE_ID_PREFIX) - 1 + APP_UNIQUE_ID_SIZE + 1)

#pragma pack(push, 1)
class AppConfig
{
public:
    // Magic number
    uint32_t magic = APP_CONFIG_MAGIC;
    // Device ID e.g. INADS-{PICO_UNIQUE_BOARD_ID}
    char device_id[DEVICE_ID_LEN];
    // SSID
    char ssid[256];
    // Password
    char password[256];
    // MQTT Broker
    char mqtt_broker[256];
    // MQTT Port
    uint16_t mqtt_port;
    // MQTT Username
    char mqtt_username[256];
    // MQTT Password
    char mqtt_password[256];
    // CRC32
    uint32_t crc32;

    AppConfig()
    {
        memset(this, 0, sizeof(AppConfig));
    }

    /// @brief Initialize the configuration
    /// @note Please call this function before using the configuration
    /// @return void
    void init()
    {
        // load the configuration from flash
        if (false == load())
        {
            Serial.printf("Configuration has not been initialized.\n");
            // if not, set the default values
            set_default();

            // save the configuration to flash
            save();
        }
        show();
    }

    /// @brief Set the default values
    /// @return void
    void set_default()
    {
        // initialize memory
        memset(this, 0, sizeof(AppConfig));

        // set magic number
        magic = APP_CONFIG_MAGIC;

        // write the default values
        std::string uuid = AppUtils().generate_uuid_v4();

        sprintf(device_id, "INADS-%s", uuid.c_str());
        Serial.printf("Generate Device ID: %s\n", device_id);

        strcpy(ssid, APP_WIFI_SSID);
        Serial.printf("Set SSID: %s\n", ssid);

        strcpy(password, APP_WIFI_PASS);
        Serial.printf("Set Password: [REDACTED]\n");

        // MQTT Broker
        strcpy(mqtt_broker, APP_MQTT_BROKER_ADDR);
        Serial.printf("Set MQTT Broker: %s\n", mqtt_broker);

        // MQTT Port
        mqtt_port = APP_MQTT_BROKER_PORT;
        Serial.printf("Set MQTT Port: %d\n", mqtt_port);

        // MQTT Username
        strcpy(mqtt_username, "");
        Serial.printf("Set MQTT Username: %s\n", mqtt_username);

        // MQTT Password
        strcpy(mqtt_password, "");
        Serial.printf("Set MQTT Password: [REDACTED]\n");

        // set crc32
        crc32 = AppUtils().crc32((const uint8_t *)this, sizeof(AppConfig) - sizeof(uint32_t));
    }

    /// @brief Save the configuration to rom
    /// @return void
    void save()
    {
        HAL_config_save((const uint8_t *)this, sizeof(AppConfig));
    }

    /// @brief Load the configuration from rom
    /// @return bool - true if the configuration has been initialized
    bool load()
    {
        return HAL_config_load((uint8_t *)this, sizeof(AppConfig));
    }

    bool validate()
    {
        // validate the configuration
        if (strlen(device_id) == 0)
        {
            return false;
        }

        if (strlen(ssid) == 0)
        {
            return false;
        }

        if (strlen(password) == 0)
        {
            return false;
        }

        if (strlen(mqtt_broker) == 0)
        {
            return false;
        }

        if (mqtt_port == 0)
        {
            return false;
        }

        if (strlen(mqtt_username) == 0)
        {
            return false;
        }

        if (strlen(mqtt_password) == 0)
        {
            return false;
        }

        // validate the CRC32
        if (crc32 != AppUtils().crc32((const uint8_t *)this, sizeof(AppConfig) - sizeof(uint32_t)))
        {
            return false;
        }

        return true;
    }

    /// @brief Show the configuration
    /// @return void
    void show()
    {
        Serial.printf("Device ID: %s\n", device_id);
        Serial.printf("SSID: %s\n", ssid);
        Serial.printf("Password: [REDACTED]\n");
        Serial.printf("MQTT Broker: %s\n", mqtt_broker);
        Serial.printf("MQTT Port: %d\n", mqtt_port);
        Serial.printf("MQTT Username: %s\n", mqtt_username);
        Serial.printf("MQTT Password: [REDACTED]\n");
        Serial.printf("CRC32: 0x%08X\n", crc32);
    }
};

extern AppConfig appConfig;
