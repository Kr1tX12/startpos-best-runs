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


        // speedhack detection
        bool m_speedhackDetected = false;
        float m_currentTimeWarp = 1;

        std::optional<std::chrono::steady_clock::time_point> m_speedhackCompare;

        std::deque<double> m_realTimeHistory;
        std::deque<double> m_gameTimeHistory;

        double m_rollingRealSum = 0.0;
        double m_rollingGameSum = 0.0;

        bool m_ignoreSpeedhackRuns = true;


        // for noclip detection
        bool m_noclipDetected = false;
        GameObject* m_lastDeathObject = nullptr;

        bool m_ignoreNoclipRuns = true;
    };

    static void onModify(auto& self);
    void destroyPlayer(PlayerObject* player, GameObject* object);
    void resetLevel();
    float getActualProgress(GJBaseGameLayer* game);
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects);
    void levelComplete();
    void delayedResetLevelReal();
    void onQuit();
    void postUpdate(float dt);
    void checkDelta(float dt);
    void updateTimeWarp(float timeWarp);
};