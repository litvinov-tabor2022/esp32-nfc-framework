#include "debugging.h"

StreamDebugger Debug(Serial, reinterpret_cast<Stream &>(TelnetPrint));
