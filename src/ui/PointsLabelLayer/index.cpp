#include "index.hpp"

PointsLabelLayer* PointsLabelLayer::create() {
    auto ret = new PointsLabelLayer();

    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool PointsLabelLayer::init() {
    if (!CCLayer::init()) return false;

    auto levelJson = RunsManager::get().getLevelJson();
    auto convertedRuns = Utils::convertRuns(levelJson.runs, levelJson.endsAt);

    float points = Utils::getLevelPoints(convertedRuns);
    int levelCompletedRuns = Utils::getLevelCompletedRuns(convertedRuns);

    if (points < 0 || std::isnan(points)) return true;

    std::string labelText;

    if (levelCompletedRuns != -1) {
        labelText = fmt::format("{}% completed - {} runs", points, levelCompletedRuns);
    } else {
        labelText = fmt::format("{}% completed", points);
    }
 
    m_pointsLabel = CCLabelBMFont::create(labelText.c_str(), "bigFont.fnt");
    m_pointsLabel->setColor(ccColor3B(115, 115, 115));
    m_pointsLabel->setAnchorPoint({ 0, 0 });

    addChild(m_pointsLabel);

    return true;
}

void PointsLabelLayer::updateLabel() {
    auto levelJson = RunsManager::get().getLevelJson();
    auto convertedRuns = Utils::convertRuns(levelJson.runs, levelJson.endsAt);

    float points = Utils::getLevelPoints(convertedRuns);
    int levelCompletedRuns = Utils::getLevelCompletedRuns(convertedRuns);

    if (points < 0 || std::isnan(points)) return;

    std::string labelText;

    if (levelCompletedRuns != -1) {
        labelText = fmt::format("{}% completed - {} runs", points, levelCompletedRuns);
    } else {
        labelText = fmt::format("{}% completed", points);
    }

    m_pointsLabel->setString(labelText.c_str());
}