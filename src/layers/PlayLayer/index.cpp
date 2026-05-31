#include "index.hpp"
#include <fstream>
#include <matjson.hpp>
#include "../../utils/index.hpp"

float BestPlayLayer::getActualProgress(GJBaseGameLayer* game) {
    float percent;
    if (game->m_level->m_timestamp > 0) {
        percent = (game->m_gameState.m_levelTime * 240.f) 
                / game->m_level->m_timestamp * 100.f;
    } else {
        percent = game->m_player1->getPositionX() 
                / game->m_levelLength * 100.f;
    }
    return std::clamp(percent, 0.f, 100.f);
}


void BestPlayLayer::delayedResetLevelReal() {
    m_fields->m_waitingForDelay = false;
    this->delayedResetLevel();
}

void BestPlayLayer::destroyPlayer(PlayerObject* player, GameObject* object) {
    PlayLayer::destroyPlayer(player, object);

    if (m_isPracticeMode && !m_fields->m_enableInPractice) return;
    if (m_isPlatformer) return;
    if (!player->m_isDead) return;
    if (!m_fields->m_hasRespawned) return;
    
    float actualProgress = getActualProgress(this);
    m_fields->m_currentRun.end = actualProgress;
    
    if (!m_fields->m_currentRun.start.has_value())
        return;
    
    float start = m_fields->m_currentRun.start.value();
    float end = m_fields->m_currentRun.end;
    
    RunUpdateResult runUpdateResult = RunsManager::get().updateRun(start, end);
    
    int minProgress = m_fields->m_minProgress;
    
    if (runUpdateResult == RunUpdateResult::NoChanges )
        return;
    
    RunsManager::get().saveRuns();

    if (runUpdateResult == RunUpdateResult::Repeated || start == 0 || end - start < minProgress)
        return;

    bool autoRetry = GameManager::get()->getGameVariable("0026");

    if (!autoRetry) {
        return;
    }

    m_fields->m_waitingForDelay = true;
    auto seq = CCSequence::create(
        CCDelayTime::create(m_fields->m_animationDuration + 0.45f),
        CCCallFunc::create(this, callfunc_selector(BestPlayLayer::delayedResetLevelReal)),
        nullptr
    );
    runAction(seq);

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    // CONTAINER
    auto container = CCNode::create();
    m_fields->m_activeBestNode = container;
    container->setScale(0);
    container->setPosition(winSize / 2);
    this->addChild(container, 10);
    this->updateLayout();

    RunJson run = { start, end, 1 };
    ConvertedRunJson convertedRun;

    if (m_fields->m_convertNewBestPopup) {
        convertedRun = Utils::convertRun(run, RunsManager::get().getLevelJson().endsAt);
    }
    
    std::string text = fmt::format(
        fmt::runtime(m_fields->m_labelTemplate),
        fmt::arg("start", std::floor(m_fields->m_convertNewBestPopup ? convertedRun.convertedStart : run.start)),
        fmt::arg("end", std::floor(m_fields->m_convertNewBestPopup ? convertedRun.convertedEnd : run.end))
    );

    // TEXT
    auto label = CCLabelBMFont::create(
        text.c_str(),
        "bigFont.fnt"
    );
    
    label->setPosition({0, -15});

    container->addChild(label);
    
    // SPRITE
    auto newBest = CCSprite::createWithSpriteFrameName("GJ_newBest_001.png");
    newBest->setPosition({0, 25});

    container->addChild(newBest);

    auto scaleUp = CCEaseBackOut::create(
        CCScaleTo::create(0.3f, 1.1f)
    );

    auto delay = CCDelayTime::create(m_fields->m_animationDuration);

    auto shrink = CCScaleTo::create(0.15f, 0.0f);
    auto fade = CCFadeOut::create(0.15f);

    auto bounceOut = CCSpawn::create(shrink, fade, nullptr);

    auto remove = CCCallFunc::create(container, callfunc_selector(CCNode::removeFromParent));

    auto sequence = CCSequence::create(
        scaleUp,
        delay,
        bounceOut,
        remove,
        nullptr
    );

    container->runAction(sequence);
}


void BestPlayLayer::levelComplete() {
    PlayLayer::levelComplete();

    if (!m_fields->m_currentRun.start.has_value())
        return;

    if (m_isPlatformer) return;

    float start = m_fields->m_currentRun.start.value();
    float end = 100.f;

    if (start < 1) return;

    auto levelID = Utils::getLevelID(m_level);
    
    RunsManager::get().updateRun(start, end);
    RunsManager::get().saveRuns();
}

void BestPlayLayer::resetLevel() {
    if (m_isPracticeMode && !m_fields->m_enableInPractice) {
        PlayLayer::resetLevel();
        return;
    }
    if (m_isPlatformer) {
        PlayLayer::resetLevel();
        return;
    };

    if (m_fields->m_waitingForDelay) {
        return;
    }

    PlayLayer::resetLevel();

    m_fields->m_hasRespawned = true;

    if (auto node = m_fields->m_activeBestNode) {
        if (node->getParent()) {
            node->stopAllActions();
            node->removeFromParent();
        }
        m_fields->m_activeBestNode = nullptr;
    }
    
    
    float actualProgress = getActualProgress(this);
    m_fields->m_currentRun.start = actualProgress;
}

bool BestPlayLayer::init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
    if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
    if (m_isPlatformer) return true;

    m_fields->m_minProgress = Mod::get()->getSettingValue<int>("min-progress");
    m_fields->m_labelTemplate = Mod::get()->getSettingValue<std::string>("display-format");
    m_fields->m_animationDuration = Mod::get()->getSettingValue<float>("popup-duration");
    m_fields->m_enableInPractice = Mod::get()->getSettingValue<bool>("enable-in-practice");
    m_fields->m_convertNewBestPopup = Mod::get()->getSettingValue<bool>("convert-new-best-popup");

    RunsManager::get().init(Utils::getLevelID(level));
    
    return true;
}

void BestPlayLayer::onQuit() {
    PlayLayer::onQuit();

    RunsManager::get().clear();
}