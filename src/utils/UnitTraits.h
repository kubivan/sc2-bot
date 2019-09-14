#pragma once 

#include <sc2api/sc2_unit.h>

namespace sc2
{
inline bool is_building_type(sc2::UNIT_TYPEID type)
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

}