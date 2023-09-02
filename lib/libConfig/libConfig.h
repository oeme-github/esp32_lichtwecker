#define WB_TITLE "Lichtwecker"
#define FIRMWARE_VERSION "v4.1.0"
#define WITH_SPIFFS
//#define WITH_SD
#define LW_CONFIG_FILE "/config_lw.map"
#define WIFI_CONFIG_FILE "/config_wifi.map"
#define WS_CONFIG_FILE "/config_ws.map"
#define MQTT_CONFIG_FILE "/config_mqtt.map"

#ifndef SWS_RX
#define SWS_RX 18
#endif
#ifndef SWS_TX
#define SWS_TX 19
#endif
#ifndef GPIO_MP3
#define GPIO_MP3 34
#endif
