#pragma once
#include <functional>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_interfaces.h>

namespace sc2
{

Filter alliance(Unit::Alliance alliance);

extern Filter building;
extern Filter self;
extern Filter ally;
extern Filter neutral;
extern Filter enemy;

Filter type(UNIT_TYPEID type);

//buiild_progress Range: [0.0, 1.0]. 1.0 == finished.
Filter built();
Filter progress_gt(float value);

Filter in_radius(const Point2D& center, int radius);

template<class Pred>
Filter filter(Pred p)
{
	return [p](const auto& u) {return p(u); };
}

template<typename Pred1, typename Pred2>
Filter operator&&(Pred1 a, Pred2 b)
{
	return [a,b](const Unit& u) {
		return a(u) && b(u);
	};
}

template<typename Pred1, typename Pred2>
Filter operator||(Pred1 a, Pred2 b)
{
	return [a,b](const Unit& u) {
		return a(u) || b(u);
	};
}

template <typename Pred>
Filter not(Pred p)
{
	return [p](const Unit& u) {
		return !p(u);
	};
}

template<typename Pred>
Filter operator!(Pred p)
{
	return not(p);
}

const Unit* closest(const sc2::Unit* unit, std::vector<const sc2::Unit*> objects);

}
