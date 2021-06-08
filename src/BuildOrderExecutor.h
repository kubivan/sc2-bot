#pragma once

#include <utils/Map.h>
#include <utils/UnitTraits.h>
#include <utils/UnitQuery.h>

#include <functional>

enum class PlacementHint
{
    Default = 0,
    WallOff,
    Safe,
    Proxy
};

class BuildOrder
{
public:
    using PrereqCheck = std::function<bool(const sc2::ObservationInterface&)>;

    struct BuildCommand
    {
        sc2::UNIT_TYPEID building_type;
        PlacementHint hint;
        PrereqCheck check;
    };

    BuildOrder& add( sc2::UNIT_TYPEID build_command
                   , PlacementHint hint = PlacementHint::Default
                   , PrereqCheck check = {})
    {
        m_commands.push_back({build_command, hint, check});
        return *this;
    }

    std::deque<BuildCommand>& orders()
    {
        return m_commands;
    }

    const std::deque<BuildCommand>& orders() const
    {
        return m_commands;
    }
private:
    std::deque<BuildCommand> m_commands;
};

BuildOrder make_4gate(const sc2::ObservationInterface& obs)
{
    auto pylon_built = [](const sc2::ObservationInterface& obs) -> bool  {
        //TODO: implement has units
        const auto has_pylon = sc2::type(sc2::UNIT_TYPEID::PROTOSS_PYLON) && sc2::built;
        return !obs.GetUnits(sc2::Unit::Self, has_pylon).empty();
    };

    auto gate_built = [](const sc2::ObservationInterface& obs) -> bool  {
        //TODO: implement has units
        const auto has_gate = sc2::type(sc2::UNIT_TYPEID::PROTOSS_GATEWAY) && sc2::built;
        return !obs.GetUnits(sc2::Unit::Self, has_gate).empty();
    };

    auto assimilator_built = [](const sc2::ObservationInterface& obs) -> bool  {
        //TODO: implement has units
        const auto done = sc2::type(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR) && sc2::built;
        return !obs.GetUnits(sc2::Unit::Self, done).empty();
    };

    BuildOrder order;
    order.add(sc2::UNIT_TYPEID::PROTOSS_PYLON, PlacementHint::WallOff)
        .add(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, PlacementHint::WallOff, pylon_built)
        .add(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR)
        .add(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, PlacementHint::Default, assimilator_built)
        .add(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, PlacementHint::Default, gate_built)
        .add(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, PlacementHint::Default)
        .add(sc2::UNIT_TYPEID::PROTOSS_PYLON, PlacementHint::Default);
    return order;
}

class BuildOrderExecutor : public EventListener
{
public:
    BuildOrderExecutor(
        SC2& sc2, sc2::utils::Map& map, sc2::utils::TechTree tech_tree, BuildOrder build_order)
        : m_sc2(sc2)
        , m_map(map)
        , m_tech_tree(tech_tree)
        , m_order(std::move(build_order))
    {

    }

