#include <Geode/Geode.hpp>
#include "../../managers/RunsManager/index.hpp"

using namespace geode::prelude;

class CopyRunsPopup : public geode::Popup {
    private: 
        TextInput* m_runsAmountInput;
        TextInput* m_templateInput;

        CCMenuItemToggler* m_showDecimalsToggler;
        CCMenuItemToggler* m_useGameplayEndsAtToggler;

        void onCopyClick(CCObject* sender);
        std::string getRunsText(int runsAmount, std::string runTemplate, bool showDecimals, bool useGameplayEndsAt);
    protected:
        bool init();
    public: 
        static CopyRunsPopup* create();
};