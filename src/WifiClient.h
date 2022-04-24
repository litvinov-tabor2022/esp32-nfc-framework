#ifndef ESP32_NFC_FRAMEWORK_WIFICLIENT_H
#define ESP32_NFC_FRAMEWORK_WIFICLIENT_H


class WifiClient {
public:
    static bool connect(const char* ssid, const char* password, u32 timeoutMs);
    static bool disconnect();
};


#endif //ESP32_NFC_FRAMEWORK_WIFICLIENT_H
