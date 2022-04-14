#include <Resources.h>
#include <debugging.h>
#include <ArduinoJson.h>

#define RESOURCES_DEBUG false

bool Resources::begin(Storage *storage) {
    Debug.println("Loading resources...");

    if (!loadNames(storage)) {
        Debug.println("Could not load names resource!");
        return false;
    }

    if (!loadPriceList(storage)) {
        Debug.println("Could not load skills resource!");
        return false;
    }

    return true;
}

bool Resources::loadPriceList(Storage *storage) {
    const auto doc = storage->loadJsonFile(PATH_PRICELIST);
    if (doc == nullptr) {
        Debug.println("Could not load names resource");
        return false;
    }

    const auto array = doc->as<JsonArray>();

    for (const auto row: array) {
        const PriceListEntry entry = PriceListEntry{
                .code =  row["code"],
                .name =  row["name"],
                .constraints = PriceListEntryConstraints{
                        .strength =  row["strength"],
                        .magic = row["magic"],
                        .dexterity = row["dexterity"],
                },
                .skill =  row["skill"],
        };

        if (RESOURCES_DEBUG) {
            Debug.printf("Loaded priceList entry: code=%s name=%s skill=%d (strength=%d magic=%d dexterity=%d)\n",
                         entry.code.c_str(), entry.name.c_str(), entry.skill, entry.constraints.strength, entry.constraints.magic,
                         entry.constraints.dexterity);
        }

        this->priceList.insert(std::make_pair(entry.code, entry));
    }

    return true;
}

bool Resources::loadNames(Storage *storage) {
    const auto doc = storage->loadJsonFile(PATH_NAMES);
    if (doc == nullptr) {
        Debug.println("Could not load names resource");
        return false;
    }

    const auto array = doc->as<JsonArray>();

    for (const auto row: array) {
        const u8 id = row["id"];
        const std::string name = row["name"];
        const u8 group = row["group"];

        if (RESOURCES_DEBUG) { Serial.printf("Loaded name mapping: %d -> (%s, %d)\n", id, name.c_str(), group); }
        players.insert(std::make_pair(id, PlayerMetadata{.name = name, .group = group}));
    }

    return true;
}

PlayerMetadata Resources::getPlayerMetadata(const u8 userId) {
    return players[userId];
}

std::vector<PriceListEntry> Resources::getPriceListPage(const u8 pageNo, const u8 pageSize) {
    u8 start = pageNo * pageSize;
    u8 end = start + pageSize - 1;

//    Serial.printf("BEF pageNo %d start %d end %d \n", pageNo, start, end);

    if (end > priceList.size()) {
        end = priceList.size() - 1;
        start = max(end - pageSize, 0);
    }

//    Serial.printf("AFT pageNo %d start %d end %d \n", pageNo, start, end);

    //TODO rewrite this shit 😱
    auto it = priceList.cbegin();

    std::vector<PriceListEntry> subset = std::vector<PriceListEntry>();

    for (int i = 0; i < start; i++) {
        it++;
    }

    for (int i = 0; i < pageSize; i++, it++) {
        subset.push_back(it->second);
    }

    return subset;
}

std::optional<PriceListEntry> Resources::getPriceListEntry(const String &code) {
    auto it = this->priceList.find(code);
    if (it == priceList.end()) return std::nullopt;

    return it->second;
}


