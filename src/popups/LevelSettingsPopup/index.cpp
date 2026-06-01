#include "index.hpp"

bool LevelSettingsPopup::init(GJGameLevel* level, std::function<void()> onConfirmCallback) {
    float padding = 10;
    const float width = 200;
    const float height = 90;

    if (!Popup::init(width, height, "best-runs-popup-bg.png"_spr)) return false;

    m_levelID = Utils::getLevelID(level);
    m_onConfirmCallback = onConfirmCallback;

    auto title = CCLabelBMFont::create(Utils::truncate(fmt::format("{} Settings", level->m_levelName), 16).c_str(), "bigFont.fnt");
    title->setPosition({width / 2, height - 15});
    title->setScale(0.4f);

    float gameplayEndsAt = std::round(RunsManager::get().getLevelJson().endsAt * 100.f) / 100.f;

    auto gameplayEndsAtStr = fmt::format("{:g}", gameplayEndsAt);
    
    m_endsAtInput = TextInput::create(width - padding * 2, gameplayEndsAtStr);
    m_endsAtInput->setScale(0.7f);
    m_endsAtInput->setPosition({ width / 2, height - 50 });
    m_endsAtInput->setFilter("0123456789.");
    m_endsAtInput->setMaxCharCount(5);
    m_endsAtInput->setLabel("Gameplay ends at:");
    m_endsAtInput->setString(gameplayEndsAtStr);

    auto inputLabel = m_endsAtInput->getChildByType<CCLabelBMFont>(0);
    inputLabel->setFntFile("bigFont.fnt");
    inputLabel->setColor(ccColor3B(120, 120, 120));

    
    auto spr = ButtonSprite::create("Save");
    auto button = CCMenuItemSpriteExtra::create(spr, this, menu_selector(LevelSettingsPopup::onConfirm));

    auto buttonMenu = CCMenu::create();
    buttonMenu->setPosition({ width / 2, 4 });
    buttonMenu->setScale(0.7f);
    buttonMenu->setAnchorPoint({ 0, 0 });
    buttonMenu->addChild(button);

    m_closeBtn->setScale(0.6f);
    m_closeBtn->m_baseScale = 0.6f;

    m_mainLayer->addChild(title);
    m_mainLayer->addChild(m_endsAtInput);
    m_mainLayer->addChild(buttonMenu);

    return true;
}

LevelSettingsPopup* LevelSettingsPopup::create(GJGameLevel* level, std::function<void()> onConfirmCallback) {
    auto ret = new LevelSettingsPopup();
    if (ret->init(level, onConfirmCallback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void LevelSettingsPopup::onConfirm(CCObject* sender) {
    auto inputResult = geode::utils::numFromString<float>(m_endsAtInput->getString());

    if (!inputResult) {
        Notification::create("Invalid number!")->show();
        return;
    };

    float inputValue = inputResult.unwrap();

    inputValue = std::floor(inputValue * 100.0f) / 100.0f;

    if (inputValue < 1 || inputValue > 100) {
        Notification::create("Enter number from 0 to 100")->show();
        return;
    }

  
    RunsManager::get().setGameplayEndsAt(inputValue);

    if (m_onConfirmCallback) m_onConfirmCallback();

    onClose(nullptr);
}