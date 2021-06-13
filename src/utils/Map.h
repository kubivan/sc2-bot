#pragma once

#include <SC2.h>
#include <utils/GridUtils.h>

namespace sc2::utils
{

class Map
{
public:
    explicit Map(SC2& sc2);
    void place_building(const Unit& u, char mark = 'b');

//private:
    SC2 m_sc2;
    GridView m_pathing_grid;
    GridView m_placement_grid;
    Grid<char> m_topology;
};

}

