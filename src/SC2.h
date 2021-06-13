#pragma once

#include <sc2api/sc2_api.h>

#include "Drawer.h"

class SC2 final
{
public:
    SC2(sc2::Agent& agent);

    const sc2::ObservationInterface& obs() const;
    sc2::ActionInterface& act();
    sc2::QueryInterface& query();
    sc2::DebugInterface& debug();

    Drawer& draw();

private:
    const sc2::ObservationInterface& m_obs;
    sc2::ActionInterface& m_actions;
    sc2::QueryInterface& m_query;
    sc2::DebugInterface& m_debug;
    Drawer m_draw;
};

