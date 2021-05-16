#include "UnitTraits.h"

#include <sc2api/sc2_interfaces.h>

bool sc2::is_building_type(sc2::UNIT_TYPEID type)
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

sc2::TechTree sc2::make_tech_tree(const sc2::ObservationInterface& obs)
{
    TechTree res;
    // Protoss Buildings                                                                                  unit  bld   wrk    rfn    sup    hall   add
    //res[sc2::UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::EFFECT_PHOTONOVERCHARGE, 0, { sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE, sc2::UNIT_TYPEID::PROTOSS_PYLON }, {}, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_PYLON] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_PYLON, 0, {}, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_NEXUS] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_NEXUS, 0, {}, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_ASSIMILATOR, 0, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_CYBERNETICSCORE, 0, { sc2::UNIT_TYPEID::PROTOSS_GATEWAY, sc2::UNIT_TYPEID::PROTOSS_WARPGATE }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_DARKSHRINE, 0, { sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_FLEETBEACON, 0, { sc2::UNIT_TYPEID::PROTOSS_STARGATE }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_FORGE] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_FORGE, 0, { sc2::UNIT_TYPEID::PROTOSS_NEXUS }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_GATEWAY] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_GATEWAY, 0, { sc2::UNIT_TYPEID::PROTOSS_NEXUS }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_STARGATE] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_STARGATE, 0, { sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_PHOTONCANNON, 0, { sc2::UNIT_TYPEID::PROTOSS_FORGE }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_ROBOTICSBAY, 0, { sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_ROBOTICSFACILITY, 0, { sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_TEMPLARARCHIVE, 0, { sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::BUILD_TWILIGHTCOUNCIL, 0, { sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE }, {} };
    res[sc2::UNIT_TYPEID::PROTOSS_WARPGATE] = { sc2::Race::Protoss, 0, 0, 0, 0, sc2::ABILITY_ID::MORPH_WARPGATE, 0, { sc2::UNIT_TYPEID::PROTOSS_GATEWAY }, { sc2::UPGRADE_ID::WARPGATERESEARCH } };

    res[sc2::UNIT_TYPEID::PROTOSS_PROBE] = { sc2::Race::Protoss, 0, 0, 1, 0, sc2::ABILITY_ID::TRAIN_PROBE, 0, { sc2::UNIT_TYPEID::PROTOSS_NEXUS }, {} };

    for (auto& [id, traits] : res)
    {
        auto& unit_type_data = obs.GetUnitTypeData();
        auto data = std::find_if(unit_type_data.begin()
            , unit_type_data.end()
            , [id](const auto& u) { return u.unit_type_id == id; });

        traits.mineral_cost = data->mineral_cost;
        traits.gas_cost = data->vespene_cost;
        auto abilities = obs.GetAbilityData();
        auto ability = std::find_if(abilities.begin()
            , abilities.end()
            , [&traits](const auto& a) { return a.ability_id == traits.build_ability; });

        traits.tile_width = ability->footprint_radius * 2;
    }

    return res;
}

