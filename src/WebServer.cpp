#include "WebServer.h"

#include "PortalFramework.h"
#include "otaWebPage.h"
#include "Update.h"
#include "OtaUpdater.h"

void handleResourcesUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

WebServer::WebServer(PortalFramework *framework) {
    webServer = new AsyncWebServer(80);
    this->framework = framework;

    webServer->on("/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        Debug.println("Handling GET /status");
        auto resp = request->beginResponse(200, CONTENT_TYPE_JSON, statusString.c_str());
        resp->addHeader(DEVICE_ID_HEADER, this->framework->getDeviceConfig().deviceId);
        request->send(resp);
    });

    webServer->on("/transactions", HTTP_GET, [this](AsyncWebServerRequest *request) {
        Debug.println("Handling GET /transactions");
        serveCommitLogFile(request);
    });

    webServer->on("/transactions", HTTP_DELETE, [this](AsyncWebServerRequest *request) {
        Debug.println("Handling DELETE /transactions");
        if (this->framework->storage.deleteCommitLogFile()) {
            request->send(200, CONTENT_TYPE_JSON, R"({"status":"deleted"})");
        } else {
            request->send(500, CONTENT_TYPE_JSON, R"({"status":"Can't delete the transactions log"})");
        }
    });

    webServer->on("/time", HTTP_PUT, [this](AsyncWebServerRequest *request) {
        Debug.println("Handling PUT /time");
        this->framework->setCurrentTime(request->arg("secs").toInt());
        request->send(200, CONTENT_TYPE_JSON, R"({"status":"updated"})");
    });

    webServer->on("/update", HTTP_GET, [&](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", upload_html_gz, upload_html_gz_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });

    webServer->on("/upload/resources", HTTP_POST, [](AsyncWebServerRequest *request) {
        Debug.println("Handling POST /upload/resources");
        request->send(200);
    }, handleResourcesUpload);

    webServer->on("/upload/flash", HTTP_POST,
                  [](AsyncWebServerRequest *request) {},
                  [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
                     size_t len, bool final) { OtaUpdater::handleDoUpdate(request, filename, index, data, len, final, U_FLASH); }
    );

    webServer->on("/upload/spiffs", HTTP_POST,
                  [](AsyncWebServerRequest *request) {},
                  [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
                     size_t len, bool final) { OtaUpdater::handleDoUpdate(request, filename, index, data, len, final, U_SPIFFS); }
    );
}

void WebServer::serveCommitLogFile(AsyncWebServerRequest *request) {
    this->framework->storage.withCommitLogFile([request](File file) mutable {
        Debug.printf("Serving commit log file, size %d bytes\n", file.size());

        AsyncWebServerResponse *response = request->beginResponse(
                CONTENT_TYPE_JSONL,
                file.size(),
                [file](uint8_t *buffer, size_t maxLen, size_t total) mutable -> size_t {
                    const int bytes = file.read(buffer, maxLen);

                    // close file at the end
                    if (bytes + total == file.size()) file.close();

                    return max(0, bytes); // return 0 even when no bytes were loaded
                }
        );

        response->addHeader(F("Access-Control-Allow-Origin"), F("*"));
        request->send(response);
    });
}

void WebServer::start() {
    // this can't be in ctor... the config is not initialized yet
    this->statusString = String(R"({"status":"ok", "device_group": "tabor2022", "device_id": ")" +
                                framework->getDeviceConfig().deviceId + "\"}");

    webServer->begin();
}

void WebServer::stop() {
    webServer->end();
}

void handleResourcesUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
        // open the file on first call and store the file handle in the request object
        request->_tempFile = SPIFFS.open("/resources/" + filename, "w");
        Serial.println("Resources Upload Start: " + String(filename));
    }

    if (len) {
        // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);
    }

    if (final) {
        // close the file handle as the upload is now done
        request->_tempFile.close();
        Serial.println("Resources Upload Complete: " + String(filename) + ", size: " + String(index + len));
    }
}
