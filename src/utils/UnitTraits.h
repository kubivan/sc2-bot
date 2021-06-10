#pragma once 

#include <sc2api/sc2_unit.h>
#include <array>

#include <sc2api/sc2_interfaces.h>

namespace sc2
{
class ObservationInterface;
}

namespace sc2::utils
{

constexpr bool is_building_type(sc2::UNIT_TYPEID type)
{
    switch (type)
    {
    // Protoss
    case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
    case UNIT_TYPEID::PROTOSS_ASSIMILATORRICH:
    case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
    case UNIT_TYPEID::PROTOSS_DARKSHRINE:
    case UNIT_TYPEID::PROTOSS_FLEETBEACON:
    case UNIT_TYPEID::PROTOSS_FORGE:
    case UNIT_TYPEID::PROTOSS_GATEWAY:
    case UNIT_TYPEID::PROTOSS_NEXUS:
    case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
    case UNIT_TYPEID::PROTOSS_PYLON:
    case UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED:
    case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
    case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
    case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
    case UNIT_TYPEID::PROTOSS_STARGATE:
    case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
    case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
    case UNIT_TYPEID::PROTOSS_WARPGATE:
        return true;
    }
    return false;
}

struct BuildingTraits
{
    sc2::Race race;
    int mineral_cost = 0; //mineral cost of the item
    int gas_cost = 0; //gas cost of the item
    int supply_cost = 0; //supply cost of the item
    int build_time = 0; //build time of the item
    sc2::AbilityID build_ability = 0; //the ability that creates this item
    sc2::AbilityID warp_ability = 0; //the ability that creates this item via warp-in
    std::vector<sc2::UnitTypeID> required_units; //owning ONE of these is required to make
    std::vector<sc2::UPGRADE_ID> required_upgrades; //having ALL of these is required to make
    int tile_width = 0;
    //std::vector<sc2::UNIT_TYPEID> builders; //Who is responsible for creating building/upgrade
};

using TechTree = std::unordered_map<sc2::UNIT_TYPEID, BuildingTraits>;

TechTree make_tech_tree(const sc2::ObservationInterface& obs);

/*constexpr*/ ABILITY_ID command(UNIT_TYPEID unit);
/*constexpr*/ ABILITY_ID command(UPGRADE_ID unit);

/*constexpr*/ UNIT_TYPEID producer(sc2::ABILITY_ID command);
/*constexpr*/ UNIT_TYPEID producer(sc2::UNIT_TYPEID unit);
/*constexpr*/ UNIT_TYPEID producer(sc2::UPGRADE_ID unit);

inline bool can_afford(sc2::UNIT_TYPEID item, sc2::utils::TechTree& tree, const sc2::ObservationInterface& obs)
{
    const auto minerals = obs.GetMinerals();
    const auto vespene = obs.GetVespene();
    const auto unit_traits = tree[item];
    return minerals >= unit_traits.mineral_cost && vespene >= unit_traits.gas_cost;
}

struct Footprint
{
    static const int MAX_SQUARE = 25;

    int size;
    std::array<Point2DI, MAX_SQUARE> data;
};

template<int W, int H>
constexpr Footprint
make_footprint(std::string_view pattern)
{
    static_assert(W * H <= Footprint::MAX_SQUARE);

    auto center = Point2DI { -1, -1 };
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            if (pattern[y*W + x] == 'c')
            {
                center.x = x;
                center.y = y;
            }
        }
    }

    auto res = Footprint{ W * H, {} };
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            const auto delta = pattern[y*W + x] == ' '
                ? Point2DI{ 0,0 } : Point2DI{ x - center.x, center.y - y };
            res.data[y * W + x] = delta;
        }
    }
    return res;
}

Footprint get_footprint(const UNIT_TYPEID type);
}
