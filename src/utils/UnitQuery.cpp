#include <functional>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_interfaces.h>
#include "UnitQuery.h"
#include "UnitTraits.h"

namespace sc2
{

Filter building = [](const Unit& u) {return is_building_type(u.unit_type); };

Filter self = alliance(Unit::Alliance::Self);

Filter ally = alliance(Unit::Alliance::Ally);

Filter neutral = alliance(Unit::Alliance::Neutral);

Filter enemy = alliance(Unit::Alliance::Enemy);

//bool self(const Unit& u)
//{
//	return alliance(Unit::Alliance::Self)(u);
//}
//
//bool ally(const Unit& u)
//{
//	return alliance(Unit::Alliance::Ally)(u);
//}
//
//bool neutral(const Unit& u)
//{
//	return alliance(Unit::Alliance::Neutral)(u);
//}

//bool enemy(const Unit& u)
//{
//	return alliance(Unit::Alliance::Enemy)(u);
//}

sc2::Filter alliance(sc2::Unit::Alliance alliance)
{
    return [alliance](const auto& u) { return u.alliance == alliance; };
}

sc2::Filter type(sc2::UNIT_TYPEID type)
{
    return [type](const auto& u) { return u.unit_type == type; };
}

Filter built()
{
    return [](const auto& u) { return u.build_progress == 1.f; };
}

Filter progress_gt(float value)
{
	assert(value <= 1);
	assert(value >= 0);
    return [value](const auto& u) { return u.build_progress > value; };
}

Filter in_radius(const Point2D& center, int radius)
{
	return [center, radius](auto u) {
		return sc2::DistanceSquared2D(center, u.pos) <= radius * radius; 
	};
}

const Unit* closest(const sc2::Unit* unit, std::vector<const sc2::Unit*> objects)
{
	assert(!objects.empty());
	return *std::min_element(objects.cbegin()
		, objects.cend()
		, [unit](const auto a, const auto b) {
			return DistanceSquared2D(unit->pos, a->pos) 
				< DistanceSquared2D(unit->pos, b->pos);
		});
}

}
