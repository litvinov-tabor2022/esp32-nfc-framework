#include "Arduino.h"

#include "ArduinoJson.h"

#include "Storage.h"
#include "debugging.h"

bool Storage::appendTransaction(const Transaction &transaction) {
    Debug.printf("Appending transaction: time %llu, user_id %d [s = %d, d = %d, m = %d, bp = %d, sk = %d]\n",
                 transaction.time, transaction.user_id, transaction.strength, transaction.dexterity, transaction.magic,
                 transaction.bonus_points, transaction.skill);

    StaticJsonDocument<256> doc;

    doc["time"] = transaction.time;
    doc["device_id"] = transaction.device_id;
    doc["user_id"] = transaction.user_id;

    if (transaction.strength != 0) {
        doc["strength"] = transaction.strength;
    }
    if (transaction.dexterity != 0) {
        doc["dexterity"] = transaction.dexterity;
    }
    if (transaction.magic != 0) {
        doc["magic"] = transaction.magic;
    }
    if (transaction.bonus_points != 0) {
        doc["bonus_points"] = transaction.bonus_points;
    }
    if (transaction.skill != 0) {
        doc["skill"] = transaction.skill;
    }

    File file = SPIFFS.open(COMMITLOG_FILE, FILE_APPEND);

    if (!file) {
        Debug.println("Failed to open commit log file for appending");
        return false;
    }

    if (!serializeJson(doc, file)) {
        Debug.println("No data were written to commit log!");
        return false;
    }

    if (!file.println()) {
        Debug.println("Writing to commit log failed!");
        return false;
    }

    file.close();

    return true;
}

bool Storage::begin() {

    if (!SPIFFS.begin(false)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return false;
    }

    if (!ensureCommitLogFileExists()) {
        Debug.println("Can't access the commit log file!");
        return false;
    }

    return true;
}

void Storage::readFile(const char *path) {
    Debug.printf("Reading file: %s\n", path);

    File file = SPIFFS.open(path);
    if (!file) {
        Debug.println("Failed to open file for reading");
        return;
    }

    Debug.print("Read from file: ");
    while (file.available()) {
        Serial.write(file.read());
    }
}

bool Storage::writeFile(const char *path, const char *message) {
    Debug.printf("Writing file: %s\n", path);

    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file) {
        Debug.println("Failed to open file for writing");
        return false;
    }

    return file.print(message);
}

bool Storage::appendFile(const char *path, const char *message) {
    Debug.printf("Appending to file: %s\n", path);

    File file = SPIFFS.open(path, FILE_APPEND);
    if (!file) {
        Debug.println("Failed to open file for appending");
        return false;
    }

    return file.print(message);
}

bool Storage::deleteFile(const char *path) {
    Debug.printf("Deleting file: %s\n", path);
    return SPIFFS.remove(path);
}

bool Storage::exists(const char *path) {
    return SPIFFS.exists(path);
}

void Storage::withCommitLogFile(const std::function<void(File)> &op) {

    if (!ensureCommitLogFileExists()) {
        Debug.println("Can't access the commit log file!");
        return;
    }

    File file = SPIFFS.open(COMMITLOG_FILE);
    if (!file) {
        Debug.println("Could not load commit log file!");
        return;
    }

    op(file);
}

bool Storage::deleteCommitLogFile() {

    return deleteFile(COMMITLOG_FILE);
}

bool Storage::ensureCommitLogFileExists() {
    if (!exists(COMMITLOG_FILE)) {
        Debug.println("Creating commit log file");
        if (!SPIFFS.open(COMMITLOG_FILE, FILE_WRITE)) return false;
    }

    return true;
}

DynamicJsonDocument *Storage::loadJsonFile(const char *path) {

    if (!exists(path)) {
        Debug.printf("Requested path %s doesn't exist!\n", path);
        return nullptr;
    }

    File file = SPIFFS.open(path, FILE_READ);

    if (!file) {
        Debug.printf("Couldn't load requested file %s!\n", path);
        return nullptr;
    }

    auto *doc = new DynamicJsonDocument(file.size() * 2);

    const DeserializationError err = deserializeJson(*doc, file);
    if (err) {
        Debug.printf("ERR loading JSON file %s: '%s'\n", path, err.c_str());
        return nullptr;
    }

    return doc;
}

