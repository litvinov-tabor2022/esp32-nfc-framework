#ifndef PORTAL_WEBSERVER_H
#define PORTAL_WEBSERVER_H

#include <vector>
#include "ESPAsyncWebServer.h"

#define CONTENT_TYPE_JSONL "application/jsonl"
#define CONTENT_TYPE_JSON "application/json"
#define DEVICE_ID_HEADER "Device-Id"

class PortalFramework;

class WebServer {
public:
    explicit WebServer(PortalFramework *framework);

    void start();
    void stop();

private:
    String statusString;

    AsyncWebServer *webServer;
    PortalFramework *framework;

    void serveCommitLogFile(AsyncWebServerRequest *request);
};

#endif //PORTAL_WEBSERVER_H