    void step() override
    {
        checkProbes();

        if (m_order.orders().empty())
            return;

        auto top_order = m_order.orders().front();
        const auto& building_traits = m_tech_tree[top_order.building_type];
        const auto build_ability = building_traits.build_ability;

        if (top_order.check && !top_order.check(m_sc2.obs()))
        {
            return;
        }

        if (!sc2::utils::can_afford(top_order.building_type, m_tech_tree, m_sc2.obs()))
        {
            return;
        }

        if (build_ability == sc2::ABILITY_ID::BUILD_ASSIMILATOR)
        {
            const auto location = sc2::utils::wave(m_map.m_topology
                , sc2::utils::get_tile_pos(m_sc2.obs().GetStartLocation())
                , [&](const sc2::Point2DI& p)
                {
                    return m_map.m_topology[p] == '$' && m_sc2.query().Placement(build_ability, { (float)p.x, (float)p.y });
                });
            if (!location)
            {
                std::cout << "CANNOT FIND ASSIMILATOR!!!" << std::endl;
                return;
            }

            const auto geyser = m_sc2.obs().GetUnits(sc2::type(sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) 
                && sc2::in_radius({float(location->x), float(location->y)}, 3) ).front();

            auto builder = sc2::closest(geyser, m_sc2.obs().GetUnits(sc2::Unit::Self, sc2::Filter(sc2::harvester) || sc2::Filter(sc2::idle)));
            m_sc2.act().UnitCommand(builder, sc2::ABILITY_ID::BUILD_ASSIMILATOR, geyser, true);
            //workaround: unit stuck into unbuilt assimilator
            m_sc2.act().UnitCommand(builder, sc2::ABILITY_ID::MOVE, m_sc2.obs().GetStartLocation(), true);

            m_active_orders.push_back({ geyser->pos, top_order.building_type, builder->tag});
        }
        else if (build_ability == sc2::ABILITY_ID::BUILD_PYLON)
        {
            auto pos = rand_point_around(m_sc2.obs().GetStartLocation(), 10.f);
            auto builder = sc2::closest(pos, m_sc2.obs().GetUnits(sc2::Unit::Self, sc2::Filter(sc2::harvester) || sc2::Filter(sc2::idle)));
            m_sc2.act().UnitCommand(builder, sc2::ABILITY_ID::BUILD_PYLON, pos);
            m_active_orders.push_back({ pos, sc2::UNIT_TYPEID::PROTOSS_PYLON, builder->tag});
        }
        else
        {
            auto builder = m_sc2.obs().GetUnits(sc2::Unit::Self, sc2::Filter(sc2::harvester) || sc2::Filter(sc2::idle)).front();
            auto pylon = closest(builder, m_sc2.obs().GetUnits(
                sc2::Unit::Self, type(sc2::UNIT_TYPEID::PROTOSS_PYLON)));
            auto target_pos = build_near(m_sc2, builder, pylon->pos, 5.f, build_ability);
            m_active_orders.push_back({ target_pos, top_order.building_type, builder->tag});
        }
        m_order.orders().pop_front();
        m_gas_reserve += building_traits.gas_cost;
        m_minerals_reserve += building_traits.mineral_cost;
    }

    void unitCreated(const sc2::Unit* unit) override
    {
        auto active_order = std::remove_if(m_active_orders.begin(), m_active_orders.end(), [unit](const auto& order) {
            return unit->unit_type == order.target && sc2::DistanceSquared2D(unit->pos, order.target_pos) <= 1;
            });
        if (active_order != m_active_orders.end())
        {
            m_active_orders.erase(active_order, m_active_orders.end());
            const auto& unit_traits = m_tech_tree[unit->unit_type];
            m_gas_reserve -= unit_traits.gas_cost;
            m_minerals_reserve -= unit_traits.mineral_cost;
        }
    }

    void buildingConstructionComplete(const sc2::Unit* unit) override
    {

    }

    void unitDestroyed(const sc2::Unit* unit) override
    {
    }

    void unitIdle(const sc2::Unit* unit) override
    {
        if (unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_PROBE)
        {
            auto minerals = m_sc2.obs().GetUnits(type(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD)
                || type(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750));

            m_sc2.act().UnitCommand(unit, sc2::ABILITY_ID::HARVEST_GATHER, closest(unit, minerals));
        }
    }

    bool canAfford(sc2::UNIT_TYPEID item)
    {
        const int minerals = m_sc2.obs().GetMinerals() - m_minerals_reserve;
        const int vespene = m_sc2.obs().GetVespene() - m_gas_reserve;
        const auto& unit_traits = m_tech_tree[item];
        return minerals >= unit_traits.mineral_cost && vespene >= unit_traits.gas_cost;
    }

    bool isComplete() const { return m_complete || m_order.orders().empty(); }
private:
    struct ActiveCommand
    {
        sc2::Point2D target_pos;
        sc2::UNIT_TYPEID target;
        sc2::Tag executor;
    };

    void checkProbes()
    {
        for (auto& nexus : m_sc2.obs().GetUnits(
            type(sc2::UNIT_TYPEID::PROTOSS_NEXUS)
            && [](const auto& u) { return u.orders.empty(); }
            ))
        {
            if (sc2::utils::can_afford(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_tech_tree, m_sc2.obs()))
                m_sc2.act().UnitCommand(nexus, sc2::ABILITY_ID::TRAIN_PROBE);
        }
    }

    SC2& m_sc2;
    sc2::utils::Map& m_map;
    sc2::utils::TechTree m_tech_tree;
    BuildOrder m_order;
    bool m_complete = false;
    std::deque<ActiveCommand> m_active_orders;
    int m_gas_reserve = 0;
    int m_minerals_reserve = 0;
};
