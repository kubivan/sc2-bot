#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_typeenums.h>

#include <unordered_map>
#include <map>
#include <vector>

class SC2;

enum class PlacementHint
{
    Default = 0,
    WallOff,
    Safe,
    Proxy
};

namespace sc2::utils
{

class Map;

class BuildingPlacer
{
public:
    BuildingPlacer(SC2& sc2);

    void init(const Map& map);

    Point2D placeBuilding(UNIT_TYPEID building, PlacementHint hint) const;
    
private:
    Point2D fallback(UNIT_TYPEID building) const;

    using Size = std::pair<int, int>;
    mutable std::unordered_map<PlacementHint, std::map<Size, std::vector<Point2D>>> m_slots;

    SC2& m_sc2;
};

} //sc2::utils
