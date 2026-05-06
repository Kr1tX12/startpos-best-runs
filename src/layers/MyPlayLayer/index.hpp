#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../types/GovnoTypes.hpp"

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool m_hasRespawned = true;
        Run currentRun = Run();
        std::vector<Run> bestRuns;

        int minProgress;
        std::string labelTemplate;
        float animationDuration;
        std::filesystem::path soundPath;
        CCNode* activeBestNode;

        bool waitingForDelay;
    };
    void destroyPlayer(PlayerObject* player, GameObject* object);
    void resetLevel();
    float getActualProgress(GJBaseGameLayer* game);
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects);
    void levelComplete();
    void delayedResetLevelReal();
};