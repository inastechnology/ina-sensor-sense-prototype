#pragma once

#include "esp_err.h"

typedef enum
{
    APP_MSG_TYPE_TEST = 0,
    APP_MSG_TYPE_STATUS,
    APP_MSG_TYPE_IMAGE,
    APP_MSG_TYPE_AUDIO,
    MAX_APP_MSG_TYPE
} app_msg_type_t;

void app_network_start();
void app_network_stop();
void app_network_loop();
bool app_network_is_connected();
bool app_network_send(app_msg_type_t kind, const uint8_t *const data, uint16_t data_len, int seqId = 0, bool retain = false);
bool app_network_send_large(app_msg_type_t kind, const uint8_t *const data, unsigned int data_len, int seqId = 0, bool retain = false);
bool app_network_reconnect();
