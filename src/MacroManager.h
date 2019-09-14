#pragma once

#include <EventListener.h>

#include <sc2api/sc2_common.h>
#include <queue>

class SC2;

class MacroManager : public EventListener
{
public:
    using BuildOrder = std::queue<sc2::ABILITY_ID>;

    ~MacroManager() override;
    MacroManager(SC2& sc2,
                 BuildOrder build_order = {});

    void step() override;

    void unitCreated(const sc2::Unit* unit) override;

    void buildingConstructionComplete(const sc2::Unit* unit) override;

    void unitDestroyed(const sc2::Unit* unit) override;

    void unitIdle(const sc2::Unit* unit) override;

private:
	struct Builder
	{
		const sc2::Unit* unit;
		sc2::UnitOrder order_bak;
	};
    bool canAfford(BuildOrder::value_type item);
    void executeBuildOrder();
    void checkProbes();
    void checkSupply();

    SC2& m_sc2;
    BuildOrder m_build_order; //BUILD_*; TRAIN_PROBE
    std::set<const sc2::Unit*> m_pylons;
    std::set<const sc2::Unit*> m_builders;
};