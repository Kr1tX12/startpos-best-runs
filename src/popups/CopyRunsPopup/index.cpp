#include "index.hpp"

CopyRunsPopup* CopyRunsPopup::create() {
    auto ret = new CopyRunsPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool CopyRunsPopup::init() {
    float padding = 10;
    const float width = 200;
    const float height = 150;

    if (!Popup::init(width, height, "best-runs-popup-bg.png"_spr)) return false;

    auto title = CCLabelBMFont::create("Copy best runs", "bigFont.fnt");
    title->setPosition({ width / 2, height - padding - 5 });
    title->setScale(0.4f);

    const float inputScale = 0.6f;

    m_runsAmountInput = TextInput::create((width - padding * 2) / inputScale, "10", "bigFont.fnt");
    m_runsAmountInput->setLabel("Number of runs");
    m_runsAmountInput->setPosition({ width / 2, height - padding - 30 });
    m_runsAmountInput->setScale(inputScale);
    m_runsAmountInput->setString("10");
    m_runsAmountInput->setFilter("1234567890");

    m_templateInput = TextInput::create((width - padding * 2) / inputScale, "{start}%-{end}% x{x} ({length})", "bigFont.fnt");
    m_templateInput->setLabel("Run template");
    m_templateInput->setPosition({ width / 2, height - padding - 60 });
    m_templateInput->setScale(inputScale);
    m_templateInput->setString("{start}-{end} x{x}");
    m_templateInput->setFilter("1234567890 qwertyuiopasdfghjklzxcvbnm[]{}\\|;':\"/.,-_=+()*&^%$#@!~`");

    auto togglersMenu = CCMenu::create();
    togglersMenu->setPosition({ width / 2, height - padding - 95 });
    togglersMenu->setLayout(ColumnLayout::create()->setAutoScale(false));




    auto showDecimalsToggleMenu = CCMenu::create();
    showDecimalsToggleMenu->setLayout(RowLayout::create()->setAutoScale(false));
    showDecimalsToggleMenu->setScale(0.5f);
    
    m_showDecimalsToggler = CCMenuItemToggler::create(CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"), CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"), this, nullptr);

    auto showDecimalsLabel = CCLabelBMFont::create("Show decimals", "bigFont.fnt");

    showDecimalsToggleMenu->addChild(m_showDecimalsToggler);
    showDecimalsToggleMenu->addChild(showDecimalsLabel);
    showDecimalsToggleMenu->updateLayout();



    auto useGameplayEndsAtToggleMenu = CCMenu::create();
    useGameplayEndsAtToggleMenu->setLayout(RowLayout::create()->setAutoScale(false));
    useGameplayEndsAtToggleMenu->setScale(0.5f);

    m_useGameplayEndsAtToggler = CCMenuItemToggler::create(CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"), CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"), this, nullptr);

    auto useGameplayEndsAtLabel = CCLabelBMFont::create("Use \"Gameplay ends at\"", "bigFont.fnt");
    useGameplayEndsAtLabel->setScale(0.62f);
 
    useGameplayEndsAtToggleMenu->addChild(m_useGameplayEndsAtToggler);
    useGameplayEndsAtToggleMenu->addChild(useGameplayEndsAtLabel);
    useGameplayEndsAtToggleMenu->updateLayout();

    

    togglersMenu->addChild(showDecimalsToggleMenu);
    togglersMenu->addChild(useGameplayEndsAtToggleMenu);

    togglersMenu->updateLayout();


    auto copyButtonSprite = ButtonSprite::create("Copy");
    auto copyButton = CCMenuItemSpriteExtra::create(copyButtonSprite, this, menu_selector(CopyRunsPopup::onCopyClick));
    auto menu = CCMenu::create();
    menu->setScale(0.7f);

    menu->addChild(copyButton);
    menu->setAnchorPoint({ 0, 0 });
    menu->setPosition({ width / 2, 0 });

    m_closeBtn->setScale(0.6f);
    m_closeBtn->m_baseScale = 0.6f;

    m_mainLayer->addChild(title);
    m_mainLayer->addChild(m_runsAmountInput);
    m_mainLayer->addChild(m_templateInput);
    m_mainLayer->addChild(menu);
    m_mainLayer->addChild(togglersMenu);

    return true;
}

void CopyRunsPopup::onCopyClick(CCObject* sender) {
    auto runsAmountResult = geode::utils::numFromString<int>(m_runsAmountInput->getString());

    if (!runsAmountResult) {
        Notification::create("Invalid number!")->show();
        return;
    }

    int runsAmount = runsAmountResult.unwrap();

    std::string runTemplate = m_templateInput->getString();

    bool showDecimals = m_showDecimalsToggler->isToggled();
    bool useGameplayEndsAt = m_useGameplayEndsAtToggler->isToggled();

    std::string allRuns = getRunsText(runsAmount, runTemplate, showDecimals, useGameplayEndsAt);

    bool copyResult = geode::utils::clipboard::write(allRuns);

    if (copyResult) Notification::create("Copied successfully!")->show();
    else Notification::create("Failed to copy!")->show();
} 

std::string CopyRunsPopup::getRunsText(int runsAmount, std::string runTemplate, bool showDecimals, bool useGameplayEndsAt) {
    std::string result = "";

    LevelJson level = RunsManager::get().getLevelJson();

    std::vector<ConvertedRunJson> convertedRuns = Utils::convertRuns(level.runs, level.endsAt);

    std::sort(convertedRuns.begin(), convertedRuns.end(),
        [useGameplayEndsAt](auto const& a, auto const& b) {
            return (useGameplayEndsAt ? (a.convertedEnd - a.convertedStart) : (a.end - a.start)) > (useGameplayEndsAt ? (b.convertedEnd - b.convertedStart) : (b.end - b.start));
        }
    );

    int runsAdded = 0;
    for (const auto& run : convertedRuns) {  
        if (runsAdded >= runsAmount) break;

        std::string convertedRunTemplate = runTemplate;
        if (run.x == 1) {
            convertedRunTemplate = geode::utils::string::replace(convertedRunTemplate, "x{x}", "");
            convertedRunTemplate = geode::utils::string::replace(convertedRunTemplate, "{x}x", "");
        }


        result += Utils::trim(fmt::format(
            fmt::runtime(convertedRunTemplate),
            fmt::arg("start", Utils::roundProgressValue(useGameplayEndsAt ? run.convertedStart : run.start, showDecimals)),
            fmt::arg("end", Utils::roundProgressValue(useGameplayEndsAt ? run.convertedEnd : run.end, showDecimals)),
            fmt::arg("x", run.x),
            fmt::arg("length", Utils::roundProgressValue(useGameplayEndsAt ? run.convertedEnd - run.convertedStart : run.end - run.start, showDecimals))
        ));

        result += "\n";

        runsAdded++;
    }

    return result;
}