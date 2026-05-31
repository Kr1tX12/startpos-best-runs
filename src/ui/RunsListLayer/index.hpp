#include <Geode/Geode.hpp>
#include "../SortButtonLayer/index.hpp"
#include "../../types/types.hpp"
#include "../../utils/index.hpp"
#include "../../managers/RunsManager/index.hpp"

using namespace geode::prelude;

class RunsListLayer : public CCLayer {
    protected:
        ScrollLayer* m_scrollLayer;
        CCLayer* m_content;
        CCLabelBMFont* m_noRunsLabel;

        float m_width;
        float m_height;
        float m_padding;

        std::function<void(float)> m_onEditClick;

        bool init(float width, float height, float padding, std::function<void(float)> onEditClick);

        void onEdit(CCObject* sender);
    public:
        static RunsListLayer* create(float width, float height, float padding, std::function<void(float)> onEditClick);
        void setRuns(RunsSort sort);
};