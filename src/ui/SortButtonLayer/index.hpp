#pragma once

#include <Geode/Geode.hpp> 

using namespace geode::prelude;

enum class RunsSort {
    Start,
    Length
};

class SortButtonLayer : public CCLayer {
    protected:
        RunsSort m_sort;
        void onClick(CCObject* sender);
        std::function<void(RunsSort)> m_onChangeCallback;
        void updateText();
        CCLabelBMFont* m_label;
        
        bool init(RunsSort sort, std::function<void(RunsSort)> onChangeCallback);

    public:
        static SortButtonLayer* create(RunsSort sort, std::function<void(RunsSort)> onChangeCallback);
};