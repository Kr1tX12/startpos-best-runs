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

    if (points < 0 || std::isnan(points)) return true;

    m_pointsLabel = CCLabelBMFont::create(std::format("{}% completed", points).c_str(), "bigFont.fnt");
    m_pointsLabel->setColor(ccColor3B(115, 115, 115));
    m_pointsLabel->setAnchorPoint({ 0, 0 });

    addChild(m_pointsLabel);

    return true;
}

void PointsLabelLayer::updateLabel() {
    auto levelJson = RunsManager::get().getLevelJson();
    auto convertedRuns = Utils::convertRuns(levelJson.runs, levelJson.endsAt);

    float points = Utils::getLevelPoints(convertedRuns);

    if (points < 0 || std::isnan(points)) return;

    m_pointsLabel->setString(std::format("{}% completed", points).c_str());
}