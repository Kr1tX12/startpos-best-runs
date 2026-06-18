#include "index.hpp"

bool RunEditPopup::init(float from, std::function<void()> onConfirmCallback) {
    float padding = 10;
    const float width = 200;
    const float height = 100;
    
    if (!Popup::init(width, height, "best-runs-popup-bg.png"_spr)) return false;
    
    m_from = from;
    m_onConfirmCallback = onConfirmCallback;
    
    auto title = CCLabelBMFont::create(fmt::format("Edit best run from {}%", from).c_str(), "bigFont.fnt");
    title->setPosition({width / 2, height - 35});
    title->setScale(0.4f);
    
    m_textInput = TextInput::create(width - padding * 2, "New best run");
    m_textInput->setScale(0.7f);
    m_textInput->setPosition({ width / 2, height - 58 });
    m_textInput->setFilter("0123456789.");
    m_textInput->setMaxCharCount(5);

    
    auto spr = ButtonSprite::create("Confirm");
    auto button = CCMenuItemSpriteExtra::create(spr, this, menu_selector(RunEditPopup::onConfirm));

    auto buttonMenu = CCMenu::create();
    buttonMenu->setPosition({ width / 2, 4 });
    buttonMenu->setScale(0.7f);
    buttonMenu->setAnchorPoint({ 0, 0 });
    buttonMenu->addChild(button);

    m_closeBtn->setScale(0.6f);
    m_closeBtn->m_baseScale = 0.6f;

    m_mainLayer->addChild(title);
    m_mainLayer->addChild(m_textInput);
    m_mainLayer->addChild(buttonMenu);

    return true;
}

RunEditPopup* RunEditPopup::create(float from, std::function<void()> onConfirmCallback) {
    auto ret = new RunEditPopup();
    if (ret->init(from, onConfirmCallback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void RunEditPopup::onConfirm(CCObject* sender) {
    auto inputResult = geode::utils::numFromString<float>(m_textInput->getString());

    if (!inputResult) return;

    float inputValue = inputResult.unwrap();

    inputValue = std::floor(inputValue * 100.0f) / 100.0f;

    if (inputValue < m_from || inputValue > 100) {
        Notification::create(fmt::format("Enter number from {} to 100", m_from))->show();
        return;
    }

    RunsManager::get().forceUpdateRun(m_from, inputValue);
    RunsManager::get().saveRuns();

    if (m_onConfirmCallback) m_onConfirmCallback();

    onClose(nullptr);
}