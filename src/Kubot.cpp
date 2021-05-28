#include <CannonRush.h>
#include <Kubot.h>
#include <MacroManager.h>
#include <Utils.h>
#include <MapSegmentation.h>

using namespace sc2;

Kubot::~Kubot() {}
Kubot::Kubot()
    : m_sc2(*this)
{
}

void
Kubot::OnGameStart()
{
    auto obs = Observation();
    dump_pahting_grid(obs->GetGameInfo().pathing_grid, "map.txt");

    auto macro = std::make_unique<MacroManager>(m_sc2, MacroManager::BuildOrder({ sc2::ABILITY_ID::TRAIN_PROBE
                                               , sc2::ABILITY_ID::TRAIN_PROBE
                                               , sc2::ABILITY_ID::BUILD_PYLON
                                               , sc2::ABILITY_ID::BUILD_FORGE }));
    m_listeners.push_back(std::move(macro));
    m_listeners.push_back(std::make_unique<CannonRush>(m_sc2));
}

void Kubot::OnStep()
{
    for (auto& listener : m_listeners)
    {
        listener->step();
    }
    m_sc2.debug().SendDebug();
}


void Kubot::OnUnitCreated(const Unit* unit)
{
    static int count = 0; // HACK: skip redundant callbacks for precreated units
    if (count < 12)
    {
        count++;
        return;;
    }

    for (auto& listener : m_listeners)
    {
        listener->unitCreated(unit);
    }
}
void Kubot::OnBuildingConstructionComplete(const Unit* unit)
{
    for (auto& listener : m_listeners)
    {
        listener->buildingConstructionComplete(unit);
    }
}

void Kubot::OnUnitDestroyed(const Unit* unit)
{
    for (auto& listener : m_listeners)
    {
        listener->unitDestroyed(unit);
    }
}

void Kubot::OnUnitIdle(const Unit* unit)
{
    for (auto& listener : m_listeners)
    {
        listener->unitIdle(unit);
    }
}
