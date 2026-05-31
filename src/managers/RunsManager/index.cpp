#include "index.hpp"

void RunsManager::init(const std::string& levelID) {
    m_levelID = levelID;
    m_levelJson = LevelJson{};
    m_initialized = true;
    loadRuns();
}

void RunsManager::clear() {
    m_levelID = "";
    m_levelJson = LevelJson{};
    m_initialized = false;
}
        
void RunsManager::loadRuns() {
    if (!m_initialized) {
        geode::log::warn("RunsManager not initialized");
        return;
    }
    
    m_levelJson = LevelJson {};
    auto path = Utils::getRunsPath(m_levelID);

    std::ifstream file(path);
    if (!file.good()) return;

    std::stringstream buffer;
    buffer << file.rdbuf();

    auto parsed = matjson::parse(buffer.str());
    if (!parsed) return;

    auto json = parsed.unwrap();

    if (!json.isObject()) return;

    auto endsAtRes = json["endsAt"].asInt();
    m_levelJson.endsAt = endsAtRes ? endsAtRes.unwrap() / 100.f : 100.f;
    
    auto runs = json["runs"];

    if (!runs.isArray()) return;

    for (auto const& run : runs) {
        auto startRes = run["start"].asInt();
        auto endRes = run["end"].asInt();
        auto xRes = run["x"].asInt();

        if (!startRes || !endRes || !xRes)
            continue;

        RunJson runJson;
        runJson.start = startRes.unwrap() / 100.f;
        runJson.end = endRes.unwrap() / 100.f;
        runJson.x = xRes.unwrap();

        m_levelJson.runs.push_back(runJson);
    }
}

void RunsManager::saveRuns() {
    if (!m_initialized) {
        geode::log::warn("RunsManager not initialized");
        return;
    }

    auto path = Utils::getRunsPath(m_levelID);

    matjson::Value obj = matjson::Value::object();
    obj["endsAt"] = (int)std::round(m_levelJson.endsAt * 100.f);

    matjson::Value runs = matjson::Value::array();

    for (auto const& run : m_levelJson.runs) {
        matjson::Value runObj = matjson::Value::object();

        float roundedStart = Utils::roundProgressValue(run.start);
        float roundedEnd = Utils::roundProgressValue(run.end);

        runObj["start"] = (int)std::round(roundedStart * 100.f);
        runObj["end"] = (int)std::round(roundedEnd * 100.f);
        runObj["x"] = run.x;

        runs.push(runObj);
    }

    obj["runs"] = runs;

    std::ofstream out(path);
    out << obj.dump(2);
}

RunUpdateResult RunsManager::updateRun(float start, float end) {
    if (!m_initialized) {
        geode::log::warn("RunsManager not initialized");
        return RunUpdateResult::NoChanges;
    }

    float roundedStart = Utils::roundProgressValue(start);
    float roundedEnd = Utils::roundProgressValue(end);

    for (auto& run : m_levelJson.runs) {
        if (std::fabs(run.start - roundedStart) > 0.001f)
            continue;

        if (roundedEnd > run.end) {
            run.end = roundedEnd;
            run.x = 1;

            return RunUpdateResult::NewBest;
        }

        if (std::floor(end) == std::floor(run.end)) {
            run.x += 1;
            return RunUpdateResult::Repeated;
        }

        if (roundedEnd < run.end) {
            return RunUpdateResult::NoChanges;
        }

        return RunUpdateResult::NoChanges;
    }

    RunJson runJson;
    runJson.start = roundedStart;
    runJson.end = roundedEnd;
    runJson.x = 1;

    m_levelJson.runs.push_back(runJson);

    return RunUpdateResult::NewStartPos;
}


void RunsManager::forceUpdateRun(float start, float end) {
    if (!m_initialized) {
        geode::log::warn("RunsManager not initialized");
        return;
    }

    float roundedStart = Utils::roundProgressValue(start);
    float roundedEnd = Utils::roundProgressValue(end);

    for (auto& run : m_levelJson.runs) {
        if (std::fabs(run.start - roundedStart) < 0.0001f) {
            run.end = roundedEnd;
            run.x = 1;
            return;
        }
    }

    RunJson runJson;
    runJson.start = roundedStart;
    runJson.end = roundedEnd;
    runJson.x = 1;

    m_levelJson.runs.push_back(runJson);
}

LevelJson& RunsManager::getLevelJson() {
    return m_levelJson;
}

void RunsManager::setGameplayEndsAt(float endsAt) {
    if (!m_initialized) {
        geode::log::warn("RunsManager not initialized");
        return;
    }

    m_levelJson.endsAt = endsAt;

    saveRuns();
}