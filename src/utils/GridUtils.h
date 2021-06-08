#pragma once

#include "Grid.h"

#include "GridView.h"
#include "UnitTraits.h"
#include <functional>
#include <set>
#include <queue>
#include <array>
#include <fstream>
#include <optional>

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

template<class TA, class TB, class TFunc>
auto unite(const TA& a
    , const TB& b
    , TFunc pred)
{
    std::vector<Point2DI> res;
    for (int y = 0; y < a.getHeight(); ++y)
    {
        for (int x = 0; x < a.getWidth(); ++x)
        {
            const auto pixel = sc2::Point2DI(x, y);
            if (pred(a[pixel], b[pixel]))
            {
                res.push_back(pixel);
            }
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

template<typename TGrid>
void
dump_grid(const TGrid& grid, const std::string& file_name)
{
    std::ofstream ofs{file_name};
    for (int y = grid.getHeight() - 1; y >= 0; --y)
    {
        for (int x = 0; x < grid.getWidth(); ++x)
        {
            ofs<<grid[{ x, y }];
        }
        ofs << std::endl;
    }
}

template<class T>
std::optional<Point2DI> wave(const Grid<T>& g, const Point2DI& center, std::function<bool(const Point2DI&)> is_goal)
{
    std::queue<Point2DI> fringe;
    fringe.push(center);

    auto point_comparator = [](const Point2DI& a, const Point2DI& b) {
        return std::tie(a.x, a.y) < std::tie(b.x, b.y);
    };
    std::set<Point2DI, decltype(point_comparator)> marked(point_comparator);
    marked.insert( center );
    while (!fringe.empty())
    {
        auto pixel = fringe.front();
        fringe.pop();

        if (is_goal(pixel))
        {
            return pixel;
        }

        auto variants = std::array<Point2DI, 6>{ Point2DI{pixel.x + 1, pixel.y}
                                               , Point2DI{pixel.x, pixel.y + 1}
                                               , Point2DI{pixel.x - 1, pixel.y}
                                               , Point2DI{pixel.x, pixel.y - 1}
                                               , Point2DI{pixel.x + 1, pixel.y + 1}
                                               , Point2DI{pixel.x - 1, pixel.y - 1}
        };

        for (const auto& v : variants)
        {
            if (v.x >= g.getWidth() || v.x < 0)
                continue;
            if (v.y >= g.getHeight() || v.y < 0)
                continue;

            if (!marked.insert(v).second)
            {
                continue;
            }
            fringe.push(v);
        }
    }
    return { };
}

template <class TGrid>
void draw_cirle(TGrid& g, const Point2DI& center, int r, typename TGrid::ValueType value = {})
{
    const int x0 = center.x;
    const int y0 = center.y;
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        g[{x0 + x, y0 + y}] = value;
        g[{x0 + y, y0 + x}] = value;
        g[{x0 - y, y0 + x}] = value;
        g[{x0 - x, y0 + y}] = value;
        g[{x0 - x, y0 - y}] = value;
        g[{x0 - y, y0 - x}] = value;
        g[{x0 + y, y0 - x}] = value;
        g[{x0 + x, y0 - y}] = value;

        if (err <= 0)
        {
            y += 1;
            err += 2 * y + 1;
        }

        if (err > 0)
        {
            x -= 1;
            err -= 2 * x + 1;
        }
    }

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
