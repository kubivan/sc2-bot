#include "MapSegmentation.h"
#include <sc2api\sc2_map_info.h>
#include <utils/GridUtils.h>
#include <utils/UnitQuery.h>

#include <fstream>
#include <functional>


MapSegmentation::MapSegmentation(SC2& sc2)
	: m_sc2(sc2)
{
}

void MapSegmentation::segment()
{
	sc2::PathingGrid(m_sc2.obs().GetGameInfo()).Dump("map2.txt");
	const auto pathing_grid = sc2::GridView{m_sc2.obs().GetGameInfo().pathing_grid};
	const auto placement_grid = sc2::GridView{ m_sc2.obs().GetGameInfo().placement_grid };
	auto ramps = sc2::zip_with(pathing_grid
		, placement_grid
		, [](bool pathing, bool plasement) ->bool {
			if (pathing && !plasement) return true;
			return false;
		});

	std::ofstream ofs("ramps.txt");
	auto combined = sc2::mark_with(pathing_grid, ramps, '1');
    for (int y = combined.getArea().Height() - 1; y >= 0; --y) 
	{
		for (int x = 0; x < combined.getArea().Width(); ++x)
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
