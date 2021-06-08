#pragma once

#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_interfaces.h>

#include <utils/UnitTraits.h>

namespace sc2
{

constexpr auto alliance(sc2::Unit::Alliance alliance)
{
    return [alliance](const auto& u) constexpr { return u.alliance == alliance; };
}

constexpr auto self = alliance(Unit::Alliance::Self);

constexpr auto ally = alliance(Unit::Alliance::Ally);

constexpr auto neutral = alliance(Unit::Alliance::Neutral);

constexpr auto enemy = alliance(Unit::Alliance::Enemy);

constexpr auto type(sc2::UNIT_TYPEID type)
{
    return [type](const auto& u) constexpr { return u.unit_type == type; };
}

constexpr auto building = [](const Unit& u) constexpr {return sc2::utils::is_building_type(u.unit_type); };

//buiild_progress Range: [0.0, 1.0]. 1.0 == finished.
constexpr auto built = [](const auto& u) constexpr { return u.build_progress == 1.f; };
constexpr auto progress_gt(float value)
{
    assert(value <= 1);
    assert(value >= 0);
    return [value] (const auto& u) constexpr { return u.build_progress > value; };
}

constexpr auto in_radius(const Point2D& center, int radius)
{
    return [center, radius](auto u) constexpr {
        return sc2::DistanceSquared2D(center, u.pos) <= radius * radius;
    };
}

template<typename Pred1, typename Pred2,
     typename std::enable_if< std::is_invocable_v<Pred1, const sc2::Unit&>
            && std::is_invocable_v<Pred2, const sc2::Unit&>
    ,bool>::type = true>
constexpr auto operator&&(Pred1 a, Pred2 b)
{
    return [a, b](const sc2::Unit& u) constexpr {
        return a(u) && b(u);
    };
}

template<typename Pred1, typename Pred2,
     typename std::enable_if< std::is_invocable_v<Pred1, const sc2::Unit&>
            && std::is_invocable_v<Pred2, const sc2::Unit&>
    ,bool>::type = true>
constexpr auto operator||(Pred1 a, Pred2 b)
{
    return [a, b](const Unit& u) constexpr {
        return a(u) || b(u);
    };
}

template<typename Pred,
     typename std::enable_if< std::is_invocable_v<Pred, const sc2::Unit&>
    ,bool>::type = true>
constexpr auto not(Pred p)
{
    return [p](const Unit& u) constexpr {
        return !p(u);
    };
}

const Unit* closest(const sc2::Unit* unit, std::vector<const sc2::Unit*> objects);

}
