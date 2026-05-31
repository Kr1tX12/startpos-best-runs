#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/TextInput.hpp>
#include "../../utils/index.hpp"
#include "../../managers/RunsManager/index.hpp"

using namespace geode::prelude;

class RunEditPopup : public geode::Popup {
    private:
        void onConfirm(CCObject* sender);
        TextInput* m_textInput;
        float m_from;
        std::function<void()> m_onConfirmCallback;
    protected:
        bool init(float from, std::function<void()> onConfirmCallback);

    public:
        static RunEditPopup* create(float from, std::function<void()> onConfirmCallback);
};