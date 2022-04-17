#ifndef ESP32_PORTAL_STORAGE_H
#define ESP32_PORTAL_STORAGE_H

#include "mutex"

#include "FS.h"
#include "SPIFFS.h"
#include "SPI.h"
#include "ArduinoJson.h"

#include "portal_defs.h"

#define COMMITLOG_FILE "/data/transactions.log"

class Storage {
public:
    bool begin();

    bool appendTransaction(const Transaction &transaction);

    void withCommitLogFile(const std::function<void(fs::File)> &op);

    bool deleteCommitLogFile();

    DynamicJsonDocument *loadJsonFile(const char *path);

    bool exists(const char *path);

private:
    bool ensureCommitLogFileExists();

    void readFile(const char *path);

    bool writeFile(const char *path, const char *message);

    bool appendFile(const char *path, const char *message);

    bool deleteFile(const char *path);
};


#endif //PORTAL_STORAGE_H
