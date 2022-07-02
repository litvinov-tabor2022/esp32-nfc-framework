#include <Resources.h>
#include <debugging.h>
#include <ArduinoJson.h>

#include <utility>

#define RESOURCES_DEBUG false

PriceList *Resources::loadPriceList() {
    const auto doc = storage->loadJsonFile(PATH_PRICELIST);
    if (doc == nullptr) {
        Debug.println("Could not load prices resource");
        return nullptr;
    }

    const auto array = doc->as<JsonArray>();

    std::map<String, PriceListEntry> priceList;

    for (const auto row: array) {
        PriceListEntry entry = PriceListEntry{
                .code =  row["code"],
                .altName =  row["altName"],
                .constraints = PriceListEntryConstraints{
                        .strength =  row["strength"],
                        .magic = row["magic"],
                        .dexterity = row["dexterity"],
                },
                .skill =  row["skill"],
        };

        if (RESOURCES_DEBUG) {
            Debug.printf("Loaded priceList entry: code=%s altName=%s skill=%d (strength=%d magic=%d dexterity=%d)\n",
                         entry.code.c_str(), entry.altName.c_str(), entry.skill, entry.constraints.strength,
                         entry.constraints.magic,
                         entry.constraints.dexterity);
        }
        String code = entry.code;
        entry.code = "A" + code;
        entry.operation = ADD;
        priceList.insert(std::make_pair(entry.code, entry)); // code for adding

        entry.code = "D" + code;
        entry.operation = REMOVE;
        priceList.insert(std::make_pair(entry.code, entry)); // code for removing
    }

    return new PriceList(priceList);
}

SkillsList *Resources::loadSkillsList() {
    const auto doc = storage->loadJsonFile(PATH_PRICELIST);
    if (doc == nullptr) {
        Debug.println("Could not load skills resource");
        return nullptr;
    }

    const auto array = doc->as<JsonArray>();

    std::vector<SkillsListEntry> skillsList;

    for (const auto row: array) {
        if (row["skill"].as<i16>() <= 0) continue; // skip "remove" pricelist entries

        const SkillsListEntry entry = SkillsListEntry{
                .name =  row["name"],
                .description =  row["description"],
                .constraints = PriceListEntryConstraints{
                        .strength =  row["strength"],
                        .magic = row["magic"],
                        .dexterity = row["dexterity"],
                },
                .skill =  row["skill"],
        };

        if (RESOURCES_DEBUG) {
            Debug.printf("Loaded skillsList entry: name=%s skill=%d (strength=%d magic=%d dexterity=%d)\n",
                         entry.name.c_str(), entry.skill, entry.constraints.strength, entry.constraints.magic,
                         entry.constraints.dexterity);
        }

        skillsList.push_back(entry);
    }

    return new SkillsList(skillsList);
}

PlayersMetadata *Resources::loadPlayersMeta() {
    const auto doc = storage->loadJsonFile(PATH_NAMES);
    if (doc == nullptr) {
        Debug.println("Could not load names resource");
        return nullptr;
    }

    const auto array = doc->as<JsonArray>();

    std::map<u8, PlayerMetadata> players;

    for (const auto row: array) {
        const u8 id = row["id"];
        const std::string name = row["name"];
        const u8 group = row["group"];

        if (RESOURCES_DEBUG) { Serial.printf("Loaded name mapping: %d -> (%s, %d)\n", id, name.c_str(), group); }
        players.insert(std::make_pair(id, PlayerMetadata{.name = name, .group = group}));
    }

    return new PlayersMetadata(players);
}

std::optional<PriceListEntry> PriceList::getItemForCode(const String &code) {
    auto it = this->priceList.find(code);
    if (it == priceList.end()) return std::nullopt;

    return it->second;
}

PlayerMetadata PlayersMetadata::getPlayerMetadata(const u8 userId) {
    return players[userId];
}

SkillsIterator SkillsList::getSkillsPageStart(u8 pageNo, u8 pageSize) {
    u8 start = pageNo * pageSize;
    u8 end = start + pageSize - 1;

    if (end > skillsList.size() && skillsList.size() > pageSize) {
        end = skillsList.size() - 1;
        start = max(end - pageSize + 1, 0);
    }

    return skillsList.cbegin() + start;
}

SkillsIterator SkillsList::getAll() {
    return skillsList.cbegin();
}

SkillsList::SkillsList(std::vector<SkillsListEntry> skillsList) : skillsList(std::move(skillsList)) {}

PlayersMetadata::PlayersMetadata(std::map<u8, PlayerMetadata> players) : players(std::move(players)) {}

PriceList::PriceList(std::map<String, PriceListEntry> priceList) : priceList(std::move(priceList)) {}

Resources::Resources(Storage *storage) : storage(storage) {}

bool Resources::begin() {
    return storage->exists(PATH_NAMES) && storage->exists(PATH_PRICELIST);
}
