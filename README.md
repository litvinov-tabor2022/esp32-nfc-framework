# ESP32 NFC Portal Framework

__Archivation status: This repo is no longer developed nor maintained. That doesn't mean it's not working or that you couldn't
use/adopt it.__

_Context:  
Every year, we create some kind of gadgets to support the game on scouts summer camp. This year, the game is about playing characters from
an ancient Czech RPG game called Brány Skeldalu._

This is a framework/library to support easy implementation of devices related to the summer camp game.  
In the game, there are real persons (players) and each one has a virtual character assigned. Skills and stats of these characters are stored
in so-called _amulets_, which are in-fact NFC tags, adjusted to some nice form. The _amulets_ are wore by the players all the time and there
are several devices which interact with the _amulets_ during some occasions (e.g. the character is getting some points as a reward for
success or the player wants to turn those points into the stats).

While the devices differ in size or form in general, the core is the same for all of them - they are based on the ESP32 chip, which is
coupled with an NFC reader & RTC module.  
Every (write) operation with the chip is logged into "transaction log" and the log is persisted in the device. Once a day, all the devices
are switched to "service mode" so they connect (over the Wi-Fi) to a central point (represented by a mobile phone
with [related app](https://github.com/litvinov-tabor2022/android-app)) and their data are synchronized, which (at that moment) means the
data on all _amulets_ are also stored in a "central database" (which enables both _amulets_ recovery in case it's lost & overview of the
characters by the org team). The data on NFC tags are stored as serialized GPB, with some consistency check to prevent errors. Transaction
logs are JSONL file in SPIFFS and are served over an HTTP server, so the app easily fetches them. Part of the synchronization is also
setting current time for all the devices, which is crucial for the transaction logs.

Everything described in the previous paragraph is provided by this single framework. The only requirement on the device is unified HW and
its wiring, everything else is handled by initializing `PortalFramework` class in the final app,
like [here](https://github.com/litvinov-tabor2022/portal-main/blob/main/src/main.cpp#L29). Once this is initialized, its functions can be
used by the rest of the device-specific application.

The devices, implementing this framework, are:

- [Main Portal](https://github.com/litvinov-tabor2022/portal-main) - device for altering the _amulets_ by org team when the player learns a
  new skill or something.
- [Lite Portal](https://github.com/litvinov-tabor2022/portal-lite) - device for quickly rewarding the players with bonus points. It's
  designed for a "quick action" while the whole camp is gathered.
- [Kiosk](https://github.com/litvinov-tabor2022/kiosk) - big device with 7" touchscreen, to be manipulated by the players by themselves.
  Basic functionality is to change their free bonus points to stats or to view already achieved skills.

Each of the device has its own 3D printing model alongside the code. The models are made in the [OpenSCAD](https://openscad.org) format.
