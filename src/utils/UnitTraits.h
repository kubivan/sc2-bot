#pragma once 

#include <sc2api/sc2_unit.h>

namespace sc2
{
bool is_building_type(sc2::UNIT_TYPEID type);

struct TraitsData
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
};

class ObservationInterface;
using TechTree = std::unordered_map<sc2::UNIT_TYPEID, TraitsData>;
TechTree make_tech_tree(const ObservationInterface& obs);

}