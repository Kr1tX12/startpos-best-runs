#include "index.hpp"

void BestEndLevelLayer::customSetup() {
    EndLevelLayer::customSetup();
    
    if (PlayLayer::get()->m_level->isPlatformer()) return;

    auto icon = CCSprite::create("icon.png"_spr);
    icon->setScale(0.28f);

    auto button = CCMenuItemSpriteExtra::create(
        icon,
        this,
        menu_selector(BestEndLevelLayer::onClick)
    );
    
    auto menu = this->getChildByID("hide-layer-menu");
    menu->addChild(button);
    menu->updateLayout();
}

void BestEndLevelLayer::onClick(CCObject* sender) {
    GJGameLevel* level = PlayLayer::get()->m_level;
    BestRunsPopup::create(level)->show();
}