#include "MacroManager.h"

#include <SC2.h>

#include <Utils.h>
#include <iostream>
#include <string>
#include <utils/UnitQuery.h>
#include <utils/Map.h>

namespace
{
bool isHarvester(const sc2::Unit& u)
{
    return !u.orders.empty() && (u.orders.front().ability_id == sc2::ABILITY_ID::HARVEST_GATHER
        || (u.orders.front().ability_id == sc2::ABILITY_ID::HARVEST_RETURN)
        );
}

}

MacroManager::~MacroManager() = default;

MacroManager::MacroManager(SC2& sc2, sc2::utils::Map& map, sc2::utils::TechTree tech_tree, BuildOrder build_order)
    : m_sc2(sc2)
    , m_build_order(std::move(build_order))
    , m_tech_tree(std::move(tech_tree))
    , m_map(map)
{
}

void
MacroManager::step()
{
    debugOutput();

    if (!m_build_order.empty())
    {
        executeBuildOrder();
        return;
    }
    checkProbes();
    checkSupply();

}

void
MacroManager::unitCreated(const sc2::Unit* unit)
{
    m_map.place_building(*unit);

    if (!m_build_order.empty())
    {
        //TODO: check correspondance between unit->unit_type and m_buildorder.front()
        m_build_order.pop();
    }
}

void
MacroManager::buildingConstructionComplete(const sc2::Unit* unit)
{
    if (unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_PYLON)
    {
        m_pylons.insert(unit);
    }
}

void
MacroManager::unitDestroyed(const sc2::Unit* unit)
{
    m_pylons.erase(unit);
}

void
MacroManager::unitIdle(const sc2::Unit* unit)
{
    if (m_builders.count(unit))
    {
        auto minerals = m_sc2.obs().GetUnits(type(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD)
            || type(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750));

        std::cout << "gazering! " << std::endl;
        m_sc2.act().UnitCommand(unit, sc2::ABILITY_ID::HARVEST_GATHER, closest(unit, minerals));
        m_builders.erase(unit);
    }
}

void
MacroManager::executeBuildOrder()
{
    const auto order = m_build_order.front();
    if (!canAfford(order))
    {
        return;
    }
    auto nexus = m_sc2.obs().GetUnits(type(sc2::UNIT_TYPEID::PROTOSS_NEXUS)).front();
    if (order == sc2::ABILITY_ID::TRAIN_PROBE && nexus->orders.empty())
    {
        m_sc2.act().UnitCommand(nexus, sc2::ABILITY_ID::TRAIN_PROBE);
        return;
    }

    // buildings
    if (!m_sc2.obs().GetUnits([order](const sc2::Unit& u)
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

    auto probes = m_sc2.obs().GetUnits(isHarvester);
    auto builder = probes.front();
    auto order_bak = builder->orders.front();
    if (is_pylon)
    {
        auto pos = rand_point_around(nexus->pos, 6.f);
        m_sc2.act().UnitCommand(builder, sc2::ABILITY_ID::BUILD_PYLON, pos);
        m_builders.insert(builder);
        return;
    }
    build_near(m_sc2, builder, (*m_pylons.cbegin())->pos, 5.f, order);
    m_builders.insert(builder);
}

void
MacroManager::checkProbes()
{
    if (m_sc2.obs().GetMinerals() < 50)
    {
        return;
    }

    auto nexuses = m_sc2.obs().GetUnits(
        type(sc2::UNIT_TYPEID::PROTOSS_NEXUS) && sc2::self);

    auto probes = m_sc2.obs().GetUnits(
        type(sc2::UNIT_TYPEID::PROTOSS_PROBE) && sc2::self);

    auto probes_needed = nexuses.size() * 24;
    if (probes.size() >= probes_needed)
    {
        return;
    }

    for (auto& nexus : nexuses)
    {
        if (nexus->orders.empty())
        {
            //m_sc2.debug().Debug
            m_sc2.act().UnitCommand(nexus, sc2::ABILITY_ID::TRAIN_PROBE);
        }
    }
}

void
MacroManager::checkSupply()
{
    if (m_sc2.obs().GetMinerals() < 100)
    {
        return;
    }
    //TODO:

}

void
MacroManager::debugOutput()
{
    const auto units = m_sc2.obs().GetUnits();
    const auto maxz_unit = *std::max_element(units.begin(), units.end()
        , [](const auto& u1, const auto& u2) { return u1->pos.z < u2->pos.z; });
    const auto max_z = maxz_unit->pos.z;


    for (auto& u : units)
    {
        if (u->unit_type == sc2::UNIT_TYPEID::PROTOSS_PROBE)
        {
            continue;
        }
        auto pos = u->pos;
        //auto tile_width = std::max(u->radius, (float)m_tech_tree[u->unit_type].tile_width / 2);
        auto tile_width = u->radius;
        m_sc2.debug().DebugBoxOut( 
            sc2::Point3D(u->pos.x - tile_width, u->pos.y - tile_width, max_z + 2.0f)
            , sc2::Point3D(u->pos.x + tile_width, u->pos.y + tile_width, max_z - 5.0f)
            , sc2::Colors::Green);
        auto str = "(" + std::to_string(pos.x) + "," + std::to_string(pos.y) + ")\n";
        m_sc2.debug().DebugTextOut(str, sc2::Point3D(pos.x, pos.y, max_z), sc2::Colors::Green, 20);
    }

}

bool
MacroManager::canAfford(BuildOrder::value_type item)
{
    const auto minerals = m_sc2.obs().GetMinerals();
    auto building = std::find_if(
        m_tech_tree.begin()
        , m_tech_tree.end()
        , [item](const auto& kv) { return kv.second.build_ability == item; });
    return minerals >= building->second.mineral_cost;
}
