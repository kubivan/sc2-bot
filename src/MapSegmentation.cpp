#include "MapSegmentation.h"
#include <sc2api/sc2_map_info.h>
#include <utils/GridUtils.h>
#include <utils/UnitQuery.h>

#include <fstream>
#include <functional>

using namespace sc2::utils;

MapSegmentation::MapSegmentation(SC2& sc2)
    : m_sc2(sc2)
    , m_pathing_grid(GridView{ m_sc2.obs().GetGameInfo().pathing_grid })
    , m_placement_grid(GridView{ m_sc2.obs().GetGameInfo().placement_grid })
    , m_ramps(zip_with(m_pathing_grid
        , m_placement_grid
        , [](bool pathing, bool placement) -> bool 
        {
            return pathing && !placement;
        }))
{
    dump_grid(m_pathing_grid, "pathing.txt");
    dump_grid(m_placement_grid, "placement.txt");
    dump_grid(m_ramps, "ramps.txt");

    const auto minerals = m_sc2.obs().GetUnits([](const sc2::Unit& u) {
        return u.unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD
            || u.unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750
            || u.unit_type == sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD
            ;
        });
    const auto geysers = m_sc2.obs().GetUnits([](const sc2::Unit& u) {
        return u.unit_type == sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER
            || u.unit_type == sc2::UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER
            ;
        });

    auto cmp = [](const sc2::Point2DI& a, const sc2::Point2DI& b) 
                 { return std::tie(a.x, a.y) < std::tie(b.x, b.y); };
    std::set < sc2::Point2DI, decltype(cmp) > minerals_pos(cmp);
    for (auto& mineral : minerals)
    {
        minerals_pos.insert(get_tile_pos(mineral->pos));
    }

    const auto abilities = m_sc2.obs().GetAbilityData();

    std::vector< std::string > map(m_pathing_grid.getHeight()
        , std::string(m_pathing_grid.getWidth(), '#'));

    for (int y = 0; y < m_pathing_grid.getHeight(); ++y)
        for (int x = 0; x < m_pathing_grid.getWidth(); ++x)
        {
            if (minerals_pos.count({ x, y }))
            {
                map[y][x] = 'm';
                continue;
            }
            map[y][x] = m_placement_grid[{x, y}] ? ' ' : '#';
        }

    for (auto& v : geysers)
    {
        const auto tile_pos = get_tile_pos(v->pos);
        map[tile_pos.y][tile_pos.x] = 'v';
    }

    std::ofstream ofs("map.txt");
    for (auto r : map)
    {
        ofs << r << std::endl;
    }

}

void
MapSegmentation::segment()
{
    sc2::PathingGrid(m_sc2.obs().GetGameInfo()).Dump("map2.txt");
}
