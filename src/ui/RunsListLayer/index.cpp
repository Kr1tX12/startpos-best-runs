#include "index.hpp"

RunsListLayer* RunsListLayer::create(float width, float height, float padding, std::function<void(float)> onEditClick) {
    auto ret = new RunsListLayer();

    if (ret && ret->init(width, height, padding, onEditClick)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool RunsListLayer::init(float width, float height, float padding, std::function<void(float)> onEditClick) {
    if (!CCLayer::init()) return false;

    m_width = width;
    m_height = height;
    m_padding = padding;
    m_onEditClick = onEditClick;


    m_scrollLayer = ScrollLayer::create({width - padding * 2, height - padding * 2 - 25});
    m_scrollLayer->setPosition({padding, padding});
    m_scrollLayer->m_contentLayer->setLayout(
      ColumnLayout::create()
          ->setAxisAlignment(AxisAlignment::Start)
          ->setAutoGrowAxis(m_scrollLayer->getContentHeight()));

    addChild(m_scrollLayer);

    m_content = CCLayer::create();
    m_content->setContentSize(m_scrollLayer->getContentSize());
    m_content->setLayout(
        ColumnLayout::create()
            ->setAutoGrowAxis(m_scrollLayer->getContentHeight())
            ->setAutoGrowAxis(m_scrollLayer->getContentHeight())
            ->setGap(5)
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->ignoreInvisibleChildren(false)
    );
    
    m_scrollLayer->m_contentLayer->addChild(m_content);

    return true;
}


void RunsListLayer::setRuns(RunsSort sort) {
    if (m_noRunsLabel) {
        m_noRunsLabel->removeFromParent();
        m_noRunsLabel = nullptr;
    }

    auto levelJson = RunsManager::get().getLevelJson();

    if (levelJson.runs.size() == 0) {
        m_noRunsLabel = CCLabelBMFont::create("It's empty :(", "bigFont.fnt");
        m_noRunsLabel->setPosition({ m_width / 2, m_height / 2 });
        m_noRunsLabel->setScale(0.35f);
        m_noRunsLabel->setColor(ccColor3B(120, 120, 120));

        addChild(m_noRunsLabel);

        return;
    }

    bool showDecimals = Mod::get()->getSettingValue<bool>("show-decimals");
    auto runs = Utils::convertRuns(levelJson.runs, levelJson.endsAt);
    
    if (sort == RunsSort::Start) {
        std::sort(runs.begin(), runs.end(),
            [](auto const& a, auto const& b) {
                return a.convertedStart < b.convertedStart;
            }
        );
    } else if (sort == RunsSort::Length) {
        std::sort(runs.begin(), runs.end(),
            [](auto const& a, auto const& b) {
                return (a.convertedEnd - a.convertedStart) > (b.convertedEnd - b.convertedStart);
            }
        );
    }          

    float biggestRun = 0;

    for (const auto& run : runs) {
        biggestRun = std::max(biggestRun, run.convertedEnd - run.convertedStart);
    }

    m_content->removeAllChildren();

    for (const auto& run : runs) {
        auto bg = CCScale9Sprite::create("square02_small.png");
        auto width = m_width - m_padding * 2;
        bg->setOpacity(20);
        bg->setContentSize({width, 30});
        
        auto menu = CCMenu::create();

        menu->setContentSize({ bg->getContentWidth() - 60, bg->getContentHeight() });
        menu->setPosition({ (bg->getContentWidth()) / 2, (bg->getContentHeight()) / 2 });
        
        bg->addChild(menu);

        
        std::string labelText;

        if (run.convertedStart == 0) {
            if (run.x > 1) {
                labelText  = fmt::format("{}% x{}", Utils::roundProgressValue(run.convertedEnd, showDecimals), run.x);
            } else {
                labelText = fmt::format("{}%", Utils::roundProgressValue(run.convertedEnd, showDecimals));
            }
        } else {
            if (run.x > 1) {
                labelText  = fmt::format("{}% - {}% x{}", Utils::roundProgressValue(run.convertedStart, showDecimals), Utils::roundProgressValue(run.convertedEnd, showDecimals), run.x);
            } else {
                labelText = fmt::format("{}% - {}%", Utils::roundProgressValue(run.convertedStart, showDecimals), Utils::roundProgressValue(run.convertedEnd, showDecimals));
            }
        }

        auto label = CCLabelBMFont::create(
            labelText.c_str(),
            "bigFont.fnt"
        );
        
        label->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);

        float labelWidth = label->getContentSize().width;

        float labelScale = 0.45f;

        if (labelWidth > 280.0f) {
            labelScale *= 280.0f / labelWidth;
        }

        label->setScale(labelScale);

        float runLength = static_cast<float>(run.convertedEnd - run.convertedStart);

        float t = biggestRun > 0
            ? runLength / static_cast<float>(biggestRun)
            : 0.f;

        t = std::clamp(t, 0.f, 1.f);

        GLubyte r, g;

        if (t < 0.5f) {
            r = 255;
            g = static_cast<GLubyte>(255 * (t * 2.f));
        }
        else {
            r = static_cast<GLubyte>(255 * (1.f - (t - 0.5f) * 2.f));
            g = 255;
        }

        auto runLengthLabel = CCLabelBMFont::create(
            std::format("{}% run", static_cast<int>(runLength)).c_str(),
            "bigFont.fnt"
        );

        runLengthLabel->setScale(0.2f);
        runLengthLabel->setPositionY(-10);
        runLengthLabel->setColor({ r, g, 0 });
        
        if (levelJson.endsAt != 100) {
            std::string originalRunText;
            if (run.start == 0) {
                originalRunText = fmt::format("{}%", Utils::roundProgressValue(run.end, showDecimals));
            } else {
                originalRunText = fmt::format("{}% - {}%", Utils::roundProgressValue(run.start, showDecimals), Utils::roundProgressValue(run.end, showDecimals));
            }

            auto originalRunLabel = CCLabelBMFont::create(
                originalRunText.c_str(),
                "bigFont.fnt"
            );
            originalRunLabel->setScale(0.2f);
            originalRunLabel->setColor({ 87, 87, 87 });
            
            float gap = 5.f;
            
            float leftWidth = runLengthLabel->getContentWidth() * runLengthLabel->getScale();
            float rightWidth = originalRunLabel->getContentWidth() * originalRunLabel->getScale();
            
            float totalWidth = leftWidth + gap + rightWidth;
            
            runLengthLabel->setAnchorPoint({0, 0.5f});
            runLengthLabel->setPosition({-totalWidth / 2, -10});
            
            originalRunLabel->setAnchorPoint({0, 0.5f});
            originalRunLabel->setPosition({-totalWidth / 2 + leftWidth + gap, -10});
            menu->addChild(originalRunLabel);
        }
            
        auto frame = CCSprite::create("square-button-frame.png"_spr);
        frame->setScale(1);
        
        auto editSprite = CCSprite::create("edit.png"_spr);
        editSprite->setScale(0.13f);
        editSprite->setPosition(frame->getContentSize() / 2);
        editSprite->setColor(ccColor3B(255, 255, 255));
        
        frame->addChild(editSprite);
        
        auto editButton = CCMenuItemSpriteExtra::create(frame, this, menu_selector(RunsListLayer::onEdit));
        editButton->setTag(run.start * 100);
        editButton->setPositionX(width / 2 - 15);
        
        menu->addChild(label);
        menu->addChild(editButton);
        menu->addChild(runLengthLabel);
        
        menu->updateLayout();
        
        m_content->addChild(bg);
    }
    
    m_content->updateLayout();
    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->moveToTop();
}

void RunsListLayer::onEdit(CCObject* sender) {
    if (m_onEditClick) {
        m_onEditClick(sender->getTag() / 100.f);
    }
}