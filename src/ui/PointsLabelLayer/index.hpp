#include <Geode/Geode.hpp>
#include "../../utils/index.hpp"
#include "../../types/types.hpp"
#include "../../managers/RunsManager/index.hpp"

using namespace geode::prelude;

class PointsLabelLayer : public CCLayer {
    protected:
        bool init();
        CCLabelBMFont* m_pointsLabel;

    public:
        static PointsLabelLayer* create();

        void updateLabel();
};