#include "MacroManager.h"

#include <SC2.h>

#include <Utils.h>
#include <iostream>
#include <string>
#include <fstream>
#include <utils/UnitQuery.h>
#include <utils/Map.h>

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
    auto& data = m_tech_tree[unit->unit_type];
    //if (!m_build_order.empty())
    if (!m_build_order.empty() && m_build_order.front() == data.build_ability )
    {
        if (m_current_order)
            m_current_order.reset();
        m_build_order.pop();
    }



}

void
MacroManager::buildingConstructionComplete(const sc2::Unit* unit)
{
    switch (unit->unit_type.ToType())
    {
    case sc2::UNIT_TYPEID::PROTOSS_PYLON:
        m_pylons.insert(unit);
    break;
    case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR:
    case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATORRICH:
    {

        using namespace sc2;

        auto harvesters = m_sc2.obs().GetUnits(harvester && not_a(target(m_sc2.obs(), sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR)));
        for (int i = 0; i < std::min(3, (int)harvesters.size()); ++i)
        {
            m_sc2.act().UnitCommand(harvesters[i], sc2::ABILITY_ID::HARVEST_GATHER, unit);
        }
    }
    break;

    }
}

void
MacroManager::unitDestroyed(const sc2::Unit* unit)
{
    if (m_current_order && m_current_order->unit->tag == unit->tag)
    {
        m_current_order.reset();
        return;
    }
    m_pylons.erase(unit);
}

void
MacroManager::unitIdle(const sc2::Unit* unit)
{
    if (unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_PROBE && m_builders.count(unit->tag))
    {
        auto minerals = m_sc2.obs().GetUnits(type(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD)
            || type(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750));

        m_sc2.act().UnitCommand(unit, sc2::ABILITY_ID::HARVEST_GATHER, closest(unit, minerals));
        m_builders.erase(unit->tag);
    }
}

constexpr auto has_order(sc2::ABILITY_ID order)
{
    return [order](const auto& u) constexpr -> bool
    {
        return std::ranges::find_if(u.orders
            , [order](auto o) constexpr { return o.ability_id == order; })
            != u.orders.end();
    };
}

void
MacroManager::executeBuildOrder()
{
    if (m_current_order)
    {
        auto executors =
            m_sc2.obs().GetUnits(sc2::Unit::Self, has_order(m_current_order->command));
        if (!executors.empty() )
        {
            return;
        }
        m_current_order.reset();
    }

    const auto order = m_build_order.front();
    if (!canAfford(order))
    {
        return;
    }
    auto nexus = m_sc2.obs().GetUnits(type(sc2::UNIT_TYPEID::PROTOSS_NEXUS)).front();
    if (order == sc2::ABILITY_ID::TRAIN_PROBE)
    {
        if (nexus->orders.empty())
            m_sc2.act().UnitCommand(nexus, sc2::ABILITY_ID::TRAIN_PROBE);
        return;
    }

    if (order == sc2::ABILITY_ID::BUILD_ASSIMILATOR)
    {
        auto probes = m_sc2.obs().GetUnits(sc2::harvester);
        auto builder = probes.front();

        const auto location = sc2::utils::wave(m_map.m_topology
            , sc2::utils::get_tile_pos(m_sc2.obs().GetStartLocation())
            , [&](const sc2::Point2DI& p)
            {
                return m_map.m_topology[p] == '$' && m_sc2.query().Placement(order, { (float)p.x, (float)p.y });
            });
        if (!location)
        {
            std::cout << "CANNOT FIND ASSIMILATOR!!!" << std::endl;
            return;
        }

        const auto geyser = m_sc2.obs().GetUnits(sc2::type(sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) 
            && sc2::in_radius({float(location->x), float(location->y)}, 3) ).front();

        m_builders.insert(builder->tag);
        m_sc2.act().UnitCommand(builder, sc2::ABILITY_ID::BUILD_ASSIMILATOR, geyser, true);
        m_sc2.act().UnitCommand(builder, sc2::ABILITY_ID::MOVE, nexus->pos, true);
        m_current_order = Order{builder, order};
        return;
    }

    const auto is_pylon = order == sc2::ABILITY_ID::BUILD_PYLON;
    if (!is_pylon && m_pylons.empty())
    {
        return;
    }

    auto probes = m_sc2.obs().GetUnits(sc2::harvester);
    auto builder = probes.front();
    if (is_pylon)
    {
        auto pos = rand_point_around(nexus->pos, 10.f);
        m_sc2.act().UnitCommand(builder, sc2::ABILITY_ID::BUILD_PYLON, pos);
        m_builders.insert(builder->tag);
        m_current_order = Order{builder, order};
    }
    else
    {
        build_near(m_sc2, builder, (*m_pylons.cbegin())->pos, 5.f, order);
        m_builders.insert(builder->tag);
        m_current_order = Order{builder, order};
    }
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
    const auto maxz_unit = *std::ranges::max_element(units
        , [](const auto& u1, const auto& u2) { return u1->pos.z < u2->pos.z; });
    const auto max_z = maxz_unit->pos.z;

    for (auto& u : units)
    {
        const auto pos = u->pos;
        //if (u->unit_type == sc2::UNIT_TYPEID::PROTOSS_PROBE)
        {
            auto str = std::string{ "(" } + std::to_string(u->tag) + ")\n";
            //for (auto& o : u->orders)
            //{
            //    str += std::to_string(o.ability_id) + ',';
            //}
            //str += ")\n";
            m_sc2.debug().DebugTextOut(str, sc2::Point3D(pos.x, pos.y, max_z), sc2::Colors::Green, 15);
            continue;
        }
        ////auto tile_width = std::max(u->radius, (float)m_tech_tree[u->unit_type].tile_width / 2);
        //auto tile_width = u->radius;
        //m_sc2.debug().DebugBoxOut( 
        //    sc2::Point3D(u->pos.x - tile_width, u->pos.y - tile_width, max_z + 2.0f)
        //    , sc2::Point3D(u->pos.x + tile_width, u->pos.y + tile_width, max_z - 5.0f)
        //    , sc2::Colors::Green);
        //auto str = "(" + std::to_string(pos.x) + "," + std::to_string(pos.y) + ")\n";
        //m_sc2.debug().DebugTextOut(str, sc2::Point3D(pos.x, pos.y, max_z), sc2::Colors::Green, 20);
    }

}

bool
MacroManager::canAfford(BuildOrder::value_type item)
{
    const auto minerals = m_sc2.obs().GetMinerals();
    auto building = std::ranges::find_if(m_tech_tree
        , [item](const auto& kv) { return kv.second.build_ability == item; });
    return minerals >= building->second.mineral_cost;
}
