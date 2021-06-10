#include "UnitTraits.h"

#include <sc2api/sc2_interfaces.h>

namespace sc2::utils
{

TechTree make_tech_tree(const sc2::ObservationInterface& obs)
{
    TechTree res;
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

    res[sc2::UNIT_TYPEID::PROTOSS_PROBE] = { sc2::Race::Protoss, 0, 0, 1, 0, sc2::ABILITY_ID::TRAIN_PROBE, 0, { sc2::UNIT_TYPEID::PROTOSS_NEXUS }, {} };

    return res;
}

Footprint
get_footprint(const UNIT_TYPEID type)
{
    switch (type)
    {
    case UNIT_TYPEID::PROTOSS_PYLON:
        return make_footprint<2,2>("##"
                                   "c#");
    case UNIT_TYPEID::PROTOSS_FORGE:
        return make_footprint<3, 3>("###"
                                    "#c#"
                                    "###");
    case UNIT_TYPEID::NEUTRAL_MINERALFIELD:
    case UNIT_TYPEID::NEUTRAL_MINERALFIELD750:
        return make_footprint<2,1>("#c");
    case UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER:
    case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
        return make_footprint<3,3>("###"
                                   "#c#"
                                   "###");
    case UNIT_TYPEID::PROTOSS_NEXUS:
        return make_footprint<5,5>(" ### "
                                   "#####"
                                   "##c##"
                                   "#####"
                                   " ### ");
    default:
        return make_footprint<3,3>("###"
                                   "#c#"
                                   "###");
    }
}

//TechTree::TechTree(ObservationInterface& obs)
//{
//}
//
//const BuildingTraits& TechTree::operator[](UNIT_TYPEID unit) const
//{
//    // TODO: insert return statement here
//}

ABILITY_ID command(UNIT_TYPEID unit)
{
    switch (unit)
    {
    case UNIT_TYPEID::PROTOSS_ADEPT:
        return ABILITY_ID::TRAIN_ADEPT;
    case UNIT_TYPEID::PROTOSS_ARCHON:
        return ABILITY_ID::MORPH_ARCHON;
    case UNIT_TYPEID::PROTOSS_ASSIMILATORRICH:
    case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
        return ABILITY_ID::BUILD_ASSIMILATOR;
    case UNIT_TYPEID::PROTOSS_CARRIER:
        return ABILITY_ID::TRAIN_CARRIER;
    case UNIT_TYPEID::PROTOSS_COLOSSUS:
        return ABILITY_ID::TRAIN_COLOSSUS;
    case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
        return ABILITY_ID::BUILD_CYBERNETICSCORE;
    case UNIT_TYPEID::PROTOSS_DARKSHRINE:
        return ABILITY_ID::BUILD_DARKSHRINE;
    case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
        return ABILITY_ID::TRAIN_DARKTEMPLAR;
    case UNIT_TYPEID::PROTOSS_DISRUPTOR:
        return ABILITY_ID::TRAIN_DISRUPTOR;
    //case UNIT_TYPEID::PROTOSS_DISRUPTORPHASED:
    //    return ABILITY_ID::TRAIN_;
    case UNIT_TYPEID::PROTOSS_FLEETBEACON:
        return ABILITY_ID::BUILD_FLEETBEACON;
    case UNIT_TYPEID::PROTOSS_FORGE:
        return ABILITY_ID::BUILD_FORGE;
    case UNIT_TYPEID::PROTOSS_GATEWAY:
        return ABILITY_ID::BUILD_GATEWAY;
    case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
        return ABILITY_ID::TRAIN_HIGHTEMPLAR;
    case UNIT_TYPEID::PROTOSS_IMMORTAL:
        return ABILITY_ID::TRAIN_IMMORTAL;
    //case UNIT_TYPEID::PROTOSS_INTERCEPTOR:
    //    return ABILITY_ID::TRAIN_INTE;
    case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
        return ABILITY_ID::TRAIN_MOTHERSHIP;
    case UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE:
        return ABILITY_ID::TRAIN_MOTHERSHIPCORE;
    case UNIT_TYPEID::PROTOSS_NEXUS:
        return ABILITY_ID::BUILD_NEXUS;
    case UNIT_TYPEID::PROTOSS_OBSERVER:
        return ABILITY_ID::TRAIN_OBSERVER;
    case UNIT_TYPEID::PROTOSS_ORACLE:
        return ABILITY_ID::TRAIN_ORACLE;
    //case UNIT_TYPEID::PROTOSS_ORACLESTASISTRAP:
    //    return ABILITY_ID::;
    case UNIT_TYPEID::PROTOSS_PHOENIX:
        return ABILITY_ID::TRAIN_PHOENIX;
    case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
        return ABILITY_ID::BUILD_PHOTONCANNON;
    case UNIT_TYPEID::PROTOSS_PROBE:
        return ABILITY_ID::TRAIN_PROBE;
    case UNIT_TYPEID::PROTOSS_PYLON:
    case UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED: //TODO: check
        return ABILITY_ID::BUILD_PYLON;
    case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
        return ABILITY_ID::BUILD_ROBOTICSBAY;
    case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
        return ABILITY_ID::BUILD_ROBOTICSFACILITY;
    case UNIT_TYPEID::PROTOSS_SENTRY:
        return ABILITY_ID::TRAIN_SENTRY;
    case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
        return ABILITY_ID::BUILD_SHIELDBATTERY;
    case UNIT_TYPEID::PROTOSS_STALKER:
        return ABILITY_ID::TRAIN_STALKER;
    case UNIT_TYPEID::PROTOSS_STARGATE:
        return ABILITY_ID::BUILD_STARGATE;
    case UNIT_TYPEID::PROTOSS_TEMPEST:
        return ABILITY_ID::TRAIN_TEMPEST;
    case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
        return ABILITY_ID::BUILD_TEMPLARARCHIVE;
    case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
        return ABILITY_ID::BUILD_TWILIGHTCOUNCIL;
    case UNIT_TYPEID::PROTOSS_VOIDRAY:
        return ABILITY_ID::TRAIN_VOIDRAY;
    case UNIT_TYPEID::PROTOSS_WARPGATE:
        return ABILITY_ID::MORPH_WARPGATE;
    }

    throw std::runtime_error("TechTree::command undefined for UNIT_ID: " + std::to_string((int)unit));
}

ABILITY_ID command(UPGRADE_ID unit)
{
    switch (unit)
    {
    case sc2::UPGRADE_ID::INVALID:
        return ABILITY_ID::INVALID;
    case sc2::UPGRADE_ID::CARRIERLAUNCHSPEEDUPGRADE:
        break;
    case sc2::UPGRADE_ID::GLIALRECONSTITUTION:
        break;
    case sc2::UPGRADE_ID::TUNNELINGCLAWS:
        break;
    case sc2::UPGRADE_ID::CHITINOUSPLATING:
        break;
    case sc2::UPGRADE_ID::HISECAUTOTRACKING:
        break;
    case sc2::UPGRADE_ID::TERRANBUILDINGARMOR:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::NEOSTEELFRAME:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::STIMPACK:
        break;
    case sc2::UPGRADE_ID::SHIELDWALL:
        break;
    case sc2::UPGRADE_ID::PUNISHERGRENADES:
        break;
    case sc2::UPGRADE_ID::HIGHCAPACITYBARRELS:
        break;
    case sc2::UPGRADE_ID::BANSHEECLOAK:
        break;
    case sc2::UPGRADE_ID::RAVENCORVIDREACTOR:
        break;
    case sc2::UPGRADE_ID::PERSONALCLOAKING:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
        break;
    case sc2::UPGRADE_ID::OBSERVERGRAVITICBOOSTER:
        break;
    case sc2::UPGRADE_ID::GRAVITICDRIVE:
        break;
    case sc2::UPGRADE_ID::EXTENDEDTHERMALLANCE:
        break;
    case sc2::UPGRADE_ID::PSISTORMTECH:
        break;
    case sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::OVERLORDSPEED:
        break;
    case sc2::UPGRADE_ID::BURROW:
        break;
    case sc2::UPGRADE_ID::ZERGLINGATTACKSPEED:
        break;
    case sc2::UPGRADE_ID::ZERGLINGMOVEMENTSPEED:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::INFESTORENERGYUPGRADE:
        break;
    case sc2::UPGRADE_ID::CENTRIFICALHOOKS:
        break;
    case sc2::UPGRADE_ID::BATTLECRUISERENABLESPECIALIZATIONS:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::WARPGATERESEARCH:
        return sc2::ABILITY_ID::RESEARCH_WARPGATE;
    case sc2::UPGRADE_ID::CHARGE:
        break;
    case sc2::UPGRADE_ID::BLINKTECH:
        break;
    case sc2::UPGRADE_ID::PHOENIXRANGEUPGRADE:
        break;
    case sc2::UPGRADE_ID::NEURALPARASITE:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::DRILLCLAWS:
        break;
    case sc2::UPGRADE_ID::ADEPTPIERCINGATTACK:
        break;
    case sc2::UPGRADE_ID::MAGFIELDLAUNCHERS:
        break;
    case sc2::UPGRADE_ID::EVOLVEGROOVEDSPINES:
        break;
    case sc2::UPGRADE_ID::EVOLVEMUSCULARAUGMENTS:
        break;
    case sc2::UPGRADE_ID::BANSHEESPEED:
        break;
    case sc2::UPGRADE_ID::RAVENRECALIBRATEDEXPLOSIVES:
        break;
    case sc2::UPGRADE_ID::MEDIVACINCREASESPEEDBOOST:
        break;
    case sc2::UPGRADE_ID::LIBERATORAGRANGEUPGRADE:
        break;
    case sc2::UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:
        break;
    case sc2::UPGRADE_ID::CYCLONELOCKONDAMAGEUPGRADE:
        break;
    case sc2::UPGRADE_ID::SMARTSERVOS:
        break;
    case sc2::UPGRADE_ID::RAPIDFIRELAUNCHERS:
        break;
    case sc2::UPGRADE_ID::ENHANCEDMUNITIONS:
        break;
    default:
        break;

    }
    throw std::runtime_error("command undefined for UNIT_ID: " + std::to_string((int)unit));
}

UNIT_TYPEID producer(sc2::ABILITY_ID command)
{
    switch (command)
    {
    case sc2::ABILITY_ID::INVALID:
        break;
    case sc2::ABILITY_ID::SMART:
        break;
    case sc2::ABILITY_ID::ATTACK:
        break;
    case sc2::ABILITY_ID::ATTACK_ATTACK:
        break;
    case sc2::ABILITY_ID::ATTACK_ATTACKBUILDING:
        break;
    case sc2::ABILITY_ID::ATTACK_REDIRECT:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_BUILDINGATTACKOFF:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_BUILDINGATTACKON:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_CLOAKOFF:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_CLOAKOFF_BANSHEE:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_CLOAKOFF_GHOST:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_CLOAKON:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_CLOAKON_BANSHEE:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_CLOAKON_GHOST:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_GENERATECREEPOFF:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_GENERATECREEPON:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_HOLDFIREOFF:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_HOLDFIREOFF_LURKER:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_HOLDFIREON:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_HOLDFIREON_GHOST:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_HOLDFIREON_LURKER:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_PULSARBEAMOFF:
        break;
    case sc2::ABILITY_ID::BEHAVIOR_PULSARBEAMON:
        break;
    case sc2::ABILITY_ID::BUILD_ARMORY:
        break;
    case sc2::ABILITY_ID::BUILD_ASSIMILATOR:
        break;
    case sc2::ABILITY_ID::BUILD_BANELINGNEST:
        break;
    case sc2::ABILITY_ID::BUILD_BARRACKS:
        break;
    case sc2::ABILITY_ID::BUILD_BUNKER:
        break;
    case sc2::ABILITY_ID::BUILD_COMMANDCENTER:
        break;
    case sc2::ABILITY_ID::BUILD_CREEPTUMOR:
        break;
    case sc2::ABILITY_ID::BUILD_CREEPTUMOR_QUEEN:
        break;
    case sc2::ABILITY_ID::BUILD_CREEPTUMOR_TUMOR:
        break;
    case sc2::ABILITY_ID::BUILD_CYBERNETICSCORE:
        break;
    case sc2::ABILITY_ID::BUILD_DARKSHRINE:
        break;
    case sc2::ABILITY_ID::BUILD_ENGINEERINGBAY:
        break;
    case sc2::ABILITY_ID::BUILD_EVOLUTIONCHAMBER:
        break;
    case sc2::ABILITY_ID::BUILD_EXTRACTOR:
        break;
    case sc2::ABILITY_ID::BUILD_FACTORY:
        break;
    case sc2::ABILITY_ID::BUILD_FLEETBEACON:
        break;
    case sc2::ABILITY_ID::BUILD_FORGE:
        break;
    case sc2::ABILITY_ID::BUILD_FUSIONCORE:
        break;
    case sc2::ABILITY_ID::BUILD_GATEWAY:
        break;
    case sc2::ABILITY_ID::BUILD_GHOSTACADEMY:
        break;
    case sc2::ABILITY_ID::BUILD_HATCHERY:
        break;
    case sc2::ABILITY_ID::BUILD_HYDRALISKDEN:
        break;
    case sc2::ABILITY_ID::BUILD_INFESTATIONPIT:
        break;
    case sc2::ABILITY_ID::BUILD_INTERCEPTORS:
        break;
    case sc2::ABILITY_ID::BUILD_MISSILETURRET:
        break;
    case sc2::ABILITY_ID::BUILD_NEXUS:
        break;
    case sc2::ABILITY_ID::BUILD_NUKE:
        break;
    case sc2::ABILITY_ID::BUILD_NYDUSNETWORK:
        break;
    case sc2::ABILITY_ID::BUILD_NYDUSWORM:
        break;
    case sc2::ABILITY_ID::BUILD_PHOTONCANNON:
        break;
    case sc2::ABILITY_ID::BUILD_PYLON:
        break;
    case sc2::ABILITY_ID::BUILD_REACTOR:
        break;
    case sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS:
        break;
    case sc2::ABILITY_ID::BUILD_REACTOR_FACTORY:
        break;
    case sc2::ABILITY_ID::BUILD_REACTOR_STARPORT:
        break;
    case sc2::ABILITY_ID::BUILD_REFINERY:
        break;
    case sc2::ABILITY_ID::BUILD_ROACHWARREN:
        break;
    case sc2::ABILITY_ID::BUILD_ROBOTICSBAY:
        break;
    case sc2::ABILITY_ID::BUILD_ROBOTICSFACILITY:
        break;
    case sc2::ABILITY_ID::BUILD_SENSORTOWER:
        break;
    case sc2::ABILITY_ID::BUILD_SHIELDBATTERY:
        break;
    case sc2::ABILITY_ID::BUILD_SPAWNINGPOOL:
        break;
    case sc2::ABILITY_ID::BUILD_SPINECRAWLER:
        break;
    case sc2::ABILITY_ID::BUILD_SPIRE:
        break;
    case sc2::ABILITY_ID::BUILD_SPORECRAWLER:
        break;
    case sc2::ABILITY_ID::BUILD_STARGATE:
        break;
    case sc2::ABILITY_ID::BUILD_STARPORT:
        break;
    case sc2::ABILITY_ID::BUILD_STASISTRAP:
        break;
    case sc2::ABILITY_ID::BUILD_SUPPLYDEPOT:
        break;
    case sc2::ABILITY_ID::BUILD_TECHLAB:
        break;
    case sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS:
        break;
    case sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY:
        break;
    case sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT:
        break;
    case sc2::ABILITY_ID::BUILD_TEMPLARARCHIVE:
        break;
    case sc2::ABILITY_ID::BUILD_TWILIGHTCOUNCIL:
        break;
    case sc2::ABILITY_ID::BUILD_ULTRALISKCAVERN:
        break;
    case sc2::ABILITY_ID::BURROWDOWN:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_BANELING:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_DRONE:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_HYDRALISK:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_INFESTOR:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_LURKER:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_QUEEN:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_RAVAGER:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_ROACH:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_SWARMHOST:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_WIDOWMINE:
        break;
    case sc2::ABILITY_ID::BURROWDOWN_ZERGLING:
        break;
    case sc2::ABILITY_ID::BURROWUP:
        break;
    case sc2::ABILITY_ID::BURROWUP_BANELING:
        break;
    case sc2::ABILITY_ID::BURROWUP_DRONE:
        break;
    case sc2::ABILITY_ID::BURROWUP_HYDRALISK:
        break;
    case sc2::ABILITY_ID::BURROWUP_INFESTOR:
        break;
    case sc2::ABILITY_ID::BURROWUP_LURKER:
        break;
    case sc2::ABILITY_ID::BURROWUP_QUEEN:
        break;
    case sc2::ABILITY_ID::BURROWUP_RAVAGER:
        break;
    case sc2::ABILITY_ID::BURROWUP_ROACH:
        break;
    case sc2::ABILITY_ID::BURROWUP_SWARMHOST:
        break;
    case sc2::ABILITY_ID::BURROWUP_WIDOWMINE:
        break;
    case sc2::ABILITY_ID::BURROWUP_ZERGLING:
        break;
    case sc2::ABILITY_ID::CANCEL:
        break;
    case sc2::ABILITY_ID::CANCELSLOT_ADDON:
        break;
    case sc2::ABILITY_ID::CANCELSLOT_QUEUE1:
        break;
    case sc2::ABILITY_ID::CANCELSLOT_QUEUE5:
        break;
    case sc2::ABILITY_ID::CANCELSLOT_QUEUECANCELTOSELECTION:
        break;
    case sc2::ABILITY_ID::CANCELSLOT_QUEUEPASSIVE:
        break;
    case sc2::ABILITY_ID::CANCEL_ADEPTPHASESHIFT:
        break;
    case sc2::ABILITY_ID::CANCEL_ADEPTSHADEPHASESHIFT:
        break;
    case sc2::ABILITY_ID::CANCEL_BARRACKSADDON:
        break;
    case sc2::ABILITY_ID::CANCEL_BUILDINPROGRESS:
        break;
    case sc2::ABILITY_ID::CANCEL_CREEPTUMOR:
        break;
    case sc2::ABILITY_ID::CANCEL_FACTORYADDON:
        break;
    case sc2::ABILITY_ID::CANCEL_GRAVITONBEAM:
        break;
    case sc2::ABILITY_ID::CANCEL_LAST:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHBROODLORD:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHLAIR:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHLURKER:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHLURKERDEN:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHMOTHERSHIP:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHORBITAL:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHOVERLORDTRANSPORT:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHOVERSEER:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHPLANETARYFORTRESS:
        break;
    case sc2::ABILITY_ID::CANCEL_MORPHRAVAGER:
        break;
    case sc2::ABILITY_ID::CANCEL_QUEUE1:
        break;
    case sc2::ABILITY_ID::CANCEL_QUEUE5:
        break;
    case sc2::ABILITY_ID::CANCEL_QUEUEADDON:
        break;
    case sc2::ABILITY_ID::CANCEL_QUEUECANCELTOSELECTION:
        break;
    case sc2::ABILITY_ID::CANCEL_QUEUEPASIVE:
        break;
    case sc2::ABILITY_ID::CANCEL_QUEUEPASSIVECANCELTOSELECTION:
        break;
    case sc2::ABILITY_ID::CANCEL_SPINECRAWLERROOT:
        break;
    case sc2::ABILITY_ID::CANCEL_STARPORTADDON:
        break;
    case sc2::ABILITY_ID::EFFECT_ABDUCT:
        break;
    case sc2::ABILITY_ID::EFFECT_ADEPTPHASESHIFT:
        break;
    case sc2::ABILITY_ID::EFFECT_AUTOTURRET:
        break;
    case sc2::ABILITY_ID::EFFECT_BLINDINGCLOUD:
        break;
    case sc2::ABILITY_ID::EFFECT_BLINK:
        break;
    case sc2::ABILITY_ID::EFFECT_BLINK_STALKER:
        break;
    case sc2::ABILITY_ID::EFFECT_CALLDOWNMULE:
        break;
    case sc2::ABILITY_ID::EFFECT_CAUSTICSPRAY:
        break;
    case sc2::ABILITY_ID::EFFECT_CHARGE:
        break;
    case sc2::ABILITY_ID::EFFECT_CHRONOBOOST:
        break;
    case sc2::ABILITY_ID::EFFECT_CONTAMINATE:
        break;
    case sc2::ABILITY_ID::EFFECT_CORROSIVEBILE:
        break;
    case sc2::ABILITY_ID::EFFECT_EMP:
        break;
    case sc2::ABILITY_ID::EFFECT_EXPLODE:
        break;
    case sc2::ABILITY_ID::EFFECT_FEEDBACK:
        break;
    case sc2::ABILITY_ID::EFFECT_FORCEFIELD:
        break;
    case sc2::ABILITY_ID::EFFECT_FUNGALGROWTH:
        break;
    case sc2::ABILITY_ID::EFFECT_GHOSTSNIPE:
        break;
    case sc2::ABILITY_ID::EFFECT_GRAVITONBEAM:
        break;
    case sc2::ABILITY_ID::EFFECT_GUARDIANSHIELD:
        break;
    case sc2::ABILITY_ID::EFFECT_HEAL:
        break;
    case sc2::ABILITY_ID::EFFECT_HUNTERSEEKERMISSILE:
        break;
    case sc2::ABILITY_ID::EFFECT_IMMORTALBARRIER:
        break;
    case sc2::ABILITY_ID::EFFECT_INFESTEDTERRANS:
        break;
    case sc2::ABILITY_ID::EFFECT_INJECTLARVA:
        break;
    case sc2::ABILITY_ID::EFFECT_KD8CHARGE:
        break;
    case sc2::ABILITY_ID::EFFECT_LOCKON:
        break;
    case sc2::ABILITY_ID::EFFECT_LOCUSTSWOOP:
        break;
    case sc2::ABILITY_ID::EFFECT_MASSRECALL:
        break;
    case sc2::ABILITY_ID::EFFECT_MASSRECALL_MOTHERSHIP:
        break;
    case sc2::ABILITY_ID::EFFECT_MASSRECALL_MOTHERSHIPCORE:
        break;
    case sc2::ABILITY_ID::EFFECT_MEDIVACIGNITEAFTERBURNERS:
        break;
    case sc2::ABILITY_ID::EFFECT_NEURALPARASITE:
        break;
    case sc2::ABILITY_ID::EFFECT_NUKECALLDOWN:
        break;
    case sc2::ABILITY_ID::EFFECT_ORACLEREVELATION:
        break;
    case sc2::ABILITY_ID::EFFECT_PARASITICBOMB:
        break;
    case sc2::ABILITY_ID::EFFECT_PHOTONOVERCHARGE:
        break;
    case sc2::ABILITY_ID::EFFECT_POINTDEFENSEDRONE:
        break;
    case sc2::ABILITY_ID::EFFECT_PSISTORM:
        break;
    case sc2::ABILITY_ID::EFFECT_PURIFICATIONNOVA:
        break;
    case sc2::ABILITY_ID::EFFECT_REPAIR:
        break;
    case sc2::ABILITY_ID::EFFECT_REPAIR_MULE:
        break;
    case sc2::ABILITY_ID::EFFECT_REPAIR_SCV:
        break;
    case sc2::ABILITY_ID::EFFECT_RESTORE:
        break;
    case sc2::ABILITY_ID::EFFECT_SALVAGE:
        break;
    case sc2::ABILITY_ID::EFFECT_SCAN:
        break;
    case sc2::ABILITY_ID::EFFECT_SHADOWSTRIDE:
        break;
    case sc2::ABILITY_ID::EFFECT_SPAWNCHANGELING:
        break;
    case sc2::ABILITY_ID::EFFECT_SPAWNLOCUSTS:
        break;
    case sc2::ABILITY_ID::EFFECT_SPRAY:
        break;
    case sc2::ABILITY_ID::EFFECT_SPRAY_PROTOSS:
        break;
    case sc2::ABILITY_ID::EFFECT_SPRAY_TERRAN:
        break;
    case sc2::ABILITY_ID::EFFECT_SPRAY_ZERG:
        break;
    case sc2::ABILITY_ID::EFFECT_STIM:
        break;
    case sc2::ABILITY_ID::EFFECT_STIM_MARAUDER:
        break;
    case sc2::ABILITY_ID::EFFECT_STIM_MARINE:
        break;
    case sc2::ABILITY_ID::EFFECT_STIM_MARINE_REDIRECT:
        break;
    case sc2::ABILITY_ID::EFFECT_SUPPLYDROP:
        break;
    case sc2::ABILITY_ID::EFFECT_TACTICALJUMP:
        break;
    case sc2::ABILITY_ID::EFFECT_TEMPESTDISRUPTIONBLAST:
        break;
    case sc2::ABILITY_ID::EFFECT_TIMEWARP:
        break;
    case sc2::ABILITY_ID::EFFECT_TRANSFUSION:
        break;
    case sc2::ABILITY_ID::EFFECT_VIPERCONSUME:
        break;
    case sc2::ABILITY_ID::EFFECT_VOIDRAYPRISMATICALIGNMENT:
        break;
    case sc2::ABILITY_ID::EFFECT_WIDOWMINEATTACK:
        break;
    case sc2::ABILITY_ID::EFFECT_YAMATOGUN:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_ADEPT:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_ARCHON:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_COLOSSUS:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_DISRUPTOR:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_HIGHTEMPLAR:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_IMMORTAL:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_ORACLE:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_PHOENIX:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_PROBE:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_STALKER:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_VOIDRAY:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_WARPPRISM:
        break;
    case sc2::ABILITY_ID::HALLUCINATION_ZEALOT:
        break;
    case sc2::ABILITY_ID::HALT:
        break;
    case sc2::ABILITY_ID::HALT_BUILDING:
        break;
    case sc2::ABILITY_ID::HALT_TERRANBUILD:
        break;
    case sc2::ABILITY_ID::HARVEST_GATHER:
        break;
    case sc2::ABILITY_ID::HARVEST_GATHER_DRONE:
        break;
    case sc2::ABILITY_ID::HARVEST_GATHER_PROBE:
        break;
    case sc2::ABILITY_ID::HARVEST_GATHER_SCV:
        break;
    case sc2::ABILITY_ID::HARVEST_RETURN:
        break;
    case sc2::ABILITY_ID::HARVEST_RETURN_DRONE:
        break;
    case sc2::ABILITY_ID::HARVEST_RETURN_MULE:
        break;
    case sc2::ABILITY_ID::HARVEST_RETURN_PROBE:
        break;
    case sc2::ABILITY_ID::HARVEST_RETURN_SCV:
        break;
    case sc2::ABILITY_ID::HOLDPOSITION:
        break;
    case sc2::ABILITY_ID::LAND:
        break;
    case sc2::ABILITY_ID::LAND_BARRACKS:
        break;
    case sc2::ABILITY_ID::LAND_COMMANDCENTER:
        break;
    case sc2::ABILITY_ID::LAND_FACTORY:
        break;
    case sc2::ABILITY_ID::LAND_ORBITALCOMMAND:
        break;
    case sc2::ABILITY_ID::LAND_STARPORT:
        break;
    case sc2::ABILITY_ID::LIFT:
        break;
    case sc2::ABILITY_ID::LIFT_BARRACKS:
        break;
    case sc2::ABILITY_ID::LIFT_COMMANDCENTER:
        break;
    case sc2::ABILITY_ID::LIFT_FACTORY:
        break;
    case sc2::ABILITY_ID::LIFT_ORBITALCOMMAND:
        break;
    case sc2::ABILITY_ID::LIFT_STARPORT:
        break;
    case sc2::ABILITY_ID::LOAD:
        break;
    case sc2::ABILITY_ID::LOADALL:
        break;
    case sc2::ABILITY_ID::LOADALL_COMMANDCENTER:
        break;
    case sc2::ABILITY_ID::LOAD_BUNKER:
        break;
    case sc2::ABILITY_ID::LOAD_MEDIVAC:
        break;
    case sc2::ABILITY_ID::MORPH_ARCHON:
        break;
    case sc2::ABILITY_ID::MORPH_BROODLORD:
        break;
    case sc2::ABILITY_ID::MORPH_GATEWAY:
        break;
    case sc2::ABILITY_ID::MORPH_GREATERSPIRE:
        break;
    case sc2::ABILITY_ID::MORPH_HELLBAT:
        break;
    case sc2::ABILITY_ID::MORPH_HELLION:
        break;
    case sc2::ABILITY_ID::MORPH_HIVE:
        break;
    case sc2::ABILITY_ID::MORPH_LAIR:
        break;
    case sc2::ABILITY_ID::MORPH_LIBERATORAAMODE:
        break;
    case sc2::ABILITY_ID::MORPH_LIBERATORAGMODE:
        break;
    case sc2::ABILITY_ID::MORPH_LURKER:
        break;
    case sc2::ABILITY_ID::MORPH_LURKERDEN:
        break;
    case sc2::ABILITY_ID::MORPH_MOTHERSHIP:
        break;
    case sc2::ABILITY_ID::MORPH_ORBITALCOMMAND:
        break;
    case sc2::ABILITY_ID::MORPH_OVERLORDTRANSPORT:
        break;
    case sc2::ABILITY_ID::MORPH_OVERSEER:
        break;
    case sc2::ABILITY_ID::MORPH_PLANETARYFORTRESS:
        break;
    case sc2::ABILITY_ID::MORPH_RAVAGER:
        break;
    case sc2::ABILITY_ID::MORPH_ROOT:
        break;
    case sc2::ABILITY_ID::MORPH_SIEGEMODE:
        break;
    case sc2::ABILITY_ID::MORPH_SPINECRAWLERROOT:
        break;
    case sc2::ABILITY_ID::MORPH_SPINECRAWLERUPROOT:
        break;
    case sc2::ABILITY_ID::MORPH_SPORECRAWLERROOT:
        break;
    case sc2::ABILITY_ID::MORPH_SPORECRAWLERUPROOT:
        break;
    case sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_LOWER:
        break;
    case sc2::ABILITY_ID::MORPH_SUPPLYDEPOT_RAISE:
        break;
    case sc2::ABILITY_ID::MORPH_THOREXPLOSIVEMODE:
        break;
    case sc2::ABILITY_ID::MORPH_THORHIGHIMPACTMODE:
        break;
    case sc2::ABILITY_ID::MORPH_UNSIEGE:
        break;
    case sc2::ABILITY_ID::MORPH_UPROOT:
        break;
    case sc2::ABILITY_ID::MORPH_VIKINGASSAULTMODE:
        break;
    case sc2::ABILITY_ID::MORPH_VIKINGFIGHTERMODE:
        break;
    case sc2::ABILITY_ID::MORPH_WARPGATE:
        break;
    case sc2::ABILITY_ID::MORPH_WARPPRISMPHASINGMODE:
        break;
    case sc2::ABILITY_ID::MORPH_WARPPRISMTRANSPORTMODE:
        break;
    case sc2::ABILITY_ID::MOVE:
        break;
    case sc2::ABILITY_ID::PATROL:
        break;
    case sc2::ABILITY_ID::RALLY_BUILDING:
        break;
    case sc2::ABILITY_ID::RALLY_COMMANDCENTER:
        break;
    case sc2::ABILITY_ID::RALLY_HATCHERY_UNITS:
        break;
    case sc2::ABILITY_ID::RALLY_HATCHERY_WORKERS:
        break;
    case sc2::ABILITY_ID::RALLY_MORPHING_UNIT:
        break;
    case sc2::ABILITY_ID::RALLY_NEXUS:
        break;
    case sc2::ABILITY_ID::RALLY_UNITS:
        break;
    case sc2::ABILITY_ID::RALLY_WORKERS:
        break;
    case sc2::ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES:
        break;
    case sc2::ABILITY_ID::RESEARCH_ADVANCEDBALLISTICS:
        break;
    case sc2::ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD:
        break;
    case sc2::ABILITY_ID::RESEARCH_BANSHEEHYPERFLIGHTROTORS:
        break;
    case sc2::ABILITY_ID::RESEARCH_BATTLECRUISERWEAPONREFIT:
        break;
    case sc2::ABILITY_ID::RESEARCH_BLINK:
        break;
    case sc2::ABILITY_ID::RESEARCH_BURROW:
        break;
    case sc2::ABILITY_ID::RESEARCH_CENTRIFUGALHOOKS:
        break;
    case sc2::ABILITY_ID::RESEARCH_CHARGE:
        break;
    case sc2::ABILITY_ID::RESEARCH_CHITINOUSPLATING:
        break;
    case sc2::ABILITY_ID::RESEARCH_COMBATSHIELD:
        break;
    case sc2::ABILITY_ID::RESEARCH_CONCUSSIVESHELLS:
        break;
    case sc2::ABILITY_ID::RESEARCH_CYCLONELOCKONDAMAGE:
        break;
    case sc2::ABILITY_ID::RESEARCH_DRILLINGCLAWS:
        break;
    case sc2::ABILITY_ID::RESEARCH_ENHANCEDMUNITIONS:
        break;
    case sc2::ABILITY_ID::RESEARCH_EXTENDEDTHERMALLANCE:
        break;
    case sc2::ABILITY_ID::RESEARCH_GLIALREGENERATION:
        break;
    case sc2::ABILITY_ID::RESEARCH_GRAVITICBOOSTER:
        break;
    case sc2::ABILITY_ID::RESEARCH_GRAVITICDRIVE:
        break;
    case sc2::ABILITY_ID::RESEARCH_GROOVEDSPINES:
        break;
    case sc2::ABILITY_ID::RESEARCH_HIGHCAPACITYFUELTANKS:
        break;
    case sc2::ABILITY_ID::RESEARCH_HISECAUTOTRACKING:
        break;
    case sc2::ABILITY_ID::RESEARCH_INFERNALPREIGNITER:
        break;
    case sc2::ABILITY_ID::RESEARCH_INTERCEPTORGRAVITONCATAPULT:
        break;
    case sc2::ABILITY_ID::RESEARCH_MAGFIELDLAUNCHERS:
        break;
    case sc2::ABILITY_ID::RESEARCH_MUSCULARAUGMENTS:
        break;
    case sc2::ABILITY_ID::RESEARCH_NEOSTEELFRAME:
        break;
    case sc2::ABILITY_ID::RESEARCH_NEURALPARASITE:
        break;
    case sc2::ABILITY_ID::RESEARCH_PATHOGENGLANDS:
        break;
    case sc2::ABILITY_ID::RESEARCH_PERSONALCLOAKING:
        break;
    case sc2::ABILITY_ID::RESEARCH_PHOENIXANIONPULSECRYSTALS:
        break;
    case sc2::ABILITY_ID::RESEARCH_PNEUMATIZEDCARAPACE:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMOR:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONS:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMOR:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONS:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDS:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_PSISTORM:
        break;
    case sc2::ABILITY_ID::RESEARCH_RAPIDFIRELAUNCHERS:
        break;
    case sc2::ABILITY_ID::RESEARCH_RAVENCORVIDREACTOR:
        break;
    case sc2::ABILITY_ID::RESEARCH_RAVENRECALIBRATEDEXPLOSIVES:
        break;
    case sc2::ABILITY_ID::RESEARCH_SHADOWSTRIKE:
        break;
    //case sc2::ABILITY_ID::RESEARCH_SMARTSERVOS:
    //    break;
    case sc2::ABILITY_ID::RESEARCH_STIMPACK:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANSTRUCTUREARMORUPGRADE:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_TUNNELINGCLAWS:
        break;
    case sc2::ABILITY_ID::RESEARCH_WARPGATE:
        return sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE;
    case sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMOR:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMORLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMORLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMORLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACK:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACKLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACKLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACKLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMOR:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMORLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMORLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMORLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGLINGADRENALGLANDS:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONS:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONSLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONSLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONSLEVEL3:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONS:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONSLEVEL1:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONSLEVEL2:
        break;
    case sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONSLEVEL3:
        break;
    case sc2::ABILITY_ID::SCAN_MOVE:
        break;
    case sc2::ABILITY_ID::STOP:
        break;
    case sc2::ABILITY_ID::STOP_BUILDING:
        break;
    case sc2::ABILITY_ID::STOP_CHEER:
        break;
    case sc2::ABILITY_ID::STOP_DANCE:
        break;
    case sc2::ABILITY_ID::STOP_REDIRECT:
        break;
    case sc2::ABILITY_ID::STOP_STOP:
        break;
    case sc2::ABILITY_ID::TRAINWARP_ADEPT:
        break;
    case sc2::ABILITY_ID::TRAINWARP_DARKTEMPLAR:
        break;
    case sc2::ABILITY_ID::TRAINWARP_HIGHTEMPLAR:
        break;
    case sc2::ABILITY_ID::TRAINWARP_SENTRY:
        break;
    case sc2::ABILITY_ID::TRAINWARP_STALKER:
        break;
    case sc2::ABILITY_ID::TRAINWARP_ZEALOT:
        break;
    case sc2::ABILITY_ID::TRAIN_ADEPT:
        break;
    case sc2::ABILITY_ID::TRAIN_BANELING:
        break;
    case sc2::ABILITY_ID::TRAIN_BANSHEE:
        break;
    case sc2::ABILITY_ID::TRAIN_BATTLECRUISER:
        break;
    case sc2::ABILITY_ID::TRAIN_CARRIER:
        break;
    case sc2::ABILITY_ID::TRAIN_COLOSSUS:
        break;
    case sc2::ABILITY_ID::TRAIN_CORRUPTOR:
        break;
    case sc2::ABILITY_ID::TRAIN_CYCLONE:
        break;
    case sc2::ABILITY_ID::TRAIN_DARKTEMPLAR:
        break;
    case sc2::ABILITY_ID::TRAIN_DISRUPTOR:
        break;
    case sc2::ABILITY_ID::TRAIN_DRONE:
        break;
    case sc2::ABILITY_ID::TRAIN_GHOST:
        break;
    case sc2::ABILITY_ID::TRAIN_HELLBAT:
        break;
    case sc2::ABILITY_ID::TRAIN_HELLION:
        break;
    case sc2::ABILITY_ID::TRAIN_HIGHTEMPLAR:
        break;
    case sc2::ABILITY_ID::TRAIN_HYDRALISK:
        break;
    case sc2::ABILITY_ID::TRAIN_IMMORTAL:
        break;
    case sc2::ABILITY_ID::TRAIN_INFESTOR:
        break;
    case sc2::ABILITY_ID::TRAIN_LIBERATOR:
        break;
    case sc2::ABILITY_ID::TRAIN_MARAUDER:
        break;
    case sc2::ABILITY_ID::TRAIN_MARINE:
        break;
    case sc2::ABILITY_ID::TRAIN_MEDIVAC:
        break;
    case sc2::ABILITY_ID::TRAIN_MOTHERSHIP:
        break;
    case sc2::ABILITY_ID::TRAIN_MOTHERSHIPCORE:
        break;
    case sc2::ABILITY_ID::TRAIN_MUTALISK:
        break;
    case sc2::ABILITY_ID::TRAIN_OBSERVER:
        break;
    case sc2::ABILITY_ID::TRAIN_ORACLE:
        break;
    case sc2::ABILITY_ID::TRAIN_OVERLORD:
        break;
    case sc2::ABILITY_ID::TRAIN_PHOENIX:
        break;
    case sc2::ABILITY_ID::TRAIN_PROBE:
        break;
    case sc2::ABILITY_ID::TRAIN_QUEEN:
        break;
    case sc2::ABILITY_ID::TRAIN_RAVEN:
        break;
    case sc2::ABILITY_ID::TRAIN_REAPER:
        break;
    case sc2::ABILITY_ID::TRAIN_ROACH:
        break;
    case sc2::ABILITY_ID::TRAIN_SCV:
        break;
    case sc2::ABILITY_ID::TRAIN_SENTRY:
        break;
    case sc2::ABILITY_ID::TRAIN_SIEGETANK:
        break;
    case sc2::ABILITY_ID::TRAIN_STALKER:
        return sc2::UNIT_TYPEID::PROTOSS_GATEWAY;
    case sc2::ABILITY_ID::TRAIN_SWARMHOST:
        break;
    case sc2::ABILITY_ID::TRAIN_TEMPEST:
        break;
    case sc2::ABILITY_ID::TRAIN_THOR:
        break;
    case sc2::ABILITY_ID::TRAIN_ULTRALISK:
        break;
    case sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER:
        break;
    case sc2::ABILITY_ID::TRAIN_VIPER:
        break;
    case sc2::ABILITY_ID::TRAIN_VOIDRAY:
        break;
    case sc2::ABILITY_ID::TRAIN_WARPPRISM:
        break;
    case sc2::ABILITY_ID::TRAIN_WIDOWMINE:
        break;
    case sc2::ABILITY_ID::TRAIN_ZEALOT:
        break;
    case sc2::ABILITY_ID::TRAIN_ZERGLING:
        break;
    case sc2::ABILITY_ID::UNLOADALL:
        break;
    case sc2::ABILITY_ID::UNLOADALLAT:
        break;
    case sc2::ABILITY_ID::UNLOADALLAT_MEDIVAC:
        break;
    case sc2::ABILITY_ID::UNLOADALLAT_OVERLORD:
        break;
    case sc2::ABILITY_ID::UNLOADALLAT_WARPPRISM:
        break;
    case sc2::ABILITY_ID::UNLOADALL_BUNKER:
        break;
    case sc2::ABILITY_ID::UNLOADALL_COMMANDCENTER:
        break;
    case sc2::ABILITY_ID::UNLOADALL_NYDASNETWORK:
        break;
    case sc2::ABILITY_ID::UNLOADALL_NYDUSWORM:
        break;
    case sc2::ABILITY_ID::UNLOADUNIT_BUNKER:
        break;
    case sc2::ABILITY_ID::UNLOADUNIT_COMMANDCENTER:
        break;
    case sc2::ABILITY_ID::UNLOADUNIT_MEDIVAC:
        break;
    case sc2::ABILITY_ID::UNLOADUNIT_NYDASNETWORK:
        break;
    case sc2::ABILITY_ID::UNLOADUNIT_OVERLORD:
        break;
    case sc2::ABILITY_ID::UNLOADUNIT_WARPPRISM:
        break;
    default:
        break;
    }
    throw std::runtime_error("produce undefined for UNIT_ID: " + std::to_string((int)command));
}

UNIT_TYPEID producer(sc2::UNIT_TYPEID unit)
{
    switch (unit)
    {
    case sc2::UNIT_TYPEID::INVALID:
        sc2::UNIT_TYPEID::INVALID;
    case sc2::UNIT_TYPEID::TERRAN_ARMORY:
        break;
    case sc2::UNIT_TYPEID::TERRAN_AUTOTURRET:
        break;
    case sc2::UNIT_TYPEID::TERRAN_BANSHEE:
        break;
    case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
        break;
    case sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING:
        break;
    case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
        break;
    case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
        break;
    case sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER:
        break;
    case sc2::UNIT_TYPEID::TERRAN_BUNKER:
        break;
    case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
        break;
    case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING:
        break;
    case sc2::UNIT_TYPEID::TERRAN_CYCLONE:
        break;
    case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
        break;
    case sc2::UNIT_TYPEID::TERRAN_FACTORY:
        break;
    case sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING:
        break;
    case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:
        break;
    case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
        break;
    case sc2::UNIT_TYPEID::TERRAN_FUSIONCORE:
        break;
    case sc2::UNIT_TYPEID::TERRAN_GHOST:
        break;
    case sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY:
        break;
    case sc2::UNIT_TYPEID::TERRAN_HELLION:
        break;
    case sc2::UNIT_TYPEID::TERRAN_HELLIONTANK:
        break;
    case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:
        break;
    case sc2::UNIT_TYPEID::TERRAN_LIBERATORAG:
        break;
    case sc2::UNIT_TYPEID::TERRAN_MARAUDER:
        break;
    case sc2::UNIT_TYPEID::TERRAN_MARINE:
        break;
    case sc2::UNIT_TYPEID::TERRAN_MEDIVAC:
        break;
    case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:
        break;
    case sc2::UNIT_TYPEID::TERRAN_MULE:
        break;
    case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
        break;
    case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING:
        break;
    case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
        break;
    case sc2::UNIT_TYPEID::TERRAN_RAVEN:
        break;
    case sc2::UNIT_TYPEID::TERRAN_REAPER:
        break;
    case sc2::UNIT_TYPEID::TERRAN_REFINERY:
        break;
    case sc2::UNIT_TYPEID::TERRAN_REFINERYRICH:
        break;
    case sc2::UNIT_TYPEID::TERRAN_SCV:
        break;
    case sc2::UNIT_TYPEID::TERRAN_SENSORTOWER:
        break;
    case sc2::UNIT_TYPEID::TERRAN_SIEGETANK:
        break;
    case sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
        break;
    case sc2::UNIT_TYPEID::TERRAN_STARPORT:
        break;
    case sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING:
        break;
    case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:
        break;
    case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
        break;
    case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
        break;
    case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED:
        break;
    case sc2::UNIT_TYPEID::TERRAN_THOR:
        break;
    case sc2::UNIT_TYPEID::TERRAN_THORAP:
        break;
    case sc2::UNIT_TYPEID::TERRAN_VIKINGASSAULT:
        break;
    case sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
        break;
    case sc2::UNIT_TYPEID::TERRAN_WIDOWMINE:
        break;
    case sc2::UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED:
        break;
    case sc2::UNIT_TYPEID::TERRAN_KD8CHARGE:
        break;
    case sc2::UNIT_TYPEID::TERRAN_NUKE:
        break;
    case sc2::UNIT_TYPEID::TERRAN_POINTDEFENSEDRONE:
        break;
    case sc2::UNIT_TYPEID::TERRAN_REACTOR:
        break;
    case sc2::UNIT_TYPEID::TERRAN_TECHLAB:
        break;
    case sc2::UNIT_TYPEID::ZERG_BANELING:
        break;
    case sc2::UNIT_TYPEID::ZERG_BANELINGBURROWED:
        break;
    case sc2::UNIT_TYPEID::ZERG_BANELINGCOCOON:
        break;
    case sc2::UNIT_TYPEID::ZERG_BANELINGNEST:
        break;
    case sc2::UNIT_TYPEID::ZERG_BROODLING:
        break;
    case sc2::UNIT_TYPEID::ZERG_BROODLORD:
        break;
    case sc2::UNIT_TYPEID::ZERG_BROODLORDCOCOON:
        break;
    case sc2::UNIT_TYPEID::ZERG_CHANGELING:
        break;
    case sc2::UNIT_TYPEID::ZERG_CHANGELINGMARINE:
        break;
    case sc2::UNIT_TYPEID::ZERG_CHANGELINGMARINESHIELD:
        break;
    case sc2::UNIT_TYPEID::ZERG_CHANGELINGZEALOT:
        break;
    case sc2::UNIT_TYPEID::ZERG_CHANGELINGZERGLING:
        break;
    case sc2::UNIT_TYPEID::ZERG_CHANGELINGZERGLINGWINGS:
        break;
    case sc2::UNIT_TYPEID::ZERG_CORRUPTOR:
        break;
    case sc2::UNIT_TYPEID::ZERG_CREEPTUMOR:
        break;
    case sc2::UNIT_TYPEID::ZERG_CREEPTUMORBURROWED:
        break;
    case sc2::UNIT_TYPEID::ZERG_CREEPTUMORQUEEN:
        break;
    case sc2::UNIT_TYPEID::ZERG_DRONE:
        break;
    case sc2::UNIT_TYPEID::ZERG_DRONEBURROWED:
        break;
    case sc2::UNIT_TYPEID::ZERG_EGG:
        break;
    case sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:
        break;
    case sc2::UNIT_TYPEID::ZERG_EXTRACTOR:
        break;
    case sc2::UNIT_TYPEID::ZERG_EXTRACTORRICH:
        break;
    case sc2::UNIT_TYPEID::ZERG_GREATERSPIRE:
        break;
    case sc2::UNIT_TYPEID::ZERG_HATCHERY:
        break;
    case sc2::UNIT_TYPEID::ZERG_HIVE:
        break;
    case sc2::UNIT_TYPEID::ZERG_HYDRALISK:
        break;
    case sc2::UNIT_TYPEID::ZERG_HYDRALISKBURROWED:
        break;
    case sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN:
        break;
    case sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT:
        break;
    case sc2::UNIT_TYPEID::ZERG_INFESTEDTERRANSEGG:
        break;
    case sc2::UNIT_TYPEID::ZERG_INFESTOR:
        break;
    case sc2::UNIT_TYPEID::ZERG_INFESTORBURROWED:
        break;
    case sc2::UNIT_TYPEID::ZERG_INFESTORTERRAN:
        break;
    case sc2::UNIT_TYPEID::ZERG_LAIR:
        break;
    case sc2::UNIT_TYPEID::ZERG_LARVA:
        break;
    case sc2::UNIT_TYPEID::ZERG_LOCUSTMP:
        break;
    case sc2::UNIT_TYPEID::ZERG_LOCUSTMPFLYING:
        break;
    case sc2::UNIT_TYPEID::ZERG_LURKERDENMP:
        break;
    case sc2::UNIT_TYPEID::ZERG_LURKERMP:
        break;
    case sc2::UNIT_TYPEID::ZERG_LURKERMPBURROWED:
        break;
    case sc2::UNIT_TYPEID::ZERG_LURKERMPEGG:
        break;
    case sc2::UNIT_TYPEID::ZERG_MUTALISK:
        break;
    case sc2::UNIT_TYPEID::ZERG_NYDUSCANAL:
        break;
    case sc2::UNIT_TYPEID::ZERG_NYDUSNETWORK:
        break;
    case sc2::UNIT_TYPEID::ZERG_OVERLORD:
        break;
    case sc2::UNIT_TYPEID::ZERG_OVERLORDCOCOON:
        break;
    case sc2::UNIT_TYPEID::ZERG_OVERLORDTRANSPORT:
        break;
    case sc2::UNIT_TYPEID::ZERG_OVERSEER:
        break;
    case sc2::UNIT_TYPEID::ZERG_QUEEN:
        break;
    case sc2::UNIT_TYPEID::ZERG_QUEENBURROWED:
        break;
    case sc2::UNIT_TYPEID::ZERG_RAVAGER:
        break;
    case sc2::UNIT_TYPEID::ZERG_RAVAGERCOCOON:
        break;
    case sc2::UNIT_TYPEID::ZERG_ROACH:
        break;
    case sc2::UNIT_TYPEID::ZERG_ROACHBURROWED:
        break;
    case sc2::UNIT_TYPEID::ZERG_ROACHWARREN:
        break;
    case sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL:
        break;
    case sc2::UNIT_TYPEID::ZERG_SPINECRAWLER:
        break;
    case sc2::UNIT_TYPEID::ZERG_SPINECRAWLERUPROOTED:
        break;
    case sc2::UNIT_TYPEID::ZERG_SPIRE:
        break;
    case sc2::UNIT_TYPEID::ZERG_SPORECRAWLER:
        break;
    case sc2::UNIT_TYPEID::ZERG_SPORECRAWLERUPROOTED:
        break;
    case sc2::UNIT_TYPEID::ZERG_SWARMHOSTBURROWEDMP:
        break;
    case sc2::UNIT_TYPEID::ZERG_SWARMHOSTMP:
        break;
    case sc2::UNIT_TYPEID::ZERG_TRANSPORTOVERLORDCOCOON:
        break;
    case sc2::UNIT_TYPEID::ZERG_ULTRALISK:
        break;
    case sc2::UNIT_TYPEID::ZERG_ULTRALISKCAVERN:
        break;
    case sc2::UNIT_TYPEID::ZERG_VIPER:
        break;
    case sc2::UNIT_TYPEID::ZERG_ZERGLING:
        break;
    case sc2::UNIT_TYPEID::ZERG_ZERGLINGBURROWED:
        break;
    case sc2::UNIT_TYPEID::ZERG_PARASITICBOMBDUMMY:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ADEPT:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ARCHON:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATORRICH:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_CARRIER:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_COLOSSUS:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_DISRUPTOR:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_DISRUPTORPHASED:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_FORGE:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_GATEWAY:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_IMMORTAL:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_INTERCEPTOR:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIP:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_NEXUS:
        return sc2::UNIT_TYPEID::PROTOSS_PROBE;
    case sc2::UNIT_TYPEID::PROTOSS_OBSERVER:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ORACLE:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ORACLESTASISTRAP:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_PHOENIX:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_PROBE:
        return sc2::UNIT_TYPEID::PROTOSS_NEXUS;
    case sc2::UNIT_TYPEID::PROTOSS_PYLON:
        return sc2::UNIT_TYPEID::PROTOSS_PROBE;
    case sc2::UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_SENTRY:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_STALKER:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_STARGATE:
        return sc2::UNIT_TYPEID::PROTOSS_PROBE;
    case sc2::UNIT_TYPEID::PROTOSS_TEMPEST:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_VOIDRAY:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_WARPGATE:
        return sc2::UNIT_TYPEID::PROTOSS_GATEWAY;
    case sc2::UNIT_TYPEID::PROTOSS_WARPPRISM:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_WARPPRISMPHASING:
        break;
    case sc2::UNIT_TYPEID::PROTOSS_ZEALOT:
    case sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLEROCKTOWERDEBRIS:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLEROCKTOWERDIAGONAL:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLEROCKTOWERPUSHUNIT:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERDEBRIS:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERDIAGONAL:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNIT:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNITRAMPLEFT:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNITRAMPRIGHT:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERRAMPLEFT:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERRAMPRIGHT:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_DEBRISRAMPLEFT:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_DEBRISRAMPRIGHT:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRIS6X6:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEBLUR:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEULBR:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEROCK6X6:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEROCKEX1DIAGONALHUGEBLUR:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_FORCEFIELD:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_KARAKFEMALE:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_SCANTIPEDE:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_UNBUILDABLEBRICKSDESTRUCTIBLE:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_UNBUILDABLEPLATESDESTRUCTIBLE:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_UTILITYBOT:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER:
        break;
    case sc2::UNIT_TYPEID::NEUTRAL_XELNAGATOWER:
        break;
    default:
        break;

    }

    throw std::runtime_error("TechTree::command undefined for UNIT_ID: " + std::to_string((int)unit));
}

UNIT_TYPEID producer(sc2::UPGRADE_ID upgrade)
{
    switch (upgrade)
    {
    case sc2::UPGRADE_ID::INVALID:
        return UNIT_TYPEID::INVALID;
    case sc2::UPGRADE_ID::CARRIERLAUNCHSPEEDUPGRADE:
        break;
    case sc2::UPGRADE_ID::GLIALRECONSTITUTION:
        break;
    case sc2::UPGRADE_ID::TUNNELINGCLAWS:
        break;
    case sc2::UPGRADE_ID::CHITINOUSPLATING:
        break;
    case sc2::UPGRADE_ID::HISECAUTOTRACKING:
        break;
    case sc2::UPGRADE_ID::TERRANBUILDINGARMOR:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::NEOSTEELFRAME:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::STIMPACK:
        break;
    case sc2::UPGRADE_ID::SHIELDWALL:
        break;
    case sc2::UPGRADE_ID::PUNISHERGRENADES:
        break;
    case sc2::UPGRADE_ID::HIGHCAPACITYBARRELS:
        break;
    case sc2::UPGRADE_ID::BANSHEECLOAK:
        break;
    case sc2::UPGRADE_ID::RAVENCORVIDREACTOR:
        break;
    case sc2::UPGRADE_ID::PERSONALCLOAKING:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL3:
        break;
    case sc2::UPGRADE_ID::OBSERVERGRAVITICBOOSTER:
        break;
    case sc2::UPGRADE_ID::GRAVITICDRIVE:
        break;
    case sc2::UPGRADE_ID::EXTENDEDTHERMALLANCE:
        break;
    case sc2::UPGRADE_ID::PSISTORMTECH:
        break;
    case sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::OVERLORDSPEED:
        break;
    case sc2::UPGRADE_ID::BURROW:
        break;
    case sc2::UPGRADE_ID::ZERGLINGATTACKSPEED:
        break;
    case sc2::UPGRADE_ID::ZERGLINGMOVEMENTSPEED:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::INFESTORENERGYUPGRADE:
        break;
    case sc2::UPGRADE_ID::CENTRIFICALHOOKS:
        break;
    case sc2::UPGRADE_ID::BATTLECRUISERENABLESPECIALIZATIONS:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::WARPGATERESEARCH:
        return sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE;
    case sc2::UPGRADE_ID::CHARGE:
        break;
        break;
    case sc2::UPGRADE_ID::BLINKTECH:
        break;
    case sc2::UPGRADE_ID::PHOENIXRANGEUPGRADE:
        break;
    case sc2::UPGRADE_ID::NEURALPARASITE:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2:
        break;
    case sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL3:
        break;
    case sc2::UPGRADE_ID::DRILLCLAWS:
        break;
    case sc2::UPGRADE_ID::ADEPTPIERCINGATTACK:
        break;
    case sc2::UPGRADE_ID::MAGFIELDLAUNCHERS:
        break;
    case sc2::UPGRADE_ID::EVOLVEGROOVEDSPINES:
        break;
    case sc2::UPGRADE_ID::EVOLVEMUSCULARAUGMENTS:
        break;
    case sc2::UPGRADE_ID::BANSHEESPEED:
        break;
    case sc2::UPGRADE_ID::RAVENRECALIBRATEDEXPLOSIVES:
        break;
    case sc2::UPGRADE_ID::MEDIVACINCREASESPEEDBOOST:
        break;
    case sc2::UPGRADE_ID::LIBERATORAGRANGEUPGRADE:
        break;
    case sc2::UPGRADE_ID::DARKTEMPLARBLINKUPGRADE:
        break;
    case sc2::UPGRADE_ID::CYCLONELOCKONDAMAGEUPGRADE:
        break;
    case sc2::UPGRADE_ID::SMARTSERVOS:
        break;
    case sc2::UPGRADE_ID::RAPIDFIRELAUNCHERS:
        break;
    case sc2::UPGRADE_ID::ENHANCEDMUNITIONS:
        break;
    default:
        break;
    }
    throw std::runtime_error("TechTree::command undefined for UNIT_ID: " + std::to_string((int)upgrade));
}

}

