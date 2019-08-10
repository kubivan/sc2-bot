#include "MacroManager.h"

#include <Utils.h>
#include <iostream>

MacroManager::~MacroManager() = default;

namespace
{
bool isHarvester(const sc2::Unit& u)
{
    return !u.orders.empty() && (u.orders.front().ability_id == sc2::ABILITY_ID::HARVEST_GATHER
        || (u.orders.front().ability_id == sc2::ABILITY_ID::HARVEST_RETURN)
        );
}
}

MacroManager::MacroManager(const API& api, BuildOrder build_order)
    : m_api(api)
    , m_build_order(std::move(build_order))
{
}

void MacroManager::step()
{
    if (!m_build_order.empty())
    {
        executeBuildOrder();
        return;
    }
    checkProbes();
    checkSupply();
}

void MacroManager::unitCreated(const sc2::Unit * unit)
{
    if (!m_build_order.empty())
    {
        //TODO: check correspondance between unit->unit_type and m_buildorder.front()
        m_build_order.pop();
    }
}

void MacroManager::buildingConstructionComplete(const sc2::Unit * unit)
{
    if (unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_PYLON)
    {
        m_pylons.insert(unit);
    }
}

void MacroManager::unitDestroyed(const sc2::Unit * unit)
{
    m_pylons.erase(unit);
}


void MacroManager::executeBuildOrder()
{
    const auto order = m_build_order.front();
    if (!canAfford(order))
    {
        std::cout << "cannot afford " << (int)order << std::endl;
        return;
    }
    auto nexus = m_api.obs->GetUnits([](const sc2::Unit& u) {return u.unit_type == sc2::UNIT_TYPEID::PROTOSS_NEXUS; }).front();
    if (order == sc2::ABILITY_ID::TRAIN_PROBE && nexus->orders.empty())
    {
        std::cout << "train probe " << (int) order << std::endl;
        m_api.actions->UnitCommand(nexus, sc2::ABILITY_ID::TRAIN_PROBE);
        return;
    }

    // buildings
    if (!m_api.obs->GetUnits([order](const sc2::Unit& u)
    {
        auto& orders = u.orders;
        return std::find_if(orders.cbegin(), orders.cend(), [order](const auto o) {
            return o.ability_id == order; }) != orders.cend();
    }).empty())
    {
        //orders already assigned
        return;
    }

    const auto is_pylon = order == sc2::ABILITY_ID::BUILD_PYLON;
    if (!is_pylon && m_pylons.empty())
    {
        return;
    }

    const auto building_position = is_pylon ? nexus->pos: (*m_pylons.cbegin())->pos;
    auto probes = m_api.obs->GetUnits(isHarvester);
    assert(!probes.empty());
    auto builder = probes.front();
    auto order_bak = builder->orders.front();
    build_near(m_api, builder, building_position, 5.f, order, true);
    m_api.actions->UnitCommand(builder, order_bak.ability_id, order_bak.target_pos, true);
}

void MacroManager::checkProbes()
{
    if (m_api.obs->GetMinerals() < 50)
    {
        return;
    }
    //TODO:

}
void MacroManager::checkSupply()
{
    if (m_api.obs->GetMinerals() < 100)
    {
        return;
    }
    //TODO:

}

bool MacroManager::canAfford(BuildOrder::value_type item)
{
    //TODO: get buildings cost via sc2api
    const auto minerals = m_api.obs->GetMinerals();
    switch (item)
    {
    case sc2::ABILITY_ID::TRAIN_PROBE:
        return minerals >= 50;
    case sc2::ABILITY_ID::BUILD_PYLON:
        return minerals >= 100;
    case sc2::ABILITY_ID::BUILD_FORGE:
        return minerals >= 150;
    }
    return false;
}
