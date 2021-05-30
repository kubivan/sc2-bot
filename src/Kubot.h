#pragma once

#include <sc2api/sc2_api.h>
#include <sc2utils/sc2_manage_process.h>

#include <SC2.h>
#include <EventListener.h>

#include <utils/GridUtils.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>

namespace sc2::utils
{
class Map;
}

class Kubot : public sc2::Agent
{
public:
    ~Kubot() override;
    Kubot();

    void OnGameStart() final override;

    void OnStep() override;

    void OnUnitCreated(const sc2::Unit* unit) override;

    void OnBuildingConstructionComplete(const sc2::Unit* unit) override;

    void OnUnitDestroyed(const sc2::Unit*) override;

    void OnUnitIdle(const sc2::Unit*) override;

    std::vector<std::unique_ptr<EventListener>> m_listeners;
    SC2 m_sc2;
    std::unique_ptr<sc2::utils::Map> m_map;
};
