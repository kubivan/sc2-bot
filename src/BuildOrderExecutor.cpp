#include "BuildOrderExecutor.h"
#include "Utils.h"

//namespace {
//
//sc2::Point2D
//find_warppos_near(SC2& sc2
//    , const sc2::utils::Map& map
//    , const sc2::Point2D& center
//    , float radius
//    , sc2::ABILITY_ID building)
//{
//    const auto max_iterations = 100000;
//    auto pos = rand_point_near(center, radius);
//    int i = 0;
//    while (map.m_topology[sc2::utils::get_tile_pos(pos)] != ' ')
//    {
//        if (i++ > max_iterations)
//            break;
//        pos = rand_point_near(center, radius);
//    }
//    return pos;
//}
//
//}

//TODO: extract
OrderTarget BuildOrderExecutor::findTarget(sc2::ABILITY_ID command) const
{
    if (command == sc2::ABILITY_ID::BUILD_ASSIMILATOR)
    {
        const auto location = sc2::utils::wave(m_map.m_topology
            , sc2::utils::get_tile_pos(m_sc2.obs().GetStartLocation())
            , [&](const sc2::Point2DI& p)
            {
                return m_map.m_topology[p] == '$' && m_sc2.query().Placement(command, { (float)p.x, (float)p.y });
            });
        if (!location)
        {
            throw std::logic_error("CANNOT FIND VESPENE GEYSER!!!");
        }

        const auto geyser = m_sc2.obs().GetUnits(sc2::type(sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER)
            && sc2::in_radius({ float(location->x), float(location->y) }, 3)).front();

        return geyser;
    }
    else if (command == sc2::ABILITY_ID::BUILD_PYLON)
    {
        return find_buildpos_near(m_sc2, m_sc2.obs().GetStartLocation(), 10.f, command);
    }
    else
    {
        auto builder = m_sc2.obs().GetUnits(sc2::Unit::Self, sc2::Filter(sc2::harvester) || sc2::Filter(sc2::idle)).front();
        auto pylon = closest(builder, m_sc2.obs().GetUnits(
            sc2::Unit::Self, type(sc2::UNIT_TYPEID::PROTOSS_PYLON)));
        return find_buildpos_near(m_sc2, pylon->pos, 5.f, command);
    }
}

BuildOrderExecutor::BuildOrderExecutor(SC2& sc2, sc2::utils::Map& map, sc2::utils::TechTree tech_tree, BuildOrder build_order)
    : m_sc2(sc2)
    , m_map(map)
    , m_tech_tree(tech_tree)
    , m_order(std::move(build_order))
{

}

void BuildOrderExecutor::schedule(const BuildOrder::BuildCommand& build_command)
{
    if (build_command.check && !build_command.check(m_sc2.obs()))
    {
        return;
    }

    const auto ability_id = sc2::utils::command(build_command.building_type);
    const auto target = findTarget(ability_id);

    const auto target_pos = std::visit([](auto&& t) {
        //TODO: revise
        using T = std::decay_t<decltype(t)>;
        if constexpr (std::is_same_v<T, sc2::Point2D>)
            return t;
        else if constexpr (std::is_same_v<T, const sc2::Unit*>)
            return sc2::Point2D(t->pos.x, t->pos.y);
        }, target);

    const auto mineral_harvester = not(sc2::target(m_sc2.obs(), sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR)) && sc2::Filter(sc2::harvester);
    const auto builder = sc2::closest(target_pos, m_sc2.obs().GetUnits(sc2::Unit::Self, mineral_harvester));

    std::visit([&](auto&& arg) {
        //workaround: unit stuck into unbuilt assimilator
        if (ability_id == sc2::ABILITY_ID::BUILD_ASSIMILATOR)
        {
            m_sc2.act().UnitCommand(builder, ability_id, arg, true);
            m_sc2.act().UnitCommand(builder, sc2::ABILITY_ID::MOVE, m_sc2.obs().GetStartLocation(), true);
            return;
        }
        m_sc2.act().UnitCommand(builder, ability_id, arg);

        }, target);

    m_active_orders.push_back(ActiveCommand{ target_pos, build_command.building_type, builder->tag });
    m_order.orders().pop_front();

    const auto& building_traits = m_tech_tree[build_command.building_type];
    m_gas_reserve += building_traits.gas_cost;
    m_minerals_reserve += building_traits.mineral_cost;
}

void BuildOrderExecutor::schedule(const BuildOrder::ResearchCommand& research)
{
    auto researchers = m_sc2.obs().GetUnits(sc2::Unit::Self, type(sc2::utils::producer(research.upgrade)) && sc2::built); //TODO add idle check
    if (researchers.empty())
        return;
    m_sc2.act().UnitCommand(researchers.front(), sc2::utils::command(research.upgrade));
    m_order.orders().pop_front();
}

void BuildOrderExecutor::step()
{
    checkProbes();

    if (m_order.orders().empty())
        return;

    auto top_order = m_order.orders().front();

    if (!canAfford(top_order))
    {
        return;
    }

    //if (top_order.check && !top_order.check(m_sc2.obs()))
    //{
    //    return;
    //}


    return std::visit([&](auto&& o) {return schedule(o); }, top_order);
}

