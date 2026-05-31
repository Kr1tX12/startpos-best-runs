#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include "../../popups/BestRunsPopup/index.hpp"

using namespace geode::prelude;

class $modify(BestEndLevelLayer, EndLevelLayer) {
    void customSetup();
    void onClick(CCObject* sender);
};