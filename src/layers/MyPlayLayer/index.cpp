#include "index.hpp"
#include <fstream>
#include <matjson.hpp>


std::filesystem::path getRunsPath(const std::string& levelID) {
    return Mod::get()->getSaveDir() / ("runs_" + levelID + ".json");
}

std::string getLevelID(GJGameLevel* level) {
    int id = level->m_levelID.value();

    if (id == 0) {
        return level->m_levelName;
    }

    return std::to_string(id);
}

void loadRuns(MyPlayLayer* self, std::string levelID) {
    auto path = getRunsPath(levelID);

    std::ifstream file(path);
    if (!file.good()) return;

    std::stringstream buffer;
    buffer << file.rdbuf();

    auto parsed = matjson::parse(buffer.str());
    if (!parsed) return;

    matjson::Value runs = parsed.unwrap();

    if (!runs.isArray()) return;


    self->m_fields->bestRuns.clear();
    for (auto& r : runs) {
        Run run;
        run.start = r["start"].asDouble().unwrap();
        run.end = r["end"].asDouble().unwrap();

        self->m_fields->bestRuns.push_back(run);
    }
}

void saveRuns(MyPlayLayer* self, std::string levelID) {
    auto path = getRunsPath(levelID);

    matjson::Value arr = matjson::Value::array();

    for (auto& r : self->m_fields->bestRuns) {
        matjson::Value obj = matjson::Value::object();
        obj["start"] = r.start;
        obj["end"] = r.end;
        arr.push(obj);
    }

    std::ofstream out(path);
    out << arr.dump(2);
}

bool updateRun(int start, int end, std::vector<Run>& bestRuns) {
    bool foundFrom = false;
    bool isBest = false;
    for (auto& r : bestRuns)
        if (r.start == start) {
            foundFrom = true;
            if (r.end < end) {
                isBest = true;
                r.end = end;
            }
            break;
        }

    if (!foundFrom) {
        bestRuns.push_back({start, end});
        isBest = true;
    }

    return isBest;
}


float MyPlayLayer::getActualProgress(GJBaseGameLayer* game) {
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


void MyPlayLayer::delayedResetLevelReal() {
    m_fields->waitingForDelay = false;
    this->delayedResetLevel();
}

void MyPlayLayer::destroyPlayer(PlayerObject* player, GameObject* object) {
    PlayLayer::destroyPlayer(player, object);

    if (m_isPracticeMode && !m_fields->enableInPractice) return;
    if (m_isPlatformer) return;
    if (!player->m_isDead) return;
    if (!m_fields->m_hasRespawned) return;
    
    auto percent = this->getCurrentPercent();
    
    float actualProgress = getActualProgress(this);
    m_fields->currentRun.end = actualProgress;
    
    // log::info("[DESTROY PLAYER] Actual Progress: {}", actualProgress);
    
    if (!m_fields->currentRun.start.has_value())
        return;
    
    int start = m_fields->currentRun.start.value();
    int end = m_fields->currentRun.end;
    
    
    if (start < 1) return;
    
    
    auto levelID = getLevelID(m_level);
    
    auto& bestRuns = m_fields->bestRuns;
    
    bool isBest = updateRun(start, end, bestRuns);
    
    
    int minProgress = m_fields->minProgress;
    
    if (!isBest || end - start < minProgress)
        return;
    
    saveRuns(this, levelID);

    bool autoRetry = GameManager::get()->getGameVariable("0026");

    if (!autoRetry) {
        return;
    }

    log::info("Retrying in some time....");
    m_fields->waitingForDelay = true;
    auto seq = CCSequence::create(
        CCDelayTime::create(m_fields->animationDuration + 0.45f),
        CCCallFunc::create(this, callfunc_selector(MyPlayLayer::delayedResetLevelReal)),
        nullptr
    );
    runAction(seq);

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    // CONTAINER
    auto container = CCNode::create();
    m_fields->activeBestNode = container;
    container->setScale(0);
    container->setPosition(winSize / 2);
    this->addChild(container, 9999);
    this->updateLayout();
    

    
    std::string text = fmt::format(
        fmt::runtime(m_fields->labelTemplate),
        fmt::arg("start", start),
        fmt::arg("end", end)
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

    auto delay = CCDelayTime::create(m_fields->animationDuration);

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


void MyPlayLayer::levelComplete() {
    PlayLayer::levelComplete();

    if (!m_fields->currentRun.start.has_value())
        return;

    if (m_isPlatformer) return;

    int start = m_fields->currentRun.start.value();
    int end = 100;

    if (start < 1) return;

    auto levelID = getLevelID(m_level);
    
    auto& bestRuns = m_fields->bestRuns;

    bool isBest = updateRun(start, end, bestRuns);
    

    saveRuns(this, levelID);
}

void MyPlayLayer::resetLevel() {

    bool autoRetry = GameManager::get()->getGameVariable("0026");

    if (!autoRetry) {
        PlayLayer::resetLevel();
        return;
    }
    
    if (m_isPracticeMode && !m_fields->enableInPractice) {
        PlayLayer::resetLevel();
        return;
    }
    if (m_isPlatformer) {
        PlayLayer::resetLevel();
        return;
    };

    if (m_fields->waitingForDelay) {
        return;
    }

    PlayLayer::resetLevel();

    m_fields->m_hasRespawned = true;

    if (m_fields->activeBestNode) {
        m_fields->activeBestNode->stopAllActions();
        m_fields->activeBestNode->removeFromParent();
        m_fields->activeBestNode = nullptr;
    }
    
    
    float actualProgress = getActualProgress(this);
    m_fields->currentRun.start = actualProgress;
    // log::info("[RESET LEVEL] actual progress: {}", actualProgress);
}

bool MyPlayLayer::init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
    if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
    if (m_isPlatformer) return true;

    m_fields->minProgress = Mod::get()->getSettingValue<int>("min-progress");
    m_fields->labelTemplate = Mod::get()->getSettingValue<std::string>("display-format");
    m_fields->animationDuration = Mod::get()->getSettingValue<float>("popup-duration");
    m_fields->enableInPractice = Mod::get()->getSettingValue<bool>("enable-in-practice");
    
    m_fields->bestRuns.clear();
    loadRuns(this, getLevelID(level));
    
    return true;
}