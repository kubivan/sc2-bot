#include "Map.h"

#include <sc2api/sc2_map_info.h>
#include <utils/GridUtils.h>
#include <utils/UnitQuery.h>

#include <fstream>
#include <functional>

namespace sc2::utils
{

Map::Map(SC2& sc2)
    : m_sc2(sc2)
    , m_pathing_grid(GridView{ m_sc2.obs().GetGameInfo().pathing_grid })
    , m_placement_grid(GridView{ m_sc2.obs().GetGameInfo().placement_grid })
    , m_topology(m_pathing_grid.getWidth(), m_pathing_grid.getHeight())
{
    dump_grid(m_pathing_grid, "pathing.txt");
    dump_grid(m_placement_grid, "placement.txt");

    for (int y = 0; y < m_pathing_grid.getHeight(); ++y)
        for (int x = 0; x < m_pathing_grid.getWidth(); ++x)
        {
            m_topology[{x, y}] = m_pathing_grid[{x, y}] ? ' ' : '#';
        }

    //set ramps position
    const auto ramps = unite(m_pathing_grid
        , m_placement_grid
        , [](bool pathing, bool placement) -> bool 
        {
            return pathing && !placement;
        });
    for (const auto& r : ramps)
    {
        m_topology[r] = '/';
    }

    //using namespace sc2::utils;
    //const auto is_mineral = type(UNIT_TYPEID::NEUTRAL_MINERALFIELD) || type(UNIT_TYPEID::NEUTRAL_MINERALFIELD750);
    //const auto resources_around_probe = obs->GetUnits(is_resource && in_radius(probe_pos, 10 ));

    const auto minerals = m_sc2.obs().GetUnits([](const sc2::Unit& u) {
        return u.unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD
            || u.unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750
            || u.unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD
            ;
        });
    for (const auto& m : minerals)
    {
        apply_footprint(m_topology, get_tile_pos(m->pos), get_footprint(m->unit_type), '*');
    }

    for (const auto& g : m_sc2.obs().GetUnits(is_geyser))
    {
        apply_footprint(m_topology, get_tile_pos(g->pos), get_footprint(g->unit_type), '$');
    }

    const auto nexuses = m_sc2.obs().GetUnits(type(sc2::UNIT_TYPEID::PROTOSS_NEXUS));
    for (const auto& n : nexuses)
    {
        apply_footprint(m_topology, get_tile_pos(n->pos), get_footprint(n->unit_type), 'n');
    }

    dump_grid(m_topology, "topology.txt");
}

void
Map::place_building(const Unit& u, char mark /*= 'b'*/)
{
    if (!is_building_type(u.unit_type))
    {
        return;
    }
    sc2::utils::place_building(m_topology, u, mark);
    dump_grid(m_topology, "topology.txt");
}

}
