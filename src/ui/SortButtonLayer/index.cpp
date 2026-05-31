#include "index.hpp"


std::string getSortText(RunsSort sort) {
    return sort == RunsSort::Start ? "Position" : sort == RunsSort::Length ? "Best" : "";
}   

SortButtonLayer* SortButtonLayer::create(RunsSort sort, std::function<void(RunsSort)> onChangeCallback) {
    auto ret = new SortButtonLayer();

    if (ret && ret->init(sort, onChangeCallback)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr; 
}

bool SortButtonLayer::init(RunsSort sort, std::function<void(RunsSort)> onChangeCallback) {
    if (!CCLayer::init()) return false;


    m_sort = sort;
    m_onChangeCallback = onChangeCallback;

    auto sortButtonSprite = ButtonSprite::create(getSortText(m_sort).c_str(), 115, 115, 1, true);
    auto sortButton = CCMenuItemSpriteExtra::create(sortButtonSprite, this, menu_selector(SortButtonLayer::onClick));
  
    m_label = sortButtonSprite->getChildByType<CCLabelBMFont>(0);

    auto menu = CCMenu::create();

    menu->setPosition(0, 0);
    menu->addChild(sortButton);

    this->addChild(menu);

    return true;
}

void SortButtonLayer::updateText() {
    m_label->setString(getSortText(m_sort).c_str());
}

void SortButtonLayer::onClick(CCObject* sender) {
    if (m_sort == RunsSort::Start) {
        m_sort = RunsSort::Length;
    } else if (m_sort == RunsSort::Length) {
        m_sort = RunsSort::Start;
    }

    updateText();

    if (m_onChangeCallback) m_onChangeCallback(m_sort);
}