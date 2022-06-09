#ifndef PORTAL_MAIN_OTAUPDATER_H
#define PORTAL_MAIN_OTAUPDATER_H

#include "ESPAsyncWebServer.h"
#include "Update.h"

class OtaUpdater{
public:
    static void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final, int cmd = U_FLASH) {
        if (!index){
            Serial.println("Update");
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
                Update.printError(Serial);
            }
        }

        if (Update.write(data, len) != len) {
            Update.printError(Serial);
        }

        if (final) {
            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
            response->addHeader("Refresh", "20");
            response->addHeader("Location", "/");
            request->send(response);
            if (!Update.end(true)){
                Update.printError(Serial);
            } else {
                Serial.println("Update complete");
                Serial.flush();
                ESP.restart();
            }
        }
    }
};

#endif //PORTAL_MAIN_OTAUPDATER_H
