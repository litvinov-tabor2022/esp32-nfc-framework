#ifndef KIOSK_RESOURCES_H
#define KIOSK_RESOURCES_H

#include <Storage.h>
#include <types.h>
#include <map>
#include <vector>
#include <optional>

#define PATH_NAMES "/resources/names.json"
#define PATH_PRICELIST "/resources/prices.json"

struct PriceListEntryConstraints{
    u8 strength;
    u8 magic;
    u8 dexterity;
};

struct PriceListEntry {
    String code;
    std::string name;
    std::string description;
    PriceListEntryConstraints constraints;
    i16 skill;
};

struct PlayerMetadata {
    std::string name;
    u8 group;
};

class Resources {
public:
    bool begin(Storage *storage);

    PlayerMetadata getPlayerMetadata(u8 userId);
    std::vector<PriceListEntry> getPriceListPage(u8 pageNo, u8 pageSize);
    std::optional<PriceListEntry> getPriceListEntry(const String& code);

private:
    bool loadNames(Storage *storage);

    bool loadPriceList(Storage *storage);

    std::map<u8, PlayerMetadata> players;
    std::map<String, PriceListEntry> priceList;
};


#endif //KIOSK_RESOURCES_H
