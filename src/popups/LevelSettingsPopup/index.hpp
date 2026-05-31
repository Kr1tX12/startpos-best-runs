#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/TextInput.hpp>
#include "../../utils/index.hpp"
#include "../../managers/RunsManager/index.hpp"

using namespace geode::prelude;

class LevelSettingsPopup : public geode::Popup {
    private:
        void onConfirm(CCObject* sender);
        TextInput* m_endsAtInput;

        std::string m_levelID;
        std::function<void()> m_onConfirmCallback;
    protected:
        bool init(GJGameLevel* level, std::function<void()> onConfirmCallback);

    public:
        static LevelSettingsPopup* create(GJGameLevel* level, std::function<void()> onConfirmCallback);
};