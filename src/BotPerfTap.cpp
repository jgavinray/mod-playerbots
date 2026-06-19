/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "BotPerfTap.h"

#include "Log.h"
#include "PlayerbotAIConfig.h"

#include <algorithm>

void BotPerfTap::RecordBotAiTime(uint32 mapId, std::chrono::microseconds elapsed)
{
    if (!sPlayerbotAIConfig->perfDumpEnabled)
        return;

    uint64 micros = static_cast<uint64>(elapsed.count());

    std::lock_guard<std::mutex> guard(_lock);
    MapTiming& timing = _timings[mapId];
    timing.totalMicros += micros;
    timing.minMicros = timing.count ? std::min(timing.minMicros, micros) : micros;
    timing.maxMicros = std::max(timing.maxMicros, micros);
    ++timing.count;
}

void BotPerfTap::MaybeDump()
{
    if (!sPlayerbotAIConfig->perfDumpEnabled)
        return;

    std::map<uint32, MapTiming> snapshot;
    {
        std::lock_guard<std::mutex> guard(_lock);
        auto const now = std::chrono::steady_clock::now();
        if (now - _lastDump < std::chrono::seconds(10))
            return;

        _lastDump = now;
        snapshot.swap(_timings);
    }

    for (auto const& [mapId, timing] : snapshot)
    {
        if (!timing.count)
            continue;

        uint64 averageMicros = timing.totalMicros / timing.count;
        LOG_INFO("playerbots",
                 "PlayerbotPerf map={} count={} total_us={} avg_us={} min_us={} max_us={}",
                 mapId, timing.count, timing.totalMicros, averageMicros, timing.minMicros, timing.maxMicros);
    }
}

void BotPerfTap::Reset()
{
    std::lock_guard<std::mutex> guard(_lock);
    _timings.clear();
    _lastDump = std::chrono::steady_clock::now();
}
