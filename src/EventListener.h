#pragma once

#include <sc2api/sc2_api.h>

//class API
//{
//public:
//    const sc2::ObservationInterface* obs;
//    sc2::ActionInterface* actions;
//    sc2::QueryInterface* query;
//	sc2::DebugInterface* debug;
//};

class EventListener
{
public:
    virtual ~EventListener() = 0 {};

    virtual void step() {};
    virtual void unitCreated(const sc2::Unit* unit) {}

    virtual void buildingConstructionComplete(const sc2::Unit* unit) {}

    virtual void unitDestroyed(const sc2::Unit*) {}

    virtual void unitIdle(const sc2::Unit*) {}
};

