#include <CannonRush.h>
#include <Kubot.h>
#include <MacroManager.h>
#include <Utils.h>
#include <utils/Map.h>
#include <utils/UnitTraits.h>
#include <utils/UnitQuery.h>

#include <BuildOrderExecutor.h>

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

    m_map = std::make_unique<sc2::utils::Map>(sc2::utils::Map(m_sc2));

    auto tech_tree = sc2::utils::make_tech_tree(*obs);
    auto opening = std::make_unique<BuildOrderExecutor>(m_sc2, *m_map, tech_tree, make_4gate(m_sc2.obs()));
    m_listeners.push_back(std::move(opening));
}

void
Kubot::OnStep()
{
    for (auto& listener : m_listeners)
    {
        listener->step();
    }
    m_sc2.debug().SendDebug();
}

void
Kubot::OnUnitCreated(const Unit* unit)
{
    static int count = 0; // HACK: skip redundant callbacks for precreated units
    if (count < 13)
    {
        count++;
        return;
    }

    for (auto& listener : m_listeners)
    {
        listener->unitCreated(unit);
    }
}
void
Kubot::OnBuildingConstructionComplete(const Unit* unit)
{
    for (auto& listener : m_listeners)
    {
        listener->buildingConstructionComplete(unit);
    }
}

void
Kubot::OnUnitDestroyed(const Unit* unit)
{
    for (auto& listener : m_listeners)
    {
        listener->unitDestroyed(unit);
    }
}

void
Kubot::OnUnitIdle(const Unit* unit)
{
    for (auto& listener : m_listeners)
    {
        listener->unitIdle(unit);
    }
}
