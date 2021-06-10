#pragma once

#include "EventListener.h"

#include <utils/Map.h>
#include <utils/UnitTraits.h>
#include <utils/UnitQuery.h>

#include <functional>
#include <variant>

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

    struct ResearchCommand
    {
        sc2::UPGRADE_ID upgrade;
    };

    struct TrainCommand
    {
        sc2::UNIT_TYPEID unit;
    };

    using Command = std::variant<BuildCommand, ResearchCommand, TrainCommand>;

    BuildOrder& build(sc2::UNIT_TYPEID unit
        , PlacementHint hint = PlacementHint::Default
        , PrereqCheck check = {})
    {
        m_commands.push_back(BuildCommand{ unit, hint, check });
        return *this;
    }

    BuildOrder& research(sc2::UPGRADE_ID upgrade)
    {
        m_commands.push_back(ResearchCommand{ upgrade });
        return *this;
    }

    BuildOrder& train(sc2::UNIT_TYPEID unit)
    {
        m_commands.push_back(TrainCommand{ unit });
        return *this;
    }

    auto& orders()
    {
        return m_commands;
    }

    const auto& orders() const
    {
        return m_commands;
    }
private:
    std::deque<Command> m_commands;
};

BuildOrder make_4gate(const sc2::ObservationInterface& obs);

using OrderTarget = std::variant<sc2::Point2D, const sc2::Unit*>;

class BuildOrderExecutor : public EventListener
{
public:
    BuildOrderExecutor(
        SC2& sc2, sc2::utils::Map& map, sc2::utils::TechTree tech_tree, BuildOrder build_order);

    //TODO: extract
    OrderTarget findTarget(sc2::ABILITY_ID command) const;

    void schedule(const BuildOrder::BuildCommand& build_command);

    void schedule(const BuildOrder::ResearchCommand& research);

    void schedule(const BuildOrder::TrainCommand& train);

    void step() override;

    void unitCreated(const sc2::Unit* unit) override;

    void buildingConstructionComplete(const sc2::Unit* unit) override;

    void unitDestroyed(const sc2::Unit* unit) override;

    void unitIdle(const sc2::Unit* unit) override;

    bool canAfford(sc2::UNIT_TYPEID item);

    bool canAfford(BuildOrder::ResearchCommand item);

    bool canAfford(BuildOrder::BuildCommand item);

    bool canAfford(BuildOrder::TrainCommand item);

    bool canAfford(const BuildOrder::Command& command);

    bool isComplete() const;
private:
    struct ActiveCommand
    {
        sc2::Point2D target_pos;
        sc2::UNIT_TYPEID target;
        sc2::Tag executor;
    };

    void checkProbes();
    void chronoBoost();

    SC2& m_sc2;
    sc2::utils::Map& m_map;
    sc2::utils::TechTree m_tech_tree;
    BuildOrder m_order;
    bool m_complete = false;
    std::deque<ActiveCommand> m_active_orders;
    int m_gas_reserve = 0;
    int m_minerals_reserve = 0;
};
