#include <functional>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_interfaces.h>
#include "UnitQuery.h"

namespace sc2
{

const Unit* closest(const sc2::Point2D& pos, const std::vector<const sc2::Unit*>& objects)
{
    assert(!objects.empty());
    return *std::min_element(objects.cbegin()
        , objects.cend()
        , [&pos](const auto a, const auto b) {
            return DistanceSquared2D(pos, a->pos)
                < DistanceSquared2D(pos, b->pos);
        });
}
const Unit* closest(const sc2::Unit* unit, const std::vector<const sc2::Unit*>& objects)
{
    return closest(unit->pos, objects);
}


}