void BuildOrderExecutor::unitCreated(const sc2::Unit* unit)
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

void BuildOrderExecutor::buildingConstructionComplete(const sc2::Unit* unit)
{
    switch (unit->unit_type.ToType())
    {
    case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR:
    case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATORRICH:
    {
        auto harvesters = m_sc2.obs().GetUnits(sc2::harvester && not(target(m_sc2.obs(), sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR)));
        for (int i = 0; i < std::min(3, (int)harvesters.size()); ++i)
        {
            m_sc2.act().UnitCommand(harvesters[i], sc2::ABILITY_ID::HARVEST_GATHER, unit);
        }
    }
    break;
    }
}

void BuildOrderExecutor::unitDestroyed(const sc2::Unit* unit)
{
}

void BuildOrderExecutor::unitIdle(const sc2::Unit* unit)
{
    if (unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_PROBE)
    {
        auto minerals = m_sc2.obs().GetUnits(type(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD)
            || type(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750));

        m_sc2.act().UnitCommand(unit, sc2::ABILITY_ID::HARVEST_GATHER, closest(unit, minerals));
    }
}

bool BuildOrderExecutor::canAfford(sc2::UNIT_TYPEID item)
{
    const int minerals = m_sc2.obs().GetMinerals() - m_minerals_reserve;
    const int vespene = m_sc2.obs().GetVespene() - m_gas_reserve;
    const auto& unit_traits = m_tech_tree[item];
    return minerals >= unit_traits.mineral_cost && vespene >= unit_traits.gas_cost;
}

bool BuildOrderExecutor::canAfford(BuildOrder::ResearchCommand item)
{
    const int minerals = m_sc2.obs().GetMinerals() - m_minerals_reserve;
    const int vespene = m_sc2.obs().GetVespene() - m_gas_reserve;
    const auto& upgrades = m_sc2.obs().GetUpgradeData();
    const auto ability_id = sc2::utils::command(item.upgrade);
    auto traits = std::find_if(upgrades.begin(), upgrades.end()
        , [&](auto& u) {return ability_id == u.ability_id; });
    assert(traits != upgrades.end());

    return minerals >= traits->mineral_cost && vespene >= traits->vespene_cost;
}

inline bool BuildOrderExecutor::canAfford(BuildOrder::BuildCommand item)
{
    const int minerals = m_sc2.obs().GetMinerals() - m_minerals_reserve;
    const int vespene = m_sc2.obs().GetVespene() - m_gas_reserve;
    const auto& unit_traits = m_tech_tree[item.building_type];
    return minerals >= unit_traits.mineral_cost && vespene >= unit_traits.gas_cost;
}

inline bool BuildOrderExecutor::canAfford(const BuildOrder::Command& command)
{
    bool res = false;
    std::visit([&](auto&& c) { res = canAfford(c); }, command);
    return res;
}

bool BuildOrderExecutor::isComplete() const { return m_complete || m_order.orders().empty(); }

inline void BuildOrderExecutor::checkProbes()
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

BuildOrder make_4gate(const sc2::ObservationInterface& obs)
{
    auto pylon_built = [](const sc2::ObservationInterface& obs) -> bool {
        //TODO: implement has units
        const auto has_pylon = sc2::type(sc2::UNIT_TYPEID::PROTOSS_PYLON) && sc2::built;
        return !obs.GetUnits(sc2::Unit::Self, has_pylon).empty();
    };

    auto gate_built = [](const sc2::ObservationInterface& obs) -> bool {
        //TODO: implement has units
        const auto has_gate = sc2::type(sc2::UNIT_TYPEID::PROTOSS_GATEWAY) && sc2::built;
        return !obs.GetUnits(sc2::Unit::Self, has_gate).empty();
    };

    auto assimilator_built = [](const sc2::ObservationInterface& obs) -> bool {
        //TODO: implement has units
        const auto done = sc2::type(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR) && sc2::built;
        return !obs.GetUnits(sc2::Unit::Self, done).empty();
    };

    auto cybernetics_built = [](const sc2::ObservationInterface& obs) -> bool {
        //TODO: implement has units
        const auto done = sc2::type(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE) && sc2::built;
        return !obs.GetUnits(sc2::Unit::Self, done).empty();
    };

    BuildOrder order;
    order.build(sc2::UNIT_TYPEID::PROTOSS_PYLON, PlacementHint::WallOff)
        .build(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, PlacementHint::WallOff, pylon_built)
        .build(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR)
        .build(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, PlacementHint::Default, assimilator_built)
        .build(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, PlacementHint::Default, gate_built)
        .build(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, PlacementHint::Default)
        .build(sc2::UNIT_TYPEID::PROTOSS_PYLON, PlacementHint::Default)
        .research(sc2::UPGRADE_ID::WARPGATERESEARCH)  // target:none
        .train(sc2::UNIT_TYPEID::PROTOSS_STALKER);
    //.cast()
    //RESEARCH_WARPGATE
    return order;
}
