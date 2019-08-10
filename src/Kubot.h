#pragma once

#include <sc2api/sc2_api.h>
#include <sc2utils/sc2_manage_process.h>

#include <EventListener.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>

class Kubot : public sc2::Agent
{
public:
    ~Kubot() override;

    void OnGameStart() final override;

    void OnStep() override;

    void OnUnitCreated(const sc2::Unit* unit) override;

    void OnBuildingConstructionComplete(const sc2::Unit* unit) override;

    void OnUnitDestroyed(const sc2::Unit*) override;

    std::vector<std::unique_ptr<EventListener>> m_listeners;
    API m_api;
};
