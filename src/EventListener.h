#pragma once

#include <sc2api/sc2_api.h>


class EventListener
{
public:
    virtual ~EventListener() = 0 {};

    virtual void step() {};
    virtual void unitCreated(const sc2::Unit* unit) {}

    virtual void buildingConstructionComplete(const sc2::Unit* unit) {}

    virtual void unitDestroyed(const sc2::Unit*) {}
};

