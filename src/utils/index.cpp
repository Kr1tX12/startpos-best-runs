#include "index.hpp"

std::string Utils::getLevelID(GJGameLevel* level) {
    int id = level->m_levelID.value();

    if (id == 0) {
        return "editor_" + std::to_string(EditorIDs::getID(level));
    }

    return std::to_string(id);
}


std::filesystem::path Utils::getRunsPath(const std::string& levelID) {
    return Mod::get()->getSaveDir() / (levelID + ".json");
}


float Utils::getLevelPoints(const std::vector<ConvertedRunJson>& bestRuns) {
    int userPoints = 0; 
    for (auto& run: bestRuns) {
        userPoints += run.end - run.start;
    }

    float rounded = std::round(userPoints * 10.0) / 10.0;

    return rounded;
}

float convertPercent(float percent, float gameplayEnd) {
    float result = (percent / gameplayEnd) * 100.0f;
    return Utils::roundProgressValue(std::clamp(result, 0.f, 100.f));
}

ConvertedRunJson Utils::convertRun(const RunJson& run, float gameplayEndsAt) {
    return {
        run.start,
        run.end,
        convertPercent(run.start, gameplayEndsAt),
        convertPercent(run.end, gameplayEndsAt),
        run.x
    };
}

std::vector<ConvertedRunJson> Utils::convertRuns(const std::vector<RunJson>& runs, float gameplayEndsAt) {
    std::vector<ConvertedRunJson> result;
    result.reserve(runs.size());

    for (const auto& run : runs) {
        result.push_back(Utils::convertRun(run, gameplayEndsAt));
    }

    return result;
}

std::string Utils::trim(const std::string& s) {
    auto notSpace = [](unsigned char c) {
        return std::isspace(c);
    };

    auto start = std::find_if_not(s.begin(), s.end(), notSpace);
    auto end = std::find_if_not(s.rbegin(), s.rend(), notSpace).base();

    if (start >= end)
        return "";

    return std::string(start, end);
}

std::string Utils::truncate(const std::string& str, size_t maxLen = 10) {
    std::string trimmed = trim(str);
    if (trimmed.size() <= maxLen)
        return trimmed;

    return trim(trimmed.substr(0, maxLen)) + "...";
}

float Utils::roundProgressValue(float progress, bool showDecimals) {
    if (!showDecimals) return std::floor(progress);
    return std::round(progress * 100.0f) / 100.0f;
}

int Utils::getLevelCompletedRuns(const std::vector<ConvertedRunJson>& runs)
{
    constexpr float EPS = 1e-4f;

    auto sortedRuns = runs;

    std::sort(sortedRuns.begin(), sortedRuns.end(),
        [](const ConvertedRunJson& a, const ConvertedRunJson& b)
        {
            return a.convertedStart < b.convertedStart;
        });

    float covered = 0.0f;
    int count = 0;
    size_t i = 0;

    while (covered < 100.0f - EPS)
    {
        float bestReach = covered;

        while (i < sortedRuns.size() && sortedRuns[i].convertedStart <= covered + EPS)
        {
            bestReach = std::max(bestReach, sortedRuns[i].convertedEnd);
            ++i;
        }

        if (bestReach <= covered + EPS)
            return -1;

        covered = bestReach;
        ++count;
    }

    return count;
}