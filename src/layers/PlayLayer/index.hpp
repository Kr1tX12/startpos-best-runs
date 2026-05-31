#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../types/types.hpp"
#include "../../managers/RunsManager/index.hpp"

using namespace geode::prelude;

class $modify(BestPlayLayer, PlayLayer) {
    struct Fields {
        bool m_hasRespawned = true;
        Run m_currentRun = Run();
        int m_minProgress;
        std::string m_labelTemplate;
        float m_animationDuration;
        bool m_enableInPractice;
        bool m_convertNewBestPopup;
        CCNode* m_activeBestNode;

        bool m_waitingForDelay;
    };
    void destroyPlayer(PlayerObject* player, GameObject* object);
    void resetLevel();
    float getActualProgress(GJBaseGameLayer* game);
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects);
    void levelComplete();
    void delayedResetLevelReal();
    void onQuit();
};