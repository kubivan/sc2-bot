#pragma once

#include <EventListener.h>

#include <sc2api/sc2_common.h>
#include <queue>

class MacroManager : public EventListener
{
public:
    using BuildOrder = std::queue<sc2::ABILITY_ID>;

    ~MacroManager() override;
    MacroManager(const API& api,
                 BuildOrder build_order = {});

    void step() override;

    void unitCreated(const sc2::Unit* unit) override;

    void buildingConstructionComplete(const sc2::Unit* unit) override;

    void unitDestroyed(const sc2::Unit* unit) override;

private:
    bool canAfford(BuildOrder::value_type item);
    void executeBuildOrder();
    void checkProbes();
    void checkSupply();

    const API& m_api;
    BuildOrder m_build_order; //BUILD_*; TRAIN_PROBE
    std::set<const sc2::Unit*> m_pylons;
};