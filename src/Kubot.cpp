#include <CannonRush.h>
#include <Kubot.h>
#include <MacroManager.h>
#include <Utils.h>

using namespace sc2;

Kubot::~Kubot() {};

void
Kubot::OnGameStart()
{
    auto obs = Observation();
    dump_pahting_grid(obs->GetGameInfo().pathing_grid, "map.txt");

    m_api.actions = Actions();
    m_api.obs = Observation();
    m_api.query = Query();

    auto macro = std::make_unique<MacroManager>( m_api, MacroManager::BuildOrder({ sc2::ABILITY_ID::TRAIN_PROBE
                                               , sc2::ABILITY_ID::TRAIN_PROBE
                                               , sc2::ABILITY_ID::BUILD_PYLON
                                               , sc2::ABILITY_ID::BUILD_FORGE }));
    m_listeners.push_back(std::move(macro));
    m_listeners.push_back(std::make_unique<CannonRush>(m_api));
}

void Kubot::OnStep()
{
    for (auto& listener : m_listeners)
    {
        listener->step();
    }
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
