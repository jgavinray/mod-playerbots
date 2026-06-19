/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_BOTPERFTAP_H
#define _PLAYERBOT_BOTPERFTAP_H

#include "Common.h"

#include <chrono>
#include <map>
#include <mutex>

class BotPerfTap
{
public:
    static BotPerfTap* instance()
    {
        static BotPerfTap instance;
        return &instance;
    }

    void RecordBotAiTime(uint32 mapId, std::chrono::microseconds elapsed);
    void MaybeDump();
    void Reset();

private:
    struct MapTiming
    {
        uint64 totalMicros = 0;
        uint64 minMicros = 0;
        uint64 maxMicros = 0;
        uint64 count = 0;
    };

    BotPerfTap() = default;

    std::mutex _lock;
    std::map<uint32, MapTiming> _timings;
    std::chrono::steady_clock::time_point _lastDump = std::chrono::steady_clock::now();
};

#define sBotPerfTap BotPerfTap::instance()

#endif
