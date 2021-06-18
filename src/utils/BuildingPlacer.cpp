#include "BuildingPlacer.h"

#include <utils/UnitTraits.h>
#include <utils/UnitQuery.h>
#include <utils/GridUtils.h>
#include <utils/Map.h>

#include <Utils.h>

namespace 
{
//"bb bbb"
//"bb bbb"
//"   bbb"
//" bbb /"
//" bbb//"
//" bbb//"


//TODO: mirror automatically
//"//bbb "
//"//bbb "
//"/ bbb "
//"bbb   "
//"bbb bb"
//"bbb bb";
constexpr auto placements = /*std::array<std::pair<PlacementHint, BuildingPlacerPattern>, 2>*/
{
    std::make_pair(PlacementHint::WallOff, sc2::utils::BuildingPlacerPattern{"bbb bb"
                                                                             "bbb bb"
                                                                             "bbb   "
                                                                             "/ bbb "
                                                                             "//bbb "
                                                                             "//bbb "
                                                                             , 6, 6, 3, '/'})
    ,
    std::make_pair(PlacementHint::WallOff, sc2::utils::BuildingPlacerPattern{" bbb//"
                                                                             " bbb//"
                                                                             " bbb /"
                                                                             "   bbb"
                                                                             "bb bbb"
                                                                             "bb bbb"
                                                                             , 6, 6, 3, '/'})
};

auto find_origin(const sc2::utils::Grid<char>& g
    , const sc2::utils::BuildingPlacerPattern& pattern
    , sc2::Point2DI start)
{
    return sc2::utils::wave(g, start, [&](const sc2::Point2DI& origin) {
        for (int y = 0; y < pattern.height(); ++y)
            for (int x = 0; x < pattern.width(); ++x)
            {
                const auto map_pixel = g[sc2::Point2DI{ origin.x + x, origin.y + y }];
                const auto pattern_pixel = pattern[sc2::Point2DI{ x, y }];
                if (pattern_pixel == 'b')
                {
                    if (map_pixel != ' ')
                        return false;
                }
                else
                if (pattern_pixel != map_pixel)
                    return false;
            }
        return true;
        },
        [&](const sc2::Point2DI& p1, const sc2::Point2DI& p2)
        {
            auto pixel_a = g[p1];
            auto pixel_b = g[p2];
            if (pixel_a == '#')
                return false;
            if (pixel_b == '#')
                return false;
            if (pixel_a == 'n')
                return true;
            return true;
        }
        );
}

}

namespace sc2::utils
{

BuildingPlacer::BuildingPlacer(SC2& sc2)
    : m_sc2(sc2)
{
}

void BuildingPlacer::init(const Map& map)
{
    for (auto& [hint, pattern] : placements)
    {
        auto origin = find_origin(map.m_topology, pattern, get_tile_pos(m_sc2.obs().GetStartLocation()));
        if (!origin)
            continue;

        for (const auto& [center, footprint] : pattern.footprints())
        {
            for (const auto& delta : footprint)
            {
                const auto tile = Point2DI{ center.x + delta.x, center.y + delta.y };
                m_sc2.draw().drawTile({ tile.x + origin->x, tile.y + origin->y }, sc2::Colors::Purple);
            }
            const auto slot_pos = Point2D{ float(origin->x + center.x), float(origin->y + center.y) };
            m_sc2.draw().drawTile(slot_pos, sc2::Colors::Red);
            m_slots[hint][{footprint.width, footprint.height}].push_back(slot_pos);
        }
    }
}

Point2D BuildingPlacer::placeBuilding(UNIT_TYPEID building, PlacementHint hint) const
{
    ConstexprMap footprints = get_footprint_map();
    const auto& footprint = footprints[building];
    auto size = Size{ footprint.width, footprint.height };

    auto& positions = m_slots[hint][size];
    if (positions.empty())
    {
        //fallback
        return fallback(building);
    }
    const auto start_pos = Point2D{ m_sc2.obs().GetStartLocation() };
    std::ranges::sort(positions, [start_pos](auto p1, auto p2) { return sc2::DistanceSquared2D(p1, start_pos) > sc2::DistanceSquared2D(p2, start_pos); });
    auto res = positions.back();
    m_sc2.draw().drawVerticalLine(res, sc2::Colors::Red);
    std::cout << res.x << " " << res.y << std::endl;
    positions.pop_back();
    return res;
}

//TODO: graceful fallback
Point2D BuildingPlacer::fallback(UNIT_TYPEID building) const
{
    const auto ability_id = sc2::utils::command(building);
    if (ability_id == sc2::ABILITY_ID::BUILD_PYLON)
    {
        return find_buildpos_near(m_sc2, m_sc2.obs().GetStartLocation(), 10.f, ability_id);
    }
    else
    {
        auto builder = m_sc2.obs().GetUnits(sc2::Unit::Self, sc2::Filter(sc2::harvester) || sc2::Filter(sc2::idle)).front();
        auto pylon = closest(builder, m_sc2.obs().GetUnits(
            sc2::Unit::Self, type(sc2::UNIT_TYPEID::PROTOSS_PYLON)));
        return find_buildpos_near(m_sc2, pylon->pos, 5.f, ability_id);
    }
}

} //sc2::utils
