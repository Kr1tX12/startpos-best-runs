#pragma once
#include <string>
#include "../types/types.hpp"
#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace Utils {
    std::string getLevelID(GJGameLevel* level);
    std::filesystem::path getRunsPath(const std::string& levelID);
    float getLevelPoints(const std::vector<ConvertedRunJson>& bestRuns);
    std::string truncate(const std::string& str, size_t maxLen);
    ConvertedRunJson convertRun(const RunJson& run, float gameplayEndsAt);
    std::vector<ConvertedRunJson> convertRuns(const std::vector<RunJson>& runs, float gameplayEndsAt);
    float roundProgressValue(float progress, bool showDecimals = true);
}
