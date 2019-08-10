#include "Utils.h"
#include "Kubot.h"

#include <cstdlib>
#include <fstream>

using namespace sc2;

float
random()
{
    return static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX);
}

float
random0(float x)
{
    return static_cast <float> (std::rand()) / (static_cast <float> (RAND_MAX / x));
}

sc2::Point2D
rand_point_near(const sc2::Point2D& center
               , float radius)
{
    float theta = random0(2 * 3.14159f);
    float distance = random0(1.0) * radius;

    float px = distance * cos(theta) + center.x;
    float py = distance * sin(theta) + center.y;
    return sc2::Point2D{ px, py };
}

void
dump_pahting_grid(const sc2::ImageData& pathing_grid, const std::string& fname)
{
    std::ofstream ofs(fname);
    auto width = pathing_grid.width / 8;
    auto height = pathing_grid.height;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            char bits = pathing_grid.data[y*width + x];
            for (int i = 7; i >= 0; --i)
            {
                ofs << ((bits >> i) & 1 ? ' ' : '#');
            }
        }
        ofs << std::endl;
    }


}

Point2D enemy_base_location(const API& api)
{
    static Point2D res = [&api]() {
    const auto nexus = api.obs->GetUnits([](const Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::PROTOSS_NEXUS;
    }).front();
    for (auto enemy_pos : api.obs->GetGameInfo().enemy_start_locations)
    {
        if (enemy_pos == Point2D{ nexus->pos })
        {
            continue;
        }
        return enemy_pos;
    }
    return Point2D{ -1, -1 };
    }();
    return res;


    assert(false);
}

sc2::Point2D build_near(const API& api
    , const sc2::Unit* probe
    , const sc2::Point2D& center
    , float radius
    , sc2::ABILITY_ID building
    , bool queued)
{
    const auto max_iterations = 10;
    auto pos = rand_point_near(center, radius);
    int i = 0;
    while (!api.query->Placement(building, pos))
    {
        if (i++ > max_iterations)
            break;
        pos = rand_point_near(center, radius);
    }

    api.actions->UnitCommand(probe, building, pos, queued);
    return pos;
}
