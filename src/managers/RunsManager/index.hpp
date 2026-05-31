#pragma once

#include <vector>
#include "../../types/types.hpp"
#include "../../utils/index.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

enum class RunUpdateResult {
    NoChanges,
    NewBest,
    Repeated,
    NewStartPos
};

class RunsManager {
    public:
        static RunsManager& get() {
            static RunsManager instance;
            return instance;
        }

        void init(const std::string& levelID);
        void clear();
        void loadRuns();
        void saveRuns();
        RunUpdateResult updateRun(float start, float end);
        void forceUpdateRun(float start, float end);
        void setGameplayEndsAt(float endsAt);
        
        LevelJson& getLevelJson();

    private:
        RunsManager() = default;
        bool m_initialized = false;

        std::string m_levelID;
        LevelJson m_levelJson;
};