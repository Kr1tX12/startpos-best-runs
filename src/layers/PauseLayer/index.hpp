#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "../../popups/BestRunsPopup/index.hpp"

using namespace geode::prelude;

class $modify(BestPauseLayer, PauseLayer) {
    void customSetup();
    void onClick(CCObject* sender);
};