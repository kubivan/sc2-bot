#include "MapSegmentation.h"
#include <sc2api\sc2_map_info.h>
#include <utils/GridUtils.h>
#include <utils/UnitQuery.h>

#include <fstream>
#include <functional>

using namespace sc2::utils;

MapSegmentation::MapSegmentation(SC2& sc2)
    : m_sc2(sc2)
{
}

void MapSegmentation::segment()
{
    sc2::PathingGrid(m_sc2.obs().GetGameInfo()).Dump("map2.txt");
    const auto pathing_grid = GridView{ m_sc2.obs().GetGameInfo().pathing_grid };
    const auto placement_grid = GridView{ m_sc2.obs().GetGameInfo().placement_grid };
    auto ramps = zip_with(pathing_grid
        , placement_grid
        , [](bool pathing, bool plasement) ->bool {
            if (pathing && !plasement) return true;
            return false;
        });

    std::ofstream ofs("ramps.txt");
    auto combined = mark_with(pathing_grid, ramps, '1');
    for (int y = combined.getHeight() - 1; y >= 0; --y)
    {
        for (int x = 0; x < combined.getWidth(); ++x)
        {
            auto bit = combined[{ x, y }];
            switch (bit)
            {
            case 1:
                ofs << ' ';
                break;
            case 0:
                ofs << '#';
                break;
            default:
                ofs << bit;
            }
        }
        ofs << std::endl;
    }

}
