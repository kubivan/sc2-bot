#include <CannonRush.h>
#include <Kubot.h>
#include <MacroManager.h>
#include <Utils.h>
#include <utils/Map.h>
#include <utils/UnitTraits.h>
#include <utils/UnitQuery.h>

#include <BuildOrderExecutor.h>

using namespace sc2;

Kubot::~Kubot() {}
Kubot::Kubot()
    : m_sc2(*this)
{
}

//TODO: move to placer
namespace sc2::utils
{
auto find_origin(const sc2::utils::Grid<char>& g
    , const BuildingPlacerPattern& pattern
    , Point2DI start)
{
    return sc2::utils::wave(g, start, [&](const Point2DI& origin) {
        for (int y = 0; y < pattern.height(); ++y)
            for (int x = 0; x < pattern.width(); ++x)
            {
                const auto map_pixel = g[Point2DI{ origin.x + x, origin.y + y }];
                const auto pattern_pixel = pattern[Point2DI{ x, y }];
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
        [&](const Point2DI& p1, const Point2DI& p2)
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

void
Kubot::OnGameStart()
{
    auto obs = Observation();
    dump_pahting_grid(obs->GetGameInfo().pathing_grid, "map.txt");

    m_map = std::make_unique<sc2::utils::Map>(sc2::utils::Map(m_sc2));
    //TODO: mirror automatically
    //"//bbb "
    //"//bbb "
    //"/ bbb "
    //"bbb   "
    //"bbb bb"
    //"bbb bb";
    constexpr auto ramp_pattern_raw =
        "bbb bb"
        "bbb bb"
        "bbb   "
        "/ bbb "
        "//bbb "
        "//bbb ";
    constexpr auto ramp_pattern = sc2::utils::BuildingPlacerPattern{ ramp_pattern_raw, 6, 6, 3};
    constexpr auto ramp_tiles = sc2::utils::make_placer(ramp_pattern);
    auto ramp_origin = sc2::utils::find_origin(m_map->m_topology, ramp_pattern, sc2::utils::get_tile_pos(obs->GetStartLocation()));
    m_sc2.draw().drawTile(*ramp_origin);
    for (auto& [center, footprint] : ramp_tiles)
    {
        for (auto t : footprint)
        {
            m_sc2.draw().drawTile({ t.x + center.x + ramp_origin->x, t.y + center.y + ramp_origin->y}, sc2::Colors::Purple);
        }
        sc2::utils::apply_footprint(m_map->m_topology
            , Point2DI{ center.x + ramp_origin->x, center.y + ramp_origin->y }
            , footprint
            , '*');
        sc2::utils::dump_grid(m_map->m_topology, "topology.txt");
    }

    auto probe = m_sc2.obs().GetUnits(sc2::type(sc2::UNIT_TYPEID::PROTOSS_PROBE)).front();
    m_sc2.act().UnitCommand(probe, sc2::ABILITY_ID::MOVE, Point2D(ramp_origin->x, ramp_origin->y));

    auto tech_tree = sc2::utils::make_tech_tree(*obs);
    auto opening = std::make_unique<BuildOrderExecutor>(m_sc2, *m_map, tech_tree, make_4gate(m_sc2.obs()));
    m_listeners.push_back(std::move(opening));
}

void
Kubot::OnStep()
{
    for (auto& listener : m_listeners)
    {
        listener->step();
    }
    m_sc2.draw().update();
    m_sc2.debug().SendDebug();
}

void
Kubot::OnUnitCreated(const Unit* unit)
{
    static int count = 0; // HACK: skip redundant callbacks for precreated units
    if (count < 13)
    {
        count++;
        return;
    }

    m_map->place_building(*unit);

    for (auto& listener : m_listeners)
    {
        listener->unitCreated(unit);
    }
}
void
Kubot::OnBuildingConstructionComplete(const Unit* unit)
{
    for (auto& listener : m_listeners)
    {
        listener->buildingConstructionComplete(unit);
    }
}

void
Kubot::OnUnitDestroyed(const Unit* unit)
{
    for (auto& listener : m_listeners)
    {
        listener->unitDestroyed(unit);
    }
}

void
Kubot::OnUnitIdle(const Unit* unit)
{
    for (auto& listener : m_listeners)
    {
        listener->unitIdle(unit);
    }
}
