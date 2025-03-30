#pragma once

// =================================================================================================
// Common Configuration
// =================================================================================================
#ifndef APP_REPORT_INTERVAL_SEC
#define APP_REPORT_INTERVAL_SEC 1200 // データ送信間隔（秒）
#endif

// =================================================================================================
// Network Configuration
// =================================================================================================
#ifndef APP_WIFI_SSID
#define APP_WIFI_SSID "********" // APの名前
#endif

#ifndef APP_WIFI_PASS
#define APP_WIFI_PASS "********" // APのパスワード（８文字以上）
#endif

#ifndef APP_INITIAL_SETTING_SSID
#define APP_INITIAL_SETTING_SSID "*********" // APの名前
#endif

#ifndef APP_INITIAL_SETTING_PASS
#define APP_INITIAL_SETTING_PASS "*********" // APのパスワード（８文字以上）
#endif

#ifndef APP_MQTT_BROKER_ADDR
#define APP_MQTT_BROKER_ADDR "my_device.local" // MQTT BrokerのIPアドレス
#endif

#ifndef APP_MQTT_BROKER_PORT
#define APP_MQTT_BROKER_PORT 1883 // MQTT Brokerのポート番号
#endif
