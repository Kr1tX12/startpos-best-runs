#include "index.hpp"

bool BestRunsPopup::init(GJGameLevel* level) {
    this->m_level = level;
    float padding = 10;
    const float width = 200;
    const float height = 250;

    if (!Popup::init(width, height, "best-runs-popup-bg.png"_spr)) return false;

    auto title = CCLabelBMFont::create(Utils::truncate(level->m_levelName, 12).c_str(), "bigFont.fnt");
    title->setPosition({ padding, height - padding - 15});
    title->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    title->setAnchorPoint({ 0, 0.5f });
    title->setScale(0.35f);

    auto settingsSprite = CCSprite::create("settings.png"_spr);
    settingsSprite->setScale(0.3f);
    settingsSprite->setColor(ccColor3B(100, 100, 100));

    auto settingsButton = CCMenuItemSpriteExtra::create(
        settingsSprite,
        this,
        menu_selector(BestRunsPopup::onSettingsClick)
    );
    
    settingsButton->setPosition({ padding + (title->getContentWidth() * title->getScale()) + 6, height - padding - 15});

    auto settingsMenu = CCMenu::create();
    settingsMenu->setPosition(0, 0);
    settingsMenu->addChild(settingsButton);

    m_mainLayer->addChild(title);
    m_mainLayer->addChild(settingsMenu);

    std::string levelID = Utils::getLevelID(level);

    LevelJson levelJson = RunsManager::get().getLevelJson();

    m_runsList = RunsListLayer::create(width, height, padding, 
        [this](float start) {
            this->onEditClick(start);
        }
    );
    m_runsList->setPosition({0, 0});
    m_runsList->setRuns(m_sort);

    m_mainLayer->addChild(m_runsList);

    auto sortButton = SortButtonLayer::create(m_sort, [this](RunsSort sort) { m_sort = sort; this->reloadRuns(); });
    auto buttonSprite = sortButton->getChildByIndex(0)->getChildByIndex(0)->getChildByIndex(0);

    sortButton->setPosition(width - padding - buttonSprite->getContentWidth() / 2 * 0.4f, height - padding - 15);
    sortButton->setScale(0.4f);
    sortButton->setAnchorPoint({ 0, 0 });

    m_mainLayer->addChild(sortButton);
    m_closeBtn->setScale(0.6f);
    m_closeBtn->m_baseScale = 0.6f;

    m_pointsLabel = PointsLabelLayer::create();
    m_pointsLabel->setPosition(padding, height - 20);
    m_pointsLabel->setScale(0.2f);
    m_pointsLabel->setAnchorPoint({0, 0});

    m_mainLayer->addChild(m_pointsLabel);

    return true;
}

BestRunsPopup* BestRunsPopup::create(GJGameLevel* level) {
    auto ret = new BestRunsPopup();
    if (ret->init(level)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void BestRunsPopup::onEditClick(float start) {
    RunEditPopup::create(start, [this]() { this->reloadRuns(); })->show();
}

void BestRunsPopup::reloadRuns() {
    std::string levelID = Utils::getLevelID(m_level);
    
    LevelJson& levelJson = RunsManager::get().getLevelJson();

    m_runsList->setRuns(m_sort);
    m_pointsLabel->updateLabel();
}

void BestRunsPopup::onSettingsClick(CCObject* sender) {
    LevelSettingsPopup::create(m_level, [this]() { this->reloadRuns(); })->show();
}