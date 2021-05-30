#pragma once

#include "Grid.h"

#include "GridView.h"
#include "UnitTraits.h"
#include <functional>
#include <array>
#include <fstream>

namespace sc2::utils
{

template<typename TPoint>
Point2DI get_tile_pos(const TPoint& pos)
{
    return Point2DI{(int)std::floor(pos.x), (int)std::floor(pos.y)};
}

template<class TA, class TB, class TFunc>
auto zip_with(const TA& a
    , const TB& b
    , TFunc pred)
{
    using VA = std::invoke_result<decltype(&TA::operator[]), const TA*, const Point2DI&>::type;
    using VB = std::invoke_result<decltype(&TB::operator[]), const TB*, const Point2DI&>::type;

    using TRes = std::invoke_result<TFunc, VA, VB>::type;
    Grid<TRes> res(a.getWidth(), a.getHeight());
    for (int y = 0; y < a.getHeight(); ++y)
    {
        for (int x = 0; x < a.getWidth(); ++x)
        {
            const auto pixel = sc2::Point2DI(x, y);
            const bool value = pred(a[pixel], b[pixel]);
            res[pixel] = value;
        }

    }
    return res;
}

template<class T, class TA, class TB>
    Grid<T> mark_with(const TA& a
    , const TB& b
    , T mark)
{
    auto res = Grid<T>{ a.getWidth(), a.getHeight() };
    for (int y = 0; y < a.getHeight(); ++y)
    {
        for (int x = 0; x < a.getWidth(); ++x)
        {
            const auto pixel = sc2::Point2DI{ x, y };
            res[pixel] = b[pixel] ? mark : a[pixel];
        }
    }
    return res;
}

template<class T, class FootPrint>
void
apply_footprint(Grid<T>& g, const Point2DI& center, const FootPrint& footprint, T value)
{
    for (int i = 0; i < footprint.size; ++i)
    {
        const auto& delta = footprint.data[i];
        const auto pos = Point2DI{ center.x + delta.x, center.y + delta.y };
        g[pos] = value;
    }
}

template <class T>
void
place_building(Grid<T>& g, const sc2::Unit& u, T value)
{
    if (!is_building_type(u.unit_type))
    {
        return;
    }
    const auto center = get_tile_pos(u.pos);

    auto footprint = sc2::utils::get_footprint(u.unit_type);
    apply_footprint(g, center, footprint, value);
}

}

