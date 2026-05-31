#pragma once

#include <Geode/Geode.hpp>

struct ConvertedRunJson {
    float start;
    float end;
    float convertedStart;
    float convertedEnd;
    int x;
};

struct RunJson {
    float start;
    float end;
    int x;
};

struct LevelJson {
    float endsAt = 100;
    std::vector<RunJson> runs;
};

struct Run {
    std::optional<float> start;
    float end = 0;
};