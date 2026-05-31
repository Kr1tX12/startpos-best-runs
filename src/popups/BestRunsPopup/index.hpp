#include <Geode/Geode.hpp>
#include "../../managers/RunsManager/index.hpp"
#include "../../utils/index.hpp"
#include "../RunEditPopup/index.hpp"
#include "../../types/types.hpp"
#include "../../ui/RunsListLayer/index.hpp"
#include "../../ui/SortButtonLayer/index.hpp"
#include "../../ui/PointsLabelLayer/index.hpp"
#include "../LevelSettingsPopup/index.hpp"

using namespace geode::prelude;

class BestRunsPopup : public geode::Popup {
    private: 
        void onEditClick(float start);
        void onSettingsClick(CCObject* sender);

        GJGameLevel* m_level;
        RunsListLayer* m_runsList;
        PointsLabelLayer* m_pointsLabel;
        RunsSort m_sort = RunsSort::Length;

        void reloadRuns();
        void changeSort(CCObject* sender);
    protected:
        bool init(GJGameLevel* level);

    public:
        static BestRunsPopup* create(GJGameLevel* level);
};