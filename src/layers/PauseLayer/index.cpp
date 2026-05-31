#include "index.hpp"

void BestPauseLayer::customSetup() {
    PauseLayer::customSetup();

    // if (PlayLayer::get()->m_level->isPlatformer()) return;

    auto icon = CCSprite::create("icon.png"_spr);
    icon->setScale(0.28f);

    auto button = CCMenuItemSpriteExtra::create(
        icon,
        this,
        menu_selector(BestPauseLayer::onClick)
    );
    
    auto menu = this->getChildByID("left-button-menu");
    menu->addChild(button);
    menu->updateLayout();
}

void BestPauseLayer::onClick(CCObject* sender) {
    GJGameLevel* level = PlayLayer::get()->m_level;
    BestRunsPopup::create(level)->show();
}